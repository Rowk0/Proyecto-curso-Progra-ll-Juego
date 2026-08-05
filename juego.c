#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <allegro5/allegro.h> 
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <math.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#define FILAS_HABITACION 17
#define COLUMNAS_HABITACION 30
#define FILAS_MAPA 10
#define COLUMNAS_MAPA 10
#define TAMANHO 64
#define LARGO_TEXTO 30
#define LARGO_SPRITES 30
#define LARGO_PANTALLA 1920
#define ANCHO_PANTALLA 1088
#define MAX_BALAS 20
#define MAX_ENEMIGOS 20
#define MAX_OBJETOS 1000
#define MAX_INTERACTUABLES 10
#define MAX_REGISTROS 1000
#define MAX_DIANAS 10

//Ideas deshechadas: 
//Movimiento de camara: implica crear otra camara estatica para cosas que no quiero que se muevan
//Ver todas las habitaciones mientras te mueves: eso implica hacer más condicionales en enemigos, reformular cargar mapa
//Hacer un indicador antes de la sala del jefe: está de más
//Cerrar las puertas al entrar a una habitacion: Con el sistema de balasdisponibles, si no tuvieras balas en una habitacion cerrada pierdes instantaneamente

////////////////////////////////////////////////////////////////  tareas

//Diseño:
//fade out al iniciar estadojuegojuego
//Mejorar diseño mapa cofre y fogata
//Hacer mas habitaciones 2 mas
//animacion de ganar

//ideas:
//trampas
//Enemigo que te persiga y dispare tres balas
//implementar mas "armas" al jugador

/////////////////////////////////////////////////////////////////  flujo trabajo

//gcc juego.c -o juego -lallegro -lallegro_main -lallegro_primitives -lallegro_image -lm -lallegro_ttf -lallegro_font -lallegro_audio -lallegro_acodec
//para compilar

//./juego 
//para correr programa

//Si no funciona la pantalla, poner "wsl --shutdown" en powershell y luego "code ."

//flujo git (antes ctrl + s para guardar archivo local)
//git add . -> git commit -m "cambios" -> git push.

//Convertir mp3 a ogg
//ffmpeg -i musica1.mp3 musica1.ogg

////////////////////////////////////////////////////////////////// Estructuras

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
	int seDisparo;
	float direccionBala;
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
	int animacionJugador;
	struct dirJugador_ dirJugador;
	int vidas;
	int invulnerable;
	int cantidadMonedas;
	int cantidadLlaves;
	int puntaje;
	int rangoDeBalas;
	int balasrestantes;
	bala_ bala[MAX_BALAS]; 
	int balasDisponibles;
	int traspasoPuerta; //1: Norte, 2: Este, 3: Sur, 4: Oeste
	int cadencia;
	int seMovio;
} jugador;

typedef struct 
{
	int posX;
	int posY;
	int tamanho;
	int seClickeo;
} mouse_;

typedef struct 
{
	int sePresionoTAB;
} teclado_;

typedef struct 
{
	int posX;
	int posY;
	int posXAnterior;
	int posYAnterior;
	int posXGeneracion;
	int posYGeneracion;
	int velocidad;
	int activa;
	int direccion; 
	int vida;
	bala_ bala[MAX_BALAS];
	int balaActual;
	int ataquesEnemigos;
	int chocoConPared;
	int auxRandAranha;
	int rangoDeBalas;
	int indiceEnemigo;
	int cadencia;
	int timerMovimientoJefe;
} enemigo;
 
typedef struct 
{
	enemigo slime[MAX_ENEMIGOS];
	enemigo jefe;
	enemigo mago[MAX_ENEMIGOS];
	enemigo aranha[MAX_ENEMIGOS];
} gestionEnemigos_;

typedef struct 
{
	int slime;
	int mago;
	int aranha;
} controlIndices_;

typedef struct 
{
	int posX;
	int posY;
	int fila;
	int columna;
	int activa;
	int destruida;
	int velocidad;
	int chocoConPared;
	int auxRanDianas;
} dianas_;

typedef struct 
{
	char registroMapa[LARGO_TEXTO];
	int fila;
	int columna;
	int mapaX;
	int mapaY;
} registroMuertes_;

typedef struct 
{
	int mapaX;
	int mapaY;
	int fila;
	int columna;
	int seObtuvoUnaRecompensa;
	int seAbrioUnCofre;
	int seObtuvoUnCargador;

	//apartado para objetos que no aparecen en el mapa
	int posX;
	int posY;
	char idItem;
	int seRecogioElItem;
} registroRecompensas_;

typedef struct 
{
	int mapaX;
	int mapaY;
	int fila;
	int columna;
	int fogataEncendida;
	int dianaDestruida;
} registroInteractuables_;

typedef struct 
{
	int mapaX;
	int mapaY;
	int fila;
	int columna;
	int idObjeto;
	int comprado;
} registroTienda_;

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
	int seGenero;
} objeto;

typedef struct 
{
	//Arreglo de objetos
	objeto monedas[MAX_OBJETOS];
	objeto llaves[MAX_OBJETOS];
	objeto cofres[MAX_OBJETOS];
	objeto municiones[MAX_OBJETOS];
	objeto vidasObjeto[MAX_OBJETOS];
	objeto mejoraDanho[MAX_OBJETOS];
	objeto mejoraRango[MAX_OBJETOS];
	objeto mejoraVelocidad[MAX_OBJETOS];

	//Objetos unicos (estructuras aparte)
	objeto mapaRojo;
	objeto mapaVerde;
	objeto mapaAzul;
	objeto mapaNaranjo;
	objeto minimapa;

	//Contadores de los arreglos de objeto
	int monedasActual;
	int llavesActual;
	int cofreActual;
	int municionesActual;
	int vidasObjetoActual;
	int mejoraDanhoActual;
	int mejoraRangoActual;
	int mejoraVelocidadActual;
} gestionObjetos_;

//Interactuables (Cosas que interactuas pero no son recogibles)
typedef struct 
{
	int posX;
	int posY;
	int fila;
	int columna;
	int activa;
	int fogataActiva;
} interactuables;

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
	int ANIMACION_INICIAL;
} estadoJuego_;

typedef struct 
{
	int verdaderaPantallaRanking;
	int pantallaPonerNombre;
	int unichar;
	int longitudNombre;
	int mouseEnPlay;
	int mouseEnRanking;
	int mouseEnVolver;
	int mouseEnReiniciar;
	int mouseEnVolverAlMenu;
	int pantallaReiniciar;
} controlMenu_;

typedef struct 
{
	//Estructura donde se guarda el estado del teclado y del mouse
	ALLEGRO_KEYBOARD_STATE teclado;
	ALLEGRO_MOUSE_STATE mouse;

	//timers para control de sprites general
	int controlSprites;
	int controlSpritesMenu;
} estadoSistema_;

typedef struct 
{
	//En este arreglo se carga el mapa, y en base a él, se dibuja
	char sala[FILAS_HABITACION][COLUMNAS_HABITACION];

	//Aqui se guarda el nombre de los archivos en un arreglo simulando un mapa
	char *mapa[FILAS_MAPA][COLUMNAS_MAPA];

	//Control de ubicacion de mapa
	int actualMapaX;
	int actualMapaY;

	//Variable que permite saber si la estadoMapa->sala actual esta vacia
	int salaVacia;
	int seGeneroUnaRecompensa;

	int minimapa[FILAS_MAPA][COLUMNAS_MAPA];
	int minimapaVendido;
} estadoMapa_;

typedef struct 
{
	dianas_ dianas[MAX_DIANAS];
	int dianasActuales;
	int cantidadDianasDestruidas;
} gestionDianas_;

typedef struct 
{
	interactuables fogata[MAX_INTERACTUABLES];
	int fogataActual;
	int cantidadfogatasActivas;
} gestionInteractuables_;

typedef struct 
{
	registroTienda_ registroTienda[MAX_REGISTROS];
	int indiceTienda;

	registroInteractuables_ registroInteractuables[MAX_REGISTROS];
	int cantidadInteractuables;

	registroRecompensas_ registroRecompensas[MAX_REGISTROS];
	int cantidadRecompensas;

	registroMuertes_ registroMuertes[MAX_REGISTROS];
	int cantidadMuertos;
} registroMundo_;

typedef struct 
{
	ALLEGRO_AUDIO_STREAM *musica;
	ALLEGRO_SAMPLE *sonido;
	int audioTimerSlime;
	int audioTimerPasosJugador;
	int inciciarTimerPasosJugador;
} controlAudio_;

typedef struct 
{
	int timer;
	int transparencia1;
	int transparencia2;
	int transparencia3;
	int transparencia4;
	int transparencia5;
} controlAnimacion_;

///////////////////////////////////////////////////////////////// Variables globales

jugador personaje;

mouse_ mouse;

/////////////////////////////////////////////////////////////////  Funciones

void DibujarMapa(char mapa[FILAS_HABITACION][COLUMNAS_HABITACION], ALLEGRO_BITMAP *spriteSheet, estadoSistema_ *estadoSistema, gestionInteractuables_ *gestionInteractuables);
void cargarMapa(char nombreMapa[LARGO_TEXTO], FILE *varMapa, char mapa[FILAS_HABITACION][COLUMNAS_HABITACION], jugador *personaje, gestionEnemigos_ *gestionEnemigos, char puertaDestino, int mapaX, int mapaY, controlIndices_ *controlIndices, gestionObjetos_ *gestionObjetos, estadoMapa_ *estadoMapa, gestionDianas_ *gestionDianas, gestionInteractuables_ *gestionInteractuables, registroMundo_ *registroMundo);
bool ColisionMapa(char mapa[FILAS_HABITACION][COLUMNAS_HABITACION], int jugadorPosXProximo, int jugadorPosYProximo);
void Logica(char mapa[FILAS_HABITACION][COLUMNAS_HABITACION], jugador *jugador, estadoJuego_ *estadoJuego, controlIndices_ *controlIndices, gestionEnemigos_ *gestionEnemigos, gestionObjetos_ *gestionObjetos, estadoSistema_ *estadoSistema, estadoMapa_ *estadoMapa, gestionDianas_ *gestionDianas, gestionInteractuables_ *gestionInteractuables, registroMundo_ *registroMundo, ranking_ *ranking, FILE *archivoRanking, controlAudio_ *controlAudio);
void MovimientoJugador(estadoSistema_ *estadoSistema, estadoMapa_ *estadoMapa, controlAudio_ *controlAudio);
void InitAllegro();
void InitGameComponents(ALLEGRO_DISPLAY *ventana, mouse_ *mouse, ranking_ *ranking, gestionEnemigos_ *gestionEnemigos, gestionObjetos_ *gestionObjetos, estadoSistema_ *estadoSistema, estadoMapa_ *estadoMapa, gestionDianas_ *gestionDianas, gestionInteractuables_ *gestionInteractuables, registroMundo_ *registroMundo, controlAnimacion_ *controlAnimacion);
void InputHandle(estadoJuego_ *estadoJuego, controlMenu_ *controlMenu, ranking_ *ranking, ALLEGRO_EVENT_QUEUE *colaEventos, estadoSistema_ *estadoSistema);
void Render(char mapa[FILAS_HABITACION][COLUMNAS_HABITACION], ALLEGRO_BITMAP *spriteSheet, ALLEGRO_BITMAP *spriteSheetBalas, ALLEGRO_BITMAP *spriteSheetCaminarCaballero, ALLEGRO_BITMAP *spriteSheetIcons, ALLEGRO_FONT *fuenteJuego, ALLEGRO_BITMAP *spriteSheetCrosshair, ALLEGRO_BITMAP *spriteSheetBalasEnemigos, ALLEGRO_BITMAP *spriteSheetIconsRaven, gestionEnemigos_ *gestionEnemigos, gestionObjetos_ *gestionObjetos, estadoSistema_ *estadoSistema, estadoMapa_ *estadoMapa, gestionDianas_ *gestionDianas, gestionInteractuables_ *gestionInteractuables, ALLEGRO_BITMAP *spriteSheetBotonesTeclado, ALLEGRO_BITMAP *spriteSheetBotonesMouse, teclado_ *teclado);
void Disparo(estadoSistema_ *estadoSistema, estadoMapa_ *estadoMapa, controlAudio_ *controlAudio);
void MovimientoCamara();
void AnimacionPersonaje(ALLEGRO_BITMAP *spriteSheet, ALLEGRO_BITMAP *spriteSheetCaminarCaballero, estadoSistema_ *estadoSistema);
void AnimacionEnemigos(ALLEGRO_BITMAP *spriteSheet, gestionEnemigos_ *gestionEnemigos, estadoSistema_ *estadoSistema);
void LogicaEnemigos(gestionEnemigos_ *gestionEnemigos, gestionObjetos_ *gestionObjetos, estadoMapa_ *estadoMapa, registroMundo_ *registroMundo, controlAudio_ *controlAudio);
void ColisionEnemigos(gestionEnemigos_ *gestionEnemigos, gestionObjetos_ *gestionObjetos, estadoMapa_ *estadoMapa, registroMundo_ *registroMundo, controlAudio_ *controlAudio);
void CambioDeHabitaciones(controlIndices_ *controlIndices, gestionEnemigos_ *gestionEnemigos, gestionObjetos_ *gestionObjetos, estadoMapa_ *estadoMapa, gestionDianas_ *gestionDianas, gestionInteractuables_ *gestionInteractuables, registroMundo_ *registroMundo);
void RenderMenu(ALLEGRO_FONT *fuenteJuego, ALLEGRO_BITMAP *menuFondo, controlMenu_ *controlMenu, ranking_ *ranking, char mapa[FILAS_HABITACION][COLUMNAS_HABITACION], ALLEGRO_BITMAP *spriteSheet, estadoSistema_ *estadoSistema, gestionInteractuables_ *gestionInteractuables);
void PersonajeInvulnerable();
void VerificarTraspasoPuertas(char mapa[FILAS_HABITACION][COLUMNAS_HABITACION], jugador *personaje);
void GeneracionDelMapa(int cantidadHabitacionesDeseadas, estadoMapa_ *estadoMapa);
void DisparoEnemigos(gestionEnemigos_ *gestionEnemigos, estadoMapa_ *estadoMapa, controlAudio_ *controlAudio);
void LogicaJefe(gestionEnemigos_ *gestionEnemigos, estadoMapa_ *estadoMapa);
void HandicapsMejorables();
void LogicaMenu(estadoJuego_ *estadoJuego, controlMenu_ *controlMenu, ranking_ *ranking, ALLEGRO_EVENT_QUEUE *colaEventos, FILE *archivoRanking, estadoSistema_ *estadoSistema, controlAudio_ *controlAudio);
void ColicionObjetos(gestionObjetos_ *gestionObjetos, estadoMapa_ *estadoMapa, registroMundo_ *registroMundo, controlAudio_ *controlAudio);
void RangoVisionEnemigo(gestionEnemigos_ *gestionEnemigos);
void VerificarSalaVacia(gestionEnemigos_ *gestionEnemigos, estadoMapa_ *estadoMapa);
void GeneracionDeRecompensas(gestionObjetos_ *gestionObjetos, estadoMapa_ *estadoMapa, registroMundo_ *registroMundo);
void ColicionInteractuables(estadoMapa_ *estadoMapa, gestionDianas_ *gestionDianas, gestionInteractuables_ *gestionInteractuables, registroMundo_ *registroMundo, controlAudio_ *controlAudio);
void LogicaDianas(estadoMapa_ *estadoMapa, gestionDianas_ *gestionDianas, controlAudio_ *controlAudio);
void RenderReiniciar(ALLEGRO_FONT *fuenteJuego, controlMenu_ *controlMenu, ranking_ *ranking);
void LogicaReiniciar(estadoJuego_ *estadoJuego, estadoSistema_ *estadoSistema, controlMenu_ *controlMenu, FILE *archivoRanking, ranking_ *ranking, controlAudio_ *controlAudio);
void SetRanking(FILE *archivoRanking, ranking_ *ranking, char RegistrarJugador[LARGO_TEXTO], int puntajeDelJugador);
void GetRanking(FILE *archivoRanking, ranking_ *ranking);
void DesactivarObjetosActivos(gestionEnemigos_ *gestionEnemigos,  controlIndices_ *controlIndices, gestionObjetos_ *gestionObjetos, estadoMapa_ *estadoMapa, gestionDianas_ *gestionDianas, gestionInteractuables_ *gestionInteractuables);
void Musica(controlAudio_ *controlAudio, char *nombreMusica);
void Sonido(controlAudio_ *controlAudio, int indiceSonido);
void ColisionMapaBalas(char mapa[FILAS_HABITACION][COLUMNAS_HABITACION]);
void LogicaAnimacion(controlAnimacion_ *controlAnimacion, estadoJuego_ *estadoJuego, gestionObjetos_ *gestionObjetos);
void RenderAnimacion(ALLEGRO_BITMAP *spriteSheet, ALLEGRO_BITMAP *spriteSheetIcons, controlAnimacion_ *controlAnimacion, ALLEGRO_FONT *fuenteJuego);
//Primeros cuatro parametros representan un cuadrado (x1,y1) esquina superior izquierda (w1,h1) sus tamaños, generalmente la cantidad de pixeles, en este caso 64
//Ultimos cuatro parametros representa otro cuadrado con otros parametros
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
	estadoJuego.ANIMACION_INICIAL = 0;
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
	ALLEGRO_BITMAP *spriteSheetBotonesTeclado;
	ALLEGRO_BITMAP *spriteSheetBotonesMouse;

	//Fonts
	ALLEGRO_FONT *fuenteJuego;

	//Variables char
	char nombreHabitacion[LARGO_TEXTO] = "habBase.txt";
	char fondoMenu[LARGO_TEXTO] = "menu_fondo_mapa.txt";

	//variables struct
	ranking_ ranking;
	controlIndices_ controlIndices;
	gestionEnemigos_ gestionEnemigos;
	gestionObjetos_ gestionObjetos;
	estadoSistema_ estadoSistema;
	estadoMapa_ estadoMapa;
	gestionDianas_ gestionDianas;
	gestionInteractuables_ gestionInteractuables;
	registroMundo_ registroMundo;
	controlAudio_ controlAudio;
	controlAnimacion_ controlAnimacion;
	teclado_ teclado;
	
	///////////////////////////////////////////////////////////////

	srand(time(NULL));

	//Inicializar funciones de Allegro
	InitAllegro();

	//Crear cola de eventos, solo se registra el teclado (Solo se usa en inputHandle())
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
	spriteSheetBotonesTeclado = al_load_bitmap("KEYBOARD.png");
	spriteSheetBotonesMouse = al_load_bitmap("MOUSE.png");

	//Si sistema es 0 el programa se cierra
	while (estadoJuego.SISTEMA)
	{
		InitGameComponents(ventana, &mouse, &ranking, &gestionEnemigos, &gestionObjetos, &estadoSistema, &estadoMapa, &gestionDianas, &gestionInteractuables, &registroMundo, &controlAnimacion);

		//Cargar mapa hace que se lea un archivo y genere una copia en estadoMapa->sala
		cargarMapa(fondoMenu, archivoMapas, estadoMapa.sala, &personaje, &gestionEnemigos, '@', estadoMapa.actualMapaX, estadoMapa.actualMapaY, &controlIndices, &gestionObjetos, &estadoMapa, &gestionDianas, &gestionInteractuables, &registroMundo);

		//El entero representa la cantidad de habitaciones deseadas para generar en el mapa, va de la mano con FILAS_MAPA Y COLUMNAS_MAPA
		GeneracionDelMapa(20, &estadoMapa); 

		//Musica(&controlAudio, "musica1.ogg");

		while (estadoJuego.MENU)
		{
			InputHandle(&estadoJuego, &controlMenu, &ranking, colaEventos, &estadoSistema);

			RenderMenu(fuenteJuego, menuFondo, &controlMenu, &ranking, estadoMapa.sala, spriteSheet, &estadoSistema, &gestionInteractuables);

			LogicaMenu(&estadoJuego, &controlMenu, &ranking, colaEventos, archivoRanking, &estadoSistema, &controlAudio);

			al_rest(0.016);
		}

		while (estadoJuego.ANIMACION_INICIAL)
		{
			InputHandle(&estadoJuego, &controlMenu, &ranking, colaEventos, &estadoSistema);

			LogicaAnimacion(&controlAnimacion, &estadoJuego, &gestionObjetos);

			RenderAnimacion(spriteSheet, spriteSheetIcons, &controlAnimacion, fuenteJuego);

			al_rest(0.016);
		}

		cargarMapa(nombreHabitacion, archivoMapas, estadoMapa.sala, &personaje, &gestionEnemigos, '@', estadoMapa.actualMapaX, estadoMapa.actualMapaY, &controlIndices, &gestionObjetos, &estadoMapa, &gestionDianas, &gestionInteractuables, &registroMundo);

		//Musica(&controlAudio, "musica2.ogg");

		while (estadoJuego.JUEGO)
		{
			//Funcion que actualiza el estadoSistema->teclado del teclado y mouse
			InputHandle(&estadoJuego, &controlMenu, &ranking, colaEventos, &estadoSistema);

			//Logica del juego. Ej: movimientos del jugador
			Logica(estadoMapa.sala, &personaje, &estadoJuego, &controlIndices, &gestionEnemigos, &gestionObjetos, &estadoSistema, &estadoMapa, &gestionDianas, &gestionInteractuables, &registroMundo, &ranking, archivoRanking, &controlAudio);

			//Dibujar aqui
			Render(estadoMapa.sala, spriteSheet, spriteSheetBalas, spriteSheetCaminarCaballero, spriteSheetIcons, fuenteJuego, spriteSheetCrosshair, spriteSheetBalasEnemigos, spriteSheetIconsRaven, &gestionEnemigos, &gestionObjetos, &estadoSistema, &estadoMapa, &gestionDianas, &gestionInteractuables, spriteSheetBotonesTeclado, spriteSheetBotonesMouse, &teclado);

			//Hacer descansar el cpu
			al_rest(0.016);
		}

		controlMenu.pantallaReiniciar = 1;

		while (estadoJuego.REINICIAR)
		{
			InputHandle(&estadoJuego, &controlMenu, &ranking, colaEventos, &estadoSistema);

			RenderReiniciar(fuenteJuego, &controlMenu, &ranking);

			LogicaReiniciar(&estadoJuego, &estadoSistema, &controlMenu, archivoRanking, &ranking, &controlAudio);

			al_rest(0.016);
		}
	}

	//Evitar fallas de memoria
	//Destruir musica al salir del programa
	if (controlAudio.musica != NULL) 
	{
    	al_destroy_audio_stream(controlAudio.musica);
	}

	al_uninstall_audio();

	return 0;
}

void LogicaAnimacion(controlAnimacion_ *controlAnimacion, estadoJuego_ *estadoJuego, gestionObjetos_ *gestionObjetos)
{
	controlAnimacion->timer++;

	if (controlAnimacion->timer > 1100) //3600 = 1 minuto
	{
		estadoJuego->ANIMACION_INICIAL = 0;
		estadoJuego->JUEGO = 1;
	}
}

void RenderAnimacion(ALLEGRO_BITMAP *spriteSheet, ALLEGRO_BITMAP *spriteSheetIcons, controlAnimacion_ *controlAnimacion, ALLEGRO_FONT *fuenteJuego)
{
	al_clear_to_color(al_map_rgb(0, 0, 0));
	//////////////////////////////////////////////// Dibujar en este espacio

	for (int i = 0; i < 4; i++)
	{
		al_draw_bitmap_region(spriteSheet, 1 * TAMANHO, 19 * TAMANHO, TAMANHO, TAMANHO, 400 + i * TAMANHO * 5, 600, 0);
	}
	
	al_draw_bitmap_region(spriteSheetIcons, 12 * TAMANHO, 18 * TAMANHO, TAMANHO, TAMANHO, 400, 530, 0);
	
	al_draw_bitmap_region(spriteSheetIcons, 8 * TAMANHO, 18 * TAMANHO, TAMANHO, TAMANHO, 720, 530, 0);
	
	al_draw_bitmap_region(spriteSheetIcons, 9 * TAMANHO, 18 * TAMANHO, TAMANHO, TAMANHO, 1040, 530, 0);

	al_draw_bitmap_region(spriteSheetIcons, 10 * TAMANHO, 18 * TAMANHO, TAMANHO, TAMANHO, 1360, 530, 0);

	al_draw_text(fuenteJuego, al_map_rgb(255, 255, 255), 100, 150, 0, "Encuentra los legendarios rollos magicos...");

	al_draw_text(fuenteJuego, al_map_rgb(255, 255, 255), 100, 250, 0, "Nadie debe ver sus secretos.");

	al_draw_text(fuenteJuego, al_map_rgb(255, 255, 255), 100, 850, 0, "Pero tu soberbia es más grande...");

	al_draw_text(fuenteJuego, al_map_rgb(255, 255, 255), 100, 950, 0, "Empieza tu busqueda.");

	//Debe estar en la ultima linea por jerarquia de dibujos
	if (controlAnimacion->timer > 60 && controlAnimacion->timer < 300)
	{
		controlAnimacion->transparencia1--;
	}

	al_draw_filled_rectangle(0, 0, 2000, 2000, al_map_rgba(0, 0, 0, controlAnimacion->transparencia1));

	if (controlAnimacion->timer > 200 && controlAnimacion->timer < 400)
	{
		controlAnimacion->transparencia2--;
	}

	al_draw_filled_rectangle(100, 250, 1000, 300, al_map_rgba(0, 0, 0, controlAnimacion->transparencia2));

	if (controlAnimacion->timer > 400 && controlAnimacion->timer < 600)
	{
		controlAnimacion->transparencia3--;
	}

	al_draw_filled_rectangle(100, 850, 1500, 900, al_map_rgba(0, 0, 0, controlAnimacion->transparencia3));

	if (controlAnimacion->timer > 600 && controlAnimacion->timer < 800)
	{
		controlAnimacion->transparencia4--;
	}

	al_draw_filled_rectangle(100, 950, 1500, 1000, al_map_rgba(0, 0, 0, controlAnimacion->transparencia4));

	if (controlAnimacion->timer > 800 && controlAnimacion->transparencia5 != 255)
	{
		controlAnimacion->transparencia5++;
	}

	al_draw_filled_rectangle(0, 0, 2000, 2000, al_map_rgba(0, 0, 0, controlAnimacion->transparencia5));

	////////////////////////////////////////////////
	al_flip_display();
}

void Sonido(controlAudio_ *controlAudio, int indiceSonido)
{
	if (indiceSonido == 1)
	{
		controlAudio->sonido = al_load_sample("audio_moneda.ogg");

		al_play_sample(controlAudio->sonido, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
	}
	else if (indiceSonido == 2)
	{
		controlAudio->sonido = al_load_sample("audio_disparo.ogg");

		al_play_sample(controlAudio->sonido, 0.6, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
	}
	else if (indiceSonido == 3)
	{
		controlAudio->sonido = al_load_sample("audio_boton.ogg");

		al_play_sample(controlAudio->sonido, 0.5, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
	}
	else if (indiceSonido == 4)
	{
		controlAudio->sonido = al_load_sample("audio_danho.ogg");

		al_play_sample(controlAudio->sonido, 1.5, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
	}
	else if (indiceSonido == 5)
	{
		controlAudio->sonido = al_load_sample("audio_mejora.ogg");

		al_play_sample(controlAudio->sonido, 1.5, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
	}
	else if (indiceSonido == 6)
	{
		controlAudio->sonido = al_load_sample("audio_pickup.ogg");

		al_play_sample(controlAudio->sonido, 2.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
	}
	else if (indiceSonido == 7)
	{
		controlAudio->sonido = al_load_sample("audio_chest.ogg");

		al_play_sample(controlAudio->sonido, 1.5, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
	}
	else if (indiceSonido == 8)
	{
		controlAudio->sonido = al_load_sample("audio_choque.ogg");

		al_play_sample(controlAudio->sonido, 0.7, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
	}
	else if (indiceSonido == 9)
	{
		controlAudio->sonido = al_load_sample("audio_fogata.ogg");

		al_play_sample(controlAudio->sonido, 1.3, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
	}
	else if (indiceSonido == 10)
	{
		//Disparo de enemigos
		controlAudio->sonido = al_load_sample("audio_disparo.ogg");

		al_play_sample(controlAudio->sonido, 0.6, 0.0, 2.0, ALLEGRO_PLAYMODE_ONCE, NULL);
	}
	else if (indiceSonido == 11)
	{
		controlAudio->sonido = al_load_sample("audio_saltoSlime.ogg");

		al_play_sample(controlAudio->sonido, 0.6, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
	}
	else if (indiceSonido == 12)
	{
		controlAudio->sonido = al_load_sample("audio_caminar1.ogg");

		al_play_sample(controlAudio->sonido, 0.3, 0.0, 2.0, ALLEGRO_PLAYMODE_ONCE, NULL);
	}

	if (controlAudio->sonido != NULL)
	{
		printf("ERROR AL CARGAR SONIDO %d", indiceSonido);
	}
}

void Musica(controlAudio_ *controlAudio, char *nombreMusica)
{
	//Si hay pista al llamar Musica(), se detruye para poner la siguiente
	if (controlAudio->musica != NULL) 
	{
    	al_destroy_audio_stream(controlAudio->musica);
	}

	//Se asigna el mp3 a una variable tipo audio stream
	controlAudio->musica = al_load_audio_stream(nombreMusica, 4, 2048);

    if (!controlAudio->musica)
    {
        printf("ERROR MUSICA");
    }

	//Mode de musica, loop
	al_set_audio_stream_playmode(controlAudio->musica, ALLEGRO_PLAYMODE_LOOP);

	//contrl del volumen de la musica
	al_set_audio_stream_gain(controlAudio->musica, 0.4);

	//Se conecta el audio al mezclador
	al_attach_audio_stream_to_mixer(controlAudio->musica, al_get_default_mixer());
}

void SetRanking(FILE *archivoRanking, ranking_ *ranking, char RegistrarJugador[LARGO_TEXTO], int puntajeDelJugador)
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

void GetRanking(FILE *archivoRanking, ranking_ *ranking)
{
	//Variables donde se guardan los textos leidos
	char nombreLeido[LARGO_TEXTO];
	int puntajeLeido;
	char linea[100];
	int auxPuntajes;
	char auxNombres[LARGO_TEXTO];

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

	//Ordenar el ranking de mayor a menor
	for (int j = 0; j < ranking->indicePuntajes; j++)
	{
		for (int i = 0; i < ranking->indicePuntajes - j; i++)
		{
			if (ranking->puntajes[i] < ranking->puntajes[i + 1])
			{
				auxPuntajes = ranking->puntajes[i];
				ranking->puntajes[i] = ranking->puntajes[i + 1];
				ranking->puntajes[i + 1] = auxPuntajes;

				strcpy(auxNombres, ranking->nombres[i]);
				strcpy(ranking->nombres[i], ranking->nombres[i + 1]);
				strcpy(ranking->nombres[i + 1], auxNombres);
			}
		}
	}

	fclose(archivoRanking);
}

void LogicaMenu(estadoJuego_ *estadoJuego, controlMenu_ *controlMenu, ranking_ *ranking, ALLEGRO_EVENT_QUEUE *colaEventos, FILE *archivoRanking, estadoSistema_ *estadoSistema, controlAudio_ *controlAudio)
{
	//Cuando el mouse posa sobre el cuadrado de jugar
	if (Colicion(mouse.posX, mouse.posY, mouse.tamanho, mouse.tamanho, 840, 524, 204, 84) && controlMenu->verdaderaPantallaRanking == 0 && controlMenu->pantallaPonerNombre == 0)
	{
		if(al_mouse_button_down(&estadoSistema->mouse, ALLEGRO_MOUSE_BUTTON_LEFT))
		{
			controlMenu->pantallaPonerNombre = 1;
		}

		if (controlMenu->mouseEnPlay == 0)
		{
			Sonido(controlAudio, 3);
		}
		
		controlMenu->mouseEnPlay = 1;
	}

	//Si el mouse esta fuera del boton play, mouse en play desactivado
	if (!Colicion(mouse.posX, mouse.posY, mouse.tamanho, mouse.tamanho, 840, 524, 204, 84) && controlMenu->verdaderaPantallaRanking == 0)
	{		
		controlMenu->mouseEnPlay = 0;
	}

	//boton pantalla ranking
	if (Colicion(mouse.posX, mouse.posY, mouse.tamanho, mouse.tamanho, 840, 624, 204, 84) && controlMenu->verdaderaPantallaRanking == 0 && controlMenu->pantallaPonerNombre == 0)
	{
		if(al_mouse_button_down(&estadoSistema->mouse, ALLEGRO_MOUSE_BUTTON_LEFT))
		{
			controlMenu->verdaderaPantallaRanking = 1;

			GetRanking(archivoRanking, ranking);
		}

		if (controlMenu->mouseEnRanking == 0)
		{
			Sonido(controlAudio, 3);
		}
		
		controlMenu->mouseEnRanking = 1;
	}

	//Si el mouse esta fuera del boton ranking, mouse en ranking descativado
	if (!Colicion(mouse.posX, mouse.posY, mouse.tamanho, mouse.tamanho, 840, 624, 204, 84) && controlMenu->verdaderaPantallaRanking == 0)
	{		
		controlMenu->mouseEnRanking = 0;
	}

	//Boton volver al menu
	if (Colicion(mouse.posX, mouse.posY, mouse.tamanho, mouse.tamanho, 100, 930, 200, 70) && controlMenu->verdaderaPantallaRanking == 1)
	{
		if(al_mouse_button_down(&estadoSistema->mouse, ALLEGRO_MOUSE_BUTTON_LEFT))
		{
			controlMenu->verdaderaPantallaRanking = 0;
		}

		if (controlMenu->mouseEnVolver == 0)
		{
			Sonido(controlAudio, 3);
		}
		
		controlMenu->mouseEnVolver = 1;
	}

	//Si el mouse esta fuera del boton volver, mouse en volver descativado
	if (!Colicion(mouse.posX, mouse.posY, mouse.tamanho, mouse.tamanho, 100, 930, 200, 70) && controlMenu->verdaderaPantallaRanking == 1)
	{		
		controlMenu->mouseEnVolver = 0;
	}
}

void RenderMenu(ALLEGRO_FONT *fuenteJuego, ALLEGRO_BITMAP *menuFondo, controlMenu_ *controlMenu, ranking_ *ranking, char mapa[FILAS_HABITACION][COLUMNAS_HABITACION], ALLEGRO_BITMAP *spriteSheet, estadoSistema_ *estadoSistema, gestionInteractuables_ *gestionInteractuables)
{
	estadoSistema->controlSpritesMenu++;

	//Mostrar Menu
	if (controlMenu->pantallaPonerNombre == 0 && controlMenu->verdaderaPantallaRanking == 0)
	{
		al_clear_to_color(al_map_rgb(0, 0, 0));
		//////////////////////////////////////////////// Dibujar en este espacio

		DibujarMapa(mapa, spriteSheet, estadoSistema, gestionInteractuables);

		//al_draw_scaled_bitmap(menuFondo, 0, 0, LARGO_PANTALLA, ANCHO_PANTALLA, 0, 0, LARGO_PANTALLA + 760, ANCHO_PANTALLA + 435, 0);

		//Boton jugar (En negro para disimular con fondo)
		al_draw_filled_rectangle(840, 524, 1044, 608, al_map_rgb(0, 0, 0));

		if (controlMenu->mouseEnPlay == 1)
		{
			al_draw_text(fuenteJuego, al_map_rgb(255, 255, 0), 850, 550, 0, "Jugar");
		}
		else
		{
			al_draw_text(fuenteJuego, al_map_rgb(255, 255, 255), 850, 550, 0, "Jugar");
		}

		//Boton ranking
		al_draw_filled_rectangle(840, 624, 1044, 708, al_map_rgb(0, 0, 0));

		if (controlMenu->mouseEnRanking == 1)
		{
			al_draw_text(fuenteJuego, al_map_rgb(255, 255, 0), 830, 650, 0, "Ranking");
		}
		else
		{
			al_draw_text(fuenteJuego, al_map_rgb(255, 255, 255), 830, 650, 0, "Ranking");
		}
		
		////////////////////////////////////////////////
		al_flip_display();
	}
	
	//Escribir nombre para empezar el juego
	if (controlMenu->pantallaPonerNombre == 1)
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

		//Solo se muestra los primeros 20 jugadores
		for (int i = 0; i < 20; i++)
		{
			al_draw_textf(fuenteJuego, al_map_rgb(255, 255, 255), 400, 100 + i * 40, 0, "%s", ranking->nombres[i]);
			al_draw_textf(fuenteJuego, al_map_rgb(255, 255, 255), 1500, 100 + i * 40, 0, "%d", ranking->puntajes[i]);
		}

		//al_draw_filled_rectangle(100, 930, 300, 1000, al_map_rgb(0, 0, 0));
	
		if (controlMenu->mouseEnVolver == 1)
		{
			al_draw_text(fuenteJuego, al_map_rgb(255, 255, 0), 100, 950, 0, "volver");
		}
		else 
		{
			al_draw_text(fuenteJuego, al_map_rgb(255, 255, 255), 100, 950, 0, "volver");
		}
		
		////////////////////////////////////////////////
		al_flip_display();	
	}
}

void LogicaReiniciar(estadoJuego_ *estadoJuego, estadoSistema_ *estadoSistema, controlMenu_ *controlMenu, FILE *archivoRanking, ranking_ *ranking, controlAudio_ *controlAudio)
{
	//Pantalla reiniciar
	if (controlMenu->pantallaReiniciar)
	{
		//Si el mouse está dentro del rectangulo del boton
		if (Colicion(mouse.posX, mouse.posY, mouse.tamanho, mouse.tamanho, 800, 500, 300, 100))
		{
			if(al_mouse_button_down(&estadoSistema->mouse, ALLEGRO_MOUSE_BUTTON_LEFT))
			{
				estadoJuego->JUEGO = 1;
				estadoJuego->REINICIAR = 0;
			}

			if (!controlMenu->mouseEnReiniciar)
			{
				Sonido(controlAudio, 3);
			}

			controlMenu->mouseEnReiniciar = 1;
		}

		if (Colicion(mouse.posX, mouse.posY, mouse.tamanho, mouse.tamanho, 730, 720, 500, 100))
		{
			if(al_mouse_button_down(&estadoSistema->mouse, ALLEGRO_MOUSE_BUTTON_LEFT))
			{
				estadoJuego->MENU = 1;
				estadoJuego->REINICIAR = 0;
			}

			if (!controlMenu->mouseEnVolverAlMenu)
			{
				Sonido(controlAudio, 3);
			}
			
			controlMenu->mouseEnVolverAlMenu = 1;
		}

		if (Colicion(mouse.posX, mouse.posY, mouse.tamanho, mouse.tamanho, 800, 610, 300, 100))
		{
			if(al_mouse_button_down(&estadoSistema->mouse, ALLEGRO_MOUSE_BUTTON_LEFT))
			{
				controlMenu->verdaderaPantallaRanking = 1;
				controlMenu->pantallaReiniciar = 0;

				GetRanking(archivoRanking, ranking);
			}

			if (!controlMenu->mouseEnRanking)
			{
				Sonido(controlAudio, 3);
			}

			controlMenu->mouseEnRanking = 1;
		}

		//Si el mouse está fuera de los rectangulos reiniciar, volver al menu y ranking
		if (!Colicion(mouse.posX, mouse.posY, mouse.tamanho, mouse.tamanho, 800, 500, 300, 100))
		{
			controlMenu->mouseEnReiniciar = 0;
		}

		if (!Colicion(mouse.posX, mouse.posY, mouse.tamanho, mouse.tamanho, 800, 610, 300, 100))
		{
			controlMenu->mouseEnRanking = 0;
		}

		if (!Colicion(mouse.posX, mouse.posY, mouse.tamanho, mouse.tamanho, 730, 720, 500, 100))
		{
			controlMenu->mouseEnVolverAlMenu = 0;
		}
	}

	//pantalla ranking
	if (controlMenu->verdaderaPantallaRanking)
	{
		if (Colicion(mouse.posX, mouse.posY, mouse.tamanho, mouse.tamanho, 100, 930, 200, 70))
		{
			if(al_mouse_button_down(&estadoSistema->mouse, ALLEGRO_MOUSE_BUTTON_LEFT))
			{
				controlMenu->verdaderaPantallaRanking = 0;
				controlMenu->pantallaReiniciar = 1;
			}

			if (controlMenu->mouseEnVolver == 0)
			{
				Sonido(controlAudio, 3);
			}
			
			controlMenu->mouseEnVolver = 1;
		}

		//Si el mouse esta fuera del boton volver, mouse en volver descativado
		if (!Colicion(mouse.posX, mouse.posY, mouse.tamanho, mouse.tamanho, 100, 930, 200, 70))
		{		
			controlMenu->mouseEnVolver = 0;
		}
	}
}

void RenderReiniciar(ALLEGRO_FONT *fuenteJuego, controlMenu_ *controlMenu, ranking_ *ranking)
{
	al_clear_to_color(al_map_rgb(0, 0, 0));
	//////////////////////////////////////////////// Dibujar en este espacio
	
	if (controlMenu->pantallaReiniciar)
	{
		al_draw_filled_rectangle(800, 500, 1100, 600, al_map_rgb(0, 0, 0));

		if (controlMenu->mouseEnReiniciar)
		{
			al_draw_text(fuenteJuego, al_map_rgb(255, 255, 0), 810, 530, 0, "Reiniciar");
		}
		else 
		{
			al_draw_text(fuenteJuego, al_map_rgb(255, 255, 255), 810, 530, 0, "Reiniciar");
		}

		al_draw_filled_rectangle(800, 610, 1100, 710, al_map_rgb(0, 0, 0));

		if (controlMenu->mouseEnRanking)
		{
			al_draw_text(fuenteJuego, al_map_rgb(255, 255, 0), 840, 640, 0, "Ranking");
		} 
		else
		{
			al_draw_text(fuenteJuego, al_map_rgb(255, 255, 255), 840, 640, 0, "Ranking");
		}

		al_draw_filled_rectangle(730, 720, 1230, 820, al_map_rgb(0, 0, 0));

		if (controlMenu->mouseEnVolverAlMenu)
		{
			al_draw_text(fuenteJuego, al_map_rgb(255, 255, 0), 750, 740, 0, "Volver al menu");
		}
		else
		{
			al_draw_text(fuenteJuego, al_map_rgb(255, 255, 255), 750, 740, 0, "Volver al menu");
		}
	}	

	if (controlMenu->verdaderaPantallaRanking)
	{
		al_draw_text(fuenteJuego, al_map_rgb(255, 255, 255), 150, 50, 0, "=====================RANKING=====================");

		for (int i = 0; i < ranking->indiceNombres; i++)
		{
			al_draw_textf(fuenteJuego, al_map_rgb(255, 255, 255), 400, 100 + i * 40, 0, "%s", ranking->nombres[i]);
			al_draw_textf(fuenteJuego, al_map_rgb(255, 255, 255), 1500, 100 + i * 40, 0, "%d", ranking->puntajes[i]);
		}

		//al_draw_filled_rectangle(100, 930, 300, 1000, al_map_rgb(0, 0, 0));
	
		if (controlMenu->mouseEnVolver == 1)
		{
			al_draw_text(fuenteJuego, al_map_rgb(255, 255, 0), 100, 950, 0, "volver");
		}
		else 
		{
			al_draw_text(fuenteJuego, al_map_rgb(255, 255, 255), 100, 950, 0, "volver");
		}
	}
	
	////////////////////////////////////////////////
	al_flip_display();
}

void Logica(char mapa[FILAS_HABITACION][COLUMNAS_HABITACION], jugador *jugador, estadoJuego_ *estadoJuego, controlIndices_ *controlIndices, gestionEnemigos_ *gestionEnemigos, gestionObjetos_ *gestionObjetos, estadoSistema_ *estadoSistema, estadoMapa_ *estadoMapa, gestionDianas_ *gestionDianas, gestionInteractuables_ *gestionInteractuables, registroMundo_ *registroMundo, ranking_ *ranking, FILE *archivoRanking, controlAudio_ *controlAudio)
{
	MovimientoJugador(estadoSistema, estadoMapa, controlAudio);

	PersonajeInvulnerable();

	Disparo(estadoSistema, estadoMapa, controlAudio);

	HandicapsMejorables();

	//MovimientoCamara();

	LogicaEnemigos(gestionEnemigos, gestionObjetos, estadoMapa, registroMundo, controlAudio);

	VerificarTraspasoPuertas(mapa, jugador);

	ColicionObjetos(gestionObjetos, estadoMapa, registroMundo, controlAudio);

	ColicionInteractuables(estadoMapa, gestionDianas, gestionInteractuables, registroMundo, controlAudio);

	CambioDeHabitaciones(controlIndices, gestionEnemigos, gestionObjetos, estadoMapa, gestionDianas, gestionInteractuables, registroMundo);

	VerificarSalaVacia(gestionEnemigos, estadoMapa);

	GeneracionDeRecompensas(gestionObjetos, estadoMapa, registroMundo);

	LogicaDianas(estadoMapa, gestionDianas, controlAudio);
	
	//Axis del mouse
	al_get_mouse_num_axes();

////////////////////////////////////////////////////////////////////////////////////////////////////////// gestion eventos

	//Verificar dianas
	gestionDianas->cantidadDianasDestruidas = 0;

	for (int i = 0; i < MAX_REGISTROS; i++)
	{
		if (registroMundo->registroInteractuables[i].dianaDestruida != 0)
		{
			gestionDianas->cantidadDianasDestruidas++;

			if (gestionDianas->cantidadDianasDestruidas >= 5 && gestionObjetos->mapaVerde.seGenero == 0)
			{
				gestionObjetos->mapaVerde.activa = 1;
				gestionObjetos->mapaVerde.posX = 8 * TAMANHO;
				gestionObjetos->mapaVerde.posY = 3 * TAMANHO;
				gestionObjetos->mapaVerde.seGenero = 1;

				registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].mapaX = estadoMapa->actualMapaX;
				registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].mapaY = estadoMapa->actualMapaY;
				registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].posX = gestionObjetos->mapaVerde.posX;
				registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].posY = gestionObjetos->mapaVerde.posY;
				registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].idItem = 'v';
				registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].seRecogioElItem = 0;
				registroMundo->cantidadRecompensas++;
			}
		}
	}
	
	//Verificar el evento de fogatas
	if (gestionInteractuables->cantidadfogatasActivas >= 4)
	{
		if (estadoMapa->actualMapaX == COLUMNAS_MAPA / 2 + 1 && estadoMapa->actualMapaY == FILAS_MAPA / 2 + 1)
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

//////////////////////////////////////////////////////////////////////////////// Ganar o perder
	
	//Cambiar por una pantalla de PERDISTE o reactivar el MENU
	if (personaje.vidas == 0)
	{
		estadoJuego->JUEGO = 0;
		estadoJuego->REINICIAR = 1;

		SetRanking(archivoRanking, ranking, ranking->nombre, personaje.puntaje);
	}

	//TERMINAR EL JUEGO
	if (gestionObjetos->mapaRojo.seObtuvo == 1 && gestionObjetos->mapaAzul.seObtuvo == 1 && gestionObjetos->mapaNaranjo.seObtuvo == 1 && gestionObjetos->mapaVerde.seObtuvo == 1 && estadoJuego->REINICIAR == 0)
	{
		estadoJuego->JUEGO = 0;
		estadoJuego->REINICIAR = 1;

		SetRanking(archivoRanking, ranking, ranking->nombre, personaje.puntaje);
	}

	////////////////////////////////////////////////////////////////////// rellenar minimapa

	for (int i = 0; i < FILAS_MAPA; i++)
	{
		for (int j = 0; j < COLUMNAS_MAPA; j++)
		{
			if (estadoMapa->mapa[i][j] != NULL)
			{
				estadoMapa->minimapa[i][j] = 1;
			}
			else if (estadoMapa->mapa[i][j] == NULL)
			{
				estadoMapa->minimapa[i][j] = 0;
			}
		}
	}
}

void Disparo(estadoSistema_ *estadoSistema, estadoMapa_ *estadoMapa, controlAudio_ *controlAudio)
{
	//Como Disparo() se encuentra en while, cada llamada se va acumulando en cadencia, lo usaremos como una especie de timer
	personaje.cadencia++;

	if(al_mouse_button_down(&estadoSistema->mouse, ALLEGRO_MOUSE_BUTTON_LEFT) && personaje.cadencia > 20)
	{
		//Desactivar boton tutorial
		mouse.seClickeo = 1;

		for (int i = 0; i < MAX_BALAS; i++)
		{
			if (!personaje.bala[i].seDisparo)
			{
				//La bala nace del centro del personaje
				personaje.bala[i].posX = personaje.posX + (TAMANHO/2);
				personaje.bala[i].posY = personaje.posY + (TAMANHO/2);

				//Se registra la posicion inicial de la bala
				personaje.bala[i].posXNacimiento = personaje.posX + (TAMANHO/2);
				personaje.bala[i].posYNacimiento = personaje.posY + (TAMANHO/2);

				//Se activa la bala 
				personaje.bala[i].activa = 1;
				personaje.bala[i].seDisparo = 1;

				//Informa al jugador de cuantas balas qeudan disponibles (se dibuja en pantalla)
				personaje.balasrestantes--;

				//Se obtiene el arcotangente en radianes entre la posicion del mouse y posicion del personaje
				personaje.bala[i].direccionBala = atan2(mouse.posY - personaje.posY, mouse.posX - personaje.posX); //atan2(y2 - y1, x2 - x1)

				//Se obtiene el angulo en de las balas
				personaje.bala[i].anguloBalaX = cos(personaje.bala[i].direccionBala) * personaje.bala[i].velocidad;
				personaje.bala[i].anguloBalaY = sin(personaje.bala[i].direccionBala) * personaje.bala[i].velocidad;

				//se reinicia el timer entre disparos
				personaje.cadencia = 0;

				//Sonido Disparo
				Sonido(controlAudio, 2);

				break;
			}
		}
	}

	//Aumenta constantemente personaje.bala[i].posY/personaje.bala[i].posX
	for (int i = 0; i < MAX_BALAS; i++)
	{
		//Si la bala esta activa su posicion aumenta en favor del angulo
		if (personaje.bala[i].activa != 0)
		{
			personaje.bala[i].posX += personaje.bala[i].anguloBalaX;
			personaje.bala[i].posY += personaje.bala[i].anguloBalaY;
		}
	}

	ColisionMapaBalas(estadoMapa->sala);
}

void DisparoEnemigos(gestionEnemigos_ *gestionEnemigos, estadoMapa_ *estadoMapa, controlAudio_ *controlAudio)
{
	int aux = 0;

	///////////////////////////////////////////////////////////// DISPARO JEFE

	gestionEnemigos->jefe.cadencia ++;

	if (gestionEnemigos->jefe.cadencia > 20 && gestionEnemigos->jefe.activa != 0)
	{
		gestionEnemigos->jefe.bala[gestionEnemigos->jefe.balaActual].posX = gestionEnemigos->jefe.posX + TAMANHO;
		gestionEnemigos->jefe.bala[gestionEnemigos->jefe.balaActual].posY = gestionEnemigos->jefe.posY + TAMANHO;
		gestionEnemigos->jefe.bala[gestionEnemigos->jefe.balaActual].posXNacimiento = gestionEnemigos->jefe.posX + TAMANHO;
		gestionEnemigos->jefe.bala[gestionEnemigos->jefe.balaActual].posYNacimiento = gestionEnemigos->jefe.posY + TAMANHO;
		gestionEnemigos->jefe.bala[gestionEnemigos->jefe.balaActual].activa = 1;

		Sonido(controlAudio, 10);

		gestionEnemigos->jefe.bala[gestionEnemigos->jefe.balaActual].direccionBala = atan2(personaje.posY - gestionEnemigos->jefe.posY, personaje.posX - gestionEnemigos->jefe.posX); //atan2(y2 - y1, x2 - x1)

		gestionEnemigos->jefe.bala[gestionEnemigos->jefe.balaActual].anguloBalaX = cos(gestionEnemigos->jefe.bala[gestionEnemigos->jefe.balaActual].direccionBala) * gestionEnemigos->jefe.bala[gestionEnemigos->jefe.balaActual].velocidad;
		gestionEnemigos->jefe.bala[gestionEnemigos->jefe.balaActual].anguloBalaY = sin(gestionEnemigos->jefe.bala[gestionEnemigos->jefe.balaActual].direccionBala) * gestionEnemigos->jefe.bala[gestionEnemigos->jefe.balaActual].velocidad;

		gestionEnemigos->jefe.balaActual++;
		gestionEnemigos->jefe.cadencia = 0;
	}

	//Cuando el arreglo este a punto de terminar, se reinicia
	if (gestionEnemigos->jefe.balaActual > MAX_BALAS - 1)
	{
		gestionEnemigos->jefe.balaActual = 0;
	}

	for (int i = 0; i < MAX_BALAS; i++)
	{
		if (gestionEnemigos->jefe.bala[i].activa != 0)
		{
			gestionEnemigos->jefe.bala[i].posX += gestionEnemigos->jefe.bala[i].anguloBalaX;
			gestionEnemigos->jefe.bala[i].posY += gestionEnemigos->jefe.bala[i].anguloBalaY;
		}

		if (ColisionMapa(estadoMapa->sala, gestionEnemigos->jefe.bala[i].posX, gestionEnemigos->jefe.bala[i].posY) || 
		ColisionMapa(estadoMapa->sala, gestionEnemigos->jefe.bala[i].posX + (TAMANHO/4) - 1, gestionEnemigos->jefe.bala[i].posY) || 
		ColisionMapa(estadoMapa->sala, gestionEnemigos->jefe.bala[i].posX + (TAMANHO/4) - 1, gestionEnemigos->jefe.bala[i].posY + (TAMANHO/4) - 1) ||
		ColisionMapa(estadoMapa->sala, gestionEnemigos->jefe.bala[i].posX, gestionEnemigos->jefe.bala[i].posY + (TAMANHO/4) - 1))
		{
			gestionEnemigos->jefe.bala[i].activa = 0;
		}

		if (gestionEnemigos->jefe.bala[i].posX > gestionEnemigos->jefe.bala[i].posXNacimiento + gestionEnemigos->jefe.rangoDeBalas || gestionEnemigos->jefe.bala[i].posX < gestionEnemigos->jefe.bala[i].posXNacimiento - gestionEnemigos->jefe.rangoDeBalas || gestionEnemigos->jefe.bala[i].posY > gestionEnemigos->jefe.bala[i].posYNacimiento + gestionEnemigos->jefe.rangoDeBalas || gestionEnemigos->jefe.bala[i].posY < gestionEnemigos->jefe.bala[i].posYNacimiento - gestionEnemigos->jefe.rangoDeBalas)
		{
			gestionEnemigos->jefe.bala[i].activa = 0;
		}
	}

	/////////////////////////////////////////////////////////////// LOGICA MAGOS

	for (int i = 0; i < MAX_ENEMIGOS; i++)
	{
		gestionEnemigos->mago[i].cadencia++;

		if (gestionEnemigos->mago[i].cadencia > 20 && gestionEnemigos->mago[i].activa != 0 && gestionEnemigos->mago[i].ataquesEnemigos == 1)
		{
			for (int j = 0; j < MAX_BALAS; j++)
			{
				if (gestionEnemigos->mago[i].bala[j].activa == 0)
				{
					gestionEnemigos->mago[i].bala[j].posX = gestionEnemigos->mago[i].posX;
					gestionEnemigos->mago[i].bala[j].posY = gestionEnemigos->mago[i].posY;
					gestionEnemigos->mago[i].bala[j].posXNacimiento = gestionEnemigos->mago[i].posX;
					gestionEnemigos->mago[i].bala[j].posYNacimiento = gestionEnemigos->mago[i].posY;
					gestionEnemigos->mago[i].bala[j].activa = 1;

					Sonido(controlAudio, 10);

					gestionEnemigos->mago[i].bala[j].direccionBalaEnemigo = atan2(personaje.posY - gestionEnemigos->mago[i].posY, personaje.posX - gestionEnemigos->mago[i].posX); //atan2(y2 - y1, x2 - x1)

					gestionEnemigos->mago[i].bala[j].anguloBalaX = cos(gestionEnemigos->mago[i].bala[j].direccionBalaEnemigo) * gestionEnemigos->mago[i].bala[j].velocidad;
					gestionEnemigos->mago[i].bala[j].anguloBalaY = sin(gestionEnemigos->mago[i].bala[j].direccionBalaEnemigo) * gestionEnemigos->mago[i].bala[j].velocidad;

					aux = 1;
					
					break;
				}
			}
		}	

		if (aux == 1)
		{
			gestionEnemigos->mago[i].cadencia = 0;
		}
	}
	
	
	for (int i = 0; i < MAX_ENEMIGOS; i++)
	{
		for (int j = 0; j < MAX_BALAS; j++)
		{
			if (gestionEnemigos->mago[i].bala[j].activa != 0)
			{
				gestionEnemigos->mago[i].bala[j].posX += gestionEnemigos->mago[i].bala[j].anguloBalaX;
				gestionEnemigos->mago[i].bala[j].posY += gestionEnemigos->mago[i].bala[j].anguloBalaY;
			}

			if (ColisionMapa(estadoMapa->sala, gestionEnemigos->mago[i].bala[j].posX, gestionEnemigos->mago[i].bala[j].posY) || 
			ColisionMapa(estadoMapa->sala, gestionEnemigos->mago[i].bala[j].posX + (TAMANHO/4) - 1, gestionEnemigos->mago[i].bala[j].posY) || 
			ColisionMapa(estadoMapa->sala, gestionEnemigos->mago[i].bala[j].posX + (TAMANHO/4) - 1, gestionEnemigos->mago[i].bala[j].posY + (TAMANHO/4) - 1) ||
			ColisionMapa(estadoMapa->sala, gestionEnemigos->mago[i].bala[j].posX, gestionEnemigos->mago[i].bala[j].posY + (TAMANHO/4) - 1))
			{
				gestionEnemigos->mago[i].bala[j].activa = 0;
			}

			//Destruccion de balas en funcion de la posicion del enemigo para que el arreglo nunca se acabe
			if (gestionEnemigos->mago[i].bala[j].posX > gestionEnemigos->mago[i].bala[j].posXNacimiento + gestionEnemigos->mago[i].rangoDeBalas || gestionEnemigos->mago[i].bala[j].posX < gestionEnemigos->mago[i].bala[j].posXNacimiento - gestionEnemigos->mago[i].rangoDeBalas || gestionEnemigos->mago[i].bala[j].posY > gestionEnemigos->mago[i].bala[j].posYNacimiento + gestionEnemigos->mago[i].rangoDeBalas ||gestionEnemigos->mago[i].bala[j].posY < gestionEnemigos->mago[i].bala[j].posYNacimiento - gestionEnemigos->mago[i].rangoDeBalas)
			{
				gestionEnemigos->mago[i].bala[j].activa = 0;
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

void MovimientoJugador(estadoSistema_ *estadoSistema, estadoMapa_ *estadoMapa, controlAudio_ *controlAudio)
{
	//Guardamos la posicion del jugador en un axiliar
	int auxX = personaje.posX;
	int auxY = personaje.posY;

	//Control de audio de caminar del jugador
	if (controlAudio->inciciarTimerPasosJugador == 1)
	{
		controlAudio->audioTimerPasosJugador++;
	}

	if (controlAudio->audioTimerPasosJugador == 19)
	{
		controlAudio->inciciarTimerPasosJugador = 0;
		controlAudio->audioTimerPasosJugador = 0;
	}

	//Si se presiona A o S o D o W empieza el audio
	if (al_key_down(&estadoSistema->teclado, ALLEGRO_KEY_W) || al_key_down(&estadoSistema->teclado, ALLEGRO_KEY_S) || al_key_down(&estadoSistema->teclado, ALLEGRO_KEY_D) || al_key_down(&estadoSistema->teclado, ALLEGRO_KEY_A))
	{
		if (controlAudio->audioTimerPasosJugador == 0)
		{
			Sonido(controlAudio, 12);
			controlAudio->inciciarTimerPasosJugador = 1;
		}
	}

	////////////////////////////////////////////////////////////////////////////7
	
	if(al_key_down(&estadoSistema->teclado, ALLEGRO_KEY_W))
	{
		personaje.posY -= personaje.velocidad; 
		personaje.animacionJugador = 1;
		personaje.seMovio = 1;
	}

	if(al_key_down(&estadoSistema->teclado, ALLEGRO_KEY_S))
	{
		personaje.posY += personaje.velocidad; 
		personaje.animacionJugador = 1;
		personaje.seMovio = 1;
	}

	//Luego de verificar la posY se comprueba si cada esquina del jugador está en colision con '#' en el arreglo del mapa
	if (ColisionMapa(estadoMapa->sala, personaje.posX, personaje.posY) || 
	ColisionMapa(estadoMapa->sala, personaje.posX + TAMANHO - 1, personaje.posY) || 
	ColisionMapa(estadoMapa->sala, personaje.posX + TAMANHO - 1, personaje.posY + TAMANHO - 1) ||
	ColisionMapa(estadoMapa->sala, personaje.posX, personaje.posY + TAMANHO - 1))
	{
		personaje.posY = auxY;
	}

	if(al_key_down(&estadoSistema->teclado, ALLEGRO_KEY_D))
	{
		personaje.posX += personaje.velocidad; 
		personaje.dirJugador.derecha = 1;
		personaje.dirJugador.izquierda = 0;
		personaje.animacionJugador = 1;
		personaje.seMovio = 1;
	}

	if(al_key_down(&estadoSistema->teclado, ALLEGRO_KEY_A))
	{
		personaje.posX -= personaje.velocidad; 
		personaje.dirJugador.derecha = 0;
		personaje.dirJugador.izquierda = 1;
		personaje.animacionJugador = 1;
		personaje.seMovio = 1;
	}

	if (ColisionMapa(estadoMapa->sala, personaje.posX, personaje.posY) || 
	ColisionMapa(estadoMapa->sala, personaje.posX + TAMANHO - 1, personaje.posY) || 
	ColisionMapa(estadoMapa->sala, personaje.posX + TAMANHO - 1, personaje.posY + TAMANHO - 1) ||
	ColisionMapa(estadoMapa->sala, personaje.posX, personaje.posY + TAMANHO - 1))
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

void DesactivarObjetosActivos(gestionEnemigos_ *gestionEnemigos,  controlIndices_ *controlIndices, gestionObjetos_ *gestionObjetos, estadoMapa_ *estadoMapa, gestionDianas_ *gestionDianas, gestionInteractuables_ *gestionInteractuables)
{
	controlIndices->aranha = 0;
	controlIndices->mago = 0;
	controlIndices->slime = 0;
	gestionObjetos->mejoraDanhoActual = 0;
	gestionObjetos->mejoraRangoActual = 0;
	gestionObjetos->mejoraVelocidadActual = 0;
	estadoMapa->salaVacia = 0;
	estadoMapa->seGeneroUnaRecompensa = 0;
	gestionDianas->dianasActuales = 0;

	//Reinicio de enemigos
	for (int k = 0; k < MAX_ENEMIGOS; k++)
	{
		gestionEnemigos->slime[k].activa = 0;
		gestionEnemigos->slime[k].ataquesEnemigos = 0;

		gestionEnemigos->aranha[k].activa = 0;
	}

	gestionEnemigos->jefe.activa = 0;

	//Reinicio de balas
	for (int l = 0; l < MAX_ENEMIGOS; l++)
	{
		personaje.bala[l].activa = 0;
		gestionEnemigos->jefe.bala[l].activa = 0;
	}

	for (int i = 0; i < MAX_ENEMIGOS; i++)
	{
		for (int j = 0; j < MAX_BALAS; j++)
		{
			gestionEnemigos->mago[i].activa = 0;
			gestionEnemigos->mago[i].bala[j].activa = 0;
			gestionEnemigos->mago[i].ataquesEnemigos = 0;
		}
	}

	//Reinicio objetos
	for (int i = 0; i < MAX_OBJETOS; i++)
	{
		gestionObjetos->monedas[i].activa = 0;
		gestionObjetos->llaves[i].activa = 0;
		gestionObjetos->cofres[i].activa = 0;
		gestionObjetos->municiones[i].activa = 0;
		gestionObjetos->mejoraDanho[i].activa = 0;
		gestionObjetos->mejoraRango[i].activa = 0;
		gestionObjetos->mejoraVelocidad[i].activa = 0;
		gestionObjetos->mejoraDanho[i].seVende = 0;
		gestionObjetos->mejoraRango[i].seVende = 0;
		gestionObjetos->mejoraVelocidad[i].seVende = 0;
		gestionObjetos->vidasObjeto[i].activa = 0;
	}

	gestionObjetos->mapaAzul.activa = 0;
	gestionObjetos->mapaRojo.activa = 0;
	gestionObjetos->mapaNaranjo.activa = 0;
	gestionObjetos->mapaVerde.activa = 0;
	gestionObjetos->minimapa.activa = 0;

	//Reinicio Interactuables
	for (int i = 0; i < MAX_INTERACTUABLES; i++)
	{
		gestionInteractuables->fogata[i].activa = 0;
		gestionInteractuables->fogata[i].fogataActiva = 0;
	}

	gestionInteractuables->fogataActual = 0;

	//Reinicio Dianas
	for (int i = 0; i < MAX_DIANAS; i++)
	{
		gestionDianas->dianas[i].activa = 0;
	}
}

void cargarMapa(char nombreMapa[LARGO_TEXTO], FILE *archivoMapa, char mapa[FILAS_HABITACION][COLUMNAS_HABITACION], jugador *jugador, gestionEnemigos_ *gestionEnemigos, char puertaDestino, int mapaX, int mapaY, controlIndices_ *controlIndices, gestionObjetos_ *gestionObjetos, estadoMapa_ *estadoMapa, gestionDianas_ *gestionDianas, gestionInteractuables_ *gestionInteractuables, registroMundo_ *registroMundo)
{
	//Variables locales temporales donde se guardan los registros por llamada
	int muerto = 0;
	int cofreAbierto = 0;
	int fogataEncendida = 0;
	int cargadorObtenido = 0;
	int auxRandTienda = 0;
	int productoTiendaGenerado = 0;
	int dianaDestruida = 0;

	if ((archivoMapa = fopen(nombreMapa,"r")) == NULL)
	{
		printf("ERROR AL ABRIR ARCHIVO MAPA");
	} 

	DesactivarObjetosActivos(gestionEnemigos, controlIndices, gestionObjetos, estadoMapa, gestionDianas, gestionInteractuables);

////////////////////////////////////////////////////////////////////////////////////////////////////

	for (int i = 0; i < MAX_REGISTROS; i++)
	{
		//Aparicion del mapa verde si no se recoge
		if (registroMundo->registroRecompensas[i].mapaX == estadoMapa->actualMapaX && registroMundo->registroRecompensas[i].mapaY == estadoMapa->actualMapaY && registroMundo->registroRecompensas[i].idItem == 'v' && registroMundo->registroRecompensas[i].seRecogioElItem == 0)
		{
			gestionObjetos->mapaVerde.activa = 1;
			gestionObjetos->mapaVerde.posX = registroMundo->registroRecompensas[i].posX;
			gestionObjetos->mapaVerde.posY = registroMundo->registroRecompensas[i].posY;
		}

		//Aparicion llaves
		if (registroMundo->registroRecompensas[i].mapaX == estadoMapa->actualMapaX && registroMundo->registroRecompensas[i].mapaY == estadoMapa->actualMapaY && registroMundo->registroRecompensas[i].idItem == 'l' && registroMundo->registroRecompensas[i].seRecogioElItem == 0)
		{
			gestionObjetos->llaves[gestionObjetos->llavesActual].activa = 1;
		}

		//Aparicion monedas
		if (registroMundo->registroRecompensas[i].mapaX == estadoMapa->actualMapaX && registroMundo->registroRecompensas[i].mapaY == estadoMapa->actualMapaY && registroMundo->registroRecompensas[i].idItem == 'm' && registroMundo->registroRecompensas[i].seRecogioElItem == 0)
		{
			gestionObjetos->monedas[gestionObjetos->monedasActual].activa = 1;
			gestionObjetos->monedas[gestionObjetos->monedasActual].posX = registroMundo->registroRecompensas[i].posX;
			gestionObjetos->monedas[gestionObjetos->monedasActual].posY = registroMundo->registroRecompensas[i].posY;
			gestionObjetos->monedasActual++;
		}

		if (registroMundo->registroRecompensas[i].mapaX == estadoMapa->actualMapaX && registroMundo->registroRecompensas[i].mapaY == estadoMapa->actualMapaY && registroMundo->registroRecompensas[i].idItem == 'e' && registroMundo->registroRecompensas[i].seRecogioElItem == 0)
		{
			gestionObjetos->monedas[gestionObjetos->monedasActual].activa = 1;
			gestionObjetos->monedas[gestionObjetos->monedasActual].especial = 1;
			gestionObjetos->monedas[gestionObjetos->monedasActual].posX = registroMundo->registroRecompensas[i].posX;
			gestionObjetos->monedas[gestionObjetos->monedasActual].posY = registroMundo->registroRecompensas[i].posY;
			gestionObjetos->monedasActual++;
		}

		//Aparicion cargadores cofre
		if (registroMundo->registroRecompensas[i].mapaX == estadoMapa->actualMapaX && registroMundo->registroRecompensas[i].mapaY == estadoMapa->actualMapaY && registroMundo->registroRecompensas[i].idItem == 'M' && registroMundo->registroRecompensas[i].seRecogioElItem == 0)
		{
			gestionObjetos->municiones[gestionObjetos->municionesActual].activa = 1;
			gestionObjetos->municiones[gestionObjetos->municionesActual].posX = registroMundo->registroRecompensas[i].posX;
			gestionObjetos->municiones[gestionObjetos->municionesActual].posY = registroMundo->registroRecompensas[i].posY;
			gestionObjetos->municionesActual++;
		}

		//Aparicion vidas cofre
		if (registroMundo->registroRecompensas[i].mapaX == estadoMapa->actualMapaX && registroMundo->registroRecompensas[i].mapaY == estadoMapa->actualMapaY && registroMundo->registroRecompensas[i].idItem == 'V' && registroMundo->registroRecompensas[i].seRecogioElItem == 0)
		{
			gestionObjetos->vidasObjeto[gestionObjetos->vidasObjetoActual].activa = 1;
			gestionObjetos->vidasObjeto[gestionObjetos->vidasObjetoActual].posX = registroMundo->registroRecompensas[i].posX;
			gestionObjetos->vidasObjeto[gestionObjetos->vidasObjetoActual].posY = registroMundo->registroRecompensas[i].posY;
			gestionObjetos->vidasObjetoActual++;
		}

		//Aparicion mejoras cofre
		if (registroMundo->registroRecompensas[i].mapaX == estadoMapa->actualMapaX && registroMundo->registroRecompensas[i].mapaY == estadoMapa->actualMapaY && registroMundo->registroRecompensas[i].idItem == 'A' && registroMundo->registroRecompensas[i].seRecogioElItem == 0)
		{
			gestionObjetos->mejoraDanho[gestionObjetos->mejoraDanhoActual].activa = 1;
			gestionObjetos->mejoraDanho[gestionObjetos->mejoraDanhoActual].posX = registroMundo->registroRecompensas[i].posX;
			gestionObjetos->mejoraDanho[gestionObjetos->mejoraDanhoActual].posY = registroMundo->registroRecompensas[i].posY;
			gestionObjetos->mejoraDanhoActual++;
		}

		if (registroMundo->registroRecompensas[i].mapaX == estadoMapa->actualMapaX && registroMundo->registroRecompensas[i].mapaY == estadoMapa->actualMapaY && registroMundo->registroRecompensas[i].idItem == 'B' && registroMundo->registroRecompensas[i].seRecogioElItem == 0)
		{
			gestionObjetos->mejoraRango[gestionObjetos->mejoraRangoActual].activa = 1;
			gestionObjetos->mejoraRango[gestionObjetos->mejoraRangoActual].posX = registroMundo->registroRecompensas[i].posX;
			gestionObjetos->mejoraRango[gestionObjetos->mejoraRangoActual].posY = registroMundo->registroRecompensas[i].posY;
			gestionObjetos->mejoraRangoActual++;
		}

		if (registroMundo->registroRecompensas[i].mapaX == estadoMapa->actualMapaX && registroMundo->registroRecompensas[i].mapaY == estadoMapa->actualMapaY && registroMundo->registroRecompensas[i].idItem == 'C' && registroMundo->registroRecompensas[i].seRecogioElItem == 0)
		{
			gestionObjetos->mejoraVelocidad[gestionObjetos->mejoraVelocidadActual].activa = 1;
			gestionObjetos->mejoraVelocidad[gestionObjetos->mejoraVelocidadActual].posX = registroMundo->registroRecompensas[i].posX;
			gestionObjetos->mejoraVelocidad[gestionObjetos->mejoraVelocidadActual].posY = registroMundo->registroRecompensas[i].posY;
			gestionObjetos->mejoraVelocidadActual++;
		}

		//Aparicion mapa rojo
		if (registroMundo->registroRecompensas[i].mapaX == estadoMapa->actualMapaX && registroMundo->registroRecompensas[i].mapaY == estadoMapa->actualMapaY && registroMundo->registroRecompensas[i].idItem == 'r' && registroMundo->registroRecompensas[i].seRecogioElItem == 0)
		{
			gestionObjetos->mapaRojo.activa = 1;
			gestionObjetos->mapaRojo.posX = registroMundo->registroRecompensas[i].posX;
			gestionObjetos->mapaRojo.posY = registroMundo->registroRecompensas[i].posY;
		}
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

			if (mapa[i][j] == 'M' && estadoMapa->minimapaVendido == 0)
			{
				gestionObjetos->minimapa.activa = 1;
				gestionObjetos->minimapa.posX = j * TAMANHO;
				gestionObjetos->minimapa.posY = i * TAMANHO - 30;
				gestionObjetos->minimapa.seVende = 1;
				gestionObjetos->minimapa.precio = 10;
			}

			if (mapa[i][j] == '+')
			{
				productoTiendaGenerado = 0;
				
				for (int k = 0; k < MAX_REGISTROS; k++)
				{
					if (registroMundo->registroTienda[k].columna == j && registroMundo->registroTienda[k].fila == i && registroMundo->registroTienda[k].mapaX == estadoMapa->actualMapaX && registroMundo->registroTienda[k].mapaY == estadoMapa->actualMapaY)
					{
						productoTiendaGenerado = 1;

						if (registroMundo->registroTienda[k].idObjeto == 1 && registroMundo->registroTienda[k].comprado == 0)
						{
							gestionObjetos->mejoraDanho[gestionObjetos->mejoraDanhoActual].activa = 1;
							gestionObjetos->mejoraDanho[gestionObjetos->mejoraDanhoActual].posX = j * TAMANHO;
							gestionObjetos->mejoraDanho[gestionObjetos->mejoraDanhoActual].posY = i * TAMANHO - 30;
							gestionObjetos->mejoraDanho[gestionObjetos->mejoraDanhoActual].fila = i;
							gestionObjetos->mejoraDanho[gestionObjetos->mejoraDanhoActual].columna = j;
							gestionObjetos->mejoraDanho[gestionObjetos->mejoraDanhoActual].seVende = 1;
							gestionObjetos->mejoraDanho[gestionObjetos->mejoraDanhoActual].precio = 10;
							gestionObjetos->mejoraDanhoActual++;
						}
						else if (registroMundo->registroTienda[k].idObjeto == 2 && registroMundo->registroTienda[k].comprado == 0)
						{
							gestionObjetos->mejoraRango[gestionObjetos->mejoraRangoActual].activa = 1;
							gestionObjetos->mejoraRango[gestionObjetos->mejoraRangoActual].posX = j * TAMANHO;
							gestionObjetos->mejoraRango[gestionObjetos->mejoraRangoActual].posY = i * TAMANHO - 30;
							gestionObjetos->mejoraRango[gestionObjetos->mejoraRangoActual].fila = i;
							gestionObjetos->mejoraRango[gestionObjetos->mejoraRangoActual].columna = j;
							gestionObjetos->mejoraRango[gestionObjetos->mejoraRangoActual].seVende = 1;
							gestionObjetos->mejoraRango[gestionObjetos->mejoraRangoActual].precio = 15;
							gestionObjetos->mejoraRangoActual++;
						}
						else if (registroMundo->registroTienda[k].idObjeto == 3 && registroMundo->registroTienda[k].comprado == 0)
						{
							gestionObjetos->mejoraVelocidad[gestionObjetos->mejoraVelocidadActual].activa = 1;
							gestionObjetos->mejoraVelocidad[gestionObjetos->mejoraVelocidadActual].posX = j * TAMANHO;
							gestionObjetos->mejoraVelocidad[gestionObjetos->mejoraVelocidadActual].posY = i * TAMANHO - 30;
							gestionObjetos->mejoraVelocidad[gestionObjetos->mejoraVelocidadActual].fila = i;
							gestionObjetos->mejoraVelocidad[gestionObjetos->mejoraVelocidadActual].columna = j;
							gestionObjetos->mejoraVelocidad[gestionObjetos->mejoraVelocidadActual].seVende = 1;
							gestionObjetos->mejoraVelocidad[gestionObjetos->mejoraVelocidadActual].precio = 10;
							gestionObjetos->mejoraVelocidadActual++;
						}

						break;
					}
				}
				
				if (productoTiendaGenerado == 0)
				{
					auxRandTienda = rand() % 3 + 1;

					if (auxRandTienda == 1)
					{
						gestionObjetos->mejoraDanho[gestionObjetos->mejoraDanhoActual].activa = 1;
						gestionObjetos->mejoraDanho[gestionObjetos->mejoraDanhoActual].posX = j * TAMANHO;
						gestionObjetos->mejoraDanho[gestionObjetos->mejoraDanhoActual].posY = i * TAMANHO - 30;
						gestionObjetos->mejoraDanho[gestionObjetos->mejoraDanhoActual].seVende = 1;
						gestionObjetos->mejoraDanho[gestionObjetos->mejoraDanhoActual].precio = 10;
						gestionObjetos->mejoraDanho[gestionObjetos->mejoraDanhoActual].idRegistro = registroMundo->indiceTienda;
						gestionObjetos->mejoraDanhoActual++;

						registroMundo->registroTienda[registroMundo->indiceTienda].columna = j;
						registroMundo->registroTienda[registroMundo->indiceTienda].fila = i;
						registroMundo->registroTienda[registroMundo->indiceTienda].mapaX = estadoMapa->actualMapaX;
						registroMundo->registroTienda[registroMundo->indiceTienda].mapaY = estadoMapa->actualMapaY;
						registroMundo->registroTienda[registroMundo->indiceTienda].idObjeto = 1;
						registroMundo->registroTienda[registroMundo->indiceTienda].comprado = 0;
						registroMundo->indiceTienda++;
					}
					else if (auxRandTienda == 2)
					{
						gestionObjetos->mejoraRango[gestionObjetos->mejoraRangoActual].activa = 1;
						gestionObjetos->mejoraRango[gestionObjetos->mejoraRangoActual].posX = j * TAMANHO;
						gestionObjetos->mejoraRango[gestionObjetos->mejoraRangoActual].posY = i * TAMANHO - 30;
						gestionObjetos->mejoraRango[gestionObjetos->mejoraRangoActual].seVende = 1;
						gestionObjetos->mejoraRango[gestionObjetos->mejoraRangoActual].precio = 15;
						gestionObjetos->mejoraRango[gestionObjetos->mejoraRangoActual].idRegistro = registroMundo->indiceTienda;
						gestionObjetos->mejoraRangoActual++;

						registroMundo->registroTienda[registroMundo->indiceTienda].columna = j;
						registroMundo->registroTienda[registroMundo->indiceTienda].fila = i;
						registroMundo->registroTienda[registroMundo->indiceTienda].mapaX = estadoMapa->actualMapaX;
						registroMundo->registroTienda[registroMundo->indiceTienda].mapaY = estadoMapa->actualMapaY;
						registroMundo->registroTienda[registroMundo->indiceTienda].idObjeto = 2;
						registroMundo->registroTienda[registroMundo->indiceTienda].comprado = 0;
						registroMundo->indiceTienda++;
					}
					else if (auxRandTienda == 3)
					{
						gestionObjetos->mejoraVelocidad[gestionObjetos->mejoraVelocidadActual].activa = 1;
						gestionObjetos->mejoraVelocidad[gestionObjetos->mejoraVelocidadActual].posX = j * TAMANHO;
						gestionObjetos->mejoraVelocidad[gestionObjetos->mejoraVelocidadActual].posY = i * TAMANHO - 30;
						gestionObjetos->mejoraVelocidad[gestionObjetos->mejoraVelocidadActual].seVende = 1;
						gestionObjetos->mejoraVelocidad[gestionObjetos->mejoraVelocidadActual].precio = 10;
						gestionObjetos->mejoraVelocidad[gestionObjetos->mejoraVelocidadActual].idRegistro = registroMundo->indiceTienda;
						gestionObjetos->mejoraVelocidadActual++;

						registroMundo->registroTienda[registroMundo->indiceTienda].columna = j;
						registroMundo->registroTienda[registroMundo->indiceTienda].fila = i;
						registroMundo->registroTienda[registroMundo->indiceTienda].mapaX = estadoMapa->actualMapaX;
						registroMundo->registroTienda[registroMundo->indiceTienda].mapaY = estadoMapa->actualMapaY;
						registroMundo->registroTienda[registroMundo->indiceTienda].idObjeto = 3;
						registroMundo->registroTienda[registroMundo->indiceTienda].comprado = 0;
						registroMundo->indiceTienda++;
					}
				}
			}

			if (mapa[i][j] == 'T')
			{
				estadoMapa->seGeneroUnaRecompensa++;
			}
			
			if (mapa[i][j] == 'f')
			{
				fogataEncendida = 0;

				for (int k = 0; k < MAX_REGISTROS; k++)
				{
					if (registroMundo->registroInteractuables[k].mapaX == estadoMapa->actualMapaX && registroMundo->registroInteractuables[k].mapaY == estadoMapa->actualMapaY && registroMundo->registroInteractuables[k].fogataEncendida == 1 && registroMundo->registroInteractuables[k].fila == i && registroMundo->registroInteractuables[k].columna == j)
					{
						fogataEncendida = 1;
						break;
					}
				}
				
				if (fogataEncendida == 0)
				{
					gestionInteractuables->fogata[gestionInteractuables->fogataActual].activa = 1;
					gestionInteractuables->fogata[gestionInteractuables->fogataActual].posX = j * TAMANHO;
					gestionInteractuables->fogata[gestionInteractuables->fogataActual].posY = i * TAMANHO;
					gestionInteractuables->fogata[gestionInteractuables->fogataActual].fila = i;
					gestionInteractuables->fogata[gestionInteractuables->fogataActual].columna = j;
					gestionInteractuables->fogataActual++;
				}
				else
				{
					gestionInteractuables->fogata[gestionInteractuables->fogataActual].activa = 1;
					gestionInteractuables->fogata[gestionInteractuables->fogataActual].fogataActiva = 1;
					gestionInteractuables->fogata[gestionInteractuables->fogataActual].posX = j * TAMANHO;
					gestionInteractuables->fogata[gestionInteractuables->fogataActual].posY = i * TAMANHO;
					gestionInteractuables->fogata[gestionInteractuables->fogataActual].fila = i;
					gestionInteractuables->fogata[gestionInteractuables->fogataActual].columna = j;
					gestionInteractuables->fogataActual++;	
				}

				if (gestionInteractuables->fogataActual >= MAX_INTERACTUABLES)
				{
					gestionInteractuables->fogataActual = 0;
				}
				
				estadoMapa->seGeneroUnaRecompensa++;
			}

			//Cargar mapa azul
			if (mapa[i][j]=='a' && gestionObjetos->mapaAzul.seObtuvo == 0)
			{
				gestionObjetos->mapaAzul.activa = 1;
				gestionObjetos->mapaAzul.posX = j * TAMANHO;
				gestionObjetos->mapaAzul.posY = i * TAMANHO;
			}

			//Cargar mapa verde
			if (mapa[i][j]=='v')
			{
				gestionObjetos->mapaVerde.activa = 1;
				gestionObjetos->mapaVerde.posX = j * TAMANHO;
				gestionObjetos->mapaVerde.posY = i * TAMANHO;
			}

			//Cargar mapa rojo
			if (mapa[i][j]=='r')
			{
				gestionObjetos->mapaRojo.activa = 1;
				gestionObjetos->mapaRojo.posX = j * TAMANHO;
				gestionObjetos->mapaRojo.posY = i * TAMANHO;
			}

			//Cargar mapa naranjo
			if (mapa[i][j]=='n' && gestionObjetos->mapaNaranjo.seObtuvo == 0)
			{
				gestionObjetos->mapaNaranjo.activa = 1;
				gestionObjetos->mapaNaranjo.posX = j * TAMANHO;
				gestionObjetos->mapaNaranjo.posY = i * TAMANHO;
			}

			//Cargar gestionObjetos->municiones
			if(mapa[i][j]=='R')
			{
				cargadorObtenido = 0;

				for (int k = 0; k < MAX_REGISTROS; k++)
				{
					if (registroMundo->registroRecompensas[k].mapaX == estadoMapa->actualMapaX && registroMundo->registroRecompensas[k].mapaY == mapaY && registroMundo->registroRecompensas[k].fila == i && registroMundo->registroRecompensas[k].columna == j && registroMundo->registroRecompensas[k].seObtuvoUnCargador == 1)
					{
						cargadorObtenido = 1;
						break;
					}
				}
				
				if (cargadorObtenido == 0)
				{
					gestionObjetos->municiones[gestionObjetos->municionesActual].activa = 1;
					gestionObjetos->municiones[gestionObjetos->municionesActual].posX = j * TAMANHO;
					gestionObjetos->municiones[gestionObjetos->municionesActual].posY = i * TAMANHO;
					gestionObjetos->municiones[gestionObjetos->municionesActual].fila = i;
					gestionObjetos->municiones[gestionObjetos->municionesActual].columna = j;
					gestionObjetos->municionesActual++;
				}
			}

			if (mapa[i][j] == 'D')
			{
				dianaDestruida = 0;

				for (int k = 0; k < MAX_REGISTROS; k++)
				{
					if (registroMundo->registroInteractuables[k].mapaX == estadoMapa->actualMapaX && registroMundo->registroInteractuables[k].mapaY == mapaY && registroMundo->registroInteractuables[k].fila == i && registroMundo->registroInteractuables[k].columna == j && registroMundo->registroInteractuables[k].dianaDestruida == 1)
					{
						dianaDestruida = 1;
					}
				}

				if (dianaDestruida == 0)
				{
					gestionDianas->dianas[gestionDianas->dianasActuales].posX = j * TAMANHO;
					gestionDianas->dianas[gestionDianas->dianasActuales].posY = i * TAMANHO;
					gestionDianas->dianas[gestionDianas->dianasActuales].fila = i;
					gestionDianas->dianas[gestionDianas->dianasActuales].columna = j;
					gestionDianas->dianas[gestionDianas->dianasActuales].activa = 1;
					gestionDianas->dianasActuales++;
				}

				estadoMapa->seGeneroUnaRecompensa++;
			}

			if (gestionDianas->dianasActuales >= MAX_DIANAS)
			{
				gestionDianas->dianasActuales = 0;
			}

			//Cargar gestionObjetos->cofres
			if (mapa[i][j]=='C')
			{
				for (int i = 0; i < 1000; i++)
				{
					cofreAbierto = 0;

					if (registroMundo->registroRecompensas[i].mapaX == mapaX && registroMundo->registroRecompensas[i].mapaY == mapaY && registroMundo->registroRecompensas[i].seAbrioUnCofre != 0)
					{
						cofreAbierto = 1;
						break;
					}
				}
				
				if (cofreAbierto == 0)
				{
					gestionObjetos->cofres[gestionObjetos->cofreActual].activa = 1;
					gestionObjetos->cofres[gestionObjetos->cofreActual].posX = j * TAMANHO;
					gestionObjetos->cofres[gestionObjetos->cofreActual].posY = i * TAMANHO;
					gestionObjetos->cofreActual++;
				}
				
				estadoMapa->seGeneroUnaRecompensa = 1;
			}

			if (mapa[i][j]=='*')
			{
				for (int k = 0; k < MAX_OBJETOS; k++)
				{	
					gestionObjetos->llaves[k].posX = j * TAMANHO;
					gestionObjetos->llaves[k].posY = i * TAMANHO;

					for (int i = 0; i < 1000; i++)
					{
						if (registroMundo->registroRecompensas[i].mapaX == mapaX && registroMundo->registroRecompensas[i].mapaY == mapaY && registroMundo->registroRecompensas[i].seObtuvoUnaRecompensa != estadoMapa->seGeneroUnaRecompensa)
						{
							estadoMapa->seGeneroUnaRecompensa ++;
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
				for (int m = 0; m < registroMundo->cantidadMuertos; m++)
				{
					muerto = 0;

					if (registroMundo->registroMuertes[m].mapaX == mapaX && registroMundo->registroMuertes[m].mapaY == mapaY && registroMundo->registroMuertes[m].fila == i && registroMundo->registroMuertes[m].columna == j)
					{
						muerto = 1;
						break;
					}
				}
				
				//Si el gestionEnemigos->slime no esta muerto se genera
				if (muerto == 0)
				{
					if(gestionEnemigos->slime[controlIndices->slime].activa == 0)
					{
						//Generar gestionEnemigos->slime en la posicion 's' del mapa
						gestionEnemigos->slime[controlIndices->slime].activa = 1;
						gestionEnemigos->slime[controlIndices->slime].posX = j * TAMANHO;
						gestionEnemigos->slime[controlIndices->slime].posY = i * TAMANHO;
						gestionEnemigos->slime[controlIndices->slime].vida = 4;

						//Registrar la ubicacion original del gestionEnemigos->slime
						gestionEnemigos->slime[controlIndices->slime].posXGeneracion = j;
						gestionEnemigos->slime[controlIndices->slime].posYGeneracion = i;

						controlIndices->slime ++;
					}
				}
			}

			if (controlIndices->slime > MAX_ENEMIGOS - 1)
			{
				controlIndices->slime = 0;
			}

			if (estadoMapa->sala[i][j] == 'j')
			{

				for (int n = 0; n < registroMundo->cantidadMuertos; n++)
				{
					muerto = 0;

					if (registroMundo->registroMuertes[n].mapaX == mapaX && registroMundo->registroMuertes[n].mapaY == mapaY && registroMundo->registroMuertes[n].fila == i && registroMundo->registroMuertes[n].columna == j)
					{
						muerto = 1;
						break;
					}
				}
				
				if (muerto == 0)
				{
					if(gestionEnemigos->jefe.activa == 0)
					{
						gestionEnemigos->jefe.activa = 1;
						gestionEnemigos->jefe.posX = j * TAMANHO;
						gestionEnemigos->jefe.posY = i * TAMANHO;
						gestionEnemigos->jefe.vida = 20;
						
						gestionEnemigos->jefe.posXGeneracion = j;
						gestionEnemigos->jefe.posYGeneracion = i;
					}
				}
			}

			//cargar magos
			if (mapa[i][j]=='m')
			{
				for (int m = 0; m < registroMundo->cantidadMuertos; m++)
				{
					muerto = 0;

					if (registroMundo->registroMuertes[m].mapaX == mapaX && registroMundo->registroMuertes[m].mapaY == mapaY && registroMundo->registroMuertes[m].fila == i && registroMundo->registroMuertes[m].columna == j)
					{
						muerto = 1;
						break;
					}
				}
				
				if (muerto == 0)
				{
					if(gestionEnemigos->mago[controlIndices->mago].activa == 0)
					{
						//Generar gestionEnemigos->slime en la posicion 's' del mapa
						gestionEnemigos->mago[controlIndices->mago].activa = 1;
						gestionEnemigos->mago[controlIndices->mago].posX = j * TAMANHO;
						gestionEnemigos->mago[controlIndices->mago].posY = i * TAMANHO;
						gestionEnemigos->mago[controlIndices->mago].vida = 4;

						//Registrar la ubicacion original del gestionEnemigos->slime
						gestionEnemigos->mago[controlIndices->mago].posXGeneracion = j;
						gestionEnemigos->mago[controlIndices->mago].posYGeneracion = i;

						controlIndices->mago ++;
					}
				}
			}

			if (controlIndices->mago > MAX_ENEMIGOS - 1)
			{
				controlIndices->mago = 0;
			}

			//cargar Arañas 
			if (mapa[i][j]=='A')
			{
				for (int m = 0; m < registroMundo->cantidadMuertos; m++)
				{
					muerto = 0;

					if (registroMundo->registroMuertes[m].mapaX == mapaX && registroMundo->registroMuertes[m].mapaY == mapaY && registroMundo->registroMuertes[m].fila == i && registroMundo->registroMuertes[m].columna == j)
					{
						muerto = 1;
						break;
					}
				}
				
				if (muerto == 0)
				{
					if(gestionEnemigos->aranha[controlIndices->aranha].activa == 0)
					{
						gestionEnemigos->aranha[controlIndices->aranha].activa = 1;
						gestionEnemigos->aranha[controlIndices->aranha].posX = j * TAMANHO;
						gestionEnemigos->aranha[controlIndices->aranha].posY = i * TAMANHO;
						gestionEnemigos->aranha[controlIndices->aranha].vida = 2;

						gestionEnemigos->aranha[controlIndices->aranha].posXGeneracion = j;
						gestionEnemigos->aranha[controlIndices->aranha].posYGeneracion = i;

						controlIndices->aranha ++;
					}
				}
			}

			if (controlIndices->aranha > MAX_ENEMIGOS - 1)
			{
				controlIndices->aranha = 0;
			}
    	}
	}
}

void DibujarMapa(char mapa[FILAS_HABITACION][COLUMNAS_HABITACION], ALLEGRO_BITMAP *spriteSheet, estadoSistema_ *estadoSistema, gestionInteractuables_ *gestionInteractuables)
{
	//# arbol (solido)
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

			if (mapa[i][j] == '+' || mapa[i][j] == 'M')
			{
				al_draw_bitmap_region(spriteSheet, 1 * TAMANHO, 15 * TAMANHO, TAMANHO, TAMANHO, j * TAMANHO, i * TAMANHO, 0);
			}

			if (mapa[i][j] == 'E' || mapa[i][j] == 'O' || mapa[i][j] == 'N' || mapa[i][j] == 'S')
			{
				al_draw_bitmap_region(spriteSheet, 4 * TAMANHO, 15 * TAMANHO, TAMANHO, TAMANHO, j * TAMANHO, i * TAMANHO, 0);
			}

			if (mapa[i][j] == 'F')
			{
				if ((estadoSistema->controlSpritesMenu >= 0 && estadoSistema->controlSpritesMenu <= 10) || (estadoSistema->controlSprites >= 0 && estadoSistema->controlSprites <= 10))
				{
					al_draw_bitmap_region(spriteSheet, 11 * TAMANHO, 16 * TAMANHO, TAMANHO, TAMANHO, j * TAMANHO, i * TAMANHO, 0);	
				}
				if ((estadoSistema->controlSpritesMenu >= 10 && estadoSistema->controlSpritesMenu <= 20) || (estadoSistema->controlSprites >= 10 && estadoSistema->controlSprites <= 20))
				{
					al_draw_bitmap_region(spriteSheet, 12 * TAMANHO, 16 * TAMANHO, TAMANHO, TAMANHO, j * TAMANHO, i * TAMANHO, 0);	
				}
				if ((estadoSistema->controlSpritesMenu >= 20 && estadoSistema->controlSpritesMenu <= 30) || (estadoSistema->controlSprites >= 20 && estadoSistema->controlSprites <= 30))
				{
					al_draw_bitmap_region(spriteSheet, 13 * TAMANHO, 16 * TAMANHO, TAMANHO, TAMANHO, j * TAMANHO, i * TAMANHO, 0);	
				}
				if ((estadoSistema->controlSpritesMenu >= 30 && estadoSistema->controlSpritesMenu <= 40) || (estadoSistema->controlSprites >= 30 && estadoSistema->controlSprites <= 40))
				{
					al_draw_bitmap_region(spriteSheet, 14 * TAMANHO, 16 * TAMANHO, TAMANHO, TAMANHO, j * TAMANHO, i * TAMANHO, 0);	
				}
			}

			if (estadoSistema->controlSpritesMenu >= 40)
			{
				estadoSistema->controlSpritesMenu = 0;
			}
			
			for (int k = 0; k < MAX_INTERACTUABLES; k++)
			{
				if (mapa[i][j] == 'f' && gestionInteractuables->fogata[k].fogataActiva == 0)
				{
					al_draw_bitmap_region(spriteSheet, 10 * TAMANHO, 18 * TAMANHO, TAMANHO, TAMANHO, gestionInteractuables->fogata[k].posX, gestionInteractuables->fogata[k].posY, 0);
				}
				else if (mapa[i][j] == 'f' && gestionInteractuables->fogata[k].fogataActiva != 0)
				{
					if (estadoSistema->controlSprites >= 0 && estadoSistema->controlSprites <= 10)
					{
						al_draw_bitmap_region(spriteSheet, 11 * TAMANHO, 18 * TAMANHO, TAMANHO, TAMANHO, gestionInteractuables->fogata[k].posX, gestionInteractuables->fogata[k].posY, 0);	
					}
					if (estadoSistema->controlSprites >= 10 && estadoSistema->controlSprites <= 20)
					{
						al_draw_bitmap_region(spriteSheet, 12 * TAMANHO, 18 * TAMANHO, TAMANHO, TAMANHO, gestionInteractuables->fogata[k].posX, gestionInteractuables->fogata[k].posY, 0);	
					}
					if (estadoSistema->controlSprites >= 20 && estadoSistema->controlSprites <= 30)
					{
						al_draw_bitmap_region(spriteSheet, 13 * TAMANHO, 18 * TAMANHO, TAMANHO, TAMANHO, gestionInteractuables->fogata[k].posX, gestionInteractuables->fogata[k].posY, 0);	
					}
					if (estadoSistema->controlSprites >= 30 && estadoSistema->controlSprites <= 40)
					{
						al_draw_bitmap_region(spriteSheet, 14 * TAMANHO, 18 * TAMANHO, TAMANHO, TAMANHO, gestionInteractuables->fogata[k].posX, gestionInteractuables->fogata[k].posY, 0);	
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

void Render(char mapa[FILAS_HABITACION][COLUMNAS_HABITACION], ALLEGRO_BITMAP *spriteSheet, ALLEGRO_BITMAP *spriteSheetBalas, ALLEGRO_BITMAP *spriteSheetCaminarCaballero, ALLEGRO_BITMAP *spriteSheetIcons, ALLEGRO_FONT *fuenteJuego, ALLEGRO_BITMAP *spriteSheetCrosshair, ALLEGRO_BITMAP *spriteSheetBalasEnemigos, ALLEGRO_BITMAP *spriteSheetIconsRaven, gestionEnemigos_ *gestionEnemigos, gestionObjetos_ *gestionObjetos, estadoSistema_ *estadoSistema, estadoMapa_ *estadoMapa, gestionDianas_ *gestionDianas, gestionInteractuables_ *gestionInteractuables, ALLEGRO_BITMAP *spriteSheetBotonesTeclado, ALLEGRO_BITMAP *spriteSheetBotonesMouse, teclado_ *teclado)
{
	al_clear_to_color(al_map_rgb(0, 0, 0));
	//////////////////////////////////////////////// Dibujar en este espacio

	DibujarMapa(mapa, spriteSheet, estadoSistema, gestionInteractuables);

	//Jugador de SpriteSheet
	AnimacionPersonaje(spriteSheet, spriteSheetCaminarCaballero, estadoSistema);

	//Botones tutorial
	if (personaje.seMovio == 0)
	{
		al_draw_scaled_bitmap(spriteSheetBotonesTeclado, 0 * 16, 2 * 16, 16, 16, personaje.posX - TAMANHO, personaje.posY - TAMANHO, TAMANHO, TAMANHO, 0);
		al_draw_scaled_bitmap(spriteSheetBotonesTeclado, 1 * 16, 2 * 16, 16, 16, personaje.posX, personaje.posY - TAMANHO, TAMANHO, TAMANHO, 0);
		al_draw_scaled_bitmap(spriteSheetBotonesTeclado, 2 * 16, 2 * 16, 16, 16, personaje.posX + TAMANHO, personaje.posY - TAMANHO, TAMANHO, TAMANHO, 0);
		al_draw_scaled_bitmap(spriteSheetBotonesTeclado, 1 * 16, 1 * 16, 16, 16, personaje.posX, personaje.posY - TAMANHO * 2, TAMANHO, TAMANHO, 0);
	}
	
	if (mouse.seClickeo == 0)
	{
		al_draw_scaled_bitmap(spriteSheetBotonesMouse, 0 * 16, 0 * 16, 16, 16, mouse.posX, mouse.posY - TAMANHO, TAMANHO, TAMANHO, 0);
	}

	if (teclado->sePresionoTAB == 0)
	{
		al_draw_scaled_bitmap(spriteSheetBotonesTeclado, 0 * 16, 5 * 16, 32, 16, personaje.posX - TAMANHO + 32, personaje.posY + TAMANHO, TAMANHO * 2, TAMANHO, 0);
	}

	//Monedas Jugador
	al_draw_bitmap_region(spriteSheetIcons, 3 * TAMANHO, 8 * TAMANHO, TAMANHO, TAMANHO, TAMANHO, TAMANHO + TAMANHO / 2, 0);
	al_draw_textf(fuenteJuego, al_map_rgb(192, 192, 192), TAMANHO + 80, TAMANHO + 50, 0, "= %d", personaje.cantidadMonedas);

	//Llaves jugador
	al_draw_bitmap_region(spriteSheetIcons, 6 * TAMANHO, 4 * TAMANHO, TAMANHO, TAMANHO, TAMANHO, TAMANHO + TAMANHO + 32, 0);
	al_draw_textf(fuenteJuego, al_map_rgb(192, 192, 192), TAMANHO + 80, TAMANHO + 120, 0, "= %d", personaje.cantidadLlaves);

	//Balas Jugador
	al_draw_scaled_bitmap(spriteSheetBalas, 2 * 16, 0 * 16, 16, 16, LARGO_PANTALLA - 350, ANCHO_PANTALLA - 200, TAMANHO * 3, TAMANHO * 3, 0); 
	al_draw_textf(fuenteJuego, al_map_rgb(192, 192, 192), LARGO_PANTALLA - 200, ANCHO_PANTALLA - 120, 0, "%d/%d", personaje.balasrestantes, MAX_BALAS);

	//Puntaje Jugador
	al_draw_textf(fuenteJuego, al_map_rgb(192, 192, 192), TAMANHO * 23, TAMANHO, 0, "Puntaje: %d", personaje.puntaje);

	//Vidas jugador
	for (int j = 0; j < personaje.vidas; j++)
	{
		al_draw_bitmap_region(spriteSheetIcons, 4 * TAMANHO, 134 * TAMANHO, TAMANHO, TAMANHO, j * TAMANHO + TAMANHO, TAMANHO / 2, 0);
	}

	//Cantidad de fogatas necesarias
	if (estadoMapa->actualMapaX == COLUMNAS_MAPA / 2 + 1 && estadoMapa->actualMapaY == FILAS_MAPA / 2 + 1)
	{
		al_draw_bitmap_region(spriteSheet, 11 * TAMANHO, 18 * TAMANHO, TAMANHO, TAMANHO, TAMANHO * 2, TAMANHO * 12, 0);
		al_draw_textf(fuenteJuego, al_map_rgb(192, 192, 192), TAMANHO * 3 + 20, TAMANHO * 12 + 20, 0, "%d/4", gestionInteractuables->cantidadfogatasActivas);
	}
	
	//Dibujo de balas
	for (int i = 0; i < MAX_BALAS; i++)
	{
		if (personaje.bala[i].activa != 0)
		{	
			al_draw_scaled_bitmap(spriteSheetBalas, 2 * 16, 0 * 16, 16, 16, personaje.bala[i].posX - 24, personaje.bala[i].posY - 24, TAMANHO, TAMANHO, 0); 
		}

		if (gestionEnemigos->jefe.bala[i].activa != 0)
		{
			al_draw_scaled_bitmap(spriteSheetBalasEnemigos, 2 * 16, 0 * 16, 16, 16, gestionEnemigos->jefe.bala[i].posX - 32, gestionEnemigos->jefe.bala[i].posY - 32, TAMANHO, TAMANHO, 0); 
		}

		for (int j = 0; j < MAX_ENEMIGOS; j++)
		{
			if (gestionEnemigos->mago[j].bala[i].activa != 0)
			{
				al_draw_scaled_bitmap(spriteSheetBalasEnemigos, 2 * 16, 0 * 16, 16, 16, gestionEnemigos->mago[j].bala[i].posX, gestionEnemigos->mago[j].bala[i].posY, TAMANHO, TAMANHO, 0); 
			}		
		}
	}

	//Dibujo de gestionDianas->dianas
	for (int i = 0; i < MAX_DIANAS; i++)
	{
		if (gestionDianas->dianas[i].activa != 0)
		{
			if (estadoSistema->controlSprites >= 0 && estadoSistema->controlSprites <= 10)
			{
				al_draw_bitmap_region(spriteSheet, 11 * TAMANHO, 17 * TAMANHO, TAMANHO, TAMANHO, gestionDianas->dianas[i].posX, gestionDianas->dianas[i].posY, 0);	
			}
			if (estadoSistema->controlSprites >= 10 && estadoSistema->controlSprites <= 20)
			{
				al_draw_bitmap_region(spriteSheet, 12 * TAMANHO, 17 * TAMANHO, TAMANHO, TAMANHO, gestionDianas->dianas[i].posX, gestionDianas->dianas[i].posY, 0);	
			}
			if (estadoSistema->controlSprites >= 20 && estadoSistema->controlSprites <= 30)
			{
				al_draw_bitmap_region(spriteSheet, 13 * TAMANHO, 17 * TAMANHO, TAMANHO, TAMANHO, gestionDianas->dianas[i].posX, gestionDianas->dianas[i].posY, 0);	
			}
			if (estadoSistema->controlSprites >= 30 && estadoSistema->controlSprites <= 40)
			{
				al_draw_bitmap_region(spriteSheet, 14 * TAMANHO, 17 * TAMANHO, TAMANHO, TAMANHO, gestionDianas->dianas[i].posX, gestionDianas->dianas[i].posY, 0);
			}
		}
	}
	
	//Dibujo de objetos
	for (int i = 0; i < MAX_OBJETOS; i++)
	{
		//llaves
		if (gestionObjetos->llaves[i].activa != 0)
		{
			al_draw_bitmap_region(spriteSheetIcons, 6 * TAMANHO, 4 * TAMANHO, TAMANHO, TAMANHO, gestionObjetos->llaves[i].posX, gestionObjetos->llaves[i].posY, 0);
		}

		//gestionObjetos->monedas
		if (gestionObjetos->monedas[i].activa != 0 && gestionObjetos->monedas[i].especial != 0)
		{
			al_draw_bitmap_region(spriteSheetIcons, 2 * TAMANHO, 8 * TAMANHO, TAMANHO, TAMANHO, gestionObjetos->monedas[i].posX, gestionObjetos->monedas[i].posY, 0);
		}
		else if (gestionObjetos->monedas[i].activa != 0)
		{
			al_draw_bitmap_region(spriteSheetIcons, 3 * TAMANHO, 8 * TAMANHO, TAMANHO, TAMANHO, gestionObjetos->monedas[i].posX, gestionObjetos->monedas[i].posY, 0);
		}	

		//cofre
		if (gestionObjetos->cofres[i].activa != 0 && gestionObjetos->cofres[i].cofreAbierto == 1)
		{
			al_draw_bitmap_region(spriteSheet, 12 * TAMANHO, 15 * TAMANHO, TAMANHO, TAMANHO, gestionObjetos->cofres[i].posX, gestionObjetos->cofres[i].posY, 0);
		}
		else if (gestionObjetos->cofres[i].activa != 0)
		{
			al_draw_bitmap_region(spriteSheet, 11 * TAMANHO, 15 * TAMANHO, TAMANHO, TAMANHO, gestionObjetos->cofres[i].posX, gestionObjetos->cofres[i].posY, 0);
		}

		//Municiones
		if (gestionObjetos->municiones[i].activa != 0)
		{
			al_draw_bitmap_region(spriteSheetIcons, 7 * TAMANHO, 42 * TAMANHO, TAMANHO, TAMANHO, gestionObjetos->municiones[i].posX, gestionObjetos->municiones[i].posY, 0);
		}
		
		//vidas
		if (gestionObjetos->vidasObjeto[i].activa != 0)
		{
			al_draw_bitmap_region(spriteSheetIcons, 4 * TAMANHO, 134 * TAMANHO, TAMANHO, TAMANHO, gestionObjetos->vidasObjeto[i].posX, gestionObjetos->vidasObjeto[i].posY, 0);
		}

		//mejora de daño
		if (gestionObjetos->mejoraDanho[i].activa != 0)
		{
			al_draw_bitmap_region(spriteSheetIcons, 0 * TAMANHO, 83 * TAMANHO, TAMANHO, TAMANHO, gestionObjetos->mejoraDanho[i].posX, gestionObjetos->mejoraDanho[i].posY, 0);
		}

		//mejora de rango
		if (gestionObjetos->mejoraRango[i].activa != 0)
		{
			al_draw_bitmap_region(spriteSheetIcons, 2 * TAMANHO, 83 * TAMANHO, TAMANHO, TAMANHO, gestionObjetos->mejoraRango[i].posX, gestionObjetos->mejoraRango[i].posY, 0);
		}
		
		//mejora de velocidad
		if (gestionObjetos->mejoraVelocidad[i].activa != 0)
		{
			al_draw_bitmap_region(spriteSheetIcons, 1 * TAMANHO, 85 * TAMANHO, TAMANHO, TAMANHO, gestionObjetos->mejoraVelocidad[i].posX, gestionObjetos->mejoraVelocidad[i].posY, 0);
		}
	}

	if (gestionObjetos->mapaAzul.activa != 0)
	{
		al_draw_bitmap_region(spriteSheetIcons, 9 * TAMANHO, 18 * TAMANHO, TAMANHO, TAMANHO, gestionObjetos->mapaAzul.posX, gestionObjetos->mapaAzul.posY, 0);
	}

	if (gestionObjetos->mapaRojo.activa != 0)
	{
		al_draw_bitmap_region(spriteSheetIcons, 8 * TAMANHO, 18 * TAMANHO, TAMANHO, TAMANHO, gestionObjetos->mapaRojo.posX, gestionObjetos->mapaRojo.posY, 0);
	}

	if (gestionObjetos->mapaNaranjo.activa != 0)
	{
		al_draw_bitmap_region(spriteSheetIcons, 12 * TAMANHO, 18 * TAMANHO, TAMANHO, TAMANHO, gestionObjetos->mapaNaranjo.posX, gestionObjetos->mapaNaranjo.posY, 0);
	}

	if (gestionObjetos->mapaVerde.activa != 0)
	{
		al_draw_bitmap_region(spriteSheetIcons, 10 * TAMANHO, 18 * TAMANHO, TAMANHO, TAMANHO, gestionObjetos->mapaVerde.posX, gestionObjetos->mapaVerde.posY, 0);
	}

	//Dibujo objetos de tienda
	for (int i = 0; i < MAX_OBJETOS; i++)
	{
		if (gestionObjetos->mejoraDanho[i].activa == 1 && gestionObjetos->mejoraDanho[i].seVende == 1)
		{
			al_draw_bitmap_region(spriteSheetIcons, 0 * TAMANHO, 83 * TAMANHO, TAMANHO, TAMANHO, gestionObjetos->mejoraDanho[i].posX, gestionObjetos->mejoraDanho[i].posY, 0);
			al_draw_textf(fuenteJuego, al_map_rgb(192, 192, 192), gestionObjetos->mejoraDanho[i].posX, gestionObjetos->mejoraDanho[i].posY + 100, 0, "%d", gestionObjetos->mejoraDanho[i].precio);
		}

		if (gestionObjetos->mejoraRango[i].activa == 1 && gestionObjetos->mejoraRango[i].seVende == 1)
		{
			al_draw_bitmap_region(spriteSheetIcons, 2 * TAMANHO, 83 * TAMANHO, TAMANHO, TAMANHO, gestionObjetos->mejoraRango[i].posX, gestionObjetos->mejoraRango[i].posY, 0);
			al_draw_textf(fuenteJuego, al_map_rgb(192, 192, 192), gestionObjetos->mejoraRango[i].posX, gestionObjetos->mejoraRango[i].posY + 100, 0, "%d", gestionObjetos->mejoraRango[i].precio);
		}

		if (gestionObjetos->mejoraVelocidad[i].activa == 1 && gestionObjetos->mejoraVelocidad[i].seVende == 1)
		{
			al_draw_bitmap_region(spriteSheetIcons, 1 * TAMANHO, 85 * TAMANHO, TAMANHO, TAMANHO, gestionObjetos->mejoraVelocidad[i].posX, gestionObjetos->mejoraVelocidad[i].posY, 0);
			al_draw_textf(fuenteJuego, al_map_rgb(192, 192, 192), gestionObjetos->mejoraVelocidad[i].posX, gestionObjetos->mejoraVelocidad[i].posY + 100, 0, "%d", gestionObjetos->mejoraVelocidad[i].precio);
		}
	}
	
	if (gestionObjetos->minimapa.activa == 1 && gestionObjetos->minimapa.seVende == 1)
	{
		al_draw_bitmap_region(spriteSheetIcons, 7 * TAMANHO, 0 * TAMANHO, TAMANHO, TAMANHO, gestionObjetos->minimapa.posX, gestionObjetos->minimapa.posY, 0);
		al_draw_textf(fuenteJuego, al_map_rgb(192, 192, 192), gestionObjetos->minimapa.posX, gestionObjetos->minimapa.posY + 100, 0, "%d", gestionObjetos->minimapa.precio);
	}
	
	//Dibujo enemigos
	AnimacionEnemigos(spriteSheet, gestionEnemigos, estadoSistema);
	
	//Puntero del mouse
	al_draw_scaled_bitmap(spriteSheetCrosshair, 10 * 16, 3 * 16, 16, 16, mouse.posX - 30, mouse.posY - 32, TAMANHO, TAMANHO, 0);

	////////////////////////////////////////////Render minimapa

	if (al_key_down(&estadoSistema->teclado, ALLEGRO_KEY_TAB))
	{
		al_draw_filled_rectangle(0, 0, 2000, 2000, al_map_rgba(0, 0, 0, 128));

		teclado->sePresionoTAB = 1;

		estadoMapa->minimapa[estadoMapa->actualMapaY][estadoMapa->actualMapaX] = 2;

		if (estadoMapa->minimapaVendido == 1)
		{
			for (int i = 0; i < FILAS_MAPA; i++)
			{
				for (int j = 0; j < COLUMNAS_MAPA; j++)
				{
					if (estadoMapa->minimapa[i][j] == 1)
					{
						al_draw_filled_rectangle(500 + j * TAMANHO, 100 + i * TAMANHO, 500 + j * TAMANHO + TAMANHO - 2, 100 + i * TAMANHO + TAMANHO - 2, al_map_rgba(255, 255, 255, 255));
					}	
					if (estadoMapa->minimapa[i][j] == 0)
					{
						//al_draw_filled_rectangle(500 + j * TAMANHO, 100 + i * TAMANHO, 500 + j * TAMANHO + TAMANHO, 100 + i * TAMANHO + TAMANHO, al_map_rgb(255, 255, 0));
					}	
					if (estadoMapa->minimapa[i][j] == 2)
					{
						al_draw_filled_rectangle(500 + j * TAMANHO, 100 + i * TAMANHO, 500 + j * TAMANHO + TAMANHO - 2, 100 + i * TAMANHO + TAMANHO - 2, al_map_rgba(255, 0, 0, 255));
					}	
				}
			}
		}
		else
		{
			al_draw_scaled_bitmap(spriteSheetIcons, 12 * TAMANHO, 0 * TAMANHO, TAMANHO, TAMANHO, 800, 400, TAMANHO * 5, TAMANHO * 5, 0);
		}
		

		if (gestionObjetos->mapaNaranjo.seObtuvo == 1)
		{
			al_draw_bitmap_region(spriteSheetIcons, 12 * TAMANHO, 18 * TAMANHO, TAMANHO, TAMANHO, 700, 750, 0);
		}

		if (gestionObjetos->mapaRojo.seObtuvo == 1)
		{
			al_draw_bitmap_region(spriteSheetIcons, 8 * TAMANHO, 18 * TAMANHO, TAMANHO, TAMANHO, 750, 750, 0);
		}

		if (gestionObjetos->mapaAzul.seObtuvo == 1)
		{
			al_draw_bitmap_region(spriteSheetIcons, 9 * TAMANHO, 18 * TAMANHO, TAMANHO, TAMANHO, 800, 750, 0);
		}

		if (gestionObjetos->mapaVerde.seObtuvo == 1)
		{
			al_draw_bitmap_region(spriteSheetIcons, 10 * TAMANHO, 18 * TAMANHO, TAMANHO, TAMANHO, 850, 750, 0);
		}
	}
	
	/////////////////////////////////////////////////////////////////////////////

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

void ColisionMapaBalas(char mapa[FILAS_HABITACION][COLUMNAS_HABITACION])
{
	int paredesPosX[600];
	int paredesPosY[600];
	int paredActual = 0;

	for (int i = 0; i < 600; i++)
	{
		paredesPosX[i] = 0; 
		paredesPosY[i] = 0; 
	}

	for (int i = 0; i < FILAS_HABITACION; i++)
	{
		for (int j = 0; j < COLUMNAS_HABITACION; j++)
		{
			if (mapa[i][j] == '#' || mapa[i][j] == 'l' || mapa[i][j] == 'P' ||  mapa[i][j] == 'c')
			{
				paredesPosX[paredActual] = j * TAMANHO;
				paredesPosY[paredActual] = i * TAMANHO;
				paredActual++;
			}
		}
	}

	for (int i = 0; i < MAX_BALAS; i++)
	{
		for (int j = 0; j < 600; j++)
		{
			if (Colicion(personaje.bala[i].posX, personaje.bala[i].posY, TAMANHO / 4, TAMANHO / 4, paredesPosX[j], paredesPosY[j], TAMANHO, TAMANHO))
			{
				personaje.bala[i].activa = 0;
			}
		}
	}
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

	//Inicializando musica y samples
	al_install_audio();
	al_init_acodec_addon();

	//Se abre un mixer donde 10 sonidos suceden a la vez 
	al_reserve_samples(10);

	//Inicializar colores transparentes
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
}

void InitGameComponents(ALLEGRO_DISPLAY *ventana, mouse_ *mouse, ranking_ *ranking, gestionEnemigos_ *gestionEnemigos, gestionObjetos_ *gestionObjetos, estadoSistema_ *estadoSistema, estadoMapa_ *estadoMapa, gestionDianas_ *gestionDianas, gestionInteractuables_ *gestionInteractuables, registroMundo_ *registroMundo, controlAnimacion_ *controlAnimacion)
{
	//Inicializar ventana
	al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
	ventana = al_create_display(640, 480);
	if(!ventana) printf("Error al abrir ventana");

	//Inicializando parametros de animacion
	controlAnimacion->transparencia1 = 255;
	controlAnimacion->transparencia2 = 255;
	controlAnimacion->transparencia3 = 255;
	controlAnimacion->transparencia4 = 255;
	controlAnimacion->transparencia5 = 0;
	controlAnimacion->timer = 0;

	//Inicializando objetos secundarios
	gestionDianas->dianasActuales = 0;
	gestionDianas->cantidadDianasDestruidas = 0;

	//Inicializando ranking
	ranking->indiceNombres = 0;
	ranking->indicePuntajes = 0;

	//Inicializando Mouse
	mouse->posX = 0;
	mouse->posY = 0;
	mouse->tamanho = 7;

	//Inicializando contadores de objetos
	gestionObjetos->monedasActual = 0;
	gestionObjetos->llavesActual = 0;
	gestionObjetos->cofreActual = 0;
	gestionObjetos->municionesActual = 0;
	gestionObjetos->vidasObjetoActual = 0;
	gestionObjetos->mejoraDanhoActual = 0;
	gestionObjetos->mejoraRangoActual = 0;
	gestionObjetos->mejoraVelocidadActual = 0;

	//Inicializando temporizadores
	estadoSistema->controlSprites = 0;
	estadoSistema->controlSpritesMenu = 0;

	//Inicializando mapa
	estadoMapa->actualMapaX = COLUMNAS_MAPA / 2 + 1;
	estadoMapa->actualMapaY = FILAS_MAPA / 2 + 1;
	estadoMapa->salaVacia = 0;
	estadoMapa->seGeneroUnaRecompensa = 0;
	estadoMapa->minimapaVendido = 0;

	//Inicializando jugador
	personaje.velocidad = 16; ////////// 7
	personaje.animacionJugador = 0;
	personaje.dirJugador.derecha = 0;
	personaje.dirJugador.izquierda = 1;
	personaje.vidas = 6;
	personaje.invulnerable = 0;
	personaje.cantidadMonedas = 10; ///////// 0
	personaje.cantidadLlaves = 1; //////////////// originalmente 0
	personaje.rangoDeBalas = 400;
	personaje.balasrestantes = MAX_BALAS;
	personaje.puntaje = 0;

	//Inicializando enemigos
	for (int i = 0; i < MAX_ENEMIGOS; i++)
	{
		gestionEnemigos->slime[i].posX = 0;
		gestionEnemigos->slime[i].posY = 0;
		gestionEnemigos->slime[i].velocidad = 3;
		gestionEnemigos->slime[i].activa = 0;
		gestionEnemigos->slime[i].direccion = 1;
		gestionEnemigos->slime[i].vida = 4;
		gestionEnemigos->slime[i].posXGeneracion = 0;
		gestionEnemigos->slime[i].posYGeneracion = 0;
		gestionEnemigos->slime[i].ataquesEnemigos = 0;
		gestionEnemigos->slime[i].indiceEnemigo = 0;

		gestionEnemigos->mago[i].posX = 0;
		gestionEnemigos->mago[i].posY = 0;
		gestionEnemigos->mago[i].velocidad = 3;
		gestionEnemigos->mago[i].activa = 0;
		gestionEnemigos->mago[i].direccion = 0;
		gestionEnemigos->mago[i].vida = 4;
		gestionEnemigos->mago[i].posXGeneracion = 0;
		gestionEnemigos->mago[i].posYGeneracion = 0;
		gestionEnemigos->mago[i].ataquesEnemigos = 0;
		gestionEnemigos->mago[i].rangoDeBalas = 450;
		gestionEnemigos->mago[i].indiceEnemigo = 0;

		gestionEnemigos->aranha[i].posX = 0;
		gestionEnemigos->aranha[i].posY = 0;
		gestionEnemigos->aranha[i].velocidad = 5;
		gestionEnemigos->aranha[i].activa = 0;
		gestionEnemigos->aranha[i].direccion = 1;
		gestionEnemigos->aranha[i].vida = 2;
		gestionEnemigos->aranha[i].posXGeneracion = 0;
		gestionEnemigos->aranha[i].posYGeneracion = 0;
		gestionEnemigos->aranha[i].ataquesEnemigos = 0;
		gestionEnemigos->aranha[i].chocoConPared = 0;
		gestionEnemigos->aranha[i].auxRandAranha = 0;
		gestionEnemigos->aranha[i].indiceEnemigo = 0;
	}

	gestionEnemigos->jefe.posX = 0;
	gestionEnemigos->jefe.posY = 0;
	gestionEnemigos->jefe.velocidad = 4;
	gestionEnemigos->jefe.activa = 0;
	gestionEnemigos->jefe.direccion = 0;
	gestionEnemigos->jefe.vida = 20;
	gestionEnemigos->jefe.posXGeneracion = 0;
	gestionEnemigos->jefe.posYGeneracion = 0;
	gestionEnemigos->jefe.ataquesEnemigos = 0;
	gestionEnemigos->jefe.rangoDeBalas = 500;

	//Inicializando balas
	for (int i = 0; i < MAX_BALAS; i++)
	{
		personaje.bala[i].posX = 0;
		personaje.bala[i].posY = 0;
		personaje.bala[i].velocidad = 10;		
		personaje.bala[i].activa = 0;
		personaje.bala[i].danho = 10; //////// 1              
		personaje.bala[i].anguloBalaX = 0;
		personaje.bala[i].anguloBalaY = 0;
		personaje.bala[i].seDisparo = 0;

		gestionEnemigos->jefe.bala[i].posX = 0;
		gestionEnemigos->jefe.bala[i].posY = 0;
		gestionEnemigos->jefe.bala[i].velocidad = 10;
		gestionEnemigos->jefe.bala[i].activa = 0;
		gestionEnemigos->jefe.bala[i].danho = 1;
		gestionEnemigos->jefe.bala[i].anguloBalaX = 0;
		gestionEnemigos->jefe.bala[i].anguloBalaY = 0;

		for (int m = 0; m < MAX_ENEMIGOS; m++)
		{
			gestionEnemigos->mago[m].bala[i].posX = 0;
			gestionEnemigos->mago[m].bala[i].posY = 0;
			gestionEnemigos->mago[m].bala[i].velocidad = 10;
			gestionEnemigos->mago[m].bala[i].activa = 0;
			gestionEnemigos->mago[m].bala[i].danho = 1;
			gestionEnemigos->mago[m].bala[i].anguloBalaX = 0;
			gestionEnemigos->mago[m].bala[i].anguloBalaY = 0;
		}
	}

	//Inicializar objetos
	for (int i = 0; i < MAX_OBJETOS; i++)
	{
		gestionObjetos->monedas[i].posX = 0;
		gestionObjetos->monedas[i].posY = 0;
		gestionObjetos->monedas[i].activa = 0;
		gestionObjetos->monedas[i].especial = 0;
		gestionObjetos->cofres[i].cofreAbierto = 0;

		gestionObjetos->mejoraDanho[i].activa = 0;

		gestionObjetos->mejoraRango[i].activa = 0;

		gestionObjetos->mejoraVelocidad[i].activa = 0;
	}

	gestionObjetos->mapaVerde.activa = 0;
	gestionObjetos->mapaVerde.columna = 0;
	gestionObjetos->mapaVerde.fila = 0;
	gestionObjetos->mapaVerde.posX = 0;
	gestionObjetos->mapaVerde.posY = 0;
	gestionObjetos->mapaVerde.seObtuvo = 0;
	gestionObjetos->mapaVerde.especial = 0;
	gestionObjetos->mapaVerde.seGenero = 0;
	gestionObjetos->mapaVerde.idRegistro = 0;

	gestionObjetos->mapaAzul.activa = 0;
	gestionObjetos->mapaAzul.columna = 0;
	gestionObjetos->mapaAzul.fila = 0;
	gestionObjetos->mapaAzul.posX = 0;
	gestionObjetos->mapaAzul.posY = 0;
	gestionObjetos->mapaAzul.seObtuvo = 0;
	gestionObjetos->mapaAzul.especial = 0;

	gestionObjetos->mapaNaranjo.activa = 0;
	gestionObjetos->mapaNaranjo.columna = 0;
	gestionObjetos->mapaNaranjo.fila = 0;
	gestionObjetos->mapaNaranjo.posX = 0;
	gestionObjetos->mapaNaranjo.posY = 0;
	gestionObjetos->mapaNaranjo.seObtuvo = 0;
	gestionObjetos->mapaNaranjo.especial = 0;

	gestionObjetos->mapaRojo.activa = 0;
	gestionObjetos->mapaRojo.columna = 0;
	gestionObjetos->mapaRojo.fila = 0;
	gestionObjetos->mapaRojo.posX = 0;
	gestionObjetos->mapaRojo.posY = 0;
	gestionObjetos->mapaRojo.seObtuvo = 0;
	gestionObjetos->mapaRojo.especial = 0;

	//Inicializar mapa
	for (int i = 0; i < FILAS_MAPA; i++)
	{
		for (int j = 0; j < COLUMNAS_MAPA; j++)
		{
			estadoMapa->mapa[i][j] = NULL;
		}
	}

	//Interactuables inicializacion
	for (int i = 0; i < MAX_INTERACTUABLES; i++)
	{
		gestionInteractuables->fogata[i].activa = 0;
		gestionInteractuables->fogata[i].posX = 0;
		gestionInteractuables->fogata[i].posY = 0;
		gestionInteractuables->fogata[i].fogataActiva = 0;
		gestionInteractuables->fogata[i].columna = 0;
		gestionInteractuables->fogata[i].fila = 0;
	}

	gestionInteractuables->cantidadfogatasActivas = 0;
	gestionInteractuables->fogataActual = 0;

	//Dianas inicializacion
	for (int i = 0; i < MAX_DIANAS; i++)
	{
		gestionDianas->dianas[i].activa = 0;
		gestionDianas->dianas[i].velocidad = 10;
		gestionDianas->dianas[i].chocoConPared = 0;
		gestionDianas->dianas[i].auxRanDianas = 0;
		gestionDianas->dianas[i].destruida = 0;
		gestionDianas->dianas[i].chocoConPared = 0;
	 	gestionDianas->dianas[i].columna = 0;
		gestionDianas->dianas[i].fila = 0;
	}

	gestionDianas->dianasActuales = 0;
	gestionDianas->cantidadDianasDestruidas = 0;

	//Inicializacion registros
	registroMundo->indiceTienda = 0;
	registroMundo->cantidadInteractuables = 0;
	registroMundo->cantidadRecompensas = 0;
	registroMundo->cantidadMuertos = 0;

	for (int i = 0; i < MAX_REGISTROS; i++)
	{
		registroMundo->registroTienda[i].comprado = 0;
		registroMundo->registroTienda[i].idObjeto = 0;
		registroMundo->registroTienda[i].columna = 0;
		registroMundo->registroTienda[i].fila = 0;
		registroMundo->registroTienda[i].mapaX = 0;
		registroMundo->registroTienda[i].mapaY = 0;

		registroMundo->registroMuertes[i].columna = 0;
		registroMundo->registroMuertes[i].fila = 0;
		registroMundo->registroMuertes[i].mapaX = 0;
		registroMundo->registroMuertes[i].mapaY = 0;
		
		registroMundo->registroInteractuables[i].columna = 0;
		registroMundo->registroInteractuables[i].fila = 0;
		registroMundo->registroInteractuables[i].fogataEncendida = 0;
		registroMundo->registroInteractuables[i].mapaX = 0;
		registroMundo->registroInteractuables[i].mapaY = 0;
		registroMundo->registroInteractuables[i].dianaDestruida = 0;

		registroMundo->registroRecompensas[i].columna = 0;
		registroMundo->registroRecompensas[i].fila = 0;
		registroMundo->registroRecompensas[i].mapaX = 0;
		registroMundo->registroRecompensas[i].mapaY = 0;
		registroMundo->registroRecompensas[i].seAbrioUnCofre = 0;
		registroMundo->registroRecompensas[i].seObtuvoUnaRecompensa = 0;
		registroMundo->registroRecompensas[i].seObtuvoUnCargador = 0;
		registroMundo->registroRecompensas[i].seRecogioElItem = 0;
		registroMundo->registroRecompensas[i].idItem = '\0';
	}
}

void InputHandle(estadoJuego_ *estadoJuego, controlMenu_ *controlMenu, ranking_ *ranking, ALLEGRO_EVENT_QUEUE *colaEventos, estadoSistema_ *estadoSistema)
{
	ALLEGRO_EVENT evento;

	al_get_keyboard_state(&estadoSistema->teclado); //Actualiza constantemente el estado del teclado
	al_get_mouse_state(&estadoSistema->mouse); // Actualiza constantemente el estado del mouse

	mouse.posX = estadoSistema->mouse.x;
	mouse.posY = estadoSistema->mouse.y;

	//Apagar programa con ESC
	if (al_key_down(&estadoSistema->teclado, ALLEGRO_KEY_ESCAPE))
	{
		estadoJuego->JUEGO = 0;
		estadoJuego->MENU = 0;
		estadoJuego->ANIMACION_INICIAL = 0;
		estadoJuego->REINICIAR = 0;
		estadoJuego->SISTEMA = 0;
	}

	//saltar animacion
	if (estadoJuego->ANIMACION_INICIAL == 1 && al_key_down(&estadoSistema->teclado, ALLEGRO_KEY_SPACE))
	{
		estadoJuego->ANIMACION_INICIAL = 0;
		estadoJuego->JUEGO = 1;
	}

	//Input de la pantallaPonerNombre al poner nombre
	while (al_get_next_event(colaEventos, &evento))
	{
		if (estadoJuego->MENU == 1 && controlMenu->pantallaPonerNombre == 1)
		{
			if (evento.type == ALLEGRO_EVENT_KEY_CHAR)
			{
				controlMenu->unichar = evento.keyboard.unichar;

				if (al_key_down(&estadoSistema->teclado, ALLEGRO_KEY_ENTER))
				{
					estadoJuego->ANIMACION_INICIAL = 1;
					estadoJuego->MENU = 0;
					controlMenu->pantallaPonerNombre = 0;
				}
				else if (al_key_down(&estadoSistema->teclado, ALLEGRO_KEY_BACKSPACE) && controlMenu->longitudNombre > 0)
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

void AnimacionPersonaje(ALLEGRO_BITMAP *spriteSheet, ALLEGRO_BITMAP *spriteSheetCaminarCaballero, estadoSistema_ *estadoSistema)
{
	estadoSistema->controlSprites += 1;
	
	if (personaje.invulnerable == 0)
	{
		//Personaje caminando hacia la derecha
		if (personaje.dirJugador.derecha != 0 && personaje.animacionJugador != 0)
		{
			if (estadoSistema->controlSprites > 0 && estadoSistema->controlSprites <= 10)
			{
				al_draw_bitmap_region(spriteSheetCaminarCaballero, 8 * TAMANHO, 24 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, ALLEGRO_FLIP_HORIZONTAL);	
			}
			if (estadoSistema->controlSprites > 10 && estadoSistema->controlSprites <= 20)
			{
				al_draw_bitmap_region(spriteSheetCaminarCaballero, 9 * TAMANHO, 24 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, ALLEGRO_FLIP_HORIZONTAL);	
			}
			if (estadoSistema->controlSprites > 20 && estadoSistema->controlSprites <= 30)
			{
				al_draw_bitmap_region(spriteSheetCaminarCaballero, 10 * TAMANHO, 24 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, ALLEGRO_FLIP_HORIZONTAL);	
			}
			if (estadoSistema->controlSprites > 30 && estadoSistema->controlSprites <= 40)
			{
				al_draw_bitmap_region(spriteSheetCaminarCaballero, 11 * TAMANHO, 24 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, ALLEGRO_FLIP_HORIZONTAL);	
			}
		}
		else if (personaje.dirJugador.derecha != 0)
		{
			if (estadoSistema->controlSprites > 0 && estadoSistema->controlSprites <= 10)
			{
				al_draw_bitmap_region(spriteSheet, 8 * TAMANHO, 24 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, ALLEGRO_FLIP_HORIZONTAL);	
			}
			if (estadoSistema->controlSprites > 10 && estadoSistema->controlSprites <= 20)
			{
				al_draw_bitmap_region(spriteSheet, 9 * TAMANHO, 24 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, ALLEGRO_FLIP_HORIZONTAL);	
			}
			if (estadoSistema->controlSprites > 20 && estadoSistema->controlSprites <= 30)
			{
				al_draw_bitmap_region(spriteSheet, 10 * TAMANHO, 24 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, ALLEGRO_FLIP_HORIZONTAL);	
			}
			if (estadoSistema->controlSprites > 30 && estadoSistema->controlSprites <= 40)
			{
				al_draw_bitmap_region(spriteSheet, 11 * TAMANHO, 24 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, ALLEGRO_FLIP_HORIZONTAL);	
			}
		}

		//Personaje caminando hacia la izquierda
		if (personaje.dirJugador.izquierda != 0 && personaje.animacionJugador != 0)
		{
			if (estadoSistema->controlSprites > 0 && estadoSistema->controlSprites <= 10)
			{
				al_draw_bitmap_region(spriteSheetCaminarCaballero, 8 * TAMANHO, 24 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, 0);	
			}
			if (estadoSistema->controlSprites > 10 && estadoSistema->controlSprites <= 20)
			{
				al_draw_bitmap_region(spriteSheetCaminarCaballero, 9 * TAMANHO, 24 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, 0);	
			}
			if (estadoSistema->controlSprites > 20 && estadoSistema->controlSprites <= 30)
			{
				al_draw_bitmap_region(spriteSheetCaminarCaballero, 10 * TAMANHO, 24 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, 0);	
			}
			if (estadoSistema->controlSprites > 30 && estadoSistema->controlSprites <= 40)
			{
				al_draw_bitmap_region(spriteSheetCaminarCaballero, 11 * TAMANHO, 24 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, 0);	
			}
		}
		else if (personaje.dirJugador.izquierda != 0)
		{
			if (estadoSistema->controlSprites > 0 && estadoSistema->controlSprites <= 10)
			{
				al_draw_bitmap_region(spriteSheet, 8 * TAMANHO, 24 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, 0);	
			}
			if (estadoSistema->controlSprites > 10 && estadoSistema->controlSprites <= 20)
			{
				al_draw_bitmap_region(spriteSheet, 9 * TAMANHO, 24 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, 0);	
			}
			if (estadoSistema->controlSprites > 20 && estadoSistema->controlSprites <= 30)
			{
				al_draw_bitmap_region(spriteSheet, 10 * TAMANHO, 24 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, 0);	
			}
			if (estadoSistema->controlSprites > 30 && estadoSistema->controlSprites <= 40)
			{
				al_draw_bitmap_region(spriteSheet, 11 * TAMANHO, 24 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, 0);	
			}
		}
	} 
	else if (PersonajeInvulnerable > 0)
	{
		//Personaje caminando hacia la derecha
		if (personaje.dirJugador.derecha != 0 && personaje.animacionJugador != 0)
		{
			if (estadoSistema->controlSprites > 0 && estadoSistema->controlSprites < 10)
			{
				al_draw_bitmap_region(spriteSheetCaminarCaballero, 8 * TAMANHO, 24 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, ALLEGRO_FLIP_HORIZONTAL);	
			}
			if (estadoSistema->controlSprites > 10 && estadoSistema->controlSprites < 20)
			{
				al_draw_bitmap_region(spriteSheetCaminarCaballero, 9 * TAMANHO, 24 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, ALLEGRO_FLIP_HORIZONTAL);	
			}
			if (estadoSistema->controlSprites > 20 && estadoSistema->controlSprites < 30)
			{
				al_draw_bitmap_region(spriteSheetCaminarCaballero, 10 * TAMANHO, 24 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, ALLEGRO_FLIP_HORIZONTAL);	
			}
			if (estadoSistema->controlSprites > 30 && estadoSistema->controlSprites < 40)
			{
				al_draw_bitmap_region(spriteSheetCaminarCaballero, 11 * TAMANHO, 24 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, ALLEGRO_FLIP_HORIZONTAL);	
			}
		}
		else if (personaje.dirJugador.derecha != 0)
		{
			if (estadoSistema->controlSprites > 0 && estadoSistema->controlSprites < 10)
			{
				al_draw_bitmap_region(spriteSheet, 8 * TAMANHO, 24 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, ALLEGRO_FLIP_HORIZONTAL);	
			}
			if (estadoSistema->controlSprites > 10 && estadoSistema->controlSprites < 20)
			{
				al_draw_bitmap_region(spriteSheet, 9 * TAMANHO, 24 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, ALLEGRO_FLIP_HORIZONTAL);	
			}
			if (estadoSistema->controlSprites > 20 && estadoSistema->controlSprites < 30)
			{
				al_draw_bitmap_region(spriteSheet, 10 * TAMANHO, 24 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, ALLEGRO_FLIP_HORIZONTAL);	
			}
			if (estadoSistema->controlSprites > 30 && estadoSistema->controlSprites < 40)
			{
				al_draw_bitmap_region(spriteSheet, 11 * TAMANHO, 24 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, ALLEGRO_FLIP_HORIZONTAL);	
			}
		}

		//Personaje caminando hacia la izquierda
		if (personaje.dirJugador.izquierda != 0 && personaje.animacionJugador != 0)
		{
			if (estadoSistema->controlSprites > 0 && estadoSistema->controlSprites < 10)
			{
				al_draw_bitmap_region(spriteSheetCaminarCaballero, 8 * TAMANHO, 24 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, 0);	
			}
			if (estadoSistema->controlSprites > 10 && estadoSistema->controlSprites < 20)
			{
				al_draw_bitmap_region(spriteSheetCaminarCaballero, 9 * TAMANHO, 24 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, 0);	
			}
			if (estadoSistema->controlSprites > 20 && estadoSistema->controlSprites < 30)
			{
				al_draw_bitmap_region(spriteSheetCaminarCaballero, 10 * TAMANHO, 24 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, 0);	
			}
			if (estadoSistema->controlSprites > 30 && estadoSistema->controlSprites < 40)
			{
				al_draw_bitmap_region(spriteSheetCaminarCaballero, 11 * TAMANHO, 24 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, 0);	
			}
		}
		else if (personaje.dirJugador.izquierda != 0)
		{
			if (estadoSistema->controlSprites > 0 && estadoSistema->controlSprites < 10)
			{
				al_draw_bitmap_region(spriteSheet, 8 * TAMANHO, 24 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, 0);	
			}
			if (estadoSistema->controlSprites > 10 && estadoSistema->controlSprites < 20)
			{
				al_draw_bitmap_region(spriteSheet, 9 * TAMANHO, 24 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, 0);	
			}
			if (estadoSistema->controlSprites > 20 && estadoSistema->controlSprites < 30)
			{
				al_draw_bitmap_region(spriteSheet, 10 * TAMANHO, 24 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, 0);	
			}
			if (estadoSistema->controlSprites > 30 && estadoSistema->controlSprites < 40)
			{
				al_draw_bitmap_region(spriteSheet, 11 * TAMANHO, 24 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, 0);	
			}
		}
	}
	
	if(estadoSistema->controlSprites >= 40)
	{
		estadoSistema->controlSprites = 0;
	}

	personaje.animacionJugador = 0;
}

void AnimacionEnemigos(ALLEGRO_BITMAP *spriteSheet, gestionEnemigos_ *gestionEnemigos, estadoSistema_ *estadoSistema)
{
	for (int i = 0; i < MAX_ENEMIGOS; i++)
	{
		if (gestionEnemigos->slime[i].activa != 0)
		{
			//Animacion en dir izquierda
			if(gestionEnemigos->slime[i].direccion == 2)
			{
				if (estadoSistema->controlSprites >= 0 && estadoSistema->controlSprites <= 10)
				{
					al_draw_bitmap_region(spriteSheet, 0 * TAMANHO, 22 * TAMANHO, TAMANHO, TAMANHO, gestionEnemigos->slime[i].posX, gestionEnemigos->slime[i].posY, 0);
				}
				if (estadoSistema->controlSprites > 10 && estadoSistema->controlSprites <= 20)
				{
					al_draw_bitmap_region(spriteSheet, 1 * TAMANHO, 22 * TAMANHO, TAMANHO, TAMANHO, gestionEnemigos->slime[i].posX, gestionEnemigos->slime[i].posY, 0);
				}
				if (estadoSistema->controlSprites > 20 && estadoSistema->controlSprites <= 30)
				{
					al_draw_bitmap_region(spriteSheet, 2 * TAMANHO, 22 * TAMANHO, TAMANHO, TAMANHO, gestionEnemigos->slime[i].posX, gestionEnemigos->slime[i].posY, 0);
				}
				if (estadoSistema->controlSprites > 30 && estadoSistema->controlSprites <= 40)
				{
					al_draw_bitmap_region(spriteSheet, 3 * TAMANHO, 22 * TAMANHO, TAMANHO, TAMANHO, gestionEnemigos->slime[i].posX, gestionEnemigos->slime[i].posY, 0);
				}
			}

			//Animacion en dir derecha
			if(gestionEnemigos->slime[i].direccion == 1)
			{
				if (estadoSistema->controlSprites >= 0 && estadoSistema->controlSprites <= 10)
				{
					al_draw_bitmap_region(spriteSheet, 0 * TAMANHO, 22 * TAMANHO, TAMANHO, TAMANHO, gestionEnemigos->slime[i].posX, gestionEnemigos->slime[i].posY, ALLEGRO_FLIP_HORIZONTAL);
				}
				if (estadoSistema->controlSprites > 10 && estadoSistema->controlSprites <= 20)
				{
					al_draw_bitmap_region(spriteSheet, 1 * TAMANHO, 22 * TAMANHO, TAMANHO, TAMANHO, gestionEnemigos->slime[i].posX, gestionEnemigos->slime[i].posY, ALLEGRO_FLIP_HORIZONTAL);
				}
				if (estadoSistema->controlSprites > 20 && estadoSistema->controlSprites <= 30)
				{
					al_draw_bitmap_region(spriteSheet, 2 * TAMANHO, 22 * TAMANHO, TAMANHO, TAMANHO, gestionEnemigos->slime[i].posX, gestionEnemigos->slime[i].posY, ALLEGRO_FLIP_HORIZONTAL);
				}
				if (estadoSistema->controlSprites > 30 && estadoSistema->controlSprites <= 40)
				{
					al_draw_bitmap_region(spriteSheet, 3 * TAMANHO, 22 * TAMANHO, TAMANHO, TAMANHO, gestionEnemigos->slime[i].posX, gestionEnemigos->slime[i].posY, ALLEGRO_FLIP_HORIZONTAL);
				}
			}
		}

		if (gestionEnemigos->mago[i].activa != 0)
		{
			if (estadoSistema->controlSprites >= 0 && estadoSistema->controlSprites <= 10)
			{
				al_draw_bitmap_region(spriteSheet, 4 * TAMANHO, 23 * TAMANHO, TAMANHO, TAMANHO, gestionEnemigos->mago[i].posX, gestionEnemigos->mago[i].posY, 0);
			}
			if (estadoSistema->controlSprites > 10 && estadoSistema->controlSprites <= 20)
			{
				al_draw_bitmap_region(spriteSheet, 5 * TAMANHO, 23 * TAMANHO, TAMANHO, TAMANHO, gestionEnemigos->mago[i].posX, gestionEnemigos->mago[i].posY, 0);
			}
			if (estadoSistema->controlSprites > 20 && estadoSistema->controlSprites <= 30)
			{
				al_draw_bitmap_region(spriteSheet, 6 * TAMANHO, 23 * TAMANHO, TAMANHO, TAMANHO, gestionEnemigos->mago[i].posX, gestionEnemigos->mago[i].posY, 0);
			}
			if (estadoSistema->controlSprites > 30 && estadoSistema->controlSprites <= 40)
			{
				al_draw_bitmap_region(spriteSheet, 7 * TAMANHO, 23 * TAMANHO, TAMANHO, TAMANHO, gestionEnemigos->mago[i].posX, gestionEnemigos->mago[i].posY, 0);
			}
		}

		if (gestionEnemigos->aranha[i].activa != 0)
		{
			if (estadoSistema->controlSprites >= 0 && estadoSistema->controlSprites <= 10)
			{
				al_draw_bitmap_region(spriteSheet, 8 * TAMANHO, 27 * TAMANHO, TAMANHO, TAMANHO, gestionEnemigos->aranha[i].posX, gestionEnemigos->aranha[i].posY, 0);
			}
			if (estadoSistema->controlSprites > 10 && estadoSistema->controlSprites <= 20)
			{
				al_draw_bitmap_region(spriteSheet, 9 * TAMANHO, 27 * TAMANHO, TAMANHO, TAMANHO, gestionEnemigos->aranha[i].posX, gestionEnemigos->aranha[i].posY, 0);
			}
			if (estadoSistema->controlSprites > 20 && estadoSistema->controlSprites <= 30)
			{
				al_draw_bitmap_region(spriteSheet, 10 * TAMANHO, 27 * TAMANHO, TAMANHO, TAMANHO, gestionEnemigos->aranha[i].posX, gestionEnemigos->aranha[i].posY, 0);
			}
			if (estadoSistema->controlSprites > 30 && estadoSistema->controlSprites <= 40)
			{
				al_draw_bitmap_region(spriteSheet, 11 * TAMANHO, 27 * TAMANHO, TAMANHO, TAMANHO, gestionEnemigos->aranha[i].posX, gestionEnemigos->aranha[i].posY, 0);
			}
		}
	}

	if (gestionEnemigos->jefe.activa != 0)
	{
		if (estadoSistema->controlSprites >= 0 && estadoSistema->controlSprites <= 10)
		{
			al_draw_scaled_bitmap(spriteSheet, 4 * TAMANHO, 24 * TAMANHO, TAMANHO, TAMANHO, gestionEnemigos->jefe.posX, gestionEnemigos->jefe.posY, TAMANHO * 2, TAMANHO * 2, 0); 
		}
		if (estadoSistema->controlSprites > 10 && estadoSistema->controlSprites <= 20)
		{
			al_draw_scaled_bitmap(spriteSheet, 5 * TAMANHO, 24 * TAMANHO, TAMANHO, TAMANHO, gestionEnemigos->jefe.posX, gestionEnemigos->jefe.posY, TAMANHO * 2, TAMANHO * 2, 0); 
		}
		if (estadoSistema->controlSprites > 20 && estadoSistema->controlSprites <= 30)
		{
			al_draw_scaled_bitmap(spriteSheet, 6 * TAMANHO, 24 * TAMANHO, TAMANHO, TAMANHO, gestionEnemigos->jefe.posX, gestionEnemigos->jefe.posY, TAMANHO * 2, TAMANHO * 2, 0); 
		}
		if (estadoSistema->controlSprites > 30 && estadoSistema->controlSprites <= 40)
		{
			al_draw_scaled_bitmap(spriteSheet, 7 * TAMANHO, 24 * TAMANHO, TAMANHO, TAMANHO, gestionEnemigos->jefe.posX, gestionEnemigos->jefe.posY, TAMANHO * 2, TAMANHO * 2, 0); 
		}		
	}
}

void LogicaEnemigos(gestionEnemigos_ *gestionEnemigos, gestionObjetos_ *gestionObjetos, estadoMapa_ *estadoMapa, registroMundo_ *registroMundo, controlAudio_ *controlAudio)
{
	//Colicion gestionEnemigos->slime y pared
	int auxXSlime = 0;
	int auxYSlime = 0;
	int auxXAranha = 0;
	int auxYAranha = 0;

	ColisionEnemigos(gestionEnemigos, gestionObjetos, estadoMapa, registroMundo, controlAudio);

	DisparoEnemigos(gestionEnemigos, estadoMapa, controlAudio);

	LogicaJefe(gestionEnemigos, estadoMapa);

	RangoVisionEnemigo(gestionEnemigos);

	///////////////////////////////////////////////////////////////////////////////////////// Araña eligiendo que eje moverse
	for (int i = 0; i < MAX_ENEMIGOS; i++)
	{
		auxXAranha = gestionEnemigos->aranha[i].posX;
		auxYAranha = gestionEnemigos->aranha[i].posY;

		if (gestionEnemigos->aranha[i].auxRandAranha == 0)
		{
			gestionEnemigos->aranha[i].auxRandAranha = rand() % 2 + 1;
		}
		
		if (gestionEnemigos->aranha[i].auxRandAranha == 1 && gestionEnemigos->aranha[i].activa != 0)
		{
			if (gestionEnemigos->aranha[i].activa != 0 && gestionEnemigos->aranha[i].chocoConPared%2 == 0)
			{
				gestionEnemigos->aranha[i].posX += gestionEnemigos->aranha[i].velocidad;
			} 
			else if (gestionEnemigos->aranha[i].activa != 0 && gestionEnemigos->aranha[i].chocoConPared%2 != 0)
			{
				gestionEnemigos->aranha[i].posX -= gestionEnemigos->aranha[i].velocidad;
			}

			if (ColisionMapa(estadoMapa->sala, gestionEnemigos->aranha[i].posX, gestionEnemigos->aranha[i].posY) || 
				ColisionMapa(estadoMapa->sala, gestionEnemigos->aranha[i].posX + TAMANHO - 1, gestionEnemigos->aranha[i].posY) || 
				ColisionMapa(estadoMapa->sala, gestionEnemigos->aranha[i].posX + TAMANHO - 1, gestionEnemigos->aranha[i].posY + TAMANHO - 1) ||
				ColisionMapa(estadoMapa->sala, gestionEnemigos->aranha[i].posX, gestionEnemigos->aranha[i].posY + TAMANHO - 1))
			{
				gestionEnemigos->aranha[i].posX = auxXAranha;
				gestionEnemigos->aranha[i].chocoConPared++;

				Sonido(controlAudio, 8);
			}
		}
		
		if (gestionEnemigos->aranha[i].auxRandAranha == 2 && gestionEnemigos->aranha[i].activa != 0)
		{
			if (gestionEnemigos->aranha[i].activa != 0 && gestionEnemigos->aranha[i].chocoConPared%2 == 0)
			{
				gestionEnemigos->aranha[i].posY += gestionEnemigos->aranha[i].velocidad;
			} 
			else if (gestionEnemigos->aranha[i].activa != 0 && gestionEnemigos->aranha[i].chocoConPared%2 != 0)
			{
				gestionEnemigos->aranha[i].posY -= gestionEnemigos->aranha[i].velocidad;
			}

			if (ColisionMapa(estadoMapa->sala, gestionEnemigos->aranha[i].posX, gestionEnemigos->aranha[i].posY) || 
				ColisionMapa(estadoMapa->sala, gestionEnemigos->aranha[i].posX + TAMANHO - 1, gestionEnemigos->aranha[i].posY) || 
				ColisionMapa(estadoMapa->sala, gestionEnemigos->aranha[i].posX + TAMANHO - 1, gestionEnemigos->aranha[i].posY + TAMANHO - 1) ||
				ColisionMapa(estadoMapa->sala, gestionEnemigos->aranha[i].posX, gestionEnemigos->aranha[i].posY + TAMANHO - 1))
			{
				gestionEnemigos->aranha[i].posY = auxYAranha;
				gestionEnemigos->aranha[i].chocoConPared++;

				Sonido(controlAudio, 8);
			}
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////// Slimes

	//Slime persiguiendo al jugador
	for (int i = 0; i < MAX_ENEMIGOS; i++)
	{		
		if (gestionEnemigos->slime[i].activa != 0 && gestionEnemigos->slime[i].ataquesEnemigos == 1)
		{
			controlAudio->audioTimerSlime++;

			if (controlAudio->audioTimerSlime == 39)
			{
				Sonido(controlAudio, 11);
			}
			
			if (controlAudio->audioTimerSlime >= 40)
			{
				controlAudio->audioTimerSlime = 0;
			}

			auxXSlime = gestionEnemigos->slime[i].posX;
			auxYSlime = gestionEnemigos->slime[i].posY;

			if (personaje.posY > gestionEnemigos->slime[i].posY)
			{
				gestionEnemigos->slime[i].posY += gestionEnemigos->slime[i].velocidad;
			}

			if (personaje.posY < gestionEnemigos->slime[i].posY)
			{
				gestionEnemigos->slime[i].posY -= gestionEnemigos->slime[i].velocidad;
			}

			if (ColisionMapa(estadoMapa->sala, gestionEnemigos->slime[i].posX, gestionEnemigos->slime[i].posY) || 
			ColisionMapa(estadoMapa->sala, gestionEnemigos->slime[i].posX + TAMANHO - 1, gestionEnemigos->slime[i].posY) || 
			ColisionMapa(estadoMapa->sala, gestionEnemigos->slime[i].posX + TAMANHO - 1, gestionEnemigos->slime[i].posY + TAMANHO - 1) ||
			ColisionMapa(estadoMapa->sala, gestionEnemigos->slime[i].posX, gestionEnemigos->slime[i].posY + TAMANHO - 1))
			{
				gestionEnemigos->slime[i].posY = auxYSlime;
			}

			if (personaje.posX < gestionEnemigos->slime[i].posX)
			{
				gestionEnemigos->slime[i].posX -= gestionEnemigos->slime[i].velocidad;
				gestionEnemigos->slime[i].direccion = 2;
			}

			if (personaje.posX > gestionEnemigos->slime[i].posX)
			{
				gestionEnemigos->slime[i].posX += gestionEnemigos->slime[i].velocidad;
				gestionEnemigos->slime[i].direccion = 1;
			}

			if (ColisionMapa(estadoMapa->sala, gestionEnemigos->slime[i].posX, gestionEnemigos->slime[i].posY) || 
			ColisionMapa(estadoMapa->sala, gestionEnemigos->slime[i].posX + TAMANHO - 1, gestionEnemigos->slime[i].posY) || 
			ColisionMapa(estadoMapa->sala, gestionEnemigos->slime[i].posX + TAMANHO - 1, gestionEnemigos->slime[i].posY + TAMANHO - 1) ||
			ColisionMapa(estadoMapa->sala, gestionEnemigos->slime[i].posX, gestionEnemigos->slime[i].posY + TAMANHO - 1))
			{
				gestionEnemigos->slime[i].posX = auxXSlime;
			}
		}
	}
}

void RangoVisionEnemigo(gestionEnemigos_ *gestionEnemigos)
{
	int rangoVision = 12 * TAMANHO;

	int offSet = (rangoVision - TAMANHO) / 2;

	for (int i = 0; i < MAX_ENEMIGOS; i++)
	{
		if (Colicion(personaje.posX, personaje.posY, TAMANHO, TAMANHO, gestionEnemigos->slime[i].posX - offSet, gestionEnemigos->slime[i].posY - offSet, rangoVision, rangoVision))
		{
			gestionEnemigos->slime[i].ataquesEnemigos = 1;
		}

		if (Colicion(personaje.posX, personaje.posY, TAMANHO, TAMANHO, gestionEnemigos->mago[i].posX - offSet, gestionEnemigos->mago[i].posY - offSet, rangoVision, rangoVision))
		{
			gestionEnemigos->mago[i].ataquesEnemigos = 1;
		}
	}
}

void LogicaJefe(gestionEnemigos_ *gestionEnemigos, estadoMapa_ *estadoMapa)
{
	int auxXJefe = 0;
	int auxYJefe = 0;

	gestionEnemigos->jefe.timerMovimientoJefe++;

	if (gestionEnemigos->jefe.activa != 0 && gestionEnemigos->jefe.timerMovimientoJefe < 60)
	{
		auxXJefe = gestionEnemigos->jefe.posX;
		auxYJefe = gestionEnemigos->jefe.posY;

		if (personaje.posY > gestionEnemigos->jefe.posY)
		{
			gestionEnemigos->jefe.posY += gestionEnemigos->jefe.velocidad;
		}

		if (personaje.posY < gestionEnemigos->jefe.posY)
		{
			gestionEnemigos->jefe.posY -= gestionEnemigos->jefe.velocidad;
		}

		if (ColisionMapa(estadoMapa->sala, gestionEnemigos->jefe.posX, gestionEnemigos->jefe.posY) || 
		ColisionMapa(estadoMapa->sala, gestionEnemigos->jefe.posX + TAMANHO * 2 - 1, gestionEnemigos->jefe.posY) || 
		ColisionMapa(estadoMapa->sala, gestionEnemigos->jefe.posX + TAMANHO * 2 - 1, gestionEnemigos->jefe.posY + TAMANHO * 2 - 1) ||
		ColisionMapa(estadoMapa->sala, gestionEnemigos->jefe.posX, gestionEnemigos->jefe.posY + TAMANHO * 2 - 1))
		{
			gestionEnemigos->jefe.posY = auxYJefe;
		}

		if (personaje.posX < gestionEnemigos->jefe.posX)
		{
			gestionEnemigos->jefe.posX -= gestionEnemigos->jefe.velocidad;
			gestionEnemigos->jefe.direccion = 2;
		}

		if (personaje.posX > gestionEnemigos->jefe.posX)
		{
			gestionEnemigos->jefe.posX += gestionEnemigos->jefe.velocidad;
			gestionEnemigos->jefe.direccion = 1;
		}

		if (ColisionMapa(estadoMapa->sala, gestionEnemigos->jefe.posX, gestionEnemigos->jefe.posY) || 
		ColisionMapa(estadoMapa->sala, gestionEnemigos->jefe.posX + TAMANHO * 2 - 1, gestionEnemigos->jefe.posY) || 
		ColisionMapa(estadoMapa->sala, gestionEnemigos->jefe.posX + TAMANHO * 2 - 1, gestionEnemigos->jefe.posY + TAMANHO * 2 - 1) ||
		ColisionMapa(estadoMapa->sala, gestionEnemigos->jefe.posX, gestionEnemigos->jefe.posY + TAMANHO * 2 - 1))
		{
			gestionEnemigos->jefe.posX = auxXJefe;
		}
	}

	if (gestionEnemigos->jefe.timerMovimientoJefe >= 120)
	{
		gestionEnemigos->jefe.timerMovimientoJefe = 0;
	}
	
}

void ColisionEnemigos(gestionEnemigos_ *gestionEnemigos, gestionObjetos_ *gestionObjetos, estadoMapa_ *estadoMapa, registroMundo_ *registroMundo, controlAudio_ *controlAudio)
{
	/////////////////////////////////////////////////////////////////////// Colisiones arañas y jugador
	for (int i = 0; i < MAX_ENEMIGOS; i++)
	{
		if (gestionEnemigos->aranha[i].activa != 0)
		{
			//Colision gestionEnemigos->aranha y bala
			for (int j = 0; j < MAX_BALAS; j++)
			{
				if (Colicion(gestionEnemigos->aranha[i].posX, gestionEnemigos->aranha[i].posY, TAMANHO, TAMANHO, personaje.bala[j].posX, personaje.bala[j].posY, TAMANHO / 4, TAMANHO / 4))
				{
					if(personaje.bala[j].activa != 0)
					{
						gestionEnemigos->aranha[i].vida -= personaje.bala[j].danho;
						personaje.bala[j].activa = 0;
					}

					if(gestionEnemigos->aranha[i].vida <= 0 && gestionEnemigos->aranha[i].activa == 1)
					{
						gestionEnemigos->aranha[i].activa = 0;
						personaje.puntaje = personaje.puntaje + 10;

						// Aparicion gestionObjetos->monedas al morir un enemigo
						gestionObjetos->monedas[gestionObjetos->monedasActual].posX = gestionEnemigos->aranha[i].posX;
						gestionObjetos->monedas[gestionObjetos->monedasActual].posY = gestionEnemigos->aranha[i].posY;
						gestionObjetos->monedas[gestionObjetos->monedasActual].activa = 1;
						
						registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].mapaX = estadoMapa->actualMapaX;
						registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].mapaY = estadoMapa->actualMapaY;
						registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].posX = gestionObjetos->monedas[gestionObjetos->monedasActual].posX;
						registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].posY = gestionObjetos->monedas[gestionObjetos->monedasActual].posY;
						registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].idItem = 'm';
						registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].seRecogioElItem = 0;
						registroMundo->cantidadRecompensas ++;

						gestionObjetos->monedasActual ++;

						if (registroMundo->cantidadMuertos < 1000)
						{
							//Cuando muere un gestionEnemigos->aranha se registra el mapa donde murio en una posicion del arreglo
							strcpy(registroMundo->registroMuertes[registroMundo->cantidadMuertos].registroMapa, estadoMapa->mapa[estadoMapa->actualMapaY][estadoMapa->actualMapaX]);

							//Se registra su lugar de aparicion original
							registroMundo->registroMuertes[registroMundo->cantidadMuertos].fila = gestionEnemigos->aranha[i].posYGeneracion;
							registroMundo->registroMuertes[registroMundo->cantidadMuertos].columna = gestionEnemigos->aranha[i].posXGeneracion;

							//Registramos la parte del mapa donde murieron
							registroMundo->registroMuertes[registroMundo->cantidadMuertos].mapaX = estadoMapa->actualMapaX;
							registroMundo->registroMuertes[registroMundo->cantidadMuertos].mapaY = estadoMapa->actualMapaY;

							registroMundo->cantidadMuertos ++;
						}
					}
				}
			}

			//Colision gestionEnemigos->aranha y jugador
			if (Colicion(gestionEnemigos->aranha[i].posX, gestionEnemigos->aranha[i].posY, TAMANHO, TAMANHO, personaje.posX, personaje.posY, TAMANHO, TAMANHO) && personaje.invulnerable == 0)
			{
				personaje.vidas --;
				personaje.invulnerable = 1;
				Sonido(controlAudio, 4);
			}
		}
	}

	/////////////////////////////////////////////////////////////////////// Colisiones slimes y jugador
	for (int i = 0; i < MAX_ENEMIGOS; i++)
	{
		if (gestionEnemigos->slime[i].activa != 0)
		{
			//Colision gestionEnemigos->slime y bala
			for (int j = 0; j < MAX_BALAS; j++)
			{
				if (Colicion(gestionEnemigos->slime[i].posX, gestionEnemigos->slime[i].posY, TAMANHO, TAMANHO, personaje.bala[j].posX, personaje.bala[j].posY, TAMANHO / 4, TAMANHO / 4))
				{
					if(personaje.bala[j].activa != 0)
					{
						gestionEnemigos->slime[i].vida -= personaje.bala[j].danho;
						personaje.bala[j].activa = 0;
					}

					if(gestionEnemigos->slime[i].vida <= 0 && gestionEnemigos->slime[i].activa == 1)
					{
						gestionEnemigos->slime[i].activa = 0;
						personaje.puntaje = personaje.puntaje + 10;

						// Aparicion gestionObjetos->monedas al morir un enemigo
						gestionObjetos->monedas[gestionObjetos->monedasActual].posX = gestionEnemigos->slime[i].posX;
						gestionObjetos->monedas[gestionObjetos->monedasActual].posY = gestionEnemigos->slime[i].posY;
						gestionObjetos->monedas[gestionObjetos->monedasActual].activa = 1;

						registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].mapaX = estadoMapa->actualMapaX;
						registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].mapaY = estadoMapa->actualMapaY;
						registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].posX = gestionObjetos->monedas[gestionObjetos->monedasActual].posX;
						registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].posY = gestionObjetos->monedas[gestionObjetos->monedasActual].posY;
						registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].idItem = 'm';
						registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].seRecogioElItem = 0;
						registroMundo->cantidadRecompensas ++;

						gestionObjetos->monedasActual ++;

						if (registroMundo->cantidadMuertos < 1000)
						{
							//Cuando muere un gestionEnemigos->slime se registra el mapa donde murio en una posicion del arreglo
							strcpy(registroMundo->registroMuertes[registroMundo->cantidadMuertos].registroMapa, estadoMapa->mapa[estadoMapa->actualMapaY][estadoMapa->actualMapaX]);

							//Se registra su lugar de aparicion original
							registroMundo->registroMuertes[registroMundo->cantidadMuertos].fila = gestionEnemigos->slime[i].posYGeneracion;
							registroMundo->registroMuertes[registroMundo->cantidadMuertos].columna = gestionEnemigos->slime[i].posXGeneracion;

							//Registramos la parte del mapa donde murieron
							registroMundo->registroMuertes[registroMundo->cantidadMuertos].mapaX = estadoMapa->actualMapaX;
							registroMundo->registroMuertes[registroMundo->cantidadMuertos].mapaY = estadoMapa->actualMapaY;

							registroMundo->cantidadMuertos ++;
						}
					}
				}
			}

			//Colision gestionEnemigos->slime y jugador
			if (Colicion(gestionEnemigos->slime[i].posX, gestionEnemigos->slime[i].posY, TAMANHO, TAMANHO, personaje.posX, personaje.posY, TAMANHO, TAMANHO) && personaje.invulnerable == 0)
			{
				personaje.vidas --;
				personaje.invulnerable = 1;
				Sonido(controlAudio, 4);
			}
		}
	}

	//Coliciones entre slimes
	for (int k = 0; k < MAX_ENEMIGOS; k++)
    {
        if (gestionEnemigos->slime[k].activa != 0) 
		{
			for (int l = 0; l < MAX_ENEMIGOS; l++)
			{
				if (gestionEnemigos->slime[l].activa != 0) 
				{
					if (Colicion(gestionEnemigos->slime[k].posX, gestionEnemigos->slime[k].posY, TAMANHO, TAMANHO, gestionEnemigos->slime[l].posX, gestionEnemigos->slime[l].posY, TAMANHO, TAMANHO) && k != l)
					{
						gestionEnemigos->slime[k].posXAnterior = gestionEnemigos->slime[k].posX;
						gestionEnemigos->slime[k].posYAnterior = gestionEnemigos->slime[k].posY;
						gestionEnemigos->slime[l].posXAnterior = gestionEnemigos->slime[l].posX;
						gestionEnemigos->slime[l].posYAnterior = gestionEnemigos->slime[l].posY;

						//enfocandonos en el primer gestionEnemigos->slime
						if (gestionEnemigos->slime[k].posX > gestionEnemigos->slime[l].posX)
						{
							gestionEnemigos->slime[l].posX -= gestionEnemigos->slime[l].velocidad;
						}
						else if (gestionEnemigos->slime[k].posX < gestionEnemigos->slime[l].posX)
						{
							gestionEnemigos->slime[l].posX += gestionEnemigos->slime[l].velocidad;
						}

						if (gestionEnemigos->slime[k].posY > gestionEnemigos->slime[l].posY)
						{
							gestionEnemigos->slime[l].posY -= gestionEnemigos->slime[l].velocidad;
						}
						else if (gestionEnemigos->slime[k].posY < gestionEnemigos->slime[l].posY)
						{
							gestionEnemigos->slime[l].posY += gestionEnemigos->slime[l].velocidad;
						}

						//Enfocandonos en el segundo
						//enfocandonos en el primer gestionEnemigos->slime
						if (gestionEnemigos->slime[l].posX > gestionEnemigos->slime[k].posX)
						{
							gestionEnemigos->slime[k].posX -= gestionEnemigos->slime[k].velocidad;
						}
						else if (gestionEnemigos->slime[l].posX < gestionEnemigos->slime[k].posX)
						{
							gestionEnemigos->slime[k].posX += gestionEnemigos->slime[k].velocidad;
						}

						if (gestionEnemigos->slime[l].posY > gestionEnemigos->slime[k].posY)
						{
							gestionEnemigos->slime[k].posY -= gestionEnemigos->slime[k].velocidad;
						}
						else if (gestionEnemigos->slime[l].posY < gestionEnemigos->slime[k].posY)
						{
							gestionEnemigos->slime[k].posY += gestionEnemigos->slime[k].velocidad;
						}

						if (ColisionMapa(estadoMapa->sala, gestionEnemigos->slime[k].posX, gestionEnemigos->slime[k].posY) ||
							ColisionMapa(estadoMapa->sala, gestionEnemigos->slime[k].posX + TAMANHO - 1, gestionEnemigos->slime[k].posY) ||
							ColisionMapa(estadoMapa->sala, gestionEnemigos->slime[k].posX + TAMANHO - 1, gestionEnemigos->slime[k].posY + TAMANHO - 1) ||
							ColisionMapa(estadoMapa->sala, gestionEnemigos->slime[k].posX, gestionEnemigos->slime[k].posY + TAMANHO - 1))
						{
							gestionEnemigos->slime[k].posX = gestionEnemigos->slime[k].posXAnterior;
							gestionEnemigos->slime[k].posY = gestionEnemigos->slime[k].posYAnterior;
						}

						if (ColisionMapa(estadoMapa->sala, gestionEnemigos->slime[l].posX, gestionEnemigos->slime[l].posY) ||
							ColisionMapa(estadoMapa->sala, gestionEnemigos->slime[l].posX + TAMANHO - 1, gestionEnemigos->slime[l].posY) ||
							ColisionMapa(estadoMapa->sala, gestionEnemigos->slime[l].posX + TAMANHO - 1, gestionEnemigos->slime[l].posY + TAMANHO - 1) ||
							ColisionMapa(estadoMapa->sala, gestionEnemigos->slime[l].posX, gestionEnemigos->slime[l].posY + TAMANHO - 1))
						{
							gestionEnemigos->slime[l].posX = gestionEnemigos->slime[l].posXAnterior;
							gestionEnemigos->slime[l].posY = gestionEnemigos->slime[l].posYAnterior;
						}
					}
				}		
			}
		}
    }

	///////////////////////////////////////////////////////////////// COlisiones con el gestionEnemigos->mago y jugador

	for (int i = 0; i < MAX_ENEMIGOS; i++)
	{
		if (gestionEnemigos->mago[i].activa != 0)
		{
			for (int j = 0; j < MAX_BALAS; j++)
			{
				//Colicion balas personaje y gestionEnemigos->mago
				if (Colicion(gestionEnemigos->mago[i].posX, gestionEnemigos->mago[i].posY, TAMANHO, TAMANHO, personaje.bala[j].posX, personaje.bala[j].posY, TAMANHO / 4, TAMANHO / 4))
				{
					if(personaje.bala[j].activa != 0)
					{
						gestionEnemigos->mago[i].vida -= personaje.bala[j].danho;
						personaje.bala[j].activa = 0;
					}

					if(gestionEnemigos->mago[i].vida <= 0 && gestionEnemigos->mago[i].activa == 1)
					{
						gestionEnemigos->mago[i].activa = 0;
						personaje.puntaje = personaje.puntaje + 10;

						gestionObjetos->monedas[gestionObjetos->monedasActual].posX = gestionEnemigos->mago[i].posX;
						gestionObjetos->monedas[gestionObjetos->monedasActual].posY = gestionEnemigos->mago[i].posY;
						gestionObjetos->monedas[gestionObjetos->monedasActual].activa = 1;

						registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].mapaX = estadoMapa->actualMapaX;
						registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].mapaY = estadoMapa->actualMapaY;
						registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].posX = gestionObjetos->monedas[gestionObjetos->monedasActual].posX;
						registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].posY = gestionObjetos->monedas[gestionObjetos->monedasActual].posY;
						registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].idItem = 'm';
						registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].seRecogioElItem = 0;
						registroMundo->cantidadRecompensas ++;

						gestionObjetos->monedasActual ++;

						if (registroMundo->cantidadMuertos < 1000)
						{
							//Cuando muere un gestionEnemigos->slime se registra el mapa donde murio en una posicion del arreglo
							strcpy(registroMundo->registroMuertes[registroMundo->cantidadMuertos].registroMapa, estadoMapa->mapa[estadoMapa->actualMapaY][estadoMapa->actualMapaX]);

							//Se registra su lugar de aparicion original
							registroMundo->registroMuertes[registroMundo->cantidadMuertos].fila = gestionEnemigos->mago[i].posYGeneracion;
							registroMundo->registroMuertes[registroMundo->cantidadMuertos].columna = gestionEnemigos->mago[i].posXGeneracion;

							//Registramos la parte del mapa donde murieron
							registroMundo->registroMuertes[registroMundo->cantidadMuertos].mapaX = estadoMapa->actualMapaX;
							registroMundo->registroMuertes[registroMundo->cantidadMuertos].mapaY = estadoMapa->actualMapaY;

							registroMundo->cantidadMuertos ++;
						}
					}
				}

				//Colicion balas de gestionEnemigos->mago y personaje
				if (Colicion(gestionEnemigos->mago[i].bala[j].posX, gestionEnemigos->mago[i].bala[j].posY, TAMANHO / 4, TAMANHO / 4, personaje.posX, personaje.posY, TAMANHO, TAMANHO) && personaje.invulnerable == 0)
				{
					if (gestionEnemigos->mago[i].bala[j].activa != 0)
					{
						personaje.vidas --;
						gestionEnemigos->mago[i].bala[j].activa = 0;
						personaje.invulnerable = 1;
						Sonido(controlAudio, 4);
					}
				}
			}

			//Colision gestionEnemigos->mago y jugador
			if (Colicion(gestionEnemigos->mago[i].posX, gestionEnemigos->mago[i].posY, TAMANHO, TAMANHO, personaje.posX, personaje.posY, TAMANHO, TAMANHO) && personaje.invulnerable == 0)
			{
				personaje.vidas --;
				personaje.invulnerable = 1;
				Sonido(controlAudio, 4);
			}
		}
	}
	

	///////////////////////////////////////////////////////////////// Colisiones con el jefe y jugador
	if (gestionEnemigos->jefe.activa != 0)
	{
		for (int k = 0; k < MAX_BALAS; k++)
		{
			if (Colicion(gestionEnemigos->jefe.posX, gestionEnemigos->jefe.posY, TAMANHO * 2, TAMANHO * 2, personaje.bala[k].posX, personaje.bala[k].posY, TAMANHO / 4, TAMANHO / 4))
			{
				if(personaje.bala[k].activa != 0)
				{
					gestionEnemigos->jefe.vida -= personaje.bala[k].danho;
					personaje.bala[k].activa = 0;
				}

				if(gestionEnemigos->jefe.vida <= 0 && gestionEnemigos->jefe.activa == 1)
				{
					gestionEnemigos->jefe.activa = 0;
					personaje.puntaje = personaje.puntaje + 100;

					gestionObjetos->monedas[gestionObjetos->monedasActual].posX = gestionEnemigos->jefe.posX;
					gestionObjetos->monedas[gestionObjetos->monedasActual].posY = gestionEnemigos->jefe.posY;
					gestionObjetos->monedas[gestionObjetos->monedasActual].especial = 1;
					gestionObjetos->monedas[gestionObjetos->monedasActual].activa = 1;
					
					registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].mapaX = estadoMapa->actualMapaX;
					registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].mapaY = estadoMapa->actualMapaY;
					registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].posX = gestionObjetos->monedas[gestionObjetos->monedasActual].posX;
					registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].posY = gestionObjetos->monedas[gestionObjetos->monedasActual].posY;
					registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].idItem = 'e';
					registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].seRecogioElItem = 0;
					registroMundo->cantidadRecompensas ++;
					
					gestionObjetos->monedasActual ++;

					gestionObjetos->mapaRojo.activa = 1;
					gestionObjetos->mapaRojo.posX = gestionEnemigos->jefe.posX;
					gestionObjetos->mapaRojo.posY = gestionEnemigos->jefe.posY;

					registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].mapaX = estadoMapa->actualMapaX;
					registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].mapaY = estadoMapa->actualMapaY;
					registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].posX = gestionObjetos->mapaRojo.posX;
					registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].posY = gestionObjetos->mapaRojo.posY;
					registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].idItem = 'r';
					registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].seRecogioElItem = 0;
					registroMundo->cantidadRecompensas ++;

					if (registroMundo->cantidadMuertos < 1000)
					{
						//Cuando muere un gestionEnemigos->slime se registra el mapa donde murio en una posicion del arreglo
						strcpy(registroMundo->registroMuertes[registroMundo->cantidadMuertos].registroMapa, estadoMapa->mapa[estadoMapa->actualMapaY][estadoMapa->actualMapaX]);

						//Se registra su lugar de aparicion original
						registroMundo->registroMuertes[registroMundo->cantidadMuertos].fila = gestionEnemigos->jefe.posYGeneracion;
						registroMundo->registroMuertes[registroMundo->cantidadMuertos].columna = gestionEnemigos->jefe.posXGeneracion;

						//Registramos la parte del mapa donde murieron
						registroMundo->registroMuertes[registroMundo->cantidadMuertos].mapaX = estadoMapa->actualMapaX;
						registroMundo->registroMuertes[registroMundo->cantidadMuertos].mapaY = estadoMapa->actualMapaY;

						registroMundo->cantidadMuertos ++;
					}
				}
			}

			//Colision jugador y balas jefe
			if (Colicion(gestionEnemigos->jefe.bala[k].posX, gestionEnemigos->jefe.bala[k].posY, TAMANHO / 4, TAMANHO / 4, personaje.posX, personaje.posY, TAMANHO, TAMANHO) && personaje.invulnerable == 0)
			{
				personaje.vidas --;
				personaje.invulnerable = 1;
				Sonido(controlAudio, 4);
			}
		}

		//Colision jefe y jugador
		if (Colicion(gestionEnemigos->jefe.posX, gestionEnemigos->jefe.posY, TAMANHO * 2, TAMANHO * 2, personaje.posX, personaje.posY, TAMANHO, TAMANHO) && personaje.invulnerable == 0)
		{
			personaje.vidas --;
			personaje.invulnerable = 1;
			Sonido(controlAudio, 4);
		}
	}
}

void ColicionObjetos(gestionObjetos_ *gestionObjetos, estadoMapa_ *estadoMapa, registroMundo_ *registroMundo, controlAudio_ *controlAudio)
{
	int auxRand = 0;

	if (gestionObjetos->mapaRojo.activa != 0)
	{
		if (Colicion(personaje.posX, personaje.posY, TAMANHO, TAMANHO, gestionObjetos->mapaRojo.posX, gestionObjetos->mapaRojo.posY, TAMANHO, TAMANHO))
		{
			gestionObjetos->mapaRojo.activa = 0;	
			gestionObjetos->mapaRojo.seObtuvo = 1;

			Sonido(controlAudio, 5);

			for (int j = 0; j < MAX_REGISTROS; j++)
			{
				if (registroMundo->registroRecompensas[j].idItem == 'r' && registroMundo->registroRecompensas[j].seRecogioElItem == 0 && registroMundo->registroRecompensas[j].posX == gestionObjetos->mapaRojo.posX && registroMundo->registroRecompensas[j].posY == gestionObjetos->mapaRojo.posY)
				{
					registroMundo->registroRecompensas[j].seRecogioElItem = 1;
				}
			}
		}
	}

	if (gestionObjetos->mapaAzul.activa != 0)
	{
		if (Colicion(personaje.posX, personaje.posY, TAMANHO, TAMANHO, gestionObjetos->mapaAzul.posX, gestionObjetos->mapaAzul.posY, TAMANHO, TAMANHO))
		{
			gestionObjetos->mapaAzul.activa = 0;	
			gestionObjetos->mapaAzul.seObtuvo = 1;

			Sonido(controlAudio, 5);
		}
	}

	if (gestionObjetos->mapaVerde.activa != 0)
	{
		if (Colicion(personaje.posX, personaje.posY, TAMANHO, TAMANHO, gestionObjetos->mapaVerde.posX, gestionObjetos->mapaVerde.posY, TAMANHO, TAMANHO))
		{
			gestionObjetos->mapaVerde.activa = 0;	
			gestionObjetos->mapaVerde.seObtuvo = 1;
			
			//Al recoger el item se guarda la accion misma
			for (int i = 0; i < MAX_REGISTROS; i++)
			{
				if (registroMundo->registroRecompensas[i].idItem == 'v')
				{
					registroMundo->registroRecompensas[i].seRecogioElItem = 1;
				}
			}			

			Sonido(controlAudio, 5);
		}
	}

	if (gestionObjetos->mapaNaranjo.activa != 0)
	{
		if (Colicion(personaje.posX, personaje.posY, TAMANHO, TAMANHO, gestionObjetos->mapaNaranjo.posX, gestionObjetos->mapaNaranjo.posY, TAMANHO, TAMANHO))
		{
			gestionObjetos->mapaNaranjo.activa = 0;
			gestionObjetos->mapaNaranjo.seObtuvo = 1;

			Sonido(controlAudio, 5);
		}
	}
	

	for (int i = 0; i < MAX_OBJETOS; i++)
	{
		//Colicion con monedas
		if (gestionObjetos->monedas[i].activa != 0)
		{
			if (Colicion(personaje.posX, personaje.posY, TAMANHO, TAMANHO, gestionObjetos->monedas[i].posX, gestionObjetos->monedas[i].posY, TAMANHO, TAMANHO))
			{
				if (gestionObjetos->monedas[i].especial != 0)
				{
					gestionObjetos->monedas[i].activa = 0;
					personaje.cantidadMonedas += 10;

					Sonido(controlAudio, 1);

					for (int j = 0; j < MAX_REGISTROS; j++)
					{
						if (registroMundo->registroRecompensas[j].idItem == 'e' && registroMundo->registroRecompensas[j].seRecogioElItem == 0 && registroMundo->registroRecompensas[j].posX == gestionObjetos->monedas[i].posX && registroMundo->registroRecompensas[j].posY == gestionObjetos->monedas[i].posY)
						{
							registroMundo->registroRecompensas[j].seRecogioElItem = 1;
						}
					}
				}
				else
				{
					gestionObjetos->monedas[i].activa = 0;
					personaje.cantidadMonedas ++;

					Sonido(controlAudio, 1);

					for (int j = 0; j < MAX_REGISTROS; j++)
					{
						if (registroMundo->registroRecompensas[j].idItem == 'm' && registroMundo->registroRecompensas[j].seRecogioElItem == 0 && registroMundo->registroRecompensas[j].posX == gestionObjetos->monedas[i].posX && registroMundo->registroRecompensas[j].posY == gestionObjetos->monedas[i].posY)
						{
							registroMundo->registroRecompensas[j].seRecogioElItem = 1;
						}
					}
				}
			}
		}

		//Colicion con llaves
		if (gestionObjetos->llaves[i].activa != 0)
		{
			if (Colicion(personaje.posX, personaje.posY, TAMANHO, TAMANHO, gestionObjetos->llaves[i].posX, gestionObjetos->llaves[i].posY, TAMANHO, TAMANHO))
			{
				gestionObjetos->llaves[i].activa = 0;
				personaje.cantidadLlaves ++;

				for (int j = 0; j < MAX_REGISTROS; j++)
				{
					if (registroMundo->registroRecompensas[j].idItem == 'l' && registroMundo->registroRecompensas[j].seRecogioElItem == 0)
					{
						registroMundo->registroRecompensas[j].seRecogioElItem = 1;
					}
				}

				Sonido(controlAudio, 6);
			}
		}

		//Colicion con municiones
		if (gestionObjetos->municiones[i].activa != 0)
		{
			if (Colicion(personaje.posX, personaje.posY, TAMANHO, TAMANHO, gestionObjetos->municiones[i].posX, gestionObjetos->municiones[i].posY, TAMANHO, TAMANHO))
			{
				//Ahora todas las balas se puden volver a disparar
				for (int k = 0; k < MAX_BALAS; k++)
				{
					personaje.bala[k].seDisparo = 0;
				}

				Sonido(controlAudio, 6);

				//Reinicia el contador visual
				personaje.balasrestantes = MAX_BALAS;
				
				//Desactiva el cargador
				gestionObjetos->municiones[i].activa = 0;

				//Se registra que el cargador recogido
				registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].mapaX = estadoMapa->actualMapaX;
				registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].mapaY = estadoMapa->actualMapaY;
				registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].fila = gestionObjetos->municiones[i].fila;
				registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].columna = gestionObjetos->municiones[i].columna;
				registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].seObtuvoUnCargador = 1;
				registroMundo->cantidadRecompensas;

				//registrar cuando se recoge cargador, caso cuando no aparece el cargador como caracter en el mapa				
				for (int j = 0; j < MAX_REGISTROS; j++)
				{
					if (registroMundo->registroRecompensas[j].idItem == 'M' && registroMundo->registroRecompensas[j].seRecogioElItem == 0 && registroMundo->registroRecompensas[j].posX == gestionObjetos->municiones[i].posX && registroMundo->registroRecompensas[j].posY == gestionObjetos->municiones[i].posY)
					{
						registroMundo->registroRecompensas[j].seRecogioElItem = 1;
					}
				}
			}
		}

		//Colicion con vidas
		if (gestionObjetos->vidasObjeto[i].activa != 0)
		{
			if (Colicion(personaje.posX, personaje.posY, TAMANHO, TAMANHO, gestionObjetos->vidasObjeto[i].posX, gestionObjetos->vidasObjeto[i].posY, TAMANHO, TAMANHO))
			{
				personaje.vidas++;
				gestionObjetos->vidasObjeto[i].activa = 0;
				Sonido(controlAudio, 6);

				for (int j = 0; j < MAX_REGISTROS; j++)
				{
					if (registroMundo->registroRecompensas[j].idItem == 'V' && registroMundo->registroRecompensas[j].seRecogioElItem == 0 && registroMundo->registroRecompensas[j].posX == gestionObjetos->vidasObjeto[i].posX && registroMundo->registroRecompensas[j].posY == gestionObjetos->vidasObjeto[i].posY)
					{
						registroMundo->registroRecompensas[j].seRecogioElItem = 1;
					}
				}
			}
		}

		//Colicion con el minimapa de la tienda
		if (gestionObjetos->minimapa.activa != 0 && gestionObjetos->minimapa.seVende == 1 && personaje.cantidadMonedas >= gestionObjetos->minimapa.precio)
		{
			if (Colicion(personaje.posX, personaje.posY, TAMANHO, TAMANHO, gestionObjetos->minimapa.posX, gestionObjetos->minimapa.posY, TAMANHO, TAMANHO))
			{
				estadoMapa->minimapaVendido = 1;
				Sonido(controlAudio, 5);

				gestionObjetos->minimapa.activa = 0;
				personaje.cantidadMonedas -= gestionObjetos->minimapa.precio;
			}
		}

		//Colicion con mejora de Danho
		if (gestionObjetos->mejoraDanho[i].activa != 0 && gestionObjetos->mejoraDanho[i].seVende == 0)
		{
			if (Colicion(personaje.posX, personaje.posY, TAMANHO, TAMANHO, gestionObjetos->mejoraDanho[i].posX, gestionObjetos->mejoraDanho[i].posY, TAMANHO, TAMANHO))
			{
				for (int k = 0; k < MAX_BALAS; k++)
				{
					personaje.bala[k].danho++;
				}

				Sonido(controlAudio, 5);

				gestionObjetos->mejoraDanho[i].activa = 0;

				for (int j = 0; j < MAX_REGISTROS; j++)
				{
					if (registroMundo->registroRecompensas[j].idItem == 'A' && registroMundo->registroRecompensas[j].seRecogioElItem == 0 && registroMundo->registroRecompensas[j].posX == gestionObjetos->mejoraDanho[i].posX && registroMundo->registroRecompensas[j].posY == gestionObjetos->mejoraDanho[i].posY)
					{
						registroMundo->registroRecompensas[j].seRecogioElItem = 1;
					}
				}
			}
		}
		else if (gestionObjetos->mejoraDanho[i].activa != 0 && gestionObjetos->mejoraDanho[i].seVende == 1 && personaje.cantidadMonedas >= gestionObjetos->mejoraDanho[i].precio)
		{
			if (Colicion(personaje.posX, personaje.posY, TAMANHO, TAMANHO, gestionObjetos->mejoraDanho[i].posX, gestionObjetos->mejoraDanho[i].posY, TAMANHO, TAMANHO))
			{
				for (int i = 0; i < MAX_BALAS; i++)
				{
					personaje.bala[i].danho++;
				}

				Sonido(controlAudio, 5);

				gestionObjetos->mejoraDanho[i].activa = 0;
				personaje.cantidadMonedas -= gestionObjetos->mejoraDanho[i].precio;
				
				//Comprobar ubicacion de la mejora
				registroMundo->registroTienda[gestionObjetos->mejoraDanho[i].idRegistro].comprado = 1;

				for (int k = 0; k < registroMundo->indiceTienda; k++)
				{
					if (registroMundo->registroTienda[k].columna == gestionObjetos->mejoraDanho[i].columna && registroMundo->registroTienda[k].fila == gestionObjetos->mejoraDanho[i].fila && registroMundo->registroTienda[k].mapaX == estadoMapa->actualMapaX && registroMundo->registroTienda[k].mapaY == estadoMapa->actualMapaY)
					{
						registroMundo->registroTienda[k].comprado = 1; 
						break; 
					}
				}
			}
		}

		//Colicion con mejora de rango
		if (gestionObjetos->mejoraRango[i].activa != 0 && gestionObjetos->mejoraRango[i].seVende == 0)
		{
			if (Colicion(personaje.posX, personaje.posY, TAMANHO, TAMANHO, gestionObjetos->mejoraRango[i].posX, gestionObjetos->mejoraRango[i].posY, TAMANHO, TAMANHO))
			{
				personaje.rangoDeBalas += 100;
				gestionObjetos->mejoraRango[i].activa = 0;

				Sonido(controlAudio, 5);

				for (int j = 0; j < MAX_REGISTROS; j++)
				{
					if (registroMundo->registroRecompensas[j].idItem == 'B' && registroMundo->registroRecompensas[j].seRecogioElItem == 0 && registroMundo->registroRecompensas[j].posX == gestionObjetos->mejoraRango[i].posX && registroMundo->registroRecompensas[j].posY == gestionObjetos->mejoraRango[i].posY)
					{
						registroMundo->registroRecompensas[j].seRecogioElItem = 1;
					}
				}
			}
		}
		else if (gestionObjetos->mejoraRango[i].activa != 0 && gestionObjetos->mejoraRango[i].seVende == 1 && personaje.cantidadMonedas >= gestionObjetos->mejoraRango[i].precio)
		{
			if (Colicion(personaje.posX, personaje.posY, TAMANHO, TAMANHO, gestionObjetos->mejoraRango[i].posX, gestionObjetos->mejoraRango[i].posY, TAMANHO, TAMANHO))
			{
				personaje.rangoDeBalas += 100;
				gestionObjetos->mejoraRango[i].activa = 0;
				personaje.cantidadMonedas -= gestionObjetos->mejoraRango[i].precio;

				Sonido(controlAudio, 5);
				
				//Comprobar ubicacion de la mejora
				registroMundo->registroTienda[gestionObjetos->mejoraRango[i].idRegistro].comprado = 1;

				for (int k = 0; k < registroMundo->indiceTienda; k++)
				{
					if (registroMundo->registroTienda[k].columna == gestionObjetos->mejoraRango[i].columna && registroMundo->registroTienda[k].fila == gestionObjetos->mejoraRango[i].fila && registroMundo->registroTienda[k].mapaX == estadoMapa->actualMapaX && registroMundo->registroTienda[k].mapaY == estadoMapa->actualMapaY)
					{
						registroMundo->registroTienda[k].comprado = 1; 
						break; 
					}
				}
			}
		}

		//Colicion con mejora de velocidad
		if (gestionObjetos->mejoraVelocidad[i].activa != 0 && gestionObjetos->mejoraVelocidad[i].seVende == 0)
		{
			if (Colicion(personaje.posX, personaje.posY, TAMANHO, TAMANHO, gestionObjetos->mejoraVelocidad[i].posX, gestionObjetos->mejoraVelocidad[i].posY, TAMANHO, TAMANHO))
			{
				personaje.velocidad += 2;
				gestionObjetos->mejoraVelocidad[i].activa = 0;
				Sonido(controlAudio, 5);

				for (int j = 0; j < MAX_REGISTROS; j++)
				{
					if (registroMundo->registroRecompensas[j].idItem == 'C' && registroMundo->registroRecompensas[j].seRecogioElItem == 0 && registroMundo->registroRecompensas[j].posX == gestionObjetos->mejoraVelocidad[i].posX && registroMundo->registroRecompensas[j].posY == gestionObjetos->mejoraVelocidad[i].posY)
					{
						registroMundo->registroRecompensas[j].seRecogioElItem = 1;
					}
				}
			}
		}
		else if (gestionObjetos->mejoraVelocidad[i].activa != 0 && gestionObjetos->mejoraVelocidad[i].seVende == 1 && personaje.cantidadMonedas >= gestionObjetos->mejoraVelocidad[i].precio)
		{
			if (Colicion(personaje.posX, personaje.posY, TAMANHO, TAMANHO, gestionObjetos->mejoraVelocidad[i].posX, gestionObjetos->mejoraVelocidad[i].posY, TAMANHO, TAMANHO))
			{
				personaje.velocidad += 2;
				gestionObjetos->mejoraVelocidad[i].activa = 0;
				personaje.cantidadMonedas -= gestionObjetos->mejoraVelocidad[i].precio;

				Sonido(controlAudio, 5);
				
				//Comprobar ubicacion de la mejora
				registroMundo->registroTienda[gestionObjetos->mejoraVelocidad[i].idRegistro].comprado = 1;

				for (int k = 0; k < registroMundo->indiceTienda; k++)
				{
					if (registroMundo->registroTienda[k].columna == gestionObjetos->mejoraVelocidad[i].columna && registroMundo->registroTienda[k].fila == gestionObjetos->mejoraVelocidad[i].fila && registroMundo->registroTienda[k].mapaX == estadoMapa->actualMapaX && registroMundo->registroTienda[k].mapaY == estadoMapa->actualMapaY)
					{
						registroMundo->registroTienda[k].comprado = 1; 
						break; 
					}
				}
			}
		}
		
		//Colicion con cofres
		if (gestionObjetos->cofres[i].activa != 0)
		{
			if (Colicion(personaje.posX, personaje.posY, TAMANHO, TAMANHO, gestionObjetos->cofres[i].posX, gestionObjetos->cofres[i].posY, TAMANHO, TAMANHO) && personaje.cantidadLlaves > 0 && gestionObjetos->cofres[i].cofreAbierto == 0)
			{
				gestionObjetos->cofres[i].cofreAbierto = 1;
				personaje.cantidadLlaves--;
				registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].mapaX = estadoMapa->actualMapaX;
				registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].mapaY = estadoMapa->actualMapaY;
				registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].seAbrioUnCofre = 1;
				registroMundo->cantidadRecompensas++;

				Sonido(controlAudio, 7);

				//Siempre dará de recompensa una recarga
				gestionObjetos->municiones[gestionObjetos->municionesActual].activa = 1;
				gestionObjetos->municiones[gestionObjetos->municionesActual].posX = gestionObjetos->cofres[i].posX + TAMANHO * 3;
				gestionObjetos->municiones[gestionObjetos->municionesActual].posY = gestionObjetos->cofres[i].posY;
				
				registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].mapaX = estadoMapa->actualMapaX;
				registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].mapaY = estadoMapa->actualMapaY;
				registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].posX = gestionObjetos->municiones[gestionObjetos->municionesActual].posX;
				registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].posY = gestionObjetos->municiones[gestionObjetos->municionesActual].posY;
				registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].idItem = 'M';
				registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].seRecogioElItem = 0;
				registroMundo->cantidadRecompensas ++;

				gestionObjetos->municionesActual++;

				//Siempre dará un corazon
				gestionObjetos->vidasObjeto[gestionObjetos->vidasObjetoActual].activa = 1;
				gestionObjetos->vidasObjeto[gestionObjetos->vidasObjetoActual].posX = gestionObjetos->cofres[i].posX - TAMANHO * 3;
				gestionObjetos->vidasObjeto[gestionObjetos->vidasObjetoActual].posY = gestionObjetos->cofres[i].posY;

				registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].mapaX = estadoMapa->actualMapaX;
				registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].mapaY = estadoMapa->actualMapaY;
				registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].posX = gestionObjetos->vidasObjeto[gestionObjetos->vidasObjetoActual].posX;
				registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].posY = gestionObjetos->vidasObjeto[gestionObjetos->vidasObjetoActual].posY;
				registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].idItem = 'V';
				registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].seRecogioElItem = 0;
				registroMundo->cantidadRecompensas ++;

				gestionObjetos->vidasObjetoActual++;

				//Siempre dará una moneda
				gestionObjetos->monedas[gestionObjetos->monedasActual].activa = 1;
				gestionObjetos->monedas[gestionObjetos->monedasActual].posX = gestionObjetos->cofres[i].posX;
				gestionObjetos->monedas[gestionObjetos->monedasActual].posY = gestionObjetos->cofres[i].posY + TAMANHO * 3;

				registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].mapaX = estadoMapa->actualMapaX;
				registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].mapaY = estadoMapa->actualMapaY;
				registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].posX = gestionObjetos->monedas[gestionObjetos->monedasActual].posX;
				registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].posY = gestionObjetos->monedas[gestionObjetos->monedasActual].posY;
				registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].idItem = 'm';
				registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].seRecogioElItem = 0;
				registroMundo->cantidadRecompensas ++;

				gestionObjetos->monedasActual++;

				//Pull de objetos que tienen probabilidad de salir
				auxRand = rand() % 3 + 1;

				//mejora de daño
				if (auxRand == 1)
				{
					for (int j = 0; j < MAX_OBJETOS; j++)
					{
						if (gestionObjetos->mejoraDanho[j].activa == 0)
						{
							gestionObjetos->mejoraDanho[j].activa = 1;
							gestionObjetos->mejoraDanho[j].posX = gestionObjetos->cofres[i].posX;
							gestionObjetos->mejoraDanho[j].posY = gestionObjetos->cofres[i].posY - TAMANHO * 3;

							registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].mapaX = estadoMapa->actualMapaX;
							registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].mapaY = estadoMapa->actualMapaY;
							registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].posX = gestionObjetos->mejoraDanho[j].posX;
							registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].posY = gestionObjetos->mejoraDanho[j].posY;
							registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].idItem = 'A';
							registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].seRecogioElItem = 0;
							registroMundo->cantidadRecompensas ++;
							break;
						}
					}
				}

				if (auxRand == 2)
				{
					for (int j = 0; j < MAX_OBJETOS; j++)
					{
						if (gestionObjetos->mejoraRango[j].activa == 0)
						{
							gestionObjetos->mejoraRango[j].activa = 1;
							gestionObjetos->mejoraRango[j].posX = gestionObjetos->cofres[i].posX;
							gestionObjetos->mejoraRango[j].posY = gestionObjetos->cofres[i].posY - TAMANHO * 3;

							registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].mapaX = estadoMapa->actualMapaX;
							registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].mapaY = estadoMapa->actualMapaY;
							registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].posX = gestionObjetos->mejoraRango[j].posX;
							registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].posY = gestionObjetos->mejoraRango[j].posY;
							registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].idItem = 'B';
							registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].seRecogioElItem = 0;
							registroMundo->cantidadRecompensas ++;
							break;
						}
					}
				}

				if (auxRand == 3)
				{
					for (int j = 0; j < MAX_OBJETOS; j++)
					{
						if (gestionObjetos->mejoraVelocidad[j].activa == 0)
						{
							gestionObjetos->mejoraVelocidad[j].activa = 1;
							gestionObjetos->mejoraVelocidad[j].posX = gestionObjetos->cofres[i].posX;
							gestionObjetos->mejoraVelocidad[j].posY = gestionObjetos->cofres[i].posY - TAMANHO * 3;

							registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].mapaX = estadoMapa->actualMapaX;
							registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].mapaY = estadoMapa->actualMapaY;
							registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].posX = gestionObjetos->mejoraVelocidad[j].posX;
							registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].posY = gestionObjetos->mejoraVelocidad[j].posY;
							registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].idItem = 'C';
							registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].seRecogioElItem = 0;
							registroMundo->cantidadRecompensas ++;
							break;
						}
					}
				}
			}
		}
	}
}

void ColicionInteractuables(estadoMapa_ *estadoMapa, gestionDianas_ *gestionDianas, gestionInteractuables_ *gestionInteractuables, registroMundo_ *registroMundo, controlAudio_ *controlAudio)
{
	for (int i = 0; i < MAX_INTERACTUABLES; i++)
	{
		if (gestionInteractuables->fogata[i].activa != 0  && gestionInteractuables->fogata[i].fogataActiva == 0)
		{
			if (Colicion(personaje.posX, personaje.posY, TAMANHO, TAMANHO, gestionInteractuables->fogata[i].posX, gestionInteractuables->fogata[i].posY, TAMANHO, TAMANHO))
			{
				gestionInteractuables->fogata[i].fogataActiva = 1;
				gestionInteractuables->cantidadfogatasActivas++;
				Sonido(controlAudio, 9);

				registroMundo->registroInteractuables[registroMundo->cantidadInteractuables].mapaX = estadoMapa->actualMapaX;
				registroMundo->registroInteractuables[registroMundo->cantidadInteractuables].mapaY = estadoMapa->actualMapaY;
				registroMundo->registroInteractuables[registroMundo->cantidadInteractuables].fila = gestionInteractuables->fogata[i].fila;
				registroMundo->registroInteractuables[registroMundo->cantidadInteractuables].columna = gestionInteractuables->fogata[i].columna;
				registroMundo->registroInteractuables[registroMundo->cantidadInteractuables].fogataEncendida = 1;
				registroMundo->cantidadInteractuables++;

				break;
			}
		}
	}

	for (int i = 0; i < MAX_DIANAS; i++)
	{
		if (gestionDianas->dianas[i].activa != 0)
		{
			for (int j = 0; j < MAX_BALAS; j++)
			{
				if (personaje.bala[j].activa != 0)
				{
					if (Colicion(personaje.bala[j].posX, personaje.bala[j].posY, TAMANHO/4, TAMANHO/4, gestionDianas->dianas[i].posX, gestionDianas->dianas[i].posY, TAMANHO, TAMANHO))
					{
						personaje.bala[j].activa = 0;
						gestionDianas->dianas[i].activa = 0;
						gestionDianas->dianas[i].destruida = 1;

						registroMundo->registroInteractuables[registroMundo->cantidadInteractuables].mapaX = estadoMapa->actualMapaX;
						registroMundo->registroInteractuables[registroMundo->cantidadInteractuables].mapaY = estadoMapa->actualMapaY;
						registroMundo->registroInteractuables[registroMundo->cantidadInteractuables].fila = gestionDianas->dianas[i].fila;
						registroMundo->registroInteractuables[registroMundo->cantidadInteractuables].columna = gestionDianas->dianas[i].columna;
						registroMundo->registroInteractuables[registroMundo->cantidadInteractuables].dianaDestruida = 1;
						registroMundo->cantidadInteractuables++;
					}
				}
			}
		}
	}
}

void LogicaDianas(estadoMapa_ *estadoMapa, gestionDianas_ *gestionDianas, controlAudio_ *controlAudio)
{
	int auxXDiana = 0;
	int auxYDiana = 0;
	int auxRand = 0;

	for (int i = 0; i < MAX_DIANAS; i++)
	{
		if (gestionDianas->dianas[i].activa != 0)
		{
			auxXDiana = gestionDianas->dianas[i].posX;
			auxYDiana = gestionDianas->dianas[i].posY;

			if (gestionDianas->dianas[i].chocoConPared % 2 == 0)
			{
				gestionDianas->dianas[i].posX += gestionDianas->dianas[i].velocidad;
			} 
			else if (gestionDianas->dianas[i].chocoConPared % 2 != 0)
			{
				gestionDianas->dianas[i].posX -= gestionDianas->dianas[i].velocidad;
			}
		}

		if (ColisionMapa(estadoMapa->sala, gestionDianas->dianas[i].posX, gestionDianas->dianas[i].posY) || 
			ColisionMapa(estadoMapa->sala, gestionDianas->dianas[i].posX + TAMANHO - 1, gestionDianas->dianas[i].posY) || 
			ColisionMapa(estadoMapa->sala, gestionDianas->dianas[i].posX + TAMANHO - 1, gestionDianas->dianas[i].posY + TAMANHO - 1) ||
			ColisionMapa(estadoMapa->sala, gestionDianas->dianas[i].posX, gestionDianas->dianas[i].posY + TAMANHO - 1))
		{
			gestionDianas->dianas[i].posX = auxXDiana;
			gestionDianas->dianas[i].chocoConPared++;
		}
	}
}

void GeneracionDeRecompensas(gestionObjetos_ *gestionObjetos, estadoMapa_ *estadoMapa, registroMundo_ *registroMundo)
{
	if (estadoMapa->salaVacia == 1 && estadoMapa->seGeneroUnaRecompensa == 0 && !(estadoMapa->actualMapaX == COLUMNAS_MAPA / 2 + 1 && estadoMapa->actualMapaY == FILAS_MAPA / 2 + 1))
	{
		estadoMapa->seGeneroUnaRecompensa = 1;

		registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].mapaX = estadoMapa->actualMapaX;
		registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].mapaY = estadoMapa->actualMapaY;
		registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].posX = gestionObjetos->llaves[gestionObjetos->llavesActual].posX;
		registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].posY = gestionObjetos->llaves[gestionObjetos->llavesActual].posY;
		registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].idItem = 'l';
		registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].seRecogioElItem = 0;
		registroMundo->registroRecompensas[registroMundo->cantidadRecompensas].seObtuvoUnaRecompensa = 1;
		registroMundo->cantidadRecompensas ++;
		
		//Se genera una llave
		gestionObjetos->llaves[gestionObjetos->llavesActual].activa = 1;
		gestionObjetos->llavesActual ++;
	}
}

void VerificarSalaVacia(gestionEnemigos_ *gestionEnemigos, estadoMapa_ *estadoMapa)
{
	int enemigosVivos = 0;

	for (int i = 0; i < MAX_ENEMIGOS; i++)
	{
		if (gestionEnemigos->slime[i].activa != 0 || gestionEnemigos->mago[i].activa != 0 || gestionEnemigos->aranha[i].activa != 0)
		{
			enemigosVivos = 1;
		}
	}

	if (gestionEnemigos->jefe.activa != 0)
	{
		enemigosVivos = 1;
	}	

	if (enemigosVivos == 1)
	{
		estadoMapa->salaVacia = 0;
	}
	else 
	{
		estadoMapa->salaVacia = 1;
	}

	//printf("estadoMapa->sala vacia = %d", estadoMapa->salaVacia);
}

void CambioDeHabitaciones(controlIndices_ *controlIndices, gestionEnemigos_ *gestionEnemigos, gestionObjetos_ *gestionObjetos, estadoMapa_ *estadoMapa, gestionDianas_ *gestionDianas, gestionInteractuables_ *gestionInteractuables, registroMundo_ *registroMundo)
{
	FILE *archivoHabitacion = NULL;
	
	if (personaje.traspasoPuerta == 4)
	{
		if (estadoMapa->mapa[estadoMapa->actualMapaY][estadoMapa->actualMapaX - 1] != NULL)
		{
			estadoMapa->actualMapaX --;
			cargarMapa(estadoMapa->mapa[estadoMapa->actualMapaY][estadoMapa->actualMapaX], archivoHabitacion, estadoMapa->sala, &personaje, gestionEnemigos, 'E', estadoMapa->actualMapaX, estadoMapa->actualMapaY, controlIndices, gestionObjetos, estadoMapa, gestionDianas, gestionInteractuables, registroMundo);
		}

		personaje.traspasoPuerta = 0;
	}
	else if (estadoMapa->mapa[estadoMapa->actualMapaY][estadoMapa->actualMapaX - 1] == NULL || estadoMapa->mapa[estadoMapa->actualMapaY][estadoMapa->actualMapaX - 1] == "hab_trial.txt" || estadoMapa->mapa[estadoMapa->actualMapaY][estadoMapa->actualMapaX - 1] == "hab_punteria.txt") 
	{
		//Las puertas que esten conectadas a una parte nula o habitacion unica del mapa se reemplazan por #
		for (int i = 0; i < FILAS_HABITACION; i++)
		{
			for (int j = 0; j < COLUMNAS_HABITACION; j++)
			{
				if (estadoMapa->sala[i][j] == 'O')
				{
					estadoMapa->sala[i][j] = '#';
				}
			}
		}
	}

	if (personaje.traspasoPuerta == 2)
	{
		if (estadoMapa->mapa[estadoMapa->actualMapaY][estadoMapa->actualMapaX + 1] != NULL)
		{
			estadoMapa->actualMapaX ++;
			cargarMapa(estadoMapa->mapa[estadoMapa->actualMapaY][estadoMapa->actualMapaX], archivoHabitacion, estadoMapa->sala, &personaje, gestionEnemigos, 'O', estadoMapa->actualMapaX, estadoMapa->actualMapaY, controlIndices, gestionObjetos, estadoMapa, gestionDianas, gestionInteractuables, registroMundo);
		}
		
		personaje.traspasoPuerta = 0;
	}
	else if (estadoMapa->mapa[estadoMapa->actualMapaY][estadoMapa->actualMapaX + 1] == NULL || estadoMapa->mapa[estadoMapa->actualMapaY][estadoMapa->actualMapaX + 1] == "hab_trial.txt" || estadoMapa->mapa[estadoMapa->actualMapaY][estadoMapa->actualMapaX + 1] == "hab_punteria.txt")
	{
		for (int i = 0; i < FILAS_HABITACION; i++)
		{
			for (int j = 0; j < COLUMNAS_HABITACION; j++)
			{
				if (estadoMapa->sala[i][j] == 'E')
				{
					estadoMapa->sala[i][j] = '#';
				}
			}
		}
	}

	if (personaje.traspasoPuerta == 3)
	{
		if (estadoMapa->mapa[estadoMapa->actualMapaY + 1][estadoMapa->actualMapaX] != NULL)
		{
			estadoMapa->actualMapaY ++;
			cargarMapa(estadoMapa->mapa[estadoMapa->actualMapaY][estadoMapa->actualMapaX], archivoHabitacion, estadoMapa->sala, &personaje, gestionEnemigos, 'N', estadoMapa->actualMapaX, estadoMapa->actualMapaY, controlIndices, gestionObjetos, estadoMapa, gestionDianas, gestionInteractuables, registroMundo);
		}
		
		personaje.traspasoPuerta = 0;
	}
	else if (estadoMapa->mapa[estadoMapa->actualMapaY + 1][estadoMapa->actualMapaX] == NULL || estadoMapa->mapa[estadoMapa->actualMapaY + 1][estadoMapa->actualMapaX] == "hab_trial.txt")
	{
		for (int i = 0; i < FILAS_HABITACION; i++)
		{
			for (int j = 0; j < COLUMNAS_HABITACION; j++)
			{
				if (estadoMapa->sala[i][j] == 'S')
				{
					estadoMapa->sala[i][j] = '#';
				}
			}
		}
	}
	
	if (personaje.traspasoPuerta == 1)
	{
		if (estadoMapa->mapa[estadoMapa->actualMapaY - 1][estadoMapa->actualMapaX] != NULL)
		{
			estadoMapa->actualMapaY --;
			cargarMapa(estadoMapa->mapa[estadoMapa->actualMapaY][estadoMapa->actualMapaX], archivoHabitacion, estadoMapa->sala, &personaje, gestionEnemigos, 'S', estadoMapa->actualMapaX, estadoMapa->actualMapaY, controlIndices, gestionObjetos, estadoMapa, gestionDianas, gestionInteractuables, registroMundo);
		}
		
		personaje.traspasoPuerta = 0;
	}
	else if (estadoMapa->mapa[estadoMapa->actualMapaY - 1][estadoMapa->actualMapaX] == NULL || estadoMapa->mapa[estadoMapa->actualMapaY - 1][estadoMapa->actualMapaX] == "hab_punteria.txt")
	{
		for (int i = 0; i < FILAS_HABITACION; i++)
		{
			for (int j = 0; j < COLUMNAS_HABITACION; j++)
			{
				if (estadoMapa->sala[i][j] == 'N')
				{
					estadoMapa->sala[i][j] = '#';
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

void GeneracionDelMapa(int cantidadHabitacionesDeseadas, estadoMapa_ *estadoMapa)
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
	//mapa es un arreglo de punteros, "habBase.txt" es automaticamente una direccion de puntero
	estadoMapa->mapa[FILAS_MAPA / 2 + 1][COLUMNAS_MAPA / 2 + 1] = "habBase.txt";
	
	while (a < cantidadHabitacionesDeseadas)
	{
		auxRand = rand() % 10 + 1;

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
		else if (auxRand == 10)
		{
			pullHabitaciones = "hab_general_5.txt";  
		}

		//////////////////////////////////////////////////////////////////////////// Garantia de salas
		//Garantia de estadoMapa->sala
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
		
		//La penultima habitacion generada siempre sera una estadoMapa->sala de recompensa si no se genero antes
		else if (a >= cantidadHabitacionesDeseadas - 2 && seGeneroUnaHabitacionRecompensa == 0)
		{
			pullHabitaciones = "hab_recompensa.txt";
		}
		
		//Cuando este por generar la ultima habitacion, obliga a que sea la del jefe
		else if (a == cantidadHabitacionesDeseadas - 1)
		{
			pullHabitaciones = "hab_jefe_1.txt";
		}

		//Buscar direccion para ponerse la estadoMapa->sala
		if (strcmp(pullHabitaciones, "hab_trial.txt") == 0)
		{
			habitacionCardinal = 1;
		}
		else if (strcmp(pullHabitaciones, "hab_punteria.txt") == 0)
		{
			habitacionCardinal = 3;
		}
		else 
		{
			habitacionCardinal = rand() % 4 + 1; // entre 1 y 4... 1: Norte, 2: Este, 3: Sur, 4: Oeste 
		}
		
		if (habitacionCardinal == 1)
		{
			if (estadoMapa->mapa[filaActual - 1][columnaActual] == NULL)
			{
				filaActual --;
				estadoMapa->mapa[filaActual][columnaActual] = pullHabitaciones;

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
			if (estadoMapa->mapa[filaActual + 1][columnaActual] == NULL)
			{
				filaActual ++;
				estadoMapa->mapa[filaActual][columnaActual] = pullHabitaciones;

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
			if (estadoMapa->mapa[filaActual][columnaActual + 1] == NULL)
			{
				columnaActual ++;
				estadoMapa->mapa[filaActual][columnaActual] = pullHabitaciones;

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
			if (estadoMapa->mapa[filaActual][columnaActual - 1] == NULL)
			{
				columnaActual --;
				estadoMapa->mapa[filaActual][columnaActual] = pullHabitaciones;

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