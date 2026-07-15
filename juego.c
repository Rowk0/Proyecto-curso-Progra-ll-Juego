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
#define FILAS_MAPA 7
#define COLUMNAS_MAPA 7
#define TAMANHO 64
#define LARGO_TEXTO 30
#define LARGO_SPRITES 30
#define LARGO_PANTALLA 1920
#define ANCHO_PANTALLA 1088
#define MAX_BALAS 21
#define MAX_ENEMIGOS 20
#define MAX_OBJETOS 100

//Ideas deshechadas: 
//Movimiento de camara: implica crear otra camara estatica para cosas que no quiero que se muevan
//Ver todas las habitaciones mientras te mueves: eso implica hacer más condicionales en enemigos, reformular cargar mapa
//Hacer un indicador antes de la sala del jefe: está de más
//Cerrar las puertas al entrar a una habitacion: Con el sistema de balasdisponibles, si no tuvieras balas en una habitacion cerrada pierdes instantaneamente

////////////////////////////////////////////////////////////////  tareas

//propuestas:
//Contador balas descendentes
//sistema de balas: tener el arreglo de balas, si dispara buscar espacios disponibles en el arreglo, si hay espacio disponible, activa bala

//Interrogatorio:
//1 elementos estaticos más
//botiquin?
//vida
//trampas?
//cofres

//1 elemento dinamico más
//enemigo con baldi vibes

//Mis ideas:
//añadir tipos de habitaciones (Tienda que provee power-ups, sala de recompensas con power-ups)
//Habitacion de punteroa
//habitacion de puzles

//Final:
//recoger orbes y ponerlos en mapgeneral para terminar el nivel

/////////////////////////////////////////////////////////////////  flujo trabajo

//gcc juego.c -o juego -lallegro -lallegro_main -lallegro_primitives -lallegro_image -lm -lallegro_ttf -lallegro_font
//para compilar

//./juego 
//para correr programa

//Si no funciona la pantalla, poner "wsl --shutdown" en powershell y luego "code ."

//flujo git (antes ctrl + s para guardar archivo local)
//git add . -> git commit -m "cambios" -> git push.

////////////////////////////////////////////////////////////////// Estructuras

struct dirBala_
{
	int derecha;
	int izquierda;
	int abajo;
	int arriba;
};

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
	struct dirBala_ dirBala;
	float anguloBalaX;
	float anguloBalaY;
	float direccionBalaEnemigo;
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
	//Rango de balas
	//Ver si poner los power ups en un arreglo
	bala_ bala[MAX_BALAS]; //Se puede agregar una variable cantidad de balas para limitar la municion...
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
} enemigo;
 
enemigo slime[MAX_ENEMIGOS];

enemigo Jefe;

enemigo mago[MAX_ENEMIGOS];

int slimeActual = 0;

int magoActual = 0;

typedef struct 
{
	char registroMapa[LARGO_TEXTO];
	int fila;
	int columna;
	int mapaX;
	int mapaY;
} registroMuertes_;

registroMuertes_ registroMuertes[1000];

int cantidadMuertos = 0;

typedef struct 
{
	int mapaX;
	int mapaY;
	int seObtuvoUnaRecompensa;
} registroRecompensas_;

registroRecompensas_ registroRecompensas[1000];

int cantidadRecompensas = 0;

typedef struct 
{
	int posX;
	int posY;
	int activa;
	int especial;
} objeto;

objeto monedas[MAX_OBJETOS];

objeto llaves[MAX_OBJETOS];

//Meter una cartucho de recarga de tipo objeto

int monedasActual = 0;
int llavesActual = 0;

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

//Cuando JUEGO = 0, el while termina y se cierra el programa
int JUEGO = 0;
int MENU = 1;

//Control de sprites del personaje
int controlSprites = 0;

//Control de movimiento del jefe
int timerMovimientoJefe = 0;

//Variable que permite saber si la sala actual esta vacia
int salaVacia = 0;

int seGeneroUnaRecompensa = 0;

/////////////////////////////////////////////////////////////////  Funciones

void DibujarMapa(char mapa[FILAS_HABITACION][COLUMNAS_HABITACION], ALLEGRO_BITMAP *spriteSheet);
char cargarMapa(char nombreMapa[LARGO_TEXTO], FILE *varMapa, char mapa[FILAS_HABITACION][COLUMNAS_HABITACION], jugador *personaje, enemigo enemigo[MAX_ENEMIGOS], char puertaDestino, int mapaX, int mapaY);
bool ColisionMapa(char mapa[FILAS_HABITACION][COLUMNAS_HABITACION], int jugadorPosXProximo, int jugadorPosYProximo);
void Logica(char mapa[FILAS_HABITACION][COLUMNAS_HABITACION], jugador *jugador);
void MovimientoJugador();
void InitAllegro();
int InitGameComponents(ALLEGRO_DISPLAY *ventana, mouse_ *mouse);
void InputHandle();
void Render(char mapa[FILAS_HABITACION][COLUMNAS_HABITACION], ALLEGRO_BITMAP *spriteSheet, ALLEGRO_BITMAP *spriteSheetBalas, ALLEGRO_BITMAP *spriteSheetCaminarCaballero, ALLEGRO_BITMAP *spriteSheetIcons, ALLEGRO_FONT *fuenteJuego, ALLEGRO_BITMAP *spriteSheetCrosshair, ALLEGRO_BITMAP *spriteSheetBalasEnemigos);
void Disparo();
void MovimientoCamara();
void AnimacionPersonaje(ALLEGRO_BITMAP *spriteSheet, ALLEGRO_BITMAP *spriteSheetCaminarCaballero);
void AnimacionEnemigos(ALLEGRO_BITMAP *spriteSheet);
void LogicaEnemigos();
void ColisionEnemigos();
void CambioDeHabitaciones();
void RenderMenu(ALLEGRO_FONT *fuenteJuego);
void PersonajeInvulnerable();
void VerificarTraspasoPuertas(char mapa[FILAS_HABITACION][COLUMNAS_HABITACION], jugador *personaje);
void GeneraciónDelMapa(int cantidadHabitacionesDeseadas);
void DisparoEnemigos();
void LogicaJefe();
void HandicapsMejorables();
void LogicaMenu();
void ColicionObjetos();
void RangoVisionEnemigo();
void VerificarSalaVacia();
void GeneracionDeRecompensas();

//Primeros cuatro parametros representan un cuadrado (x1,y1) esquina superior izquierda (w1,h1) sus tamaños, generalmente la cantidad de pixeles, en este caso 64
//Ultimo cuatro representa otro cuadrado con otros parametros
//Compara si hay entre colicion entre ambos, y si hay devuelve true
bool Colicion(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);

//////////////////////////////////////////////////////////////////////////////////////////// main

int main(int argc, char **argv)
{ 
	/////////////////////////////////////////////////////////////// Declaraciones de una vez
	FILE *archivoMapas = NULL;
	char nombreHabitacion[LARGO_TEXTO] = "habBase.txt";

	//Crear ventana
    ALLEGRO_DISPLAY *ventana;

	//Sprites
	ALLEGRO_BITMAP *spriteSheet;
	ALLEGRO_BITMAP *spriteSheetBalas;
	ALLEGRO_BITMAP *spriteSheetCaminarCaballero;
	ALLEGRO_BITMAP *spriteSheetIcons;
	ALLEGRO_BITMAP *spriteSheetCrosshair;
	ALLEGRO_BITMAP *spriteSheetBalasEnemigos;

	//Fonts
	ALLEGRO_FONT *fuenteJuego;
	
	///////////////////////////////////////////////////////////////

	InitAllegro();
	
	InitGameComponents(ventana, &mouse);

	srand(time(NULL));

	GeneraciónDelMapa(10);

	fuenteJuego = al_load_ttf_font("PressStart2P-Regular.ttf", 32, 0);

	while (MENU)
	{
		InputHandle();

		RenderMenu(fuenteJuego);

		LogicaMenu();

		al_rest(0.016);
	}

	cargarMapa(nombreHabitacion, archivoMapas, sala, &personaje, slime, '@', actualMapaX, actualMapaY);

	spriteSheet = al_load_bitmap("64x64.png");

	spriteSheetBalas = al_load_bitmap("sp_guns.png");

	spriteSheetCaminarCaballero = al_load_bitmap("64x64_caminar.png");

	spriteSheetIcons = al_load_bitmap("64x64_icons.png");

	spriteSheetCrosshair = al_load_bitmap ("crosshair.png");

	spriteSheetBalasEnemigos = al_load_bitmap("sp_gunsEnemigo.png");

	while (JUEGO)
	{
		//Funcion que actualiza el estado del teclado y mouse
		InputHandle();

		//Logica del juego. Ej: movimientos del jugador
		Logica(sala, &personaje);

		//Dibujar aqui
		Render(sala, spriteSheet, spriteSheetBalas, spriteSheetCaminarCaballero, spriteSheetIcons, fuenteJuego, spriteSheetCrosshair, spriteSheetBalasEnemigos);

		//Hacer descansar el cpu
		al_rest(0.016); 
	}

	return 0;
}

void LogicaMenu()
{
	if (Colicion(mouse.posX, mouse.posY, mouse.tamanho, mouse.tamanho, LARGO_PANTALLA / 2 - 120, ANCHO_PANTALLA / 2 + 180, TAMANHO + 140, 20 + TAMANHO))
	{
		if(al_mouse_button_down(&estadoMouse, ALLEGRO_MOUSE_BUTTON_LEFT))
		{
			MENU = 0;
			JUEGO = 1;
		}
	}
}

void RenderMenu(ALLEGRO_FONT *fuenteJuego)
{
	al_clear_to_color(al_map_rgb(0, 0, 0));
	//////////////////////////////////////////////// Dibujar en este espacio

	al_draw_filled_rectangle(LARGO_PANTALLA / 2 - 120, ANCHO_PANTALLA / 2 + 180, LARGO_PANTALLA / 2 + TAMANHO + 20, ANCHO_PANTALLA / 2 + 200 + TAMANHO, al_map_rgb(255, 255, 255));

	al_draw_text(fuenteJuego, al_map_rgb(0, 255, 255), LARGO_PANTALLA / 2 - 100, ANCHO_PANTALLA / 2 + 200, 0, "Jugar");

	////////////////////////////////////////////////
	al_flip_display();
}

void Logica(char mapa[FILAS_HABITACION][COLUMNAS_HABITACION], jugador *jugador)
{
	MovimientoJugador();

	PersonajeInvulnerable();

	Disparo();

	HandicapsMejorables();

	//MovimientoCamara();

	LogicaEnemigos();

	VerificarTraspasoPuertas(mapa, jugador);

	ColicionObjetos();

	CambioDeHabitaciones();

	VerificarSalaVacia();

	GeneracionDeRecompensas();
	
	//Axis del mouse
	al_get_mouse_num_axes();

	//Cambiar por una pantalla de PERDISTE o reactivar el MENU
	if (personaje.vidas == 0)
	{
		JUEGO = 0;
	}
}

void Disparo()
{
	//Como Disparo() se encuentra en while, cada llamada se va acumulando en cadencia, lo usaremos como una especie de timer
	cadencia++;

	if(al_mouse_button_down(&estadoMouse, ALLEGRO_MOUSE_BUTTON_LEFT) && cadencia > 20 && balaActual != 20)
	{
		//comprobar que queden balas para disparar
		//hacer el recorrido del arreglo de balas, determinar cual está inactivo, para ver si usarlo
		
		personaje.bala[balaActual].posX = personaje.posX + (TAMANHO/2);
		personaje.bala[balaActual].posY = personaje.posY + (TAMANHO/2);
		personaje.bala[balaActual].activa = 1;
		direccionBala = atan2(mouse.posY - personaje.posY, mouse.posX - personaje.posX); //atan2(y2 - y1, x2 - x1)
		personaje.bala[balaActual].anguloBalaX = cos(direccionBala) * personaje.bala[balaActual].velocidad;
		personaje.bala[balaActual].anguloBalaY = sin(direccionBala) * personaje.bala[balaActual].velocidad;
		balaActual++;
		cadencia = 0;
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

	//Cuando el arreglo este a punto de terminar, se reinicia
	/*if (balaActual > MAX_BALAS - 1)
	{
		balaActual = 0;
	}*/
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
			if (mago[i].bala[j].posX > mago[i].posX + 1000 || mago[i].bala[j].posX < mago[i].posX - 1000 || mago[i].bala[j].posY > mago[i].posY + 1000 ||mago[i].bala[j].posY < mago[i].posY - 1000)
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
	int rangoBala = 350;

	for (int i = 0; i < MAX_BALAS; i++)
	{
		if (personaje.bala[i].posX > personaje.posX + TAMANHO/2 + rangoBala || personaje.bala[i].posX < personaje.posX + TAMANHO/2 - rangoBala || personaje.bala[i].posY < personaje.posY + TAMANHO/2 - rangoBala || personaje.bala[i].posY > personaje.posY + TAMANHO/2 + rangoBala)
		{
			personaje.bala[i].activa = 0;
		}	
	}
}

char cargarMapa(char nombreMapa[LARGO_TEXTO], FILE *archivoMapa, char mapa[FILAS_HABITACION][COLUMNAS_HABITACION], jugador *jugador, enemigo enemigo[MAX_ENEMIGOS], char puertaDestino, int mapaX, int mapaY)
{
	int muerto = 0;
	slimeActual = 0;
	salaVacia = 0;
	seGeneroUnaRecompensa = 0;

	if ((archivoMapa = fopen(nombreMapa,"r")) == NULL)
	{
		return 0;
	} 

	//Reinicio de enemigos
	for (int k = 0; k < MAX_ENEMIGOS; k++)
	{
		slime[k].activa = 0;
		slime[k].ataquesEnemigos = 0;
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
	}
	
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
				
				//Si el slime no esta muerto se genera
				if (muerto == 0)
				{
					if(Jefe.activa == 0)
					{
						//Generar slime en la posicion 's' del mapa
						Jefe.activa = 1;
						Jefe.posX = j * TAMANHO;
						Jefe.posY = i * TAMANHO;
						Jefe.vida = 20;

						//Registrar la ubicacion original del slime
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
    	}
	}
}

void DibujarMapa(char mapa[FILAS_HABITACION][COLUMNAS_HABITACION], ALLEGRO_BITMAP *spriteSheet)
{
	for (int i = 0; i < FILAS_HABITACION; i++)
	{
		for (int j = 0; j < COLUMNAS_HABITACION; j++)
		{
			if (mapa[i][j] == '#')
			{
				//al_draw_bitmap_region(spriteSheet, 1 * TAMANHO, 16 * TAMANHO, TAMANHO, TAMANHO, j * TAMANHO, i * TAMANHO, 0);

				al_draw_bitmap_region(spriteSheet, 5 * TAMANHO, 15 * TAMANHO, TAMANHO, TAMANHO, j * TAMANHO, i * TAMANHO, 0);
			}

			/*if (mapa[i][j] == '.')
			{
				al_draw_bitmap_region(spriteSheet, 2 * TAMANHO, 15 * TAMANHO, TAMANHO, TAMANHO, j * TAMANHO, i * TAMANHO, 0);
			}*/

			/*if (mapa[i][j] == '@')
			{
				al_draw_bitmap_region(spriteSheet, 2 * TAMANHO, 15 * TAMANHO, TAMANHO, TAMANHO, j * TAMANHO, i * TAMANHO, 0);
			}*/

			/*if (mapa[i][j] == 's')
			{
				al_draw_bitmap_region(spriteSheet, 2 * TAMANHO, 15 * TAMANHO, TAMANHO, TAMANHO, j * TAMANHO, i * TAMANHO, 0);
			}*/

			/*if (mapa[i][j] == 'a')
			{
				al_draw_bitmap_region(spriteSheet, 5 * TAMANHO, 15 * TAMANHO, TAMANHO, TAMANHO, j * TAMANHO, i * TAMANHO, 0);
			}*/

			if (mapa[i][j] == 'p')
			{
				al_draw_bitmap_region(spriteSheet, 4 * TAMANHO, 15 * TAMANHO, TAMANHO, TAMANHO, j * TAMANHO, i * TAMANHO, 0);
			}

		}
		printf("\n");
	}
}

void Render(char mapa[FILAS_HABITACION][COLUMNAS_HABITACION], ALLEGRO_BITMAP *spriteSheet, ALLEGRO_BITMAP *spriteSheetBalas, ALLEGRO_BITMAP *spriteSheetCaminarCaballero, ALLEGRO_BITMAP *spriteSheetIcons, ALLEGRO_FONT *fuenteJuego, ALLEGRO_BITMAP *spriteSheetCrosshair, ALLEGRO_BITMAP *spriteSheetBalasEnemigos)
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
	al_draw_textf(fuenteJuego, al_map_rgb(192, 192, 192), LARGO_PANTALLA - 200, ANCHO_PANTALLA - 120, 0, "%d/%d", balaActual, MAX_BALAS - 1);

	//Puntaje Jugador
	al_draw_textf(fuenteJuego, al_map_rgb(192, 192, 192), TAMANHO * 23, TAMANHO, 0, "Puntaje: %d", personaje.puntaje);

	//Vidas jugador
	for (int j = 0; j < personaje.vidas; j++)
	{
		al_draw_bitmap_region(spriteSheetIcons, 4 * TAMANHO, 134 * TAMANHO, TAMANHO, TAMANHO, j * TAMANHO + TAMANHO, TAMANHO / 2, 0);
	}
	
	//Dibujo de balas
	for (int i = 0; i < MAX_BALAS; i++)
	{
		if (personaje.bala[i].activa != 0)
		{
			//Bala cuadrada
			//al_draw_filled_rectangle(personaje.bala[i].posX, personaje.bala[i].posY, personaje.bala[i].posX + (TAMANHO / 4), personaje.bala[i].posY + (TAMANHO / 4), al_map_rgb(0, 255, 255));

			//Bala sprite 
			//al_draw_bitmap_region(spriteSheetBalas, 2 * 16, 0 * 16, 16, 16, personaje.bala[i].posX, personaje.bala[i].posY, 0);

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
	}
	
	//Dibujo enemigos
	AnimacionEnemigos(spriteSheet);
	
	//Puntero del mouse
	//al_draw_filled_rectangle(mouse.posX - (mouse.tamanho/ 2), mouse.posY - (mouse.tamanho / 2), mouse.posX + (mouse.tamanho / 2), mouse.posY + (mouse.tamanho / 2), al_map_rgb(0, 255, 255));

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

int InitGameComponents(ALLEGRO_DISPLAY *ventana, mouse_ *mouse)
{
	//Inicializar ventana
	al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
	ventana = al_create_display(640, 480);
	if(!ventana) return -1;

	//Inicializando Mouse
	mouse->posX = 0;
	mouse->posY = 0;
	mouse->tamanho = 7;

	//Inicializando jugador
	personaje.velocidad = 7;
	personaje.movimientoJugador = 0;
	personaje.dirJugador.derecha = 0;
	personaje.dirJugador.izquierda = 1;
	personaje.vidas = 3;
	personaje.invulnerable = 0;
	personaje.cantidadMonedas = 0;
	personaje.cantidadLlaves = 0;
	//variable total balas disponibles

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

	//Inicializando balas
	for (int i = 0; i < MAX_BALAS; i++)
	{
		personaje.bala[i].posX = 0;
		personaje.bala[i].posY = 0;
		personaje.bala[i].velocidad = 10;
		personaje.bala[i].activa = 0;
		personaje.bala[i].dirBala.abajo = 0;
		personaje.bala[i].dirBala.arriba = 0;
		personaje.bala[i].dirBala.derecha = 0;
		personaje.bala[i].dirBala.izquierda = 0;
		personaje.bala[i].danho = 10;               ///// originalmente 1
		personaje.bala[i].anguloBalaX = 0;
		personaje.bala[i].anguloBalaY = 0;

		Jefe.bala[i].posX = 0;
		Jefe.bala[i].posY = 0;
		Jefe.bala[i].velocidad = 10;
		Jefe.bala[i].activa = 0;
		Jefe.bala[i].dirBala.abajo = 0;
		Jefe.bala[i].dirBala.arriba = 0;
		Jefe.bala[i].dirBala.derecha = 0;
		Jefe.bala[i].dirBala.izquierda = 0;
		Jefe.bala[i].danho = 1;
		Jefe.bala[i].anguloBalaX = 0;
		Jefe.bala[i].anguloBalaY = 0;

		for (int m = 0; m < MAX_ENEMIGOS; m++)
		{
			mago[m].bala[i].posX = 0;
			mago[m].bala[i].posY = 0;
			mago[m].bala[i].velocidad = 10;
			mago[m].bala[i].activa = 0;
			mago[m].bala[i].dirBala.abajo = 0;
			mago[m].bala[i].dirBala.arriba = 0;
			mago[m].bala[i].dirBala.derecha = 0;
			mago[m].bala[i].dirBala.izquierda = 0;
			mago[m].bala[i].danho = 1;
			mago[m].bala[i].anguloBalaX = 0;
			mago[m].bala[i].anguloBalaY = 0;
		}

		for (int i = 0; i < MAX_OBJETOS; i++)
		{
			monedas[i].posX = 0;
			monedas[i].posY = 0;
			monedas[i].activa = 0;
			monedas[i].especial = 0;
		}
	}

	for (int i = 0; i < FILAS_MAPA; i++)
	{
		for (int j = 0; i < COLUMNAS_MAPA; i++)
		{
			mapa[i][j] = NULL;
		}
	}
}

void InputHandle()
{
	al_get_keyboard_state(&estado); //Llena la estructura con el estado actual del taclado
	al_get_mouse_state(&estadoMouse);

	mouse.posX = estadoMouse.x;
	mouse.posY = estadoMouse.y;

	//Apagar programa con ESC
	if (al_key_down(&estado, ALLEGRO_KEY_ESCAPE))
	{
		JUEGO = 0;
		MENU = 0;
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

	ColisionEnemigos();

	DisparoEnemigos();

	LogicaJefe();

	RangoVisionEnemigo();

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
	for (int i = 0; i < MAX_OBJETOS; i++)
	{
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

		if (llaves[i].activa != 0)
		{
			if (Colicion(personaje.posX, personaje.posY, TAMANHO, TAMANHO, llaves[i].posX, llaves[i].posY, TAMANHO, TAMANHO))
			{
				llaves[i].activa = 0;
				personaje.cantidadLlaves ++;
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
		if (slime[i].activa != 0)
		{
			enemigosVivos = 1;
		}
	}

	for (int i = 0; i < MAX_ENEMIGOS; i++)
	{
		if (mago[i].activa != 0)
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

	printf("sala vacia = %d", salaVacia);
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
	else if (mapa[actualMapaY][actualMapaX - 1] == NULL) //Las puertas que esten conectadas a una parte nula de mapa se reemplazan por #
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
	else if (mapa[actualMapaY][actualMapaX + 1] == NULL)
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
	else if (mapa[actualMapaY + 1][actualMapaX] == NULL)
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

void GeneraciónDelMapa(int cantidadHabitacionesDeseadas)
{
	int habitacionCardinal = 0;
	int a = 0;
	int filaActual = FILAS_MAPA / 2 + 1;
	int columnaActual = COLUMNAS_MAPA / 2 + 1;
	int terminoProceso = 0;
	char *pullHabitaciones;
	int auxRand = 0;

	//La habitacion central siempre será la misma
	mapa[FILAS_MAPA / 2 + 1][COLUMNAS_MAPA / 2 + 1] = "habBase.txt";
	
	while (a < cantidadHabitacionesDeseadas)
	{
		auxRand = rand() % 4 + 1;

		if (auxRand == 1)
		{
			pullHabitaciones = "hab_general_1.txt";  
		}
		if (auxRand == 2)
		{
			pullHabitaciones = "hab_general_2.txt";  
		}
		if (auxRand == 3)
		{
			pullHabitaciones = "hab_general_3.txt";  
		}
		if (auxRand == 4)
		{
			pullHabitaciones = "hab_general_4.txt";  
		}

		//Cuando este por generar la ultima habitacion, obliga a que sea la del jefe
		if (a == cantidadHabitacionesDeseadas - 1)
		{
			pullHabitaciones = "hab_jefe_1.txt";
		}

		habitacionCardinal = rand() % 4 + 1; // entre 1 y 4... 1: Norte, 2: Este, 3: Sur, 4: Oeste 

		if (habitacionCardinal == 1)
		{
			if (mapa[filaActual - 1][columnaActual] == NULL)
			{
				filaActual --;
				mapa[filaActual][columnaActual] = pullHabitaciones;

				filaActual = FILAS_MAPA / 2 + 1;
				columnaActual = COLUMNAS_MAPA / 2 + 1;
				a ++;

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

				printf("Habitacion 4 generada");
			}
			else
			{
				columnaActual --;
			}	
		}
	}
}