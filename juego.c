#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <allegro5/allegro.h> 
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <math.h>
#include <allegro5/allegro_ttf.h>
#define FILAS_HABITACION 17
#define COLUMNAS_HABITACION 30
#define FILAS_MAPA 9
#define COLUMNAS_MAPA 9
#define TAMANHO 64
#define LARGO_TEXTO 30
#define LARGO_SPRITES 30
#define LARGO_PANTALLA 1920
#define ANCHO_PANTALLA 1088
#define MAX_BALAS 20
#define MAX_ENEMIGOS 20
#define MAX_OBJETOS 100
#define MAX_INTERACTUABLES 10
#define MAX_REGISTROS 1000
#define MAX_DIANAS 10

//Ideas deshechadas: 
//Movimiento de camara: implica crear otra camara estatica para cosas que no quiero que se muevan
//Ver todas las habitaciones mientras te mueves: eso implica hacer más condicionales en enemigos, reformular cargar mapa
//Hacer un indicador antes de la sala del jefe: está de más
//Cerrar las puertas al entrar a una habitacion: Con el sistema de balasdisponibles, si no tuvieras balas en una habitacion cerrada pierdes instantaneamente

////////////////////////////////////////////////////////////////  tareas

//Mañana:
//cambiar todas las variables globales
//Separar en funciones cargar mapa, sprites, colisiones

//Mejorar diseño mapas
//Arreglar reaparicion de dianas una vez destruidas

//Propuestas:
//generacion aleatoria de arboles
//Switch main

//IMPORTANTE:
//BALAAAAAAAAAAAAAAAAAS
//BAJAR VARIABLES GLOBALES

//ideas:
//trampas
//Enemigo que te persiga y dispare tres balas

//Ultimo:
//Arreglar problemas al reiniciar
//Dos jugadores
//Joystick
//sonidos y musica

/////////////////////////////////////////////////////////////////  flujo trabajo

//gcc juego.c -o juego -lallegro -lallegro_main -lallegro_primitives -lallegro_image -lm -lallegro_ttf -lallegro_font
//para compilar

//./juego 
//para correr programa

//Si no funciona la pantalla, poner "wsl --shutdown" en powershell y luego "code ."

//flujo git (antes ctrl + s para guardar archivo local)
//git add . -> git commit -m "cambios" -> git push.

////////////////////////////////////////////////////////////////// Estructuras

//Control de balas
int balaActual = 0;
int balaActualEnemigo = 0;

//cadencia de disparo
int cadencia = 0;
int cadenciaEnemigo = 0;

//Se guarda la direccion de la bala en base al mouse
float direccionBala = 0.0;
float direccionBalaEnemigo = 0.0; 

typedef	struct 
{
	int posX;
	int posY;
	int velocidad;
	int activa; 
	int danho;
	float anguloBalaX;
	float anguloBalaY;
	float direccionBalaEnemigo;
	int posXNacimiento;
	int posYNacimiento;
} bala_;

struct dirJugador_
{
	int derecha;
	int izquierda;
};

typedef struct 
{
	int posX;
	int posY;
	int velocidad;
	int movimientoJugador;
	struct dirJugador_ dirJugador;
	int vidas;
	int invulnerable;
	int cantidadMonedas;
	int cantidadLlaves;
	int puntaje;
	int rangoDeBalas;
	int cantidadDeBalas;
	bala_ bala[MAX_BALAS]; 
	int balasDisponibles;
	int traspasoPuerta; //1: Norte, 2: Este, 3: Sur, 4: Oeste
} jugador;

jugador personaje;

typedef struct 
{
	int posX;
	int posY;
	int tamanho;
} mouse_;

mouse_ mouse;

typedef struct 
{
	int tipo;
	int posX;
	int posY;
	int velocidad;
	int activa;
	int direccion; //1 = derecha, 2 = izquierda, 3 = arriba, 4 = abajo
	int vida;
	int posXGeneracion;
	int posYGeneracion;
	bala_ bala[MAX_BALAS];
	int posXAnterior;
	int posYAnterior;
	int ataquesEnemigos;
	int chocoConPared;
	int auxRandAranha;
	int rangoDeBalas;
} enemigo;
 
enemigo slime[MAX_ENEMIGOS];

enemigo Jefe;

enemigo mago[MAX_ENEMIGOS];

enemigo aranha[MAX_ENEMIGOS];

int slimeActual = 0;

int magoActual = 0;

int aranhaActual = 0;

typedef struct 
{
	int posX;
	int posY;
	int activa;
	int destruida;
	int velocidad;
	int chocoConPared;
	int auxRanDianas;
} dianas_;

dianas_ dianas[MAX_DIANAS];

int dianasActuales = 0;

int cantidadDianasDestruidas = 0;

typedef struct 
{
	char registroMapa[LARGO_TEXTO];
	int fila;
	int columna;
	int mapaX;
	int mapaY;
} registroMuertes_;

registroMuertes_ registroMuertes[MAX_REGISTROS];

int cantidadMuertos = 0;

typedef struct 
{
	int mapaX;
	int mapaY;
	int fila;
	int columna;
	int seObtuvoUnaRecompensa;
	int seAbrioUnCofre;
	int seObtuvoUnCargador;
} registroRecompensas_;

registroRecompensas_ registroRecompensas[MAX_REGISTROS];

int cantidadRecompensas = 0;

typedef struct 
{
	int mapaX;
	int mapaY;
	int fila;
	int columna;
	int fogataEncendida;
} registroInteractuables_;

registroInteractuables_ registroInteractuables[MAX_REGISTROS];

int cantidadInteractuables = 0;

typedef struct 
{
	int mapaX;
	int mapaY;
	int fila;
	int columna;
	int idObjeto;
	int comprado;
} registroTienda_;

registroTienda_ registroTienda[MAX_REGISTROS];

int indiceTienda = 0;

typedef struct 
{
	int posX;
	int posY;
	int activa;
	int especial;
	int cofreAbierto;
	int seObtuvo;
	int fila;
	int columna;
	int seVende;
	int precio;
	int idRegistro;
} objeto;

objeto monedas[MAX_OBJETOS];

objeto llaves[MAX_OBJETOS];

objeto cofres[MAX_OBJETOS];

objeto municiones[MAX_OBJETOS];

objeto vidasObjeto[MAX_OBJETOS];

objeto mejoraDanho[MAX_OBJETOS];

objeto mejoraRango[MAX_OBJETOS];

objeto mejoraVelocidad[MAX_OBJETOS];

objeto mapaRojo;

objeto mapaVerde;

objeto mapaAzul;

objeto mapaNaranjo;

int monedasActual = 0;
int llavesActual = 0;
int cofreActual = 0;
int municionesActual = 0;
int vidasObjetoActual = 0;
int mejoraDanhoActual = 0;
int mejoraRangoActual = 0;
int mejoraVelocidadActual = 0;

//Interactuables
typedef struct 
{
	int posX;
	int posY;
	int fila;
	int columna;
	int activa;
	int fogataActiva;
} interactuables;

interactuables fogata[MAX_INTERACTUABLES];

int fogataActual = 0;

int cantidadfogatasActivas = 0;

typedef struct 
{
	char nombre[LARGO_TEXTO];
	int puntajes[1000];
	char nombres[1000][LARGO_TEXTO];
	int indiceNombres;
	int indicePuntajes;
} ranking_;

typedef struct 
{
	int JUEGO;
	int REINICIAR;
	int MENU;
	int SISTEMA;
} estadoJuego_;

typedef struct 
{
	int verdaderaPantallaRanking;
	int pantallaRanking;
	int unichar;
	int longitudNombre;
} controlMenu_;

///////////////////////////////////////////////////////////////// Variables globales

//Estructura donde se guarda el estado del teclado y del mouse
ALLEGRO_KEYBOARD_STATE estado; 
ALLEGRO_MOUSE_STATE estadoMouse;

//En este arreglo se carga el mapa, y en base a él, se dibuja
char sala[FILAS_HABITACION][COLUMNAS_HABITACION];

//Aqui se guarda el nombre de los archivos en un arreglo simulando un mapa
char *mapa[FILAS_MAPA][COLUMNAS_MAPA];

//Control de ubicacion de mapa
int actualMapaX = COLUMNAS_MAPA / 2 + 1;
int actualMapaY = FILAS_MAPA / 2 + 1;

//Control de sprites del personaje
int controlSprites = 0;
int controlSpritesMenu = 0;

//Control de movimiento del jefe
int timerMovimientoJefe = 0;

//Variable que permite saber si la sala actual esta vacia
int salaVacia = 0;

int seGeneroUnaRecompensa = 0;

/////////////////////////////////////////////////////////////////  Funciones

void DibujarMapa(char mapa[FILAS_HABITACION][COLUMNAS_HABITACION], ALLEGRO_BITMAP *spriteSheet);
char cargarMapa(char nombreMapa[LARGO_TEXTO], FILE *varMapa, char mapa[FILAS_HABITACION][COLUMNAS_HABITACION], jugador *personaje, enemigo enemigo[MAX_ENEMIGOS], char puertaDestino, int mapaX, int mapaY);
bool ColisionMapa(char mapa[FILAS_HABITACION][COLUMNAS_HABITACION], int jugadorPosXProximo, int jugadorPosYProximo);
void Logica(char mapa[FILAS_HABITACION][COLUMNAS_HABITACION], jugador *jugador, estadoJuego_ *estadoJuego);
void MovimientoJugador();
void InitAllegro();
int InitGameComponents(ALLEGRO_DISPLAY *ventana, mouse_ *mouse, ranking_ *ranking);
void InputHandle(estadoJuego_ *estadoJuego, controlMenu_ *controlMenu, ranking_ *ranking, ALLEGRO_EVENT_QUEUE *colaEventos);
void Render(char mapa[FILAS_HABITACION][COLUMNAS_HABITACION], ALLEGRO_BITMAP *spriteSheet, ALLEGRO_BITMAP *spriteSheetBalas, ALLEGRO_BITMAP *spriteSheetCaminarCaballero, ALLEGRO_BITMAP *spriteSheetIcons, ALLEGRO_FONT *fuenteJuego, ALLEGRO_BITMAP *spriteSheetCrosshair, ALLEGRO_BITMAP *spriteSheetBalasEnemigos, ALLEGRO_BITMAP *spriteSheetIconsRaven);
void Disparo();
void MovimientoCamara();
void AnimacionPersonaje(ALLEGRO_BITMAP *spriteSheet, ALLEGRO_BITMAP *spriteSheetCaminarCaballero);
void AnimacionEnemigos(ALLEGRO_BITMAP *spriteSheet);
void LogicaEnemigos();
void ColisionEnemigos();
void CambioDeHabitaciones();
void RenderMenu(ALLEGRO_FONT *fuenteJuego, ALLEGRO_BITMAP *menuFondo, controlMenu_ *controlMenu, ranking_ *ranking, char mapa[FILAS_HABITACION][COLUMNAS_HABITACION], ALLEGRO_BITMAP *spriteSheet);
void PersonajeInvulnerable();
void VerificarTraspasoPuertas(char mapa[FILAS_HABITACION][COLUMNAS_HABITACION], jugador *personaje);
void GeneracionDelMapa(int cantidadHabitacionesDeseadas);
void DisparoEnemigos();
void LogicaJefe();
void HandicapsMejorables();
void LogicaMenu(estadoJuego_ *estadoJuego, controlMenu_ *controlMenu, ranking_ *ranking, ALLEGRO_EVENT_QUEUE *colaEventos, FILE *archivoRanking);
void ColicionObjetos();
void RangoVisionEnemigo();
void VerificarSalaVacia();
void GeneracionDeRecompensas();
void ColicionInteractuables();
void LogicaDianas();
void RenderReiniciar(ALLEGRO_FONT *fuenteJuego);
void LogicaReiniciar(estadoJuego_ *estadoJuego);
void Ranking(FILE *archivoRanking, int obtenerRanking, char RegistrarJugador[LARGO_TEXTO], int puntajeDelJugador, ranking_ *ranking);

//Primeros cuatro parametros representan un cuadrado (x1,y1) esquina superior izquierda (w1,h1) sus tamaños, generalmente la cantidad de pixeles, en este caso 64
//Ultimo cuatro representa otro cuadrado con otros parametros
//Compara si hay entre colicion entre ambos, y si hay devuelve true
bool Colicion(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);

//////////////////////////////////////////////////////////////////////////////////////////// 

int main(int argc, char **argv)
{ 
	/////////////////////////////////////////////////////////////// Declaraciones de una vez
	//Estado del juego
	estadoJuego_ estadoJuego;

	estadoJuego.JUEGO = 0;
	estadoJuego.MENU = 1;
	estadoJuego.REINICIAR = 0;
	estadoJuego.SISTEMA = 1;

	//Archivos
	FILE *archivoMapas = NULL;
	FILE *archivoRanking = NULL;

	//Crear ventana
    ALLEGRO_DISPLAY *ventana;

	//Control del menu
	controlMenu_ controlMenu;
	ALLEGRO_EVENT_QUEUE *colaEventos = NULL;

	//Sprites
	ALLEGRO_BITMAP *spriteSheet;
	ALLEGRO_BITMAP *spriteSheetBalas;
	ALLEGRO_BITMAP *spriteSheetCaminarCaballero;
	ALLEGRO_BITMAP *spriteSheetIcons;
	ALLEGRO_BITMAP *spriteSheetCrosshair;
	ALLEGRO_BITMAP *spriteSheetBalasEnemigos;
	ALLEGRO_BITMAP *menuFondo;
	ALLEGRO_BITMAP *spriteSheetIconsRaven;

	//Fonts
	ALLEGRO_FONT *fuenteJuego;

	//Variables texto	
	char nombreHabitacion[LARGO_TEXTO] = "habBase.txt";
	char fondoMenu[LARGO_TEXTO] = "menu_fondo_mapa.txt";

	//variables struct
	ranking_ ranking;
	
	///////////////////////////////////////////////////////////////

	srand(time(NULL));

	InitAllegro();

	//Crear cola de eventos, solo se registra el teclado
	colaEventos = al_create_event_queue();
	al_register_event_source(colaEventos, al_get_keyboard_event_source());

	//Cargar fuentes de texto
	fuenteJuego = al_load_ttf_font("PressStart2P-Regular.ttf", 32, 0);

	//Cargar Sprites
	menuFondo = al_load_bitmap("menu_fondo.png");
	spriteSheet = al_load_bitmap("64x64.png");
	spriteSheetBalas = al_load_bitmap("sp_guns.png");
	spriteSheetCaminarCaballero = al_load_bitmap("64x64_caminar.png");
	spriteSheetIcons = al_load_bitmap("64x64_icons.png");
	spriteSheetCrosshair = al_load_bitmap ("crosshair.png");
	spriteSheetBalasEnemigos = al_load_bitmap("sp_gunsEnemigo.png");
	spriteSheetIconsRaven = al_load_bitmap("IconSet.png");

	while (estadoJuego.SISTEMA)
	{
		InitGameComponents(ventana, &mouse, &ranking);

		cargarMapa(fondoMenu, archivoMapas, sala, &personaje, slime, '@', actualMapaX, actualMapaY);

		GeneracionDelMapa(20); 

		while (estadoJuego.MENU)
		{
			InputHandle(&estadoJuego, &controlMenu, &ranking, colaEventos);

			RenderMenu(fuenteJuego, menuFondo, &controlMenu, &ranking, sala, spriteSheet);

			LogicaMenu(&estadoJuego, &controlMenu, &ranking, colaEventos, archivoRanking);

			al_rest(0.016);
		}

		cargarMapa(nombreHabitacion, archivoMapas, sala, &personaje, slime, '@', actualMapaX, actualMapaY);

		while (estadoJuego.JUEGO)
		{
			//Funcion que actualiza el estado del teclado y mouse
			InputHandle(&estadoJuego, &controlMenu, &ranking, colaEventos);

			//Logica del juego. Ej: movimientos del jugador
			Logica(sala, &personaje, &estadoJuego);

			//Dibujar aqui
			Render(sala, spriteSheet, spriteSheetBalas, spriteSheetCaminarCaballero, spriteSheetIcons, fuenteJuego, spriteSheetCrosshair, spriteSheetBalasEnemigos, spriteSheetIconsRaven);

			//Hacer descansar el cpu
			al_rest(0.016); 
		}

		Ranking(archivoRanking, 0, ranking.nombre, personaje.puntaje, &ranking);

		while (estadoJuego.REINICIAR)
		{
			InputHandle(&estadoJuego, &controlMenu, &ranking, colaEventos);

			RenderReiniciar(fuenteJuego);

			LogicaReiniciar(&estadoJuego);

			al_rest(0.016);
		}
	}

	return 0;
}

void Ranking(FILE *archivoRanking, int obtenerRanking, char RegistrarJugador[LARGO_TEXTO], int puntajeDelJugador, ranking_ *ranking)
{
	//Variables donde se guardan los textos leidos
	char nombreLeido[LARGO_TEXTO];
	int puntajeLeido;
	char linea[100];

/// setRanking();
	if (obtenerRanking == 0)
	{
		//Si el jugador no puso nombre se le nombra anonimo
		if (RegistrarJugador[0] == '\0')
		{
			strcpy(RegistrarJugador, "Anonimo");
		}

		//abrir archivo
		archivoRanking = fopen("ranking.txt", "a");

		//revisar que no esté nulo
		if (archivoRanking == NULL) 
		{
			printf("Error al abrir el archivo"); 
		}

		//Editando archivo
		fprintf(archivoRanking, "%s %d\n", RegistrarJugador, puntajeDelJugador);

		//cerrando archivo;
		fclose(archivoRanking);
	}
	

////// getRanking();
	if (obtenerRanking == 1)
	{
		//abrir archivo
		archivoRanking = fopen("ranking.txt", "r");

		if (archivoRanking == NULL) 
		{
			printf("Error al abrir el archivo"); 
		}

		ranking->indiceNombres = 0;
		ranking->indicePuntajes = 0;

		//Se obtiene la primera linea del archivo
		while (fgets(linea, 100, archivoRanking) != NULL)
		{
			//Se lee la linea y se deja los nombres y enteros 
			sscanf(linea, "%s %d", nombreLeido, &puntajeLeido);

			strcpy(ranking->nombres[ranking->indiceNombres], nombreLeido);
			ranking->puntajes[ranking->indicePuntajes] = puntajeLeido;

			ranking->indiceNombres++;
			ranking->indicePuntajes++;
		}

		fclose(archivoRanking);
	}
}

void LogicaMenu(estadoJuego_ *estadoJuego, controlMenu_ *controlMenu, ranking_ *ranking, ALLEGRO_EVENT_QUEUE *colaEventos, FILE *archivoRanking)
{
	//Cuando el mouse posa sobre el cuadrado de jugar
	if (Colicion(mouse.posX, mouse.posY, mouse.tamanho, mouse.tamanho, 840, 524, 204, 84) && controlMenu->verdaderaPantallaRanking == 0)
	{
		if(al_mouse_button_down(&estadoMouse, ALLEGRO_MOUSE_BUTTON_LEFT))
		{
			controlMenu->pantallaRanking = 1;
		}
	}

	//boton pantalla ranking
	if (Colicion(mouse.posX, mouse.posY, mouse.tamanho, mouse.tamanho, 840, 624, 204, 84) && controlMenu->verdaderaPantallaRanking == 0)
	{
		if(al_mouse_button_down(&estadoMouse, ALLEGRO_MOUSE_BUTTON_LEFT))
		{
			controlMenu->verdaderaPantallaRanking = 1;

			Ranking(archivoRanking, 1, ranking->nombre, personaje.puntaje, ranking);
		}
	}

	//Boton volver al menu
	if (Colicion(mouse.posX, mouse.posY, mouse.tamanho, mouse.tamanho, 100, 900, 200, 150) && controlMenu->verdaderaPantallaRanking == 1)
	{
		if(al_mouse_button_down(&estadoMouse, ALLEGRO_MOUSE_BUTTON_LEFT))
		{
			controlMenu->verdaderaPantallaRanking = 0;
		}
	}
}

void RenderMenu(ALLEGRO_FONT *fuenteJuego, ALLEGRO_BITMAP *menuFondo, controlMenu_ *controlMenu, ranking_ *ranking, char mapa[FILAS_HABITACION][COLUMNAS_HABITACION], ALLEGRO_BITMAP *spriteSheet)
{
	controlSpritesMenu++;

	//Mostrar Menu
	if (controlMenu->pantallaRanking == 0 && controlMenu->verdaderaPantallaRanking == 0)
	{
		al_clear_to_color(al_map_rgb(0, 0, 0));
		//////////////////////////////////////////////// Dibujar en este espacio

		DibujarMapa(mapa, spriteSheet);

		//al_draw_scaled_bitmap(menuFondo, 0, 0, LARGO_PANTALLA, ANCHO_PANTALLA, 0, 0, LARGO_PANTALLA + 760, ANCHO_PANTALLA + 435, 0);

		//Boton jugar (En negro para disimular con fondo)
		al_draw_filled_rectangle(840, 524, 1044, 608, al_map_rgb(0, 0, 0));

		al_draw_text(fuenteJuego, al_map_rgb(255, 255, 255), 850, 550, 0, "Jugar");

		//Boton ranking
		al_draw_filled_rectangle(840, 624, 1044, 708, al_map_rgb(0, 0, 0));

		al_draw_text(fuenteJuego, al_map_rgb(255, 255, 255), 830, 650, 0, "Ranking");

		////////////////////////////////////////////////
		al_flip_display();
	}
	
	//Escribir nombre para empezar el juego
	if (controlMenu->pantallaRanking == 1)
	{
		al_clear_to_color(al_map_rgb(0, 0, 0));
		//////////////////////////////////////////////// Dibujar en este espacio

		al_draw_text(fuenteJuego, al_map_rgb(255, 255, 255), 600, 200, 0, "Ingresa tu nombre:");

		al_draw_textf(fuenteJuego, al_map_rgb(255, 255, 255), 600, 300, 0, "%s|", ranking->nombre);

		////////////////////////////////////////////////
		al_flip_display();	
	}

	//Pantalla de ranking
	if (controlMenu->verdaderaPantallaRanking == 1)
	{
		al_clear_to_color(al_map_rgb(0, 0, 0));
		//////////////////////////////////////////////// Dibujar en este espacio

		al_draw_text(fuenteJuego, al_map_rgb(255, 255, 255), 150, 50, 0, "=====================RANKING=====================");

		for (int i = 0; i < ranking->indiceNombres; i++)
		{
			al_draw_textf(fuenteJuego, al_map_rgb(255, 255, 255), 400, 100 + i * 40, 0, "%s - %d", ranking->nombres[i], ranking->puntajes[i]);
		}

		al_draw_filled_rectangle(100, 900, 300, 1050, al_map_rgb(255, 255, 255));
	
		al_draw_text(fuenteJuego, al_map_rgb(0, 0, 0), 100, 950, 0, "volver");
		
		////////////////////////////////////////////////
		al_flip_display();	
	}
}

void LogicaReiniciar(estadoJuego_ *estadoJuego)
{
	if (Colicion(mouse.posX, mouse.posY, mouse.tamanho, mouse.tamanho, LARGO_PANTALLA / 2 - 120, ANCHO_PANTALLA / 2 + 180, TAMANHO + 140, 20 + TAMANHO))
	{
		if(al_mouse_button_down(&estadoMouse, ALLEGRO_MOUSE_BUTTON_LEFT))
		{
			estadoJuego->MENU = 1;
			estadoJuego->REINICIAR = 0;
		}
	}
}

void RenderReiniciar(ALLEGRO_FONT *fuenteJuego)
{
	al_clear_to_color(al_map_rgb(0, 0, 0));
	//////////////////////////////////////////////// Dibujar en este espacio
	
	al_draw_filled_rectangle(LARGO_PANTALLA / 2 - 120, ANCHO_PANTALLA / 2 + 180, LARGO_PANTALLA / 2 + TAMANHO + 20, ANCHO_PANTALLA / 2 + 200 + TAMANHO, al_map_rgb(102, 0, 161));

	al_draw_text(fuenteJuego, al_map_rgb(0, 255, 255), LARGO_PANTALLA / 2 - 100, ANCHO_PANTALLA / 2 + 200, 0, "Reiniciar");

	////////////////////////////////////////////////
	al_flip_display();
}

void Logica(char mapa[FILAS_HABITACION][COLUMNAS_HABITACION], jugador *jugador, estadoJuego_ *estadoJuego)
{
	MovimientoJugador();

	PersonajeInvulnerable();

	Disparo();

	HandicapsMejorables();

	//MovimientoCamara();

	LogicaEnemigos();

	VerificarTraspasoPuertas(mapa, jugador);

	ColicionObjetos();

	ColicionInteractuables();

	CambioDeHabitaciones();

	VerificarSalaVacia();

	GeneracionDeRecompensas();

	LogicaDianas();
	
	//Axis del mouse
	al_get_mouse_num_axes();

	//TERMINAR EL JUEGO
	if (mapaRojo.seObtuvo == 1 && mapaAzul.seObtuvo == 1 && mapaNaranjo.seObtuvo == 1 && mapaVerde.seObtuvo == 1 && estadoJuego->REINICIAR == 0)
	{
		estadoJuego->JUEGO = 0;
		estadoJuego->REINICIAR = 1;
	}

	//Verificar evento de dianas
	cantidadDianasDestruidas = 0;

	for (int i = 0; i < MAX_DIANAS; i++)
	{
		if (dianas[i].destruida != 0)
		{
			cantidadDianasDestruidas++;

			if (cantidadDianasDestruidas >= 5 && mapaVerde.especial == 0)
			{
				mapaVerde.activa = 1;
				mapaVerde.posX = LARGO_PANTALLA / 2;
				mapaVerde.posY = ANCHO_PANTALLA / 2;
				mapaVerde.especial = 1;
			}
		}
	}

	//Verificar el evento de fogatas
	if (cantidadfogatasActivas >= 4)
	{
		if (actualMapaX == COLUMNAS_MAPA / 2 + 1 && actualMapaY == FILAS_MAPA / 2 + 1)
		{
			for (int i = 0; i < FILAS_HABITACION; i++)
			{
				for (int j = 0; j < COLUMNAS_HABITACION; j++)
				{
					if (mapa[i][j] == 'l')
					{
						mapa[i][j] = '.';
					}
				}
			}
		}
	}
	
	//Cambiar por una pantalla de PERDISTE o reactivar el MENU
	if (personaje.vidas == 0)
	{
		estadoJuego->JUEGO = 0;
		estadoJuego->REINICIAR = 1;
	}
}

void Disparo()
{
	//Como Disparo() se encuentra en while, cada llamada se va acumulando en cadencia, lo usaremos como una especie de timer
	cadencia++;

	if(al_mouse_button_down(&estadoMouse, ALLEGRO_MOUSE_BUTTON_LEFT) && cadencia > 20 && personaje.cantidadDeBalas < MAX_BALAS)
	{
		for (int i = 0; i < MAX_BALAS; i++)
		{
			if (personaje.bala[i].activa == 0)
			{
				personaje.bala[i].posX = personaje.posX + (TAMANHO/2);
				personaje.bala[i].posY = personaje.posY + (TAMANHO/2);
				personaje.bala[i].posXNacimiento = personaje.posX + (TAMANHO/2);
				personaje.bala[i].posYNacimiento = personaje.posY + (TAMANHO/2);
				personaje.bala[i].activa = 1;
				direccionBala = atan2(mouse.posY - personaje.posY, mouse.posX - personaje.posX); //atan2(y2 - y1, x2 - x1)
				personaje.bala[i].anguloBalaX = cos(direccionBala) * personaje.bala[i].velocidad;
				personaje.bala[i].anguloBalaY = sin(direccionBala) * personaje.bala[i].velocidad;
				personaje.cantidadDeBalas++;
				cadencia = 0;
				break;
			}
		}
	}

	//Aumenta constantemente personaje.bala[i].posY/personaje.bala[i].posX
	for (int i = 0; i < MAX_BALAS; i++)
	{
		if (personaje.bala[i].activa != 0)
		{
			personaje.bala[i].posX += personaje.bala[i].anguloBalaX;
			personaje.bala[i].posY += personaje.bala[i].anguloBalaY;
		}

		if (ColisionMapa(sala, personaje.bala[i].posX, personaje.bala[i].posY) || 
		ColisionMapa(sala, personaje.bala[i].posX + (TAMANHO/4) - 1, personaje.bala[i].posY) || 
		ColisionMapa(sala, personaje.bala[i].posX + (TAMANHO/4) - 1, personaje.bala[i].posY + (TAMANHO/4) - 1) ||
		ColisionMapa(sala, personaje.bala[i].posX, personaje.bala[i].posY + (TAMANHO/4) - 1))
		{
			personaje.bala[i].activa = 0;
		}
	}
}

void DisparoEnemigos()
{
	int aux = 0;

	///////////////////////////////////////////////////////////// DISPARO JEFE

	cadenciaEnemigo ++;

	if (cadenciaEnemigo > 20 && Jefe.activa != 0)
	{
		Jefe.bala[balaActualEnemigo].posX = Jefe.posX + TAMANHO;
		Jefe.bala[balaActualEnemigo].posY = Jefe.posY + TAMANHO;
		Jefe.bala[balaActualEnemigo].posXNacimiento = Jefe.posX + TAMANHO;
		Jefe.bala[balaActualEnemigo].posYNacimiento = Jefe.posY + TAMANHO;
		Jefe.bala[balaActualEnemigo].activa = 1;

		direccionBalaEnemigo = atan2(personaje.posY - Jefe.posY, personaje.posX - Jefe.posX); //atan2(y2 - y1, x2 - x1)

		Jefe.bala[balaActualEnemigo].anguloBalaX = cos(direccionBalaEnemigo) * Jefe.bala[balaActualEnemigo].velocidad;
		Jefe.bala[balaActualEnemigo].anguloBalaY = sin(direccionBalaEnemigo) * Jefe.bala[balaActualEnemigo].velocidad;

		balaActualEnemigo++;
		cadenciaEnemigo = 0;
	}

	//Cuando el arreglo este a punto de terminar, se reinicia
	if (balaActualEnemigo > MAX_BALAS - 1)
	{
		balaActualEnemigo = 0;
	}

	for (int i = 0; i < MAX_BALAS; i++)
	{
		if (Jefe.bala[i].activa != 0)
		{
			Jefe.bala[i].posX += Jefe.bala[i].anguloBalaX;
			Jefe.bala[i].posY += Jefe.bala[i].anguloBalaY;
		}

		if (ColisionMapa(sala, Jefe.bala[i].posX, Jefe.bala[i].posY) || 
		ColisionMapa(sala, Jefe.bala[i].posX + (TAMANHO/4) - 1, Jefe.bala[i].posY) || 
		ColisionMapa(sala, Jefe.bala[i].posX + (TAMANHO/4) - 1, Jefe.bala[i].posY + (TAMANHO/4) - 1) ||
		ColisionMapa(sala, Jefe.bala[i].posX, Jefe.bala[i].posY + (TAMANHO/4) - 1))
		{
			Jefe.bala[i].activa = 0;
		}

		if (Jefe.bala[i].posX > Jefe.bala[i].posXNacimiento + Jefe.rangoDeBalas || Jefe.bala[i].posX < Jefe.bala[i].posXNacimiento - Jefe.rangoDeBalas || Jefe.bala[i].posY > Jefe.bala[i].posYNacimiento + Jefe.rangoDeBalas || Jefe.bala[i].posY < Jefe.bala[i].posYNacimiento - Jefe.rangoDeBalas)
		{
			Jefe.bala[i].activa = 0;
		}
	}

	/////////////////////////////////////////////////////////////// LOGICA MAGOS

	for (int i = 0; i < MAX_ENEMIGOS; i++)
	{
		if (cadenciaEnemigo > 20 && mago[i].activa != 0 && mago[i].ataquesEnemigos == 1)
		{
			for (int j = 0; j < MAX_BALAS; j++)
			{
				if (mago[i].bala[j].activa == 0)
				{
					mago[i].bala[j].posX = mago[i].posX;
					mago[i].bala[j].posY = mago[i].posY;
					mago[i].bala[j].posXNacimiento = mago[i].posX;
					mago[i].bala[j].posYNacimiento = mago[i].posY;
					mago[i].bala[j].activa = 1;

					mago[i].bala[j].direccionBalaEnemigo = atan2(personaje.posY - mago[i].posY, personaje.posX - mago[i].posX); //atan2(y2 - y1, x2 - x1)

					mago[i].bala[j].anguloBalaX = cos(mago[i].bala[j].direccionBalaEnemigo) * mago[i].bala[j].velocidad;
					mago[i].bala[j].anguloBalaY = sin(mago[i].bala[j].direccionBalaEnemigo) * mago[i].bala[j].velocidad;

					aux = 1;
					
					break;
				}
			}
		}	
	}
	
	if (aux == 1)
	{
		cadenciaEnemigo = 0;
	}
	
	for (int i = 0; i < MAX_ENEMIGOS; i++)
	{
		for (int j = 0; j < MAX_BALAS; j++)
		{
			if (mago[i].bala[j].activa != 0)
			{
				mago[i].bala[j].posX += mago[i].bala[j].anguloBalaX;
				mago[i].bala[j].posY += mago[i].bala[j].anguloBalaY;
			}

			if (ColisionMapa(sala, mago[i].bala[j].posX, mago[i].bala[j].posY) || 
			ColisionMapa(sala, mago[i].bala[j].posX + (TAMANHO/4) - 1, mago[i].bala[j].posY) || 
			ColisionMapa(sala, mago[i].bala[j].posX + (TAMANHO/4) - 1, mago[i].bala[j].posY + (TAMANHO/4) - 1) ||
			ColisionMapa(sala, mago[i].bala[j].posX, mago[i].bala[j].posY + (TAMANHO/4) - 1))
			{
				mago[i].bala[j].activa = 0;
			}

			//Destruccion de balas en funcion de la posicion del enemigo para que el arreglo nunca se acabe
			if (mago[i].bala[j].posX > mago[i].bala[j].posXNacimiento + mago[i].rangoDeBalas || mago[i].bala[j].posX < mago[i].bala[j].posXNacimiento - mago[i].rangoDeBalas || mago[i].bala[j].posY > mago[i].bala[j].posYNacimiento + mago[i].rangoDeBalas ||mago[i].bala[j].posY < mago[i].bala[j].posYNacimiento - mago[i].rangoDeBalas)
			{
				mago[i].bala[j].activa = 0;
			}
		}
	}
}

void PersonajeInvulnerable()
{
	if (personaje.invulnerable > 0)
	{
		personaje.invulnerable ++;

		if (personaje.invulnerable > 60)
		{
			personaje.invulnerable = 0;
		}
	}
}

void MovimientoJugador()
{
	//Guardamos la posicion del jugador en un axiliar
	int auxX = personaje.posX;
	int auxY = personaje.posY;

	if(al_key_down(&estado, ALLEGRO_KEY_W))
	{
		personaje.posY -= personaje.velocidad; 
		personaje.movimientoJugador = 1;
	}

	if(al_key_down(&estado, ALLEGRO_KEY_S))
	{
		personaje.posY += personaje.velocidad; 
		personaje.movimientoJugador = 1;
	}

	//Luego de verificar la posY se comprueba si cada esquina del jugador está en colision con '#' en el arreglo del mapa
	if (ColisionMapa(sala, personaje.posX, personaje.posY) || 
	ColisionMapa(sala, personaje.posX + TAMANHO - 1, personaje.posY) || 
	ColisionMapa(sala, personaje.posX + TAMANHO - 1, personaje.posY + TAMANHO - 1) ||
	ColisionMapa(sala, personaje.posX, personaje.posY + TAMANHO - 1))
	{
		personaje.posY = auxY;
	}

	if(al_key_down(&estado, ALLEGRO_KEY_D))
	{
		personaje.posX += personaje.velocidad; 
		personaje.dirJugador.derecha = 1;
		personaje.dirJugador.izquierda = 0;
		personaje.movimientoJugador = 1;
	}

	if(al_key_down(&estado, ALLEGRO_KEY_A))
	{
		personaje.posX -= personaje.velocidad; 
		personaje.dirJugador.derecha = 0;
		personaje.dirJugador.izquierda = 1;
		personaje.movimientoJugador = 1;
	}

	if (ColisionMapa(sala, personaje.posX, personaje.posY) || 
	ColisionMapa(sala, personaje.posX + TAMANHO - 1, personaje.posY) || 
	ColisionMapa(sala, personaje.posX + TAMANHO - 1, personaje.posY + TAMANHO - 1) ||
	ColisionMapa(sala, personaje.posX, personaje.posY + TAMANHO - 1))
	{
		personaje.posX = auxX;
	}
}

void HandicapsMejorables()
{
	for (int i = 0; i < MAX_BALAS; i++)
	{
		if (personaje.bala[i].posX > personaje.bala[i].posXNacimiento + personaje.rangoDeBalas || personaje.bala[i].posX < personaje.bala[i].posXNacimiento - personaje.rangoDeBalas || personaje.bala[i].posY < personaje.bala[i].posYNacimiento - personaje.rangoDeBalas || personaje.bala[i].posY > personaje.bala[i].posYNacimiento + personaje.rangoDeBalas)
		{
			personaje.bala[i].activa = 0;
		}	
	}
}

char cargarMapa(char nombreMapa[LARGO_TEXTO], FILE *archivoMapa, char mapa[FILAS_HABITACION][COLUMNAS_HABITACION], jugador *jugador, enemigo enemigo[MAX_ENEMIGOS], char puertaDestino, int mapaX, int mapaY)
{
	int muerto = 0;
	int cofreAbierto = 0;
	int fogataEncendida = 0;
	int cargadorObtenido = 0;
	int auxRandTienda = 0;
	int productoTiendaGenerado = 0;
	aranhaActual = 0;
	magoActual = 0;
	slimeActual = 0;
	mejoraDanhoActual = 0;
	mejoraRangoActual = 0;
	mejoraVelocidadActual = 0;
	salaVacia = 0;
	seGeneroUnaRecompensa = 0;
	dianasActuales = 0;

	if ((archivoMapa = fopen(nombreMapa,"r")) == NULL)
	{
		return 0;
	} 

	//Reinicio de enemigos
	for (int k = 0; k < MAX_ENEMIGOS; k++)
	{
		slime[k].activa = 0;
		slime[k].ataquesEnemigos = 0;

		aranha[k].activa = 0;
	}

	Jefe.activa = 0;

	//Reinicio de balas
	for (int l = 0; l < MAX_ENEMIGOS; l++)
	{
		personaje.bala[l].activa = 0;
		Jefe.bala[l].activa = 0;
	}

	for (int i = 0; i < MAX_ENEMIGOS; i++)
	{
		for (int j = 0; j < MAX_BALAS; j++)
		{
			mago[i].activa = 0;
			mago[i].bala[j].activa = 0;
			mago[i].ataquesEnemigos = 0;
		}
	}

	//Reinicio objetos
	for (int i = 0; i < MAX_OBJETOS; i++)
	{
		monedas[i].activa = 0;
		llaves[i].activa = 0;
		cofres[i].activa = 0;
		municiones[i].activa = 0;
		mejoraDanho[i].activa = 0;
		mejoraRango[i].activa = 0;
		mejoraVelocidad[i].activa = 0;
		mejoraDanho[i].seVende = 0;
		mejoraRango[i].seVende = 0;
		mejoraVelocidad[i].seVende = 0;
	}

	mapaAzul.activa = 0;
	mapaRojo.activa = 0;
	mapaNaranjo.activa = 0;
	mapaVerde.activa = 0;

	//Reinicio Interactuables
	for (int i = 0; i < MAX_INTERACTUABLES; i++)
	{
		fogata[i].activa = 0;
		fogata[i].fogataActiva = 0;
	}

	fogataActual = 0;

	//Reinicio Dianas
	for (int i = 0; i < MAX_DIANAS; i++)
	{
		dianas[i].activa = 0;
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////

	for (int i = 0; i < FILAS_HABITACION; i++) 
	{
    	for (int j = 0; j < COLUMNAS_HABITACION; j++) 
		{
			//fscanf ignora los espacios y saltos de lineas 
        	fscanf(archivoMapa, " %c", &mapa[i][j]);

			//Ubicar posicion personaje
			if (mapa[i][j]=='@' && puertaDestino == '@')
			{
				jugador->posX = j * TAMANHO;
				jugador->posY = i * TAMANHO;
			}

			if (mapa[i][j] == '+')
			{
				productoTiendaGenerado = 0;
				
				for (int k = 0; k < MAX_REGISTROS; k++)
				{
					if (registroTienda[k].columna == j && registroTienda[k].fila == i && registroTienda[k].mapaX == actualMapaX && registroTienda[k].mapaY == actualMapaY)
					{
						productoTiendaGenerado = 1;

						if (registroTienda[k].idObjeto == 1 && registroTienda[k].comprado == 0)
						{
							mejoraDanho[mejoraDanhoActual].activa = 1;
							mejoraDanho[mejoraDanhoActual].posX = j * TAMANHO;
							mejoraDanho[mejoraDanhoActual].posY = i * TAMANHO - 30;
							mejoraDanho[mejoraDanhoActual].fila = i;
							mejoraDanho[mejoraDanhoActual].columna = j;
							mejoraDanho[mejoraDanhoActual].seVende = 1;
							mejoraDanho[mejoraDanhoActual].precio = 10;
							mejoraDanhoActual++;
						}
						else if (registroTienda[k].idObjeto == 2 && registroTienda[k].comprado == 0)
						{
							mejoraRango[mejoraRangoActual].activa = 1;
							mejoraRango[mejoraRangoActual].posX = j * TAMANHO;
							mejoraRango[mejoraRangoActual].posY = i * TAMANHO - 30;
							mejoraRango[mejoraRangoActual].fila = i;
							mejoraRango[mejoraRangoActual].columna = j;
							mejoraRango[mejoraRangoActual].seVende = 1;
							mejoraRango[mejoraRangoActual].precio = 15;
							mejoraRangoActual++;
						}
						else if (registroTienda[k].idObjeto == 3 && registroTienda[k].comprado == 0)
						{
							mejoraVelocidad[mejoraVelocidadActual].activa = 1;
							mejoraVelocidad[mejoraVelocidadActual].posX = j * TAMANHO;
							mejoraVelocidad[mejoraVelocidadActual].posY = i * TAMANHO - 30;
							mejoraVelocidad[mejoraVelocidadActual].fila = i;
							mejoraVelocidad[mejoraVelocidadActual].columna = j;
							mejoraVelocidad[mejoraVelocidadActual].seVende = 1;
							mejoraVelocidad[mejoraVelocidadActual].precio = 10;
							mejoraVelocidadActual++;
						}

						break;
					}
				}
				
				if (productoTiendaGenerado == 0)
				{
					auxRandTienda = rand() % 3 + 1;

					if (auxRandTienda == 1)
					{
						mejoraDanho[mejoraDanhoActual].activa = 1;
						mejoraDanho[mejoraDanhoActual].posX = j * TAMANHO;
						mejoraDanho[mejoraDanhoActual].posY = i * TAMANHO - 30;
						mejoraDanho[mejoraDanhoActual].seVende = 1;
						mejoraDanho[mejoraDanhoActual].precio = 10;
						mejoraDanho[mejoraDanhoActual].idRegistro = indiceTienda;
						mejoraDanhoActual++;

						registroTienda[indiceTienda].columna = j;
						registroTienda[indiceTienda].fila = i;
						registroTienda[indiceTienda].mapaX = actualMapaX;
						registroTienda[indiceTienda].mapaY = actualMapaY;
						registroTienda[indiceTienda].idObjeto = 1;
						registroTienda[indiceTienda].comprado = 0;
						indiceTienda++;
					}
					else if (auxRandTienda == 2)
					{
						mejoraRango[mejoraRangoActual].activa = 1;
						mejoraRango[mejoraRangoActual].posX = j * TAMANHO;
						mejoraRango[mejoraRangoActual].posY = i * TAMANHO - 30;
						mejoraRango[mejoraRangoActual].seVende = 1;
						mejoraRango[mejoraRangoActual].precio = 15;
						mejoraRango[mejoraRangoActual].idRegistro = indiceTienda;
						mejoraRangoActual++;

						registroTienda[indiceTienda].columna = j;
						registroTienda[indiceTienda].fila = i;
						registroTienda[indiceTienda].mapaX = actualMapaX;
						registroTienda[indiceTienda].mapaY = actualMapaY;
						registroTienda[indiceTienda].idObjeto = 2;
						registroTienda[indiceTienda].comprado = 0;
						indiceTienda++;
					}
					else if (auxRandTienda == 3)
					{
						mejoraVelocidad[mejoraVelocidadActual].activa = 1;
						mejoraVelocidad[mejoraVelocidadActual].posX = j * TAMANHO;
						mejoraVelocidad[mejoraVelocidadActual].posY = i * TAMANHO - 30;
						mejoraVelocidad[mejoraVelocidadActual].seVende = 1;
						mejoraVelocidad[mejoraVelocidadActual].precio = 10;
						mejoraVelocidad[mejoraVelocidadActual].idRegistro = indiceTienda;
						mejoraVelocidadActual++;

						registroTienda[indiceTienda].columna = j;
						registroTienda[indiceTienda].fila = i;
						registroTienda[indiceTienda].mapaX = actualMapaX;
						registroTienda[indiceTienda].mapaY = actualMapaY;
						registroTienda[indiceTienda].idObjeto = 3;
						registroTienda[indiceTienda].comprado = 0;
						indiceTienda++;
					}
				}
			}

			if (mapa[i][j] == 'T')
			{
				seGeneroUnaRecompensa++;
			}
			
			if (mapa[i][j] == 'f')
			{
				fogataEncendida = 0;

				for (int k = 0; k < MAX_REGISTROS; k++)
				{
					if (registroInteractuables[k].mapaX == actualMapaX && registroInteractuables[k].mapaY == actualMapaY && registroInteractuables[k].fogataEncendida == 1 && registroInteractuables[k].fila == i && registroInteractuables[k].columna == j)
					{
						fogataEncendida = 1;
						break;
					}
				}
				
				if (fogataEncendida == 0)
				{
					fogata[fogataActual].activa = 1;
					fogata[fogataActual].posX = j * TAMANHO;
					fogata[fogataActual].posY = i * TAMANHO;
					fogata[fogataActual].fila = i;
					fogata[fogataActual].columna = j;
					fogataActual++;
				}
				else
				{
					fogata[fogataActual].activa = 1;
					fogata[fogataActual].fogataActiva = 1;
					fogata[fogataActual].posX = j * TAMANHO;
					fogata[fogataActual].posY = i * TAMANHO;
					fogata[fogataActual].fila = i;
					fogata[fogataActual].columna = j;
					fogataActual++;	
				}

				if (fogataActual >= MAX_INTERACTUABLES)
				{
					fogataActual = 0;
				}
				
				seGeneroUnaRecompensa++;
			}

			//Cargar mapa azul
			if (mapa[i][j]=='a' && mapaAzul.seObtuvo == 0)
			{
				mapaAzul.activa = 1;
				mapaAzul.posX = j * TAMANHO;
				mapaAzul.posY = i * TAMANHO;
			}

			//Cargar mapa verde
			if (mapa[i][j]=='v')
			{
				mapaVerde.activa = 1;
				mapaVerde.posX = j * TAMANHO;
				mapaVerde.posY = i * TAMANHO;
			}

			//Cargar mapa rojo
			if (mapa[i][j]=='r')
			{
				mapaRojo.activa = 1;
				mapaRojo.posX = j * TAMANHO;
				mapaRojo.posY = i * TAMANHO;
			}

			//Cargar mapa naranjo
			if (mapa[i][j]=='n' && mapaNaranjo.seObtuvo == 0)
			{
				mapaNaranjo.activa = 1;
				mapaNaranjo.posX = j * TAMANHO;
				mapaNaranjo.posY = i * TAMANHO;
			}

			//Cargar municiones
			if(mapa[i][j]=='R')
			{
				cargadorObtenido = 0;

				for (int k = 0; k < MAX_REGISTROS; k++)
				{
					if (registroRecompensas[k].mapaX == actualMapaX && registroRecompensas[k].mapaY == mapaY && registroRecompensas[k].fila == i && registroRecompensas[k].columna == j && registroRecompensas[k].seObtuvoUnCargador == 1)
					{
						cargadorObtenido = 1;
						break;
					}
				}
				
				if (cargadorObtenido == 0)
				{
					municiones[municionesActual].activa = 1;
					municiones[municionesActual].posX = j * TAMANHO;
					municiones[municionesActual].posY = i * TAMANHO;
					municiones[municionesActual].fila = i;
					municiones[municionesActual].columna = j;
					municionesActual++;
				}
			}

			if (mapa[i][j] == 'D')
			{
				if (dianas[dianasActuales].destruida == 0)
				{
					dianas[dianasActuales].posX = j * TAMANHO;
					dianas[dianasActuales].posY = i * TAMANHO;
					dianas[dianasActuales].activa = 1;
					dianasActuales++;

					seGeneroUnaRecompensa++;
				}
			}

			if (dianasActuales >= MAX_DIANAS)
			{
				dianasActuales = 0;
			}

			//Cargar cofres
			if (mapa[i][j]=='C')
			{
				for (int i = 0; i < 1000; i++)
				{
					cofreAbierto = 0;

					if (registroRecompensas[i].mapaX == mapaX && registroRecompensas[i].mapaY == mapaY && registroRecompensas[i].seAbrioUnCofre != 0)
					{
						cofreAbierto = 1;
						break;
					}
				}
				
				if (cofreAbierto == 0)
				{
					cofres[cofreActual].activa = 1;
					cofres[cofreActual].posX = j * TAMANHO;
					cofres[cofreActual].posY = i * TAMANHO;
					cofreActual++;
				}
				
				seGeneroUnaRecompensa = 1;
			}

			if (mapa[i][j]=='*')
			{
				for (int k = 0; k < MAX_OBJETOS; k++)
				{
					monedas[k].posX = j * TAMANHO;
					monedas[k].posY = i * TAMANHO;

					llaves[k].posX = j * TAMANHO;
					llaves[k].posY = i * TAMANHO;

					for (int i = 0; i < 1000; i++)
					{
						if (registroRecompensas[i].mapaX == mapaX && registroRecompensas[i].mapaY == mapaY && registroRecompensas[i].seObtuvoUnaRecompensa != seGeneroUnaRecompensa)
						{
							seGeneroUnaRecompensa ++;
						}
					}
				}
			}

			if (mapa[i][j] == puertaDestino)
			{
				jugador->posX = j * TAMANHO;
				jugador->posY = i * TAMANHO;

				if (puertaDestino == 'N')
				{
					jugador->posY += TAMANHO; 	
				}

				if (puertaDestino == 'S')
				{
					jugador->posY -= TAMANHO;
				}

				if (puertaDestino == 'E')
				{
					jugador->posX -= TAMANHO; 	
				}

				if (puertaDestino == 'O')
				{
					jugador->posX += TAMANHO; 	
				}
				
			}
			
			//ubicar posicion enemigo
			if (mapa[i][j]=='s')
			{
				for (int m = 0; m < cantidadMuertos; m++)
				{
					muerto = 0;

					if (registroMuertes[m].mapaX == mapaX && registroMuertes[m].mapaY == mapaY && registroMuertes[m].fila == i && registroMuertes[m].columna == j)
					{
						muerto = 1;
						break;
					}
				}
				
				//Si el slime no esta muerto se genera
				if (muerto == 0)
				{
					if(slime[slimeActual].activa == 0)
					{
						//Generar slime en la posicion 's' del mapa
						slime[slimeActual].activa = 1;
						slime[slimeActual].posX = j * TAMANHO;
						slime[slimeActual].posY = i * TAMANHO;
						slime[slimeActual].vida = 4;

						//Registrar la ubicacion original del slime
						slime[slimeActual].posXGeneracion = j;
						slime[slimeActual].posYGeneracion = i;

						slimeActual ++;
					}
				}
			}

			if (slimeActual > MAX_ENEMIGOS - 1)
			{
				slimeActual = 0;
			}

			if (sala[i][j] == 'j')
			{

				for (int n = 0; n < cantidadMuertos; n++)
				{
					muerto = 0;

					if (registroMuertes[n].mapaX == mapaX && registroMuertes[n].mapaY == mapaY && registroMuertes[n].fila == i && registroMuertes[n].columna == j)
					{
						muerto = 1;
						break;
					}
				}
				
				if (muerto == 0)
				{
					if(Jefe.activa == 0)
					{
						Jefe.activa = 1;
						Jefe.posX = j * TAMANHO;
						Jefe.posY = i * TAMANHO;
						Jefe.vida = 20;
						
						Jefe.posXGeneracion = j;
						Jefe.posYGeneracion = i;
					}
				}
			}

			//cargar magos
			if (mapa[i][j]=='m')
			{
				for (int m = 0; m < cantidadMuertos; m++)
				{
					muerto = 0;

					if (registroMuertes[m].mapaX == mapaX && registroMuertes[m].mapaY == mapaY && registroMuertes[m].fila == i && registroMuertes[m].columna == j)
					{
						muerto = 1;
						break;
					}
				}
				
				if (muerto == 0)
				{
					if(mago[magoActual].activa == 0)
					{
						//Generar slime en la posicion 's' del mapa
						mago[magoActual].activa = 1;
						mago[magoActual].posX = j * TAMANHO;
						mago[magoActual].posY = i * TAMANHO;
						mago[magoActual].vida = 4;

						//Registrar la ubicacion original del slime
						mago[magoActual].posXGeneracion = j;
						mago[magoActual].posYGeneracion = i;

						magoActual ++;
					}
				}
			}

			if (magoActual > MAX_ENEMIGOS - 1)
			{
				magoActual = 0;
			}

			//cargar Arañas 
			if (mapa[i][j]=='A')
			{
				for (int m = 0; m < cantidadMuertos; m++)
				{
					muerto = 0;

					if (registroMuertes[m].mapaX == mapaX && registroMuertes[m].mapaY == mapaY && registroMuertes[m].fila == i && registroMuertes[m].columna == j)
					{
						muerto = 1;
						break;
					}
				}
				
				if (muerto == 0)
				{
					if(aranha[aranhaActual].activa == 0)
					{
						aranha[aranhaActual].activa = 1;
						aranha[aranhaActual].posX = j * TAMANHO;
						aranha[aranhaActual].posY = i * TAMANHO;
						aranha[aranhaActual].vida = 2;

						aranha[aranhaActual].posXGeneracion = j;
						aranha[aranhaActual].posYGeneracion = i;

						aranhaActual ++;
					}
				}
			}

			if (aranhaActual > MAX_ENEMIGOS - 1)
			{
				aranhaActual = 0;
			}
    	}
	}
}

void DibujarMapa(char mapa[FILAS_HABITACION][COLUMNAS_HABITACION], ALLEGRO_BITMAP *spriteSheet)
{
	//# arbol
	//T tienda de campaña
	//+ roca que genera objetos vendibles (solo en tienda)
	//E O N S puertas
	//f fogata
	//p pasto
	//L celda con llave
	//l celda
	//t tumba
	//k suelo de tumba
	//F linterna de fuego
	//v vallas
	//C techo piedra
	//P piedra 
	//o ventana
	//m mago enemigo
	//A aranha enemiga
	//s slime enemigo
	//a objetivo importante ya puesto
	//v objetivo importante ya puesto
	//n objetivo importante ya puesto
	//r objetivo importante ya puesto
	//* ubicacion de recompensa al limpiar sala

	for (int i = 0; i < FILAS_HABITACION; i++)
	{
		for (int j = 0; j < COLUMNAS_HABITACION; j++)
		{
			if (mapa[i][j] == '#')
			{
				al_draw_bitmap_region(spriteSheet, 5 * TAMANHO, 15 * TAMANHO, TAMANHO, TAMANHO, j * TAMANHO, i * TAMANHO, 0);
			}

			if (mapa[i][j] == 't')
			{
				al_draw_bitmap_region(spriteSheet, 8 * TAMANHO, 19 * TAMANHO, TAMANHO, TAMANHO, j * TAMANHO, i * TAMANHO, 0);
			}

			if (mapa[i][j] == 'o')
			{
				al_draw_bitmap_region(spriteSheet, 8 * TAMANHO, 18 * TAMANHO, TAMANHO, TAMANHO, j * TAMANHO, i * TAMANHO, 0);
			}

			if (mapa[i][j] == 'V')
			{
				al_draw_bitmap_region(spriteSheet, 3 * TAMANHO, 16 * TAMANHO, TAMANHO, TAMANHO, j * TAMANHO, i * TAMANHO, 0);
			}

			if (mapa[i][j] == 'k')
			{
				al_draw_bitmap_region(spriteSheet, 8 * TAMANHO, 20 * TAMANHO, TAMANHO, TAMANHO, j * TAMANHO, i * TAMANHO, 0);
			}

			if (mapa[i][j] == 'P')
			{
				al_draw_bitmap_region(spriteSheet, 4 * TAMANHO, 14 * TAMANHO, TAMANHO, TAMANHO, j * TAMANHO, i * TAMANHO, 0);
			}

			if (mapa[i][j] == 'c')
			{
				al_draw_bitmap_region(spriteSheet, 5 * TAMANHO, 14 * TAMANHO, TAMANHO, TAMANHO, j * TAMANHO, i * TAMANHO, 0);
			}

			if (mapa[i][j] == 'i')
			{
				al_draw_bitmap_region(spriteSheet, 4 * TAMANHO, 17 * TAMANHO, TAMANHO, TAMANHO, j * TAMANHO, i * TAMANHO, 0);
			}

			if (mapa[i][j] == 'T')
			{
				al_draw_bitmap_region(spriteSheet, 7 * TAMANHO, 15 * TAMANHO, TAMANHO, TAMANHO, j * TAMANHO, i * TAMANHO, 0);
			}

			if (mapa[i][j] == '+')
			{
				al_draw_bitmap_region(spriteSheet, 1 * TAMANHO, 15 * TAMANHO, TAMANHO, TAMANHO, j * TAMANHO, i * TAMANHO, 0);
			}

			if (mapa[i][j] == 'E' || mapa[i][j] == 'O' || mapa[i][j] == 'N' || mapa[i][j] == 'S')
			{
				al_draw_bitmap_region(spriteSheet, 4 * TAMANHO, 15 * TAMANHO, TAMANHO, TAMANHO, j * TAMANHO, i * TAMANHO, 0);
			}

			if (mapa[i][j] == 'F')
			{
				if ((controlSpritesMenu >= 0 && controlSpritesMenu <= 10) || (controlSprites >= 0 && controlSprites <= 10))
				{
					al_draw_bitmap_region(spriteSheet, 11 * TAMANHO, 16 * TAMANHO, TAMANHO, TAMANHO, j * TAMANHO, i * TAMANHO, 0);	
				}
				if ((controlSpritesMenu >= 10 && controlSpritesMenu <= 20) || (controlSprites >= 10 && controlSprites <= 20))
				{
					al_draw_bitmap_region(spriteSheet, 12 * TAMANHO, 16 * TAMANHO, TAMANHO, TAMANHO, j * TAMANHO, i * TAMANHO, 0);	
				}
				if ((controlSpritesMenu >= 20 && controlSpritesMenu <= 30) || (controlSprites >= 20 && controlSprites <= 30))
				{
					al_draw_bitmap_region(spriteSheet, 13 * TAMANHO, 16 * TAMANHO, TAMANHO, TAMANHO, j * TAMANHO, i * TAMANHO, 0);	
				}
				if ((controlSpritesMenu >= 30 && controlSpritesMenu <= 40) || (controlSprites >= 30 && controlSprites <= 40))
				{
					al_draw_bitmap_region(spriteSheet, 14 * TAMANHO, 16 * TAMANHO, TAMANHO, TAMANHO, j * TAMANHO, i * TAMANHO, 0);	
				}
			}

			if (controlSpritesMenu >= 40)
			{
				controlSpritesMenu = 0;
			}
			
			for (int k = 0; k < MAX_INTERACTUABLES; k++)
			{
				if (mapa[i][j] == 'f' && fogata[k].fogataActiva == 0)
				{
					al_draw_bitmap_region(spriteSheet, 10 * TAMANHO, 18 * TAMANHO, TAMANHO, TAMANHO, fogata[k].posX, fogata[k].posY, 0);
				}
				else if (mapa[i][j] == 'f' && fogata[k].fogataActiva != 0)
				{
					if (controlSprites >= 0 && controlSprites <= 10)
					{
						al_draw_bitmap_region(spriteSheet, 11 * TAMANHO, 18 * TAMANHO, TAMANHO, TAMANHO, fogata[k].posX, fogata[k].posY, 0);	
					}
					if (controlSprites >= 10 && controlSprites <= 20)
					{
						al_draw_bitmap_region(spriteSheet, 12 * TAMANHO, 18 * TAMANHO, TAMANHO, TAMANHO, fogata[k].posX, fogata[k].posY, 0);	
					}
					if (controlSprites >= 20 && controlSprites <= 30)
					{
						al_draw_bitmap_region(spriteSheet, 13 * TAMANHO, 18 * TAMANHO, TAMANHO, TAMANHO, fogata[k].posX, fogata[k].posY, 0);	
					}
					if (controlSprites >= 30 && controlSprites <= 40)
					{
						al_draw_bitmap_region(spriteSheet, 14 * TAMANHO, 18 * TAMANHO, TAMANHO, TAMANHO, fogata[k].posX, fogata[k].posY, 0);	
					}
				}
			}
			
			if (mapa[i][j] == 'p')
			{
				al_draw_bitmap_region(spriteSheet, 4 * TAMANHO, 15 * TAMANHO, TAMANHO, TAMANHO, j * TAMANHO, i * TAMANHO, 0);
			}

			if (mapa[i][j] == 'L')
			{
				al_draw_bitmap_region(spriteSheet, 10 * TAMANHO, 17 * TAMANHO, TAMANHO, TAMANHO, j * TAMANHO, i * TAMANHO, 0);
			}
			
			if (mapa[i][j] == 'l')
			{
				al_draw_bitmap_region(spriteSheet, 9 * TAMANHO, 17 * TAMANHO, TAMANHO, TAMANHO, j * TAMANHO, i * TAMANHO, 0);
			}
		}
		printf("\n");
	}
}

void Render(char mapa[FILAS_HABITACION][COLUMNAS_HABITACION], ALLEGRO_BITMAP *spriteSheet, ALLEGRO_BITMAP *spriteSheetBalas, ALLEGRO_BITMAP *spriteSheetCaminarCaballero, ALLEGRO_BITMAP *spriteSheetIcons, ALLEGRO_FONT *fuenteJuego, ALLEGRO_BITMAP *spriteSheetCrosshair, ALLEGRO_BITMAP *spriteSheetBalasEnemigos, ALLEGRO_BITMAP *spriteSheetIconsRaven)
{
	al_clear_to_color(al_map_rgb(0, 0, 0));
	//////////////////////////////////////////////// Dibujar en este espacio

	DibujarMapa(mapa, spriteSheet);

	//Jugador de SpriteSheet
	AnimacionPersonaje(spriteSheet, spriteSheetCaminarCaballero);

	//Monedas Jugador
	al_draw_bitmap_region(spriteSheetIcons, 3 * TAMANHO, 8 * TAMANHO, TAMANHO, TAMANHO, TAMANHO, TAMANHO + TAMANHO / 2, 0);
	al_draw_textf(fuenteJuego, al_map_rgb(192, 192, 192), TAMANHO + 80, TAMANHO + 50, 0, "= %d", personaje.cantidadMonedas);

	//Llaves jugador
	al_draw_bitmap_region(spriteSheetIcons, 6 * TAMANHO, 4 * TAMANHO, TAMANHO, TAMANHO, TAMANHO, TAMANHO + TAMANHO + 32, 0);
	al_draw_textf(fuenteJuego, al_map_rgb(192, 192, 192), TAMANHO + 80, TAMANHO + 120, 0, "= %d", personaje.cantidadLlaves);

	//Balas Jugador
	al_draw_scaled_bitmap(spriteSheetBalas, 2 * 16, 0 * 16, 16, 16, LARGO_PANTALLA - 350, ANCHO_PANTALLA - 200, TAMANHO * 3, TAMANHO * 3, 0); 
	al_draw_textf(fuenteJuego, al_map_rgb(192, 192, 192), LARGO_PANTALLA - 200, ANCHO_PANTALLA - 120, 0, "%d/%d", -personaje.cantidadDeBalas + MAX_BALAS, MAX_BALAS);

	//Puntaje Jugador
	al_draw_textf(fuenteJuego, al_map_rgb(192, 192, 192), TAMANHO * 23, TAMANHO, 0, "Puntaje: %d", personaje.puntaje);

	//Vidas jugador
	for (int j = 0; j < personaje.vidas; j++)
	{
		al_draw_bitmap_region(spriteSheetIcons, 4 * TAMANHO, 134 * TAMANHO, TAMANHO, TAMANHO, j * TAMANHO + TAMANHO, TAMANHO / 2, 0);
	}

	//Cantidad de fogatas necesarias
	if (actualMapaX == COLUMNAS_MAPA / 2 + 1 && actualMapaY == FILAS_MAPA / 2 + 1)
	{
		al_draw_bitmap_region(spriteSheet, 11 * TAMANHO, 18 * TAMANHO, TAMANHO, TAMANHO, TAMANHO * 2, TAMANHO * 12, 0);
		al_draw_textf(fuenteJuego, al_map_rgb(192, 192, 192), TAMANHO * 3 + 20, TAMANHO * 12 + 20, 0, "%d/4", cantidadfogatasActivas);
	}
	
	//Dibujo de balas
	for (int i = 0; i < MAX_BALAS; i++)
	{
		if (personaje.bala[i].activa != 0)
		{	
			al_draw_scaled_bitmap(spriteSheetBalas, 2 * 16, 0 * 16, 16, 16, personaje.bala[i].posX - 24, personaje.bala[i].posY - 24, TAMANHO, TAMANHO, 0); 
		}

		if (Jefe.bala[i].activa != 0)
		{
			al_draw_scaled_bitmap(spriteSheetBalasEnemigos, 2 * 16, 0 * 16, 16, 16, Jefe.bala[i].posX - 32, Jefe.bala[i].posY - 32, TAMANHO, TAMANHO, 0); 
		}

		for (int j = 0; j < MAX_ENEMIGOS; j++)
		{
			if (mago[j].bala[i].activa != 0)
			{
				al_draw_scaled_bitmap(spriteSheetBalasEnemigos, 2 * 16, 0 * 16, 16, 16, mago[j].bala[i].posX, mago[j].bala[i].posY, TAMANHO, TAMANHO, 0); 
			}		
		}
	}

	//Dibujo de dianas
	for (int i = 0; i < MAX_DIANAS; i++)
	{
		if (dianas[i].activa != 0)
		{
			if (controlSprites >= 0 && controlSprites <= 10)
			{
				al_draw_bitmap_region(spriteSheet, 11 * TAMANHO, 17 * TAMANHO, TAMANHO, TAMANHO, dianas[i].posX, dianas[i].posY, 0);	
			}
			if (controlSprites >= 10 && controlSprites <= 20)
			{
				al_draw_bitmap_region(spriteSheet, 12 * TAMANHO, 17 * TAMANHO, TAMANHO, TAMANHO, dianas[i].posX, dianas[i].posY, 0);	
			}
			if (controlSprites >= 20 && controlSprites <= 30)
			{
				al_draw_bitmap_region(spriteSheet, 13 * TAMANHO, 17 * TAMANHO, TAMANHO, TAMANHO, dianas[i].posX, dianas[i].posY, 0);	
			}
			if (controlSprites >= 30 && controlSprites <= 40)
			{
				al_draw_bitmap_region(spriteSheet, 14 * TAMANHO, 17 * TAMANHO, TAMANHO, TAMANHO, dianas[i].posX, dianas[i].posY, 0);
			}
		}
	}
	
	//Dibujo de objetos
	for (int i = 0; i < MAX_OBJETOS; i++)
	{
		//llaves
		if (llaves[i].activa != 0)
		{
			al_draw_bitmap_region(spriteSheetIcons, 6 * TAMANHO, 4 * TAMANHO, TAMANHO, TAMANHO, llaves[i].posX, llaves[i].posY, 0);
		}

		//monedas
		if (monedas[i].activa != 0 && monedas[i].especial != 0)
		{
			al_draw_bitmap_region(spriteSheetIcons, 2 * TAMANHO, 8 * TAMANHO, TAMANHO, TAMANHO, monedas[i].posX, monedas[i].posY, 0);
		}
		else if (monedas[i].activa != 0)
		{
			al_draw_bitmap_region(spriteSheetIcons, 3 * TAMANHO, 8 * TAMANHO, TAMANHO, TAMANHO, monedas[i].posX, monedas[i].posY, 0);
		}	

		//cofre
		if (cofres[i].activa != 0 && cofres[i].cofreAbierto == 1)
		{
			al_draw_bitmap_region(spriteSheet, 12 * TAMANHO, 15 * TAMANHO, TAMANHO, TAMANHO, cofres[i].posX, cofres[i].posY, 0);
		}
		else if (cofres[i].activa != 0)
		{
			al_draw_bitmap_region(spriteSheet, 11 * TAMANHO, 15 * TAMANHO, TAMANHO, TAMANHO, cofres[i].posX, cofres[i].posY, 0);
		}

		//Municiones
		if (municiones[i].activa != 0)
		{
			al_draw_bitmap_region(spriteSheetIcons, 7 * TAMANHO, 42 * TAMANHO, TAMANHO, TAMANHO, municiones[i].posX, municiones[i].posY, 0);
		}
		
		//vidas
		if (vidasObjeto[i].activa != 0)
		{
			al_draw_bitmap_region(spriteSheetIcons, 4 * TAMANHO, 134 * TAMANHO, TAMANHO, TAMANHO, vidasObjeto[i].posX, vidasObjeto[i].posY, 0);
		}

		//mejora de daño
		if (mejoraDanho[i].activa != 0)
		{
			al_draw_bitmap_region(spriteSheetIcons, 0 * TAMANHO, 83 * TAMANHO, TAMANHO, TAMANHO, mejoraDanho[i].posX, mejoraDanho[i].posY, 0);
		}

		//mejora de rango
		if (mejoraRango[i].activa != 0)
		{
			al_draw_bitmap_region(spriteSheetIcons, 2 * TAMANHO, 83 * TAMANHO, TAMANHO, TAMANHO, mejoraRango[i].posX, mejoraRango[i].posY, 0);
		}
		
		//mejora de velocidad
		if (mejoraVelocidad[i].activa != 0)
		{
			al_draw_bitmap_region(spriteSheetIcons, 1 * TAMANHO, 85 * TAMANHO, TAMANHO, TAMANHO, mejoraVelocidad[i].posX, mejoraVelocidad[i].posY, 0);
		}
	}

	if (mapaAzul.activa != 0)
	{
		al_draw_bitmap_region(spriteSheetIcons, 9 * TAMANHO, 18 * TAMANHO, TAMANHO, TAMANHO, mapaAzul.posX, mapaAzul.posY, 0);
	}

	if (mapaRojo.activa != 0)
	{
		al_draw_bitmap_region(spriteSheetIcons, 8 * TAMANHO, 18 * TAMANHO, TAMANHO, TAMANHO, mapaRojo.posX, mapaRojo.posY, 0);
	}

	if (mapaNaranjo.activa != 0)
	{
		al_draw_bitmap_region(spriteSheetIcons, 12 * TAMANHO, 18 * TAMANHO, TAMANHO, TAMANHO, mapaNaranjo.posX, mapaNaranjo.posY, 0);
	}

	if (mapaVerde.activa != 0)
	{
		al_draw_bitmap_region(spriteSheetIcons, 10 * TAMANHO, 18 * TAMANHO, TAMANHO, TAMANHO, mapaVerde.posX, mapaVerde.posY, 0);
	}

	//Dibujo objetos de tienda
	for (int i = 0; i < MAX_OBJETOS; i++)
	{
		if (mejoraDanho[i].activa == 1 && mejoraDanho[i].seVende == 1)
		{
			al_draw_bitmap_region(spriteSheetIcons, 0 * TAMANHO, 83 * TAMANHO, TAMANHO, TAMANHO, mejoraDanho[i].posX, mejoraDanho[i].posY, 0);
			al_draw_textf(fuenteJuego, al_map_rgb(192, 192, 192), mejoraDanho[i].posX, mejoraDanho[i].posY + 100, 0, "%d", mejoraDanho[i].precio);
		}

		if (mejoraRango[i].activa == 1 && mejoraRango[i].seVende == 1)
		{
			al_draw_bitmap_region(spriteSheetIcons, 2 * TAMANHO, 83 * TAMANHO, TAMANHO, TAMANHO, mejoraRango[i].posX, mejoraRango[i].posY, 0);
			al_draw_textf(fuenteJuego, al_map_rgb(192, 192, 192), mejoraRango[i].posX, mejoraRango[i].posY + 100, 0, "%d", mejoraRango[i].precio);
		}

		if (mejoraVelocidad[i].activa == 1 && mejoraVelocidad[i].seVende == 1)
		{
			al_draw_bitmap_region(spriteSheetIcons, 1 * TAMANHO, 85 * TAMANHO, TAMANHO, TAMANHO, mejoraVelocidad[i].posX, mejoraVelocidad[i].posY, 0);
			al_draw_textf(fuenteJuego, al_map_rgb(192, 192, 192), mejoraVelocidad[i].posX, mejoraVelocidad[i].posY + 100, 0, "%d", mejoraVelocidad[i].precio);
		}
	}
	
	//Dibujo enemigos
	AnimacionEnemigos(spriteSheet);
	
	//Puntero del mouse
	al_draw_scaled_bitmap(spriteSheetCrosshair, 10 * 16, 3 * 16, 16, 16, mouse.posX - 30, mouse.posY - 32, TAMANHO, TAMANHO, 0);

	////////////////////////////////////////////////
	al_flip_display();
}

bool ColisionMapa(char mapa[FILAS_HABITACION][COLUMNAS_HABITACION], int jugadorPosXProximo, int jugadorPosYProximo)
{
	int columna = 0;
	int fila = 0;

	if (jugadorPosXProximo < 0 || jugadorPosYProximo < 0)
    {
        return false; 
    }

	//Convertimos la posicion del jugador en indice del arreglo del mapa
	columna = jugadorPosXProximo / TAMANHO;
	fila = jugadorPosYProximo / TAMANHO;

	//Validamos que no nos salimos del arreglo
	if(fila >= 0 && fila < FILAS_HABITACION && columna >= 0 && columna < COLUMNAS_HABITACION)
	{
		if(mapa[fila][columna] == '#')
		{
			return true;
		}

		if(mapa[fila][columna] == 'l')
		{
			return true;
		}

		if(mapa[fila][columna] == 'P')
		{
			return true;
		}

		if(mapa[fila][columna] == 'V')
		{
			return true;
		}

		if(mapa[fila][columna] == 'c')
		{
			return true;
		}
	}

	return false;
}

bool Colicion(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
	return ( (x1 < x2 + w2) && (x2 < x1 + w1) && (y1 < y2 + h2) && (y2 < y1 + h1));
}

void InitAllegro()
{
	//Inicializar dibujos
	al_init_primitives_addon();

	//Inicializar imagenes
	al_init_image_addon();

	//Inicializar Allegro
	if(!al_init()) printf("ERROR ALLEGRO");

	//Inicializar teclado
	if(!al_install_keyboard()) printf("ERROR TECLADO");

	//Inicializar mouse
	if(!al_install_mouse()) printf("ERROR MOUSE");

	//Inicializar fuentes de texto
	al_init_font_addon();

	al_init_ttf_addon();
}

int InitGameComponents(ALLEGRO_DISPLAY *ventana, mouse_ *mouse, ranking_ *ranking)
{
	//Inicializar ventana
	al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
	ventana = al_create_display(640, 480);
	if(!ventana) return -1;

	ranking->indiceNombres = 0;
	ranking->indicePuntajes = 0;

	//Inicializando Mouse
	mouse->posX = 0;
	mouse->posY = 0;
	mouse->tamanho = 7;

	//Inicializando jugador
	personaje.velocidad = 16; ////////// 7
	personaje.movimientoJugador = 0;
	personaje.dirJugador.derecha = 0;
	personaje.dirJugador.izquierda = 1;
	personaje.vidas = 6;
	personaje.invulnerable = 0;
	personaje.cantidadMonedas = 200; ///////// 0
	personaje.cantidadLlaves = 2; //////////////// originalmente 0
	personaje.rangoDeBalas = 400;
	personaje.cantidadDeBalas = 0;
	personaje.puntaje = 0;

	//Inicializando enemigos
	for (int i = 0; i < MAX_ENEMIGOS; i++)
	{
		slime[i].posX = 0;
		slime[i].posY = 0;
		slime[i].velocidad = 3;
		slime[i].activa = 0;
		slime[i].tipo = 0;
		slime[i].direccion = 1;
		slime[i].vida = 4;
		slime[i].posXGeneracion = 0;
		slime[i].posYGeneracion = 0;
		slime[i].ataquesEnemigos = 0;

		mago[i].posX = 0;
		mago[i].posY = 0;
		mago[i].velocidad = 3;
		mago[i].activa = 0;
		mago[i].tipo = 0;
		mago[i].direccion = 0;
		mago[i].vida = 4;
		mago[i].posXGeneracion = 0;
		mago[i].posYGeneracion = 0;
		mago[i].ataquesEnemigos = 0;
		mago[i].rangoDeBalas = 450;

		aranha[i].posX = 0;
		aranha[i].posY = 0;
		aranha[i].velocidad = 5;
		aranha[i].activa = 0;
		aranha[i].tipo = 0;
		aranha[i].direccion = 1;
		aranha[i].vida = 2;
		aranha[i].posXGeneracion = 0;
		aranha[i].posYGeneracion = 0;
		aranha[i].ataquesEnemigos = 0;
		aranha[i].chocoConPared = 0;
		aranha[i].auxRandAranha = 0;
	}

	Jefe.posX = 0;
	Jefe.posY = 0;
	Jefe.velocidad = 4;
	Jefe.activa = 0;
	Jefe.tipo = 0;
	Jefe.direccion = 0;
	Jefe.vida = 20;
	Jefe.posXGeneracion = 0;
	Jefe.posYGeneracion = 0;
	Jefe.ataquesEnemigos = 0;
	Jefe.rangoDeBalas = 500;

	//Inicializando balas
	for (int i = 0; i < MAX_BALAS; i++)
	{
		personaje.bala[i].posX = 0;
		personaje.bala[i].posY = 0;
		personaje.bala[i].velocidad = 10;		
		personaje.bala[i].activa = 0;
		personaje.bala[i].danho = 3;               ///// originalmente 1
		personaje.bala[i].anguloBalaX = 0;
		personaje.bala[i].anguloBalaY = 0;

		Jefe.bala[i].posX = 0;
		Jefe.bala[i].posY = 0;
		Jefe.bala[i].velocidad = 10;
		Jefe.bala[i].activa = 0;
		Jefe.bala[i].danho = 1;
		Jefe.bala[i].anguloBalaX = 0;
		Jefe.bala[i].anguloBalaY = 0;

		for (int m = 0; m < MAX_ENEMIGOS; m++)
		{
			mago[m].bala[i].posX = 0;
			mago[m].bala[i].posY = 0;
			mago[m].bala[i].velocidad = 10;
			mago[m].bala[i].activa = 0;
			mago[m].bala[i].danho = 1;
			mago[m].bala[i].anguloBalaX = 0;
			mago[m].bala[i].anguloBalaY = 0;
		}
	}

	//Inicializar objetos
	for (int i = 0; i < MAX_OBJETOS; i++)
	{
		monedas[i].posX = 0;
		monedas[i].posY = 0;
		monedas[i].activa = 0;
		monedas[i].especial = 0;

		mejoraDanho[i].activa = 0;

		mejoraRango[i].activa = 0;

		mejoraVelocidad[i].activa = 0;
	}

	mapaVerde.activa = 0;
	mapaVerde.columna = 0;
	mapaVerde.fila = 0;
	mapaVerde.posX = 0;
	mapaVerde.posY = 0;
	mapaVerde.seObtuvo = 0;
	mapaVerde.especial = 0;

	mapaAzul.activa = 0;
	mapaAzul.columna = 0;
	mapaAzul.fila = 0;
	mapaAzul.posX = 0;
	mapaAzul.posY = 0;
	mapaAzul.seObtuvo = 0;
	mapaAzul.especial = 0;

	mapaNaranjo.activa = 0;
	mapaNaranjo.columna = 0;
	mapaNaranjo.fila = 0;
	mapaNaranjo.posX = 0;
	mapaNaranjo.posY = 0;
	mapaNaranjo.seObtuvo = 0;
	mapaNaranjo.especial = 0;

	mapaRojo.activa = 0;
	mapaRojo.columna = 0;
	mapaRojo.fila = 0;
	mapaRojo.posX = 0;
	mapaRojo.posY = 0;
	mapaRojo.seObtuvo = 0;
	mapaRojo.especial = 0;

	//Inicializar mapa
	for (int i = 0; i < FILAS_MAPA; i++)
	{
		for (int j = 0; i < COLUMNAS_MAPA; i++)
		{
			mapa[i][j] = NULL;
		}
	}

	//Interactuables inicializacion
	for (int i = 0; i < MAX_INTERACTUABLES; i++)
	{
		fogata[i].activa = 0;
		fogata[i].posX = 0;
		fogata[i].posY = 0;
		fogata[i].fogataActiva = 0;
		fogata[i].columna = 0;
		fogata[i].fila = 0;
	}

	cantidadfogatasActivas = 0;
	fogataActual = 0;

	//Dianas inicializacion
	for (int i = 0; i < MAX_DIANAS; i++)
	{
		dianas[i].activa = 0;
		dianas[i].velocidad = 10;
		dianas[i].chocoConPared = 0;
		dianas[i].auxRanDianas = 0;
		dianas[i].destruida = 0;
	}

	//Inicializacion registros
	for (int i = 0; i < MAX_REGISTROS; i++)
	{
		registroMuertes[i].columna = 0;
		registroMuertes[i].fila = 0;
		registroMuertes[i].mapaX = 0;
		registroMuertes[i].mapaY = 0;
		
		registroInteractuables[i].columna = 0;
		registroInteractuables[i].fila = 0;
		registroInteractuables[i].fogataEncendida = 0;
		registroInteractuables[i].mapaX = 0;
		registroInteractuables[i].mapaY = 0;

		registroRecompensas[i].columna = 0;
		registroRecompensas[i].fila = 0;
		registroRecompensas[i].mapaX = 0;
		registroRecompensas[i].mapaY = 0;
		registroRecompensas[i].seAbrioUnCofre = 0;
		registroRecompensas[i].seObtuvoUnaRecompensa = 0;
		registroRecompensas[i].seObtuvoUnCargador = 0;
	}
}

void InputHandle(estadoJuego_ *estadoJuego, controlMenu_ *controlMenu, ranking_ *ranking, ALLEGRO_EVENT_QUEUE *colaEventos)
{
	al_get_keyboard_state(&estado); //Llena la estructura con el estado actual del teclado
	al_get_mouse_state(&estadoMouse);

	mouse.posX = estadoMouse.x;
	mouse.posY = estadoMouse.y;

	ALLEGRO_EVENT evento;

	//Apagar programa con ESC
	if (al_key_down(&estado, ALLEGRO_KEY_ESCAPE))
	{
		estadoJuego->JUEGO = 0;
		estadoJuego->MENU = 0;
		estadoJuego->REINICIAR = 0;
		estadoJuego->SISTEMA = 0;
	}

	//Input de la pantallaRanking al poner nombre
	while (al_get_next_event(colaEventos, &evento))
	{
		if (estadoJuego->MENU == 1 && controlMenu->pantallaRanking == 1)
		{
			if (evento.type == ALLEGRO_EVENT_KEY_CHAR)
			{
				controlMenu->unichar = evento.keyboard.unichar;

				if (al_key_down(&estado, ALLEGRO_KEY_ENTER))
				{
					estadoJuego->MENU = 0;
					controlMenu->pantallaRanking = 0;
					estadoJuego->JUEGO = 1;
				}
				else if (al_key_down(&estado, ALLEGRO_KEY_BACKSPACE) && controlMenu->longitudNombre > 0)
				{
					controlMenu->longitudNombre--;
					ranking->nombre[controlMenu->longitudNombre] = '\0';
				}
				else if (controlMenu->unichar > 32 && controlMenu->unichar <= 126 && controlMenu->longitudNombre < LARGO_TEXTO - 1)
				{
					ranking->nombre[controlMenu->longitudNombre] = (char)controlMenu->unichar;
					controlMenu->longitudNombre++;
					ranking->nombre[controlMenu->longitudNombre] = '\0';
				}
			}	
		}
	}
}

void MovimientoCamara()
{
	ALLEGRO_TRANSFORM camara;

	//Poner en objetivo el objeto a mover
	al_identity_transform(&camara);

	//Hacer los movimientos
	//Movimiento pantalla en eje x
	int camaraLimiteX = 1920;
	int camaraLimiteY = 1088;

	if (personaje.posX < camaraLimiteX * 0.40) //Izquierda
	{
		al_translate_transform(&camara, camaraLimiteX * 0.40 - personaje.posX, 0);
	}
	else if (personaje.posX > camaraLimiteX * 0.60) //Derecha
	{
		al_translate_transform(&camara, camaraLimiteX * 0.60 - personaje.posX, 0);
	}
	
	//Movimiento pantalla en eje y
	if (personaje.posY > ANCHO_PANTALLA * 0.60) //Arriba
	{
		al_translate_transform(&camara, 0, ANCHO_PANTALLA * 0.60 - personaje.posY);
	}
	else if (personaje.posY < ANCHO_PANTALLA * 0.40) //Abajo
	{
		al_translate_transform(&camara, 0, ANCHO_PANTALLA * 0.40 - personaje.posY);
	}

	//Usar los movimientos
	al_use_transform(&camara);
}

void AnimacionPersonaje(ALLEGRO_BITMAP *spriteSheet, ALLEGRO_BITMAP *spriteSheetCaminarCaballero)
{
	controlSprites += 1;

	printf("%d", personaje.movimientoJugador);
	
	if (personaje.invulnerable == 0)
	{
		//Personaje caminando hacia la derecha
		if (personaje.dirJugador.derecha != 0 && personaje.movimientoJugador != 0)
		{
			if (controlSprites > 0 && controlSprites <= 10)
			{
				al_draw_bitmap_region(spriteSheetCaminarCaballero, 0 * TAMANHO, 23 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, ALLEGRO_FLIP_HORIZONTAL);	
			}
			if (controlSprites > 10 && controlSprites <= 20)
			{
				al_draw_bitmap_region(spriteSheetCaminarCaballero, 1 * TAMANHO, 23 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, ALLEGRO_FLIP_HORIZONTAL);	
			}
			if (controlSprites > 20 && controlSprites <= 30)
			{
				al_draw_bitmap_region(spriteSheetCaminarCaballero, 2 * TAMANHO, 23 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, ALLEGRO_FLIP_HORIZONTAL);	
			}
			if (controlSprites > 30 && controlSprites <= 40)
			{
				al_draw_bitmap_region(spriteSheetCaminarCaballero, 3 * TAMANHO, 23 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, ALLEGRO_FLIP_HORIZONTAL);	
			}
		}
		else if (personaje.dirJugador.derecha != 0)
		{
			if (controlSprites > 0 && controlSprites <= 10)
			{
				al_draw_bitmap_region(spriteSheet, 0 * TAMANHO, 23 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, ALLEGRO_FLIP_HORIZONTAL);	
			}
			if (controlSprites > 10 && controlSprites <= 20)
			{
				al_draw_bitmap_region(spriteSheet, 1 * TAMANHO, 23 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, ALLEGRO_FLIP_HORIZONTAL);	
			}
			if (controlSprites > 20 && controlSprites <= 30)
			{
				al_draw_bitmap_region(spriteSheet, 2 * TAMANHO, 23 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, ALLEGRO_FLIP_HORIZONTAL);	
			}
			if (controlSprites > 30 && controlSprites <= 40)
			{
				al_draw_bitmap_region(spriteSheet, 3 * TAMANHO, 23 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, ALLEGRO_FLIP_HORIZONTAL);	
			}
		}

		//Personaje caminando hacia la izquierda
		if (personaje.dirJugador.izquierda != 0 && personaje.movimientoJugador != 0)
		{
			if (controlSprites > 0 && controlSprites <= 10)
			{
				al_draw_bitmap_region(spriteSheetCaminarCaballero, 0 * TAMANHO, 23 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, 0);	
			}
			if (controlSprites > 10 && controlSprites <= 20)
			{
				al_draw_bitmap_region(spriteSheetCaminarCaballero, 1 * TAMANHO, 23 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, 0);	
			}
			if (controlSprites > 20 && controlSprites <= 30)
			{
				al_draw_bitmap_region(spriteSheetCaminarCaballero, 2 * TAMANHO, 23 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, 0);	
			}
			if (controlSprites > 30 && controlSprites <= 40)
			{
				al_draw_bitmap_region(spriteSheetCaminarCaballero, 3 * TAMANHO, 23 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, 0);	
			}
		}
		else if (personaje.dirJugador.izquierda != 0)
		{
			if (controlSprites > 0 && controlSprites <= 10)
			{
				al_draw_bitmap_region(spriteSheet, 0 * TAMANHO, 23 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, 0);	
			}
			if (controlSprites > 10 && controlSprites <= 20)
			{
				al_draw_bitmap_region(spriteSheet, 1 * TAMANHO, 23 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, 0);	
			}
			if (controlSprites > 20 && controlSprites <= 30)
			{
				al_draw_bitmap_region(spriteSheet, 2 * TAMANHO, 23 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, 0);	
			}
			if (controlSprites > 30 && controlSprites <= 40)
			{
				al_draw_bitmap_region(spriteSheet, 3 * TAMANHO, 23 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, 0);	
			}
		}
	} 
	else if (PersonajeInvulnerable > 0)
	{
		//Personaje caminando hacia la derecha
		if (personaje.dirJugador.derecha != 0 && personaje.movimientoJugador != 0)
		{
			if (controlSprites > 0 && controlSprites < 10)
			{
				al_draw_bitmap_region(spriteSheetCaminarCaballero, 0 * TAMANHO, 23 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, ALLEGRO_FLIP_HORIZONTAL);	
			}
			if (controlSprites > 10 && controlSprites < 20)
			{
				al_draw_bitmap_region(spriteSheetCaminarCaballero, 1 * TAMANHO, 23 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, ALLEGRO_FLIP_HORIZONTAL);	
			}
			if (controlSprites > 20 && controlSprites < 30)
			{
				al_draw_bitmap_region(spriteSheetCaminarCaballero, 2 * TAMANHO, 23 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, ALLEGRO_FLIP_HORIZONTAL);	
			}
			if (controlSprites > 30 && controlSprites < 40)
			{
				al_draw_bitmap_region(spriteSheetCaminarCaballero, 3 * TAMANHO, 23 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, ALLEGRO_FLIP_HORIZONTAL);	
			}
		}
		else if (personaje.dirJugador.derecha != 0)
		{
			if (controlSprites > 0 && controlSprites < 10)
			{
				al_draw_bitmap_region(spriteSheet, 0 * TAMANHO, 23 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, ALLEGRO_FLIP_HORIZONTAL);	
			}
			if (controlSprites > 10 && controlSprites < 20)
			{
				al_draw_bitmap_region(spriteSheet, 1 * TAMANHO, 23 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, ALLEGRO_FLIP_HORIZONTAL);	
			}
			if (controlSprites > 20 && controlSprites < 30)
			{
				al_draw_bitmap_region(spriteSheet, 2 * TAMANHO, 23 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, ALLEGRO_FLIP_HORIZONTAL);	
			}
			if (controlSprites > 30 && controlSprites < 40)
			{
				al_draw_bitmap_region(spriteSheet, 3 * TAMANHO, 23 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, ALLEGRO_FLIP_HORIZONTAL);	
			}
		}

		//Personaje caminando hacia la izquierda
		if (personaje.dirJugador.izquierda != 0 && personaje.movimientoJugador != 0)
		{
			if (controlSprites > 0 && controlSprites < 10)
			{
				al_draw_bitmap_region(spriteSheetCaminarCaballero, 0 * TAMANHO, 23 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, 0);	
			}
			if (controlSprites > 10 && controlSprites < 20)
			{
				al_draw_bitmap_region(spriteSheetCaminarCaballero, 1 * TAMANHO, 23 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, 0);	
			}
			if (controlSprites > 20 && controlSprites < 30)
			{
				al_draw_bitmap_region(spriteSheetCaminarCaballero, 2 * TAMANHO, 23 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, 0);	
			}
			if (controlSprites > 30 && controlSprites < 40)
			{
				al_draw_bitmap_region(spriteSheetCaminarCaballero, 3 * TAMANHO, 23 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, 0);	
			}
		}
		else if (personaje.dirJugador.izquierda != 0)
		{
			if (controlSprites > 0 && controlSprites < 10)
			{
				al_draw_bitmap_region(spriteSheet, 0 * TAMANHO, 23 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, 0);	
			}
			if (controlSprites > 10 && controlSprites < 20)
			{
				al_draw_bitmap_region(spriteSheet, 1 * TAMANHO, 23 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, 0);	
			}
			if (controlSprites > 20 && controlSprites < 30)
			{
				al_draw_bitmap_region(spriteSheet, 2 * TAMANHO, 23 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, 0);	
			}
			if (controlSprites > 30 && controlSprites < 40)
			{
				al_draw_bitmap_region(spriteSheet, 3 * TAMANHO, 23 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, 0);	
			}
		}
	}
	
	if(controlSprites >= 40)
	{
		controlSprites = 0;
	}

	personaje.movimientoJugador = 0;
}

void AnimacionEnemigos(ALLEGRO_BITMAP *spriteSheet)
{
	for (int i = 0; i < MAX_ENEMIGOS; i++)
	{
		if (slime[i].activa != 0)
		{
			//Animacion en dir izquierda
			if(slime[i].direccion == 2)
			{
				if (controlSprites >= 0 && controlSprites <= 10)
				{
					al_draw_bitmap_region(spriteSheet, 0 * TAMANHO, 22 * TAMANHO, TAMANHO, TAMANHO, slime[i].posX, slime[i].posY, 0);
				}
				if (controlSprites > 10 && controlSprites <= 20)
				{
					al_draw_bitmap_region(spriteSheet, 1 * TAMANHO, 22 * TAMANHO, TAMANHO, TAMANHO, slime[i].posX, slime[i].posY, 0);
				}
				if (controlSprites > 20 && controlSprites <= 30)
				{
					al_draw_bitmap_region(spriteSheet, 2 * TAMANHO, 22 * TAMANHO, TAMANHO, TAMANHO, slime[i].posX, slime[i].posY, 0);
				}
				if (controlSprites > 30 && controlSprites <= 40)
				{
					al_draw_bitmap_region(spriteSheet, 3 * TAMANHO, 22 * TAMANHO, TAMANHO, TAMANHO, slime[i].posX, slime[i].posY, 0);
				}
			}

			//Animacion en dir derecha
			if(slime[i].direccion == 1)
			{
				if (controlSprites >= 0 && controlSprites <= 10)
				{
					al_draw_bitmap_region(spriteSheet, 0 * TAMANHO, 22 * TAMANHO, TAMANHO, TAMANHO, slime[i].posX, slime[i].posY, ALLEGRO_FLIP_HORIZONTAL);
				}
				if (controlSprites > 10 && controlSprites <= 20)
				{
					al_draw_bitmap_region(spriteSheet, 1 * TAMANHO, 22 * TAMANHO, TAMANHO, TAMANHO, slime[i].posX, slime[i].posY, ALLEGRO_FLIP_HORIZONTAL);
				}
				if (controlSprites > 20 && controlSprites <= 30)
				{
					al_draw_bitmap_region(spriteSheet, 2 * TAMANHO, 22 * TAMANHO, TAMANHO, TAMANHO, slime[i].posX, slime[i].posY, ALLEGRO_FLIP_HORIZONTAL);
				}
				if (controlSprites > 30 && controlSprites <= 40)
				{
					al_draw_bitmap_region(spriteSheet, 3 * TAMANHO, 22 * TAMANHO, TAMANHO, TAMANHO, slime[i].posX, slime[i].posY, ALLEGRO_FLIP_HORIZONTAL);
				}
			}
		}

		if (mago[i].activa != 0)
		{
			if (controlSprites >= 0 && controlSprites <= 10)
			{
				al_draw_bitmap_region(spriteSheet, 4 * TAMANHO, 23 * TAMANHO, TAMANHO, TAMANHO, mago[i].posX, mago[i].posY, 0);
			}
			if (controlSprites > 10 && controlSprites <= 20)
			{
				al_draw_bitmap_region(spriteSheet, 5 * TAMANHO, 23 * TAMANHO, TAMANHO, TAMANHO, mago[i].posX, mago[i].posY, 0);
			}
			if (controlSprites > 20 && controlSprites <= 30)
			{
				al_draw_bitmap_region(spriteSheet, 6 * TAMANHO, 23 * TAMANHO, TAMANHO, TAMANHO, mago[i].posX, mago[i].posY, 0);
			}
			if (controlSprites > 30 && controlSprites <= 40)
			{
				al_draw_bitmap_region(spriteSheet, 7 * TAMANHO, 23 * TAMANHO, TAMANHO, TAMANHO, mago[i].posX, mago[i].posY, 0);
			}
		}

		if (aranha[i].activa != 0)
		{
			if (controlSprites >= 0 && controlSprites <= 10)
			{
				al_draw_bitmap_region(spriteSheet, 8 * TAMANHO, 27 * TAMANHO, TAMANHO, TAMANHO, aranha[i].posX, aranha[i].posY, 0);
			}
			if (controlSprites > 10 && controlSprites <= 20)
			{
				al_draw_bitmap_region(spriteSheet, 9 * TAMANHO, 27 * TAMANHO, TAMANHO, TAMANHO, aranha[i].posX, aranha[i].posY, 0);
			}
			if (controlSprites > 20 && controlSprites <= 30)
			{
				al_draw_bitmap_region(spriteSheet, 10 * TAMANHO, 27 * TAMANHO, TAMANHO, TAMANHO, aranha[i].posX, aranha[i].posY, 0);
			}
			if (controlSprites > 30 && controlSprites <= 40)
			{
				al_draw_bitmap_region(spriteSheet, 11 * TAMANHO, 27 * TAMANHO, TAMANHO, TAMANHO, aranha[i].posX, aranha[i].posY, 0);
			}
		}
	}

	if (Jefe.activa != 0)
	{
		if (controlSprites >= 0 && controlSprites <= 10)
		{
			al_draw_scaled_bitmap(spriteSheet, 4 * TAMANHO, 24 * TAMANHO, TAMANHO, TAMANHO, Jefe.posX, Jefe.posY, TAMANHO * 2, TAMANHO * 2, 0); 
		}
		if (controlSprites > 10 && controlSprites <= 20)
		{
			al_draw_scaled_bitmap(spriteSheet, 5 * TAMANHO, 24 * TAMANHO, TAMANHO, TAMANHO, Jefe.posX, Jefe.posY, TAMANHO * 2, TAMANHO * 2, 0); 
		}
		if (controlSprites > 20 && controlSprites <= 30)
		{
			al_draw_scaled_bitmap(spriteSheet, 6 * TAMANHO, 24 * TAMANHO, TAMANHO, TAMANHO, Jefe.posX, Jefe.posY, TAMANHO * 2, TAMANHO * 2, 0); 
		}
		if (controlSprites > 30 && controlSprites <= 40)
		{
			al_draw_scaled_bitmap(spriteSheet, 7 * TAMANHO, 24 * TAMANHO, TAMANHO, TAMANHO, Jefe.posX, Jefe.posY, TAMANHO * 2, TAMANHO * 2, 0); 
		}		
	}
}

void LogicaEnemigos()
{
	//Colicion slime y pared
	int auxXSlime = 0;
	int auxYSlime = 0;
	int auxXAranha = 0;
	int auxYAranha = 0;

	ColisionEnemigos();

	DisparoEnemigos();

	LogicaJefe();

	RangoVisionEnemigo();

	//Araña eligiendo que eje moverse
	for (int i = 0; i < MAX_ENEMIGOS; i++)
	{
		auxXAranha = aranha[i].posX;
		auxYAranha = aranha[i].posY;

		if (aranha[i].auxRandAranha == 0)
		{
			aranha[i].auxRandAranha = rand() % 2 + 1;
		}
		
		if (aranha[i].auxRandAranha == 1)
		{
			if (aranha[i].activa != 0 && aranha[i].chocoConPared%2 == 0)
			{
				aranha[i].posX += aranha[i].velocidad;
			} 
			else if (aranha[i].activa != 0 && aranha[i].chocoConPared%2 != 0)
			{
				aranha[i].posX -= aranha[i].velocidad;
			}

			if (ColisionMapa(sala, aranha[i].posX, aranha[i].posY) || 
				ColisionMapa(sala, aranha[i].posX + TAMANHO - 1, aranha[i].posY) || 
				ColisionMapa(sala, aranha[i].posX + TAMANHO - 1, aranha[i].posY + TAMANHO - 1) ||
				ColisionMapa(sala, aranha[i].posX, aranha[i].posY + TAMANHO - 1))
			{
				aranha[i].posX = auxXAranha;
				aranha[i].chocoConPared++;
			}
		}
		
		if (aranha[i].auxRandAranha == 2)
		{
			if (aranha[i].activa != 0 && aranha[i].chocoConPared%2 == 0)
			{
				aranha[i].posY += aranha[i].velocidad;
			} 
			else if (aranha[i].activa != 0 && aranha[i].chocoConPared%2 != 0)
			{
				aranha[i].posY -= aranha[i].velocidad;
			}

			if (ColisionMapa(sala, aranha[i].posX, aranha[i].posY) || 
				ColisionMapa(sala, aranha[i].posX + TAMANHO - 1, aranha[i].posY) || 
				ColisionMapa(sala, aranha[i].posX + TAMANHO - 1, aranha[i].posY + TAMANHO - 1) ||
				ColisionMapa(sala, aranha[i].posX, aranha[i].posY + TAMANHO - 1))
			{
				aranha[i].posY = auxYAranha;
				aranha[i].chocoConPared++;
			}
		}
	}

	//Slime persiguiendo al jugador
	for (int i = 0; i < MAX_ENEMIGOS; i++)
	{		
		if (slime[i].activa != 0 && slime[i].ataquesEnemigos == 1)
		{
			auxXSlime = slime[i].posX;
			auxYSlime = slime[i].posY;

			if (personaje.posY > slime[i].posY)
			{
				slime[i].posY += slime[i].velocidad;
			}

			if (personaje.posY < slime[i].posY)
			{
				slime[i].posY -= slime[i].velocidad;
			}

			if (ColisionMapa(sala, slime[i].posX, slime[i].posY) || 
			ColisionMapa(sala, slime[i].posX + TAMANHO - 1, slime[i].posY) || 
			ColisionMapa(sala, slime[i].posX + TAMANHO - 1, slime[i].posY + TAMANHO - 1) ||
			ColisionMapa(sala, slime[i].posX, slime[i].posY + TAMANHO - 1))
			{
				slime[i].posY = auxYSlime;
			}

			if (personaje.posX < slime[i].posX)
			{
				slime[i].posX -= slime[i].velocidad;
				slime[i].direccion = 2;
			}

			if (personaje.posX > slime[i].posX)
			{
				slime[i].posX += slime[i].velocidad;
				slime[i].direccion = 1;
			}

			if (ColisionMapa(sala, slime[i].posX, slime[i].posY) || 
			ColisionMapa(sala, slime[i].posX + TAMANHO - 1, slime[i].posY) || 
			ColisionMapa(sala, slime[i].posX + TAMANHO - 1, slime[i].posY + TAMANHO - 1) ||
			ColisionMapa(sala, slime[i].posX, slime[i].posY + TAMANHO - 1))
			{
				slime[i].posX = auxXSlime;
			}
		}
	}
}

void RangoVisionEnemigo()
{
	int rangoVision = 12 * TAMANHO;

	int offSet = (rangoVision - TAMANHO) / 2;

	for (int i = 0; i < MAX_ENEMIGOS; i++)
	{
		if (Colicion(personaje.posX, personaje.posY, TAMANHO, TAMANHO, slime[i].posX - offSet, slime[i].posY - offSet, rangoVision, rangoVision))
		{
			slime[i].ataquesEnemigos = 1;
		}

		if (Colicion(personaje.posX, personaje.posY, TAMANHO, TAMANHO, mago[i].posX - offSet, mago[i].posY - offSet, rangoVision, rangoVision))
		{
			mago[i].ataquesEnemigos = 1;
		}
	}
}

void LogicaJefe()
{
	int auxXJefe = 0;
	int auxYJefe = 0;

	timerMovimientoJefe ++;

	if (Jefe.activa != 0 && timerMovimientoJefe < 60)
	{
		auxXJefe = Jefe.posX;
		auxYJefe = Jefe.posY;

		if (personaje.posY > Jefe.posY)
		{
			Jefe.posY += Jefe.velocidad;
		}

		if (personaje.posY < Jefe.posY)
		{
			Jefe.posY -= Jefe.velocidad;
		}

		if (ColisionMapa(sala, Jefe.posX, Jefe.posY) || 
		ColisionMapa(sala, Jefe.posX + TAMANHO * 2 - 1, Jefe.posY) || 
		ColisionMapa(sala, Jefe.posX + TAMANHO * 2 - 1, Jefe.posY + TAMANHO * 2 - 1) ||
		ColisionMapa(sala, Jefe.posX, Jefe.posY + TAMANHO * 2 - 1))
		{
			Jefe.posY = auxYJefe;
		}

		if (personaje.posX < Jefe.posX)
		{
			Jefe.posX -= Jefe.velocidad;
			Jefe.direccion = 2;
		}

		if (personaje.posX > Jefe.posX)
		{
			Jefe.posX += Jefe.velocidad;
			Jefe.direccion = 1;
		}

		if (ColisionMapa(sala, Jefe.posX, Jefe.posY) || 
		ColisionMapa(sala, Jefe.posX + TAMANHO * 2 - 1, Jefe.posY) || 
		ColisionMapa(sala, Jefe.posX + TAMANHO * 2 - 1, Jefe.posY + TAMANHO * 2 - 1) ||
		ColisionMapa(sala, Jefe.posX, Jefe.posY + TAMANHO * 2 - 1))
		{
			Jefe.posX = auxXJefe;
		}
	}

	if (timerMovimientoJefe >= 120)
	{
		timerMovimientoJefe = 0;
	}
	
}

void ColisionEnemigos()
{
	/////////////////////////////////////////////////////////////////////// Colisiones arañas y jugador
	for (int i = 0; i < MAX_ENEMIGOS; i++)
	{
		if (aranha[i].activa != 0)
		{
			//Colision aranha y bala
			for (int j = 0; j < MAX_BALAS; j++)
			{
				if (Colicion(aranha[i].posX, aranha[i].posY, TAMANHO, TAMANHO, personaje.bala[j].posX, personaje.bala[j].posY, TAMANHO / 4, TAMANHO / 4))
				{
					if(personaje.bala[j].activa != 0)
					{
						aranha[i].vida -= personaje.bala[j].danho;
						personaje.bala[j].activa = 0;
					}

					if(aranha[i].vida <= 0 && aranha[i].activa == 1)
					{
						aranha[i].activa = 0;
						personaje.puntaje = personaje.puntaje + 10;

						// Aparicion monedas al morir un enemigo
						monedas[monedasActual].posX = aranha[i].posX;
						monedas[monedasActual].posY = aranha[i].posY;
						monedas[monedasActual].activa = 1;
						monedasActual ++;


						if (cantidadMuertos < 1000)
						{
							//Cuando muere un aranha se registra el mapa donde murio en una posicion del arreglo
							strcpy(registroMuertes[cantidadMuertos].registroMapa, mapa[actualMapaY][actualMapaX]);

							//Se registra su lugar de aparicion original
							registroMuertes[cantidadMuertos].fila = aranha[i].posYGeneracion;
							registroMuertes[cantidadMuertos].columna = aranha[i].posXGeneracion;

							//Registramos la parte del mapa donde murieron
							registroMuertes[cantidadMuertos].mapaX = actualMapaX;
							registroMuertes[cantidadMuertos].mapaY = actualMapaY;

							cantidadMuertos ++;
						}
					}
				}
			}

			//Colision aranha y jugador
			if (Colicion(aranha[i].posX, aranha[i].posY, TAMANHO, TAMANHO, personaje.posX, personaje.posY, TAMANHO, TAMANHO) && personaje.invulnerable == 0)
			{
				personaje.vidas --;
				personaje.invulnerable = 1;
			}
		}
	}


	/////////////////////////////////////////////////////////////////////// Colisiones slimes y jugador
	for (int i = 0; i < MAX_ENEMIGOS; i++)
	{
		if (slime[i].activa != 0)
		{
			//Colision slime y bala
			for (int j = 0; j < MAX_BALAS; j++)
			{
				if (Colicion(slime[i].posX, slime[i].posY, TAMANHO, TAMANHO, personaje.bala[j].posX, personaje.bala[j].posY, TAMANHO / 4, TAMANHO / 4))
				{
					if(personaje.bala[j].activa != 0)
					{
						slime[i].vida -= personaje.bala[j].danho;
						personaje.bala[j].activa = 0;
					}

					if(slime[i].vida <= 0 && slime[i].activa == 1)
					{
						slime[i].activa = 0;
						personaje.puntaje = personaje.puntaje + 10;

						// Aparicion monedas al morir un enemigo
						monedas[monedasActual].posX = slime[i].posX;
						monedas[monedasActual].posY = slime[i].posY;
						monedas[monedasActual].activa = 1;
						monedasActual ++;


						if (cantidadMuertos < 1000)
						{
							//Cuando muere un slime se registra el mapa donde murio en una posicion del arreglo
							strcpy(registroMuertes[cantidadMuertos].registroMapa, mapa[actualMapaY][actualMapaX]);

							//Se registra su lugar de aparicion original
							registroMuertes[cantidadMuertos].fila = slime[i].posYGeneracion;
							registroMuertes[cantidadMuertos].columna = slime[i].posXGeneracion;

							//Registramos la parte del mapa donde murieron
							registroMuertes[cantidadMuertos].mapaX = actualMapaX;
							registroMuertes[cantidadMuertos].mapaY = actualMapaY;

							cantidadMuertos ++;
						}
					}
				}
			}

			//Colision slime y jugador
			if (Colicion(slime[i].posX, slime[i].posY, TAMANHO, TAMANHO, personaje.posX, personaje.posY, TAMANHO, TAMANHO) && personaje.invulnerable == 0)
			{
				personaje.vidas --;
				personaje.invulnerable = 1;
			}
		}
	}

	//Coliciones entre slimes
	for (int k = 0; k < MAX_ENEMIGOS; k++)
    {
        if (slime[k].activa != 0) 
		{
			for (int l = 0; l < MAX_ENEMIGOS; l++)
			{
				if (slime[l].activa != 0) 
				{
					if (Colicion(slime[k].posX, slime[k].posY, TAMANHO, TAMANHO, slime[l].posX, slime[l].posY, TAMANHO, TAMANHO) && k != l)
					{
						slime[k].posXAnterior = slime[k].posX;
						slime[k].posYAnterior = slime[k].posY;
						slime[l].posXAnterior = slime[l].posX;
						slime[l].posYAnterior = slime[l].posY;

						//enfocandonos en el primer slime
						if (slime[k].posX > slime[l].posX)
						{
							slime[l].posX -= slime[l].velocidad;
						}
						else if (slime[k].posX < slime[l].posX)
						{
							slime[l].posX += slime[l].velocidad;
						}

						if (slime[k].posY > slime[l].posY)
						{
							slime[l].posY -= slime[l].velocidad;
						}
						else if (slime[k].posY < slime[l].posY)
						{
							slime[l].posY += slime[l].velocidad;
						}

						//Enfocandonos en el segundo
						//enfocandonos en el primer slime
						if (slime[l].posX > slime[k].posX)
						{
							slime[k].posX -= slime[k].velocidad;
						}
						else if (slime[l].posX < slime[k].posX)
						{
							slime[k].posX += slime[k].velocidad;
						}

						if (slime[l].posY > slime[k].posY)
						{
							slime[k].posY -= slime[k].velocidad;
						}
						else if (slime[l].posY < slime[k].posY)
						{
							slime[k].posY += slime[k].velocidad;
						}

						if (ColisionMapa(sala, slime[k].posX, slime[k].posY) ||
							ColisionMapa(sala, slime[k].posX + TAMANHO - 1, slime[k].posY) ||
							ColisionMapa(sala, slime[k].posX + TAMANHO - 1, slime[k].posY + TAMANHO - 1) ||
							ColisionMapa(sala, slime[k].posX, slime[k].posY + TAMANHO - 1))
						{
							slime[k].posX = slime[k].posXAnterior;
							slime[k].posY = slime[k].posYAnterior;
						}

						if (ColisionMapa(sala, slime[l].posX, slime[l].posY) ||
							ColisionMapa(sala, slime[l].posX + TAMANHO - 1, slime[l].posY) ||
							ColisionMapa(sala, slime[l].posX + TAMANHO - 1, slime[l].posY + TAMANHO - 1) ||
							ColisionMapa(sala, slime[l].posX, slime[l].posY + TAMANHO - 1))
						{
							slime[l].posX = slime[l].posXAnterior;
							slime[l].posY = slime[l].posYAnterior;
						}
					}
				}		
			}
		}
    }

	///////////////////////////////////////////////////////////////// COlisiones con el mago y jugador

	for (int i = 0; i < MAX_ENEMIGOS; i++)
	{
		if (mago[i].activa != 0)
		{
			for (int j = 0; j < MAX_BALAS; j++)
			{
				//Colicion balas personaje y mago
				if (Colicion(mago[i].posX, mago[i].posY, TAMANHO, TAMANHO, personaje.bala[j].posX, personaje.bala[j].posY, TAMANHO / 4, TAMANHO / 4))
				{
					if(personaje.bala[j].activa != 0)
					{
						mago[i].vida -= personaje.bala[j].danho;
						personaje.bala[j].activa = 0;
					}

					if(mago[i].vida <= 0 && mago[i].activa == 1)
					{
						mago[i].activa = 0;
						personaje.puntaje = personaje.puntaje + 10;

						monedas[monedasActual].posX = mago[i].posX;
						monedas[monedasActual].posY = mago[i].posY;
						monedas[monedasActual].activa = 1;
						monedasActual ++;

						if (cantidadMuertos < 1000)
						{
							//Cuando muere un slime se registra el mapa donde murio en una posicion del arreglo
							strcpy(registroMuertes[cantidadMuertos].registroMapa, mapa[actualMapaY][actualMapaX]);

							//Se registra su lugar de aparicion original
							registroMuertes[cantidadMuertos].fila = mago[i].posYGeneracion;
							registroMuertes[cantidadMuertos].columna = mago[i].posXGeneracion;

							//Registramos la parte del mapa donde murieron
							registroMuertes[cantidadMuertos].mapaX = actualMapaX;
							registroMuertes[cantidadMuertos].mapaY = actualMapaY;

							cantidadMuertos ++;
						}
					}
				}

				//Colicion balas de mago y personaje
				if (Colicion(mago[i].bala[j].posX, mago[i].bala[j].posY, TAMANHO / 4, TAMANHO / 4, personaje.posX, personaje.posY, TAMANHO, TAMANHO) && personaje.invulnerable == 0)
				{
					if (mago[i].bala[j].activa != 0)
					{
						personaje.vidas --;
						mago[i].bala[j].activa = 0;
						personaje.invulnerable = 1;
					}
				}
			}

			//Colision mago y jugador
			if (Colicion(mago[i].posX, mago[i].posY, TAMANHO, TAMANHO, personaje.posX, personaje.posY, TAMANHO, TAMANHO) && personaje.invulnerable == 0)
			{
				personaje.vidas --;
				personaje.invulnerable = 1;
			}
		}
	}
	

	///////////////////////////////////////////////////////////////// Colisiones con el jefe y jugador
	if (Jefe.activa != 0)
	{
		for (int k = 0; k < MAX_BALAS; k++)
		{
			if (Colicion(Jefe.posX, Jefe.posY, TAMANHO * 2, TAMANHO * 2, personaje.bala[k].posX, personaje.bala[k].posY, TAMANHO / 4, TAMANHO / 4))
			{
				if(personaje.bala[k].activa != 0)
				{
					Jefe.vida -= personaje.bala[k].danho;
					personaje.bala[k].activa = 0;
				}

				if(Jefe.vida <= 0 && Jefe.activa == 1)
				{
					Jefe.activa = 0;
					personaje.puntaje = personaje.puntaje + 100;

					monedas[monedasActual].posX = Jefe.posX;
					monedas[monedasActual].posY = Jefe.posY;
					monedas[monedasActual].especial = 1;
					monedas[monedasActual].activa = 1;
					monedasActual ++;

					mapaRojo.activa = 1;
					mapaRojo.posX = Jefe.posX;
					mapaRojo.posY = Jefe.posY;

					if (cantidadMuertos < 1000)
					{
						//Cuando muere un slime se registra el mapa donde murio en una posicion del arreglo
						strcpy(registroMuertes[cantidadMuertos].registroMapa, mapa[actualMapaY][actualMapaX]);

						//Se registra su lugar de aparicion original
						registroMuertes[cantidadMuertos].fila = Jefe.posYGeneracion;
						registroMuertes[cantidadMuertos].columna = Jefe.posXGeneracion;

						//Registramos la parte del mapa donde murieron
						registroMuertes[cantidadMuertos].mapaX = actualMapaX;
						registroMuertes[cantidadMuertos].mapaY = actualMapaY;

						cantidadMuertos ++;
					}
				}
			}

			//Colision jugador y balas jefe
			if (Colicion(Jefe.bala[k].posX, Jefe.bala[k].posY, TAMANHO / 4, TAMANHO / 4, personaje.posX, personaje.posY, TAMANHO, TAMANHO) && personaje.invulnerable == 0)
			{
				personaje.vidas --;
				personaje.invulnerable = 1;
			}
		}

		//Colision jefe y jugador
		if (Colicion(Jefe.posX, Jefe.posY, TAMANHO * 2, TAMANHO * 2, personaje.posX, personaje.posY, TAMANHO, TAMANHO) && personaje.invulnerable == 0)
		{
			personaje.vidas --;
			personaje.invulnerable = 1;
		}
	}
}

void ColicionObjetos()
{
	int auxRand = 0;

	if (mapaRojo.activa != 0)
	{
		if (Colicion(personaje.posX, personaje.posY, TAMANHO, TAMANHO, mapaRojo.posX, mapaRojo.posY, TAMANHO, TAMANHO))
		{
			mapaRojo.activa = 0;	
			mapaRojo.seObtuvo = 1;
		}
	}

	if (mapaAzul.activa != 0)
	{
		if (Colicion(personaje.posX, personaje.posY, TAMANHO, TAMANHO, mapaAzul.posX, mapaAzul.posY, TAMANHO, TAMANHO))
		{
			mapaAzul.activa = 0;	
			mapaAzul.seObtuvo = 1;
		}
	}

	if (mapaVerde.activa != 0)
	{
		if (Colicion(personaje.posX, personaje.posY, TAMANHO, TAMANHO, mapaVerde.posX, mapaVerde.posY, TAMANHO, TAMANHO))
		{
			mapaVerde.activa = 0;	
			mapaVerde.seObtuvo = 1;
		}
	}

	if (mapaNaranjo.activa != 0)
	{
		if (Colicion(personaje.posX, personaje.posY, TAMANHO, TAMANHO, mapaNaranjo.posX, mapaNaranjo.posY, TAMANHO, TAMANHO))
		{
			mapaNaranjo.activa = 0;	
			mapaNaranjo.seObtuvo = 1;
		}
	}
	

	for (int i = 0; i < MAX_OBJETOS; i++)
	{
		//Colicion con monedas
		if (monedas[i].activa != 0)
		{
			if (Colicion(personaje.posX, personaje.posY, TAMANHO, TAMANHO, monedas[i].posX, monedas[i].posY, TAMANHO, TAMANHO))
			{
				if (monedas[i].especial != 0)
				{
					monedas[i].activa = 0;
					personaje.cantidadMonedas += 10;
				}
				else
				{
					monedas[i].activa = 0;
					personaje.cantidadMonedas ++;
				}
			}
		}

		//Colicion con llaves
		if (llaves[i].activa != 0)
		{
			if (Colicion(personaje.posX, personaje.posY, TAMANHO, TAMANHO, llaves[i].posX, llaves[i].posY, TAMANHO, TAMANHO))
			{
				llaves[i].activa = 0;
				personaje.cantidadLlaves ++;
			}
		}

		//Colicion con municiones
		if (municiones[i].activa != 0)
		{
			if (Colicion(personaje.posX, personaje.posY, TAMANHO, TAMANHO, municiones[i].posX, municiones[i].posY, TAMANHO, TAMANHO))
			{
				personaje.cantidadDeBalas = 0;
				municiones[i].activa = 0;

				registroRecompensas[cantidadRecompensas].mapaX = actualMapaX;
				registroRecompensas[cantidadRecompensas].mapaY = actualMapaY;
				registroRecompensas[cantidadRecompensas].fila = municiones[i].fila;
				registroRecompensas[cantidadRecompensas].columna = municiones[i].columna;
				registroRecompensas[cantidadRecompensas].seObtuvoUnCargador = 1;
				cantidadRecompensas;
			}
		}

		//Colicion con vidas
		if (vidasObjeto[i].activa != 0)
		{
			if (Colicion(personaje.posX, personaje.posY, TAMANHO, TAMANHO, vidasObjeto[i].posX, vidasObjeto[i].posY, TAMANHO, TAMANHO))
			{
				personaje.vidas++;
				vidasObjeto[i].activa = 0;
			}
		}

		//Colicion con mejora de Danho
		if (mejoraDanho[i].activa != 0 && mejoraDanho[i].seVende == 0)
		{
			if (Colicion(personaje.posX, personaje.posY, TAMANHO, TAMANHO, mejoraDanho[i].posX, mejoraDanho[i].posY, TAMANHO, TAMANHO))
			{
				for (int k = 0; k < MAX_BALAS; k++)
				{
					personaje.bala[k].danho++;
				}

				mejoraDanho[i].activa = 0;
			}
		}
		else if (mejoraDanho[i].activa != 0 && mejoraDanho[i].seVende == 1 && personaje.cantidadMonedas >= mejoraDanho[i].precio)
		{
			if (Colicion(personaje.posX, personaje.posY, TAMANHO, TAMANHO, mejoraDanho[i].posX, mejoraDanho[i].posY, TAMANHO, TAMANHO))
			{
				for (int i = 0; i < MAX_BALAS; i++)
				{
					personaje.bala[i].danho++;
				}

				mejoraDanho[i].activa = 0;
				personaje.cantidadMonedas -= mejoraDanho[i].precio;
				
				//Comprobar ubicacion de la mejora
				registroTienda[mejoraDanho[i].idRegistro].comprado = 1;

				for (int k = 0; k < indiceTienda; k++)
				{
					if (registroTienda[k].columna == mejoraDanho[i].columna && registroTienda[k].fila == mejoraDanho[i].fila && registroTienda[k].mapaX == actualMapaX && registroTienda[k].mapaY == actualMapaY)
					{
						registroTienda[k].comprado = 1; 
						break; 
					}
				}
			}
		}

		//Colicion con mejora de rango
		if (mejoraRango[i].activa != 0 && mejoraRango[i].seVende == 0)
		{
			if (Colicion(personaje.posX, personaje.posY, TAMANHO, TAMANHO, mejoraRango[i].posX, mejoraRango[i].posY, TAMANHO, TAMANHO))
			{
				personaje.rangoDeBalas += 100;
				mejoraRango[i].activa = 0;
			}
		}
		else if (mejoraRango[i].activa != 0 && mejoraRango[i].seVende == 1 && personaje.cantidadMonedas >= mejoraRango[i].precio)
		{
			if (Colicion(personaje.posX, personaje.posY, TAMANHO, TAMANHO, mejoraRango[i].posX, mejoraRango[i].posY, TAMANHO, TAMANHO))
			{
				personaje.rangoDeBalas += 100;
				mejoraRango[i].activa = 0;
				personaje.cantidadMonedas -= mejoraRango[i].precio;
				
				//Comprobar ubicacion de la mejora
				registroTienda[mejoraRango[i].idRegistro].comprado = 1;

				for (int k = 0; k < indiceTienda; k++)
				{
					if (registroTienda[k].columna == mejoraRango[i].columna && registroTienda[k].fila == mejoraRango[i].fila && registroTienda[k].mapaX == actualMapaX && registroTienda[k].mapaY == actualMapaY)
					{
						registroTienda[k].comprado = 1; 
						break; 
					}
				}
			}
		}

		//Colicion con mejora de velocidad
		if (mejoraVelocidad[i].activa != 0 && mejoraVelocidad[i].seVende == 0)
		{
			if (Colicion(personaje.posX, personaje.posY, TAMANHO, TAMANHO, mejoraVelocidad[i].posX, mejoraVelocidad[i].posY, TAMANHO, TAMANHO))
			{
				personaje.velocidad += 2;
				mejoraVelocidad[i].activa = 0;
			}
		}
		else if (mejoraVelocidad[i].activa != 0 && mejoraVelocidad[i].seVende == 1 && personaje.cantidadMonedas >= mejoraVelocidad[i].precio)
		{
			if (Colicion(personaje.posX, personaje.posY, TAMANHO, TAMANHO, mejoraVelocidad[i].posX, mejoraVelocidad[i].posY, TAMANHO, TAMANHO))
			{
				personaje.velocidad += 2;
				mejoraVelocidad[i].activa = 0;
				personaje.cantidadMonedas -= mejoraVelocidad[i].precio;
				
				//Comprobar ubicacion de la mejora
				registroTienda[mejoraVelocidad[i].idRegistro].comprado = 1;

				for (int k = 0; k < indiceTienda; k++)
				{
					if (registroTienda[k].columna == mejoraVelocidad[i].columna && registroTienda[k].fila == mejoraVelocidad[i].fila && registroTienda[k].mapaX == actualMapaX && registroTienda[k].mapaY == actualMapaY)
					{
						registroTienda[k].comprado = 1; 
						break; 
					}
				}
			}
		}
		
		//Colicion con cofres
		if (cofres[i].activa != 0)
		{
			if (Colicion(personaje.posX, personaje.posY, TAMANHO, TAMANHO, cofres[i].posX, cofres[i].posY, TAMANHO, TAMANHO) && personaje.cantidadLlaves > 0 && cofres[i].cofreAbierto == 0)
			{
				cofres[i].cofreAbierto = 1;
				personaje.cantidadLlaves--;
				registroRecompensas[cantidadRecompensas].mapaX = actualMapaX;
				registroRecompensas[cantidadRecompensas].mapaY = actualMapaY;
				registroRecompensas[cantidadRecompensas].seAbrioUnCofre = 1;
				cantidadRecompensas++;

				//Siempre dará de recompensa una recarga
				municiones[municionesActual].activa = 1;
				municiones[municionesActual].posX = cofres[i].posX + TAMANHO * 3;
				municiones[municionesActual].posY = cofres[i].posY;
				municionesActual++;

				//Siempre dará un corazon
				vidasObjeto[vidasObjetoActual].activa = 1;
				vidasObjeto[vidasObjetoActual].posX = cofres[i].posX - TAMANHO * 3;
				vidasObjeto[vidasObjetoActual].posY = cofres[i].posY;
				vidasObjetoActual++;

				//Siempre dará una moneda
				monedas[monedasActual].activa = 1;
				monedas[monedasActual].posX = cofres[i].posX;
				monedas[monedasActual].posY = cofres[i].posY + TAMANHO * 3;
				monedasActual++;

				//Pull de objetos que tienen probabilidad de salir
				auxRand = rand() % 3 + 1;

				//mejora de daño
				if (auxRand == 1)
				{
					for (int j = 0; j < MAX_OBJETOS; j++)
					{
						if (mejoraDanho[j].activa == 0)
						{
							mejoraDanho[j].activa = 1;
							mejoraDanho[j].posX = cofres[i].posX;
							mejoraDanho[j].posY = cofres[i].posY - TAMANHO * 3;
							break;
						}
					}
				}

				if (auxRand == 2)
				{
					for (int j = 0; j < MAX_OBJETOS; j++)
					{
						if (mejoraRango[j].activa == 0)
						{
							mejoraRango[j].activa = 1;
							mejoraRango[j].posX = cofres[i].posX;
							mejoraRango[j].posY = cofres[i].posY - TAMANHO * 3;
							break;
						}
					}
				}

				if (auxRand == 3)
				{
					for (int j = 0; j < MAX_OBJETOS; j++)
					{
						if (mejoraVelocidad[j].activa == 0)
						{
							mejoraVelocidad[j].activa = 1;
							mejoraVelocidad[j].posX = cofres[i].posX;
							mejoraVelocidad[j].posY = cofres[i].posY - TAMANHO * 3;
							break;
						}
					}
				}
			}
		}
	}
}

void ColicionInteractuables()
{
	for (int i = 0; i < MAX_INTERACTUABLES; i++)
	{
		if (fogata[i].activa != 0  && fogata[i].fogataActiva == 0)
		{
			if (Colicion(personaje.posX, personaje.posY, TAMANHO, TAMANHO, fogata[i].posX, fogata[i].posY, TAMANHO, TAMANHO))
			{
				fogata[i].fogataActiva = 1;
				cantidadfogatasActivas++;

				registroInteractuables[cantidadInteractuables].mapaX = actualMapaX;
				registroInteractuables[cantidadInteractuables].mapaY = actualMapaY;
				registroInteractuables[cantidadInteractuables].fila = fogata[i].fila;
				registroInteractuables[cantidadInteractuables].columna = fogata[i].columna;
				registroInteractuables[cantidadInteractuables].fogataEncendida = 1;
				cantidadInteractuables++;

				break;
			}
		}
	}

	for (int i = 0; i < MAX_DIANAS; i++)
	{
		if (dianas[i].activa != 0)
		{
			for (int j = 0; j < MAX_BALAS; j++)
			{
				if (personaje.bala[j].activa != 0)
				{
					if (Colicion(personaje.bala[j].posX, personaje.bala[j].posY, TAMANHO/4, TAMANHO/4, dianas[i].posX, dianas[i].posY, TAMANHO, TAMANHO))
					{
						personaje.bala[j].activa = 0;
						dianas[i].activa = 0;
						dianas[i].destruida = 1;
					}
				}
			}
		}
	}
}

void LogicaDianas()
{
	int auxXDiana = 0;
	int auxYDiana = 0;
	int auxRand = 0;

	for (int i = 0; i < MAX_DIANAS; i++)
	{
		auxXDiana = dianas[i].posX;
		auxYDiana = dianas[i].posY;

		if (dianas[i].auxRanDianas == 0)
		{
			dianas[i].auxRanDianas = rand() % 2 + 1;
		}
		
		if (dianas[i].auxRanDianas == 1)
		{
			if (dianas[i].activa != 0 && dianas[i].chocoConPared%2 == 0)
			{
				dianas[i].posX += dianas[i].velocidad;
			} 
			else if (dianas[i].activa != 0 && dianas[i].chocoConPared%2 != 0)
			{
				dianas[i].posX -= dianas[i].velocidad;
			}

			if (ColisionMapa(sala, dianas[i].posX, dianas[i].posY) || 
				ColisionMapa(sala, dianas[i].posX + TAMANHO - 1, dianas[i].posY) || 
				ColisionMapa(sala, dianas[i].posX + TAMANHO - 1, dianas[i].posY + TAMANHO - 1) ||
				ColisionMapa(sala, dianas[i].posX, dianas[i].posY + TAMANHO - 1))
			{
				dianas[i].posX = auxXDiana;
				dianas[i].chocoConPared++;
			}
		}
		
		if (dianas[i].auxRanDianas == 2)
		{
			if (dianas[i].activa != 0 && dianas[i].chocoConPared%2 == 0)
			{
				dianas[i].posY += dianas[i].velocidad;
			} 
			else if (dianas[i].activa != 0 && dianas[i].chocoConPared%2 != 0)
			{
				dianas[i].posY -= dianas[i].velocidad;
			}

			if (ColisionMapa(sala, dianas[i].posX, dianas[i].posY) || 
				ColisionMapa(sala, dianas[i].posX + TAMANHO - 1, dianas[i].posY) || 
				ColisionMapa(sala, dianas[i].posX + TAMANHO - 1, dianas[i].posY + TAMANHO - 1) ||
				ColisionMapa(sala, dianas[i].posX, dianas[i].posY + TAMANHO - 1))
			{
				dianas[i].posY = auxYDiana;
				dianas[i].chocoConPared++;
			}
		}
	}
}

void GeneracionDeRecompensas()
{
	if (salaVacia == 1 && seGeneroUnaRecompensa == 0 && !(actualMapaX == COLUMNAS_MAPA / 2 + 1 && actualMapaY == FILAS_MAPA / 2 + 1))
	{
		seGeneroUnaRecompensa = 1;

		registroRecompensas[cantidadRecompensas].mapaX = actualMapaX;
		registroRecompensas[cantidadRecompensas].mapaY = actualMapaY;
		registroRecompensas[cantidadRecompensas].seObtuvoUnaRecompensa = 1;
		cantidadRecompensas ++;
		//Hacer un pull de objetos aleatorios, por ahora la llave
		llaves[llavesActual].activa = 1;
		llavesActual ++;
	}
}

void VerificarSalaVacia()
{
	int enemigosVivos = 0;

	for (int i = 0; i < MAX_ENEMIGOS; i++)
	{
		if (slime[i].activa != 0 || mago[i].activa != 0 || aranha[i].activa != 0)
		{
			enemigosVivos = 1;
		}
	}

	if (Jefe.activa != 0)
	{
		enemigosVivos = 1;
	}	

	if (enemigosVivos == 1)
	{
		salaVacia = 0;
	}
	else 
	{
		salaVacia = 1;
	}

	//printf("sala vacia = %d", salaVacia);
}

void CambioDeHabitaciones()
{
	FILE *archivoHabitacion = NULL;
	
	if (personaje.traspasoPuerta == 4)
	{
		if (mapa[actualMapaY][actualMapaX - 1] != NULL)
		{
			actualMapaX --;
			cargarMapa(mapa[actualMapaY][actualMapaX], archivoHabitacion, sala, &personaje, slime, 'E', actualMapaX, actualMapaY);
		}

		personaje.traspasoPuerta = 0;
	}
	else if (mapa[actualMapaY][actualMapaX - 1] == NULL || mapa[actualMapaY][actualMapaX - 1] == "hab_trial.txt") //Las puertas que esten conectadas a una parte nula de mapa se reemplazan por #
	{
		for (int i = 0; i < FILAS_HABITACION; i++)
		{
			for (int j = 0; j < COLUMNAS_HABITACION; j++)
			{
				if (sala[i][j] == 'O')
				{
					sala[i][j] = '#';
				}
			}
		}
	}

	if (personaje.traspasoPuerta == 2)
	{
		if (mapa[actualMapaY][actualMapaX + 1] != NULL)
		{
			actualMapaX ++;
			cargarMapa(mapa[actualMapaY][actualMapaX], archivoHabitacion, sala, &personaje, slime, 'O', actualMapaX, actualMapaY);
		}
		
		personaje.traspasoPuerta = 0;
	}
	else if (mapa[actualMapaY][actualMapaX + 1] == NULL || mapa[actualMapaY][actualMapaX + 1] == "hab_trial.txt")
	{
		for (int i = 0; i < FILAS_HABITACION; i++)
		{
			for (int j = 0; j < COLUMNAS_HABITACION; j++)
			{
				if (sala[i][j] == 'E')
				{
					sala[i][j] = '#';
				}
			}
		}
	}

	if (personaje.traspasoPuerta == 3)
	{
		if (mapa[actualMapaY + 1][actualMapaX] != NULL)
		{
			actualMapaY ++;
			cargarMapa(mapa[actualMapaY][actualMapaX], archivoHabitacion, sala, &personaje, slime, 'N', actualMapaX, actualMapaY);
		}
		
		personaje.traspasoPuerta = 0;
	}
	else if (mapa[actualMapaY + 1][actualMapaX] == NULL || mapa[actualMapaY + 1][actualMapaX] == "hab_trial.txt")
	{
		for (int i = 0; i < FILAS_HABITACION; i++)
		{
			for (int j = 0; j < COLUMNAS_HABITACION; j++)
			{
				if (sala[i][j] == 'S')
				{
					sala[i][j] = '#';
				}
			}
		}
	}
	
	if (personaje.traspasoPuerta == 1)
	{
		if (mapa[actualMapaY - 1][actualMapaX] != NULL)
		{
			actualMapaY --;
			cargarMapa(mapa[actualMapaY][actualMapaX], archivoHabitacion, sala, &personaje, slime, 'S', actualMapaX, actualMapaY);
		}
		
		personaje.traspasoPuerta = 0;
	}
	else if (mapa[actualMapaY - 1][actualMapaX] == NULL)
	{
		for (int i = 0; i < FILAS_HABITACION; i++)
		{
			for (int j = 0; j < COLUMNAS_HABITACION; j++)
			{
				if (sala[i][j] == 'N')
				{
					sala[i][j] = '#';
				}
			}
		}
	}
}

void VerificarTraspasoPuertas(char mapa[FILAS_HABITACION][COLUMNAS_HABITACION], jugador *personaje)
{
	//Centro del personaje
	int columna = (personaje->posX + (TAMANHO / 2)) / TAMANHO;
	int fila = (personaje->posY + (TAMANHO / 2)) / TAMANHO;

	//Validamos que no nos salimos del arreglo
	if(fila >= 0 && fila < FILAS_HABITACION && columna >= 0 && columna < COLUMNAS_HABITACION)
	{
		if(mapa[fila][columna] == 'N')
		{
			personaje->traspasoPuerta = 1;
		}

		if(mapa[fila][columna] == 'S')
		{
			personaje->traspasoPuerta = 3;
		}

		if(mapa[fila][columna] == 'E')
		{
			personaje->traspasoPuerta = 2;
		}

		if(mapa[fila][columna] == 'O')
		{
			personaje->traspasoPuerta = 4;
		}
	}
}

void GeneracionDelMapa(int cantidadHabitacionesDeseadas)
{
	int habitacionCardinal = 0;
	int a = 0;
	int filaActual = FILAS_MAPA / 2 + 1;
	int columnaActual = COLUMNAS_MAPA / 2 + 1;
	int terminoProceso = 0;
	char *pullHabitaciones;
	int auxRand = 0;
	int seGeneroUnaHabitacionRecompensa = 0;
	int fogatasGeneradas = 0;
	int seGeneroUnaHabitacioPunteria = 0;
	int habitacionTrialGenerada = 0;
	int tiendaGenerada = 0;

	//La habitacion central siempre será la misma
	mapa[FILAS_MAPA / 2 + 1][COLUMNAS_MAPA / 2 + 1] = "habBase.txt";
	
	while (a < cantidadHabitacionesDeseadas)
	{
		auxRand = rand() % 9 + 1;
		auxRand = 4;

		if (auxRand == 1)
		{
			pullHabitaciones = "hab_general_1.txt";  
		}
		else if (auxRand == 2)
		{
			pullHabitaciones = "hab_general_2.txt";  
		}
		else if (auxRand == 3)
		{
			pullHabitaciones = "hab_general_3.txt";  
		}
		else if (auxRand == 4)
		{
			pullHabitaciones = "hab_general_4.txt";  
		}
		else if (auxRand == 5)
		{
			pullHabitaciones = "hab_recompensa.txt";
		}
		else if (auxRand == 6 && fogatasGeneradas < 3)
		{
			pullHabitaciones = "hab_fogata.txt";
		}
		else if (auxRand == 7 && seGeneroUnaHabitacioPunteria == 0)
		{
			pullHabitaciones = "hab_punteria.txt";
		}
		else if (auxRand == 8 && habitacionTrialGenerada == 0)
		{
			pullHabitaciones = "hab_trial.txt";
		}
		else if (auxRand == 9 && tiendaGenerada == 0)
		{
			pullHabitaciones = "hab_tienda.txt";
		}

		//Garantia de sala
		if (a >= cantidadHabitacionesDeseadas - 8 && tiendaGenerada == 0)
		{
			pullHabitaciones = "hab_tienda.txt";
		}

		else if (a >= cantidadHabitacionesDeseadas - 7 && habitacionTrialGenerada == 0)
		{
			pullHabitaciones = "hab_trial.txt";
		}
		
		else if (a >= cantidadHabitacionesDeseadas - 6 && seGeneroUnaHabitacioPunteria == 0)
		{
			pullHabitaciones = "hab_punteria.txt";
		}

		else if (a >= cantidadHabitacionesDeseadas - 5 && fogatasGeneradas < 3)
		{
			pullHabitaciones = "hab_fogata.txt"	;
		}
		
		//La penultima habitacion generada siempre sera una sala de recompensa si no se genero antes
		else if (a >= cantidadHabitacionesDeseadas - 2 && seGeneroUnaHabitacionRecompensa == 0)
		{
			pullHabitaciones = "hab_recompensa.txt";
		}
		
		//Cuando este por generar la ultima habitacion, obliga a que sea la del jefe
		else if (a == cantidadHabitacionesDeseadas - 1)
		{
			pullHabitaciones = "hab_jefe_1.txt";
		}

		//Buscar direccion para ponerse la sala
		habitacionCardinal = rand() % 4 + 1; // entre 1 y 4... 1: Norte, 2: Este, 3: Sur, 4: Oeste 

		if (strcmp(pullHabitaciones, "hab_trial.txt") == 0)
		{
			habitacionCardinal = 1;
		}
		
		if (habitacionCardinal == 1)
		{
			if (mapa[filaActual - 1][columnaActual] == NULL)
			{
				filaActual --;
				mapa[filaActual][columnaActual] = pullHabitaciones;

				filaActual = FILAS_MAPA / 2 + 1;
				columnaActual = COLUMNAS_MAPA / 2 + 1;
				a ++;

				//Verificacion de salas importantes
				if (strcmp(pullHabitaciones, "hab_fogata.txt") == 0)
				{
					fogatasGeneradas++;
				}

				if (strcmp(pullHabitaciones, "hab_tienda.txt") == 0)
				{
					tiendaGenerada++;
				}
				
				if (strcmp(pullHabitaciones, "hab_recompensa.txt") == 0)
				{
					seGeneroUnaHabitacionRecompensa++;
				}

				if (strcmp(pullHabitaciones, "hab_punteria.txt") == 0)
				{
					seGeneroUnaHabitacioPunteria++;
				}

				if (strcmp(pullHabitaciones, "hab_trial.txt") == 0)
				{
					habitacionTrialGenerada++;
				}

				printf("Habitacion 1 generada");
			}
			else
			{
				filaActual --;
			}	
		}

		if (habitacionCardinal == 3)
		{
			if (mapa[filaActual + 1][columnaActual] == NULL)
			{
				filaActual ++;
				mapa[filaActual][columnaActual] = pullHabitaciones;

				filaActual = FILAS_MAPA / 2 + 1;
				columnaActual = COLUMNAS_MAPA / 2 + 1;
				a ++;

				//Verificacion de salas importantes
				if (strcmp(pullHabitaciones, "hab_fogata.txt") == 0)
				{
					fogatasGeneradas++;
				}

				if (strcmp(pullHabitaciones, "hab_tienda.txt") == 0)
				{
					tiendaGenerada++;
				}
				
				if (strcmp(pullHabitaciones, "hab_recompensa.txt") == 0)
				{
					seGeneroUnaHabitacionRecompensa++;
				}

				if (strcmp(pullHabitaciones, "hab_punteria.txt") == 0)
				{
					seGeneroUnaHabitacioPunteria++;
				}

				printf("Habitacion 3 generada");
			}
			else
			{
				filaActual ++;
			}	
		}

		if (habitacionCardinal == 2)
		{
			if (mapa[filaActual][columnaActual + 1] == NULL)
			{
				columnaActual ++;
				mapa[filaActual][columnaActual] = pullHabitaciones;

				filaActual = FILAS_MAPA / 2 + 1;
				columnaActual = COLUMNAS_MAPA / 2 + 1;
				a ++;

				//Verificacion de salas importantes
				if (strcmp(pullHabitaciones, "hab_fogata.txt") == 0)
				{
					fogatasGeneradas++;
				}

				if (strcmp(pullHabitaciones, "hab_tienda.txt") == 0)
				{
					tiendaGenerada++;
				}
				
				if (strcmp(pullHabitaciones, "hab_recompensa.txt") == 0)
				{
					seGeneroUnaHabitacionRecompensa++;
				}

				if (strcmp(pullHabitaciones, "hab_punteria.txt") == 0)
				{
					seGeneroUnaHabitacioPunteria++;
				}

				printf("Habitacion 2 generada");
			}
			else
			{
				columnaActual ++;
			}	
		}

		if (habitacionCardinal == 4)
		{
			if (mapa[filaActual][columnaActual - 1] == NULL)
			{
				columnaActual --;
				mapa[filaActual][columnaActual] = pullHabitaciones;

				filaActual = FILAS_MAPA / 2 + 1;
				columnaActual = COLUMNAS_MAPA / 2 + 1;
				a ++;

				//Verificacion de salas importantes
				if (strcmp(pullHabitaciones, "hab_fogata.txt") == 0)
				{
					fogatasGeneradas++;
				}

				if (strcmp(pullHabitaciones, "hab_tienda.txt") == 0)
				{
					tiendaGenerada++;
				}
				
				if (strcmp(pullHabitaciones, "hab_recompensa.txt") == 0)
				{
					seGeneroUnaHabitacionRecompensa++;
				}

				if (strcmp(pullHabitaciones, "hab_punteria.txt") == 0)
				{
					seGeneroUnaHabitacioPunteria++;
				}

				printf("Habitacion 4 generada");
			}
			else
			{
				columnaActual --;
			}	
		}
	}
}