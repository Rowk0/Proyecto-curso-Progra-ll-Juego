#include <stdio.h>
#include <allegro5/allegro.h> 
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <math.h>
#include <allegro5/allegro_ttf.h>
#define FILAS_HABITACION 17
#define COLUMNAS_HABITACION 30
#define FILAS_MAPA 5
#define COLUMNAS_MAPA 5
#define TAMANHO 64
#define LARGO_TEXTO 30
#define LARGO_SPRITES 30
#define LARGO_PANTALLA 1920
#define ANCHO_PANTALLA 1088
#define MAX_BALAS 2
#define MAX_ENEMIGOS 20

//Ideas deshechadas: 
//Movimiento de camara: implica crear otra camara estatica para cosas que no quiero que se muevan
//Ver todas las habitaciones mientras te mueves: eso implica hacer más condicionales en enemigos, reformular cargar mapa

////////////////////////////////////////////////////////////////  tareas

//Menu funcional
//Enemigo estatico que dispare patrones.
//Puntaje y monedas

//3 elementos estaticos
//3 elementos dinamicos

//Resolver la activacion de slimes una vez limpias las salas
//añadir tipos de habitaciones 
//Cmabiar metodo de cambio entre habitaciones por puertas
//aleatorizar la generacion de habitaciones
//Más interacciones (vida, dinero, power-ups)

/////////////////////////////////////////////////////////////////  flujo trabajo

//gcc juego.c -o juego -lallegro -lallegro_main -lallegro_primitives -lallegro_image -lm -lallegro_ttf -lallegro_font
//para compilar

//./juego 
//para correr programa

//Si no funciona la pantalla, poner "wsl --shutdown" en powershell y luego "code ."

//flujo git (antes ctrl + s para guardar archivo local)
//git add . -> git commit -m "cambios" -> git push.

////////////////////////////////////////////////////////////////// Estructuras

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
	//CANTIDAD DE BALAS
	//bala_ bala[MAX_BALAS];
} jugador;

jugador personaje;

typedef struct 
{
	int posX;
	int posY;
	int tamanho;
} mouse_;

mouse_ mouse;

struct dirBala_
{
	int derecha;
	int izquierda;
	int abajo;
	int arriba;
};

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
} bala_;

bala_ bala[MAX_BALAS];

//Control de balas
int balaActual = 0;

//cadencia de disparo
int cadencia = 0;

typedef struct 
{
	int tipo;
	int posX;
	int posY;
	int velocidad;
	int activa;
	int direccion; //1 = derecha, 2 = izquierda, 3 = arriba, 4 = abajo
	int vida;
} enemigo;
 
enemigo slime[MAX_ENEMIGOS];

int slimeActual = 0;

///////////////////////////////////////////////////////////////// Variables globales

//Estructura donde se guarda el estado del teclado y del mouse
ALLEGRO_KEYBOARD_STATE estado; 
ALLEGRO_MOUSE_STATE estadoMouse;

//En este arreglo se carga el mapa, y en base a él, se dibuja
char sala[FILAS_HABITACION][COLUMNAS_HABITACION];

//Aqui se guarda el nombre de los archivos en un arreglo simulando un mapa
char *mapa[FILAS_MAPA][COLUMNAS_MAPA];

//Control de ubicacion de mapa
int actualMapaX = 3;
int actualMapaY = 3;

//Cuando JUEGO = 0, el while termina y se cierra el programa
int JUEGO = 1;

int MENU = 0;

//Control de sprites del personaje
int controlSprites = 0;

//Se guarda la direccion de la bala en base al mouse
float direccionBala = 0.0;

/////////////////////////////////////////////////////////////////  Funciones

void DibujarMapa(char mapa[FILAS_HABITACION][COLUMNAS_HABITACION], ALLEGRO_BITMAP *spriteSheet);
char cargarMapa(char nombreMapa[LARGO_TEXTO], FILE *varMapa, char mapa[FILAS_HABITACION][COLUMNAS_HABITACION], jugador *personaje, enemigo enemigo[MAX_ENEMIGOS]);
bool ColisionMapa(char mapa[FILAS_HABITACION][COLUMNAS_HABITACION], int jugadorPosXProximo, int jugadorPosYProximo);
void Logica();
void MovimientoJugador();
void InitAllegro();
int InitGameComponents(ALLEGRO_DISPLAY *ventana, mouse_ *mouse);
void InputHandle();
void Render(char mapa[FILAS_HABITACION][COLUMNAS_HABITACION], ALLEGRO_BITMAP *spriteSheet, ALLEGRO_BITMAP *spriteSheetBalas, ALLEGRO_BITMAP *spriteSheetCaminarCaballero, ALLEGRO_BITMAP *spriteSheetIcons, ALLEGRO_FONT *fuenteJuego);
void Disparo();
void MovimientoCamara();
void AnimacionPersonaje(ALLEGRO_BITMAP *spriteSheet, ALLEGRO_BITMAP *spriteSheetCaminarCaballero);
void AnimacionEnemigos(ALLEGRO_BITMAP *spriteSheet);
void LogicaEnemigos();
void ColisionEnemigos();
void CambioDeHabitaciones();
void RenderMenu(ALLEGRO_FONT *fuenteJuego);
void PersonajeInvulnerable();

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
	char nombreHabitacion2[LARGO_TEXTO] = "mapaTest2.txt";

	//Crear ventana
    ALLEGRO_DISPLAY *ventana;

	//Sprites
	ALLEGRO_BITMAP *spriteSheet;
	ALLEGRO_BITMAP *spriteSheetBalas;
	ALLEGRO_BITMAP *spriteSheetCaminarCaballero;
	ALLEGRO_BITMAP *spriteSheetIcons;

	//Fonts
	ALLEGRO_FONT *fuenteJuego;
	
	///////////////////////////////////////////////////////////////

	InitAllegro();
	
	InitGameComponents(ventana, &mouse);

	fuenteJuego = al_load_ttf_font("PressStart2P-Regular.ttf", 32, 0);

	while (MENU)
	{
		InputHandle();

		RenderMenu(fuenteJuego);

		al_rest(0.016);
	}

	cargarMapa(nombreHabitacion, archivoMapas, sala, &personaje, slime);

	spriteSheet = al_load_bitmap("64x64.png");

	spriteSheetBalas = al_load_bitmap("sp_guns.png");

	spriteSheetCaminarCaballero = al_load_bitmap("64x64_caminar.png");

	spriteSheetIcons = al_load_bitmap("64x64_icons.png");

	while (JUEGO)
	{
		//Funcion que actualiza el estado del teclado y mouse
		InputHandle();

		//Logica del juego. Ej: movimientos del jugador
		Logica();

		//Dibujar aqui
		Render(sala, spriteSheet, spriteSheetBalas, spriteSheetCaminarCaballero, spriteSheetIcons, fuenteJuego);

		//Hacer descansar el cpu
		al_rest(0.016); 
	}

	return 0;
}

void Logica()
{
	MovimientoJugador();

	PersonajeInvulnerable();

	Disparo();

	//MovimientoCamara();

	LogicaEnemigos();

	CambioDeHabitaciones();
	
	//Axis del mouse
	al_get_mouse_num_axes();

	//Cambiar por una pantalla de PERDISTE
	if (personaje.vidas == 0)
	{
		JUEGO = 0;
	}
	
}

void Disparo()
{
	//Como Disparo() se encuentra en while, cada llamada se va acumulando en cadencia, lo usaremos como una especie de timer
	cadencia++;

	/*if(al_key_down(&estado, ALLEGRO_KEY_UP) && cadencia > 20)
	{	
		bala[balaActual].posX = personaje.posX + (TAMANHO/2);
		bala[balaActual].posY = personaje.posY + (TAMANHO/2);
		bala[balaActual].activa = 1;
		bala[balaActual].dirBala.arriba = 1;
		bala[balaActual].dirBala.abajo = 0;
		bala[balaActual].dirBala.derecha = 0;
		bala[balaActual].dirBala.izquierda= 0;
		balaActual++;
		cadencia = 0;
	}

	if(al_key_down(&estado, ALLEGRO_KEY_DOWN) && cadencia > 20)
	{	
		bala[balaActual].posX = personaje.posX + (TAMANHO/2);
		bala[balaActual].posY = personaje.posY + (TAMANHO/2);
		bala[balaActual].activa = 1;
		bala[balaActual].dirBala.arriba = 0;
		bala[balaActual].dirBala.abajo = 1;
		bala[balaActual].dirBala.derecha = 0;
		bala[balaActual].dirBala.izquierda= 0;
		balaActual++;
		cadencia = 0;
	}

	if(al_key_down(&estado, ALLEGRO_KEY_RIGHT) && cadencia > 20)
	{	
		bala[balaActual].posX = personaje.posX + (TAMANHO/2);
		bala[balaActual].posY = personaje.posY + (TAMANHO/2);
		bala[balaActual].activa = 1;
		bala[balaActual].dirBala.arriba = 0;
		bala[balaActual].dirBala.abajo = 0;
		bala[balaActual].dirBala.derecha = 1;
		bala[balaActual].dirBala.izquierda= 0;
		balaActual++;
		cadencia = 0;
	}

	if(al_key_down(&estado, ALLEGRO_KEY_LEFT) && cadencia > 20)
	{	
		bala[balaActual].posX = personaje.posX + (TAMANHO/2);
		bala[balaActual].posY = personaje.posY + (TAMANHO/2);
		bala[balaActual].activa = 1;
		bala[balaActual].dirBala.arriba = 0;
		bala[balaActual].dirBala.abajo = 0;
		bala[balaActual].dirBala.derecha = 0;
		bala[balaActual].dirBala.izquierda= 1;
		balaActual++;
		cadencia = 0;
	}*/

	if(al_mouse_button_down(&estadoMouse, ALLEGRO_MOUSE_BUTTON_LEFT) && cadencia > 20)
	{
		bala[balaActual].posX = personaje.posX + (TAMANHO/2);
		bala[balaActual].posY = personaje.posY + (TAMANHO/2);
		bala[balaActual].activa = 1;
		direccionBala = atan2(mouse.posY - personaje.posY, mouse.posX - personaje.posX); //atan2(y2 - y1, x2 - x1)
		bala[balaActual].anguloBalaX = cos(direccionBala) * bala[balaActual].velocidad;
		bala[balaActual].anguloBalaY = sin(direccionBala) * bala[balaActual].velocidad;
		balaActual++;
		cadencia = 0;
	}

	//Aumenta constantemente bala[i].posY/bala[i].posX
	for (int i = 0; i < MAX_BALAS; i++)
	{

		///////////////////////////////////////////// bala desde el mouse
		if (bala[i].activa != 0)
		{
			bala[i].posX += bala[i].anguloBalaX;
			bala[i].posY += bala[i].anguloBalaY;
		}
		//////////////////////////////////////////// bala desde el teclado

		/*if (bala[i].dirBala.arriba != 0)
		{
			bala[i].posY -= bala[i].velocidad;
		}

		if (bala[i].dirBala.abajo != 0)
		{
			bala[i].posY += bala[i].velocidad;
		}

		if (bala[i].dirBala.derecha != 0)
		{
			bala[i].posX += bala[i].velocidad;
		}

		if (bala[i].dirBala.izquierda != 0)
		{
			bala[i].posX -= bala[i].velocidad;
		}*/

		if (ColisionMapa(sala, bala[i].posX, bala[i].posY) || 
		ColisionMapa(sala, bala[i].posX + (TAMANHO/4) - 1, bala[i].posY) || 
		ColisionMapa(sala, bala[i].posX + (TAMANHO/4) - 1, bala[i].posY + (TAMANHO/4) - 1) ||
		ColisionMapa(sala, bala[i].posX, bala[i].posY + (TAMANHO/4) - 1))
		{
			bala[i].activa = 0;
		}
	}

	//Cuando el arreglo este a punto de terminar, se reinicia
	if (balaActual > MAX_BALAS - 1)
	{
		balaActual = 0;
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

char cargarMapa(char nombreMapa[LARGO_TEXTO], FILE *archivoMapa, char mapa[FILAS_HABITACION][COLUMNAS_HABITACION], jugador *personaje, enemigo enemigo[MAX_ENEMIGOS])
{

	if ((archivoMapa = fopen(nombreMapa,"r")) == NULL)
	{
		return 0;
	} 

	//Reinicio de enemigos
	for (int k = 0; k < MAX_ENEMIGOS; k++)
	{
		slime[k].activa = 0;
	}

	//Reinicio de balas
	for (int l = 0; l < MAX_ENEMIGOS; l++)
	{
		bala[l].activa = 0;
	}

	for (int i = 0; i < FILAS_HABITACION; i++) 
	{
    	for (int j = 0; j < COLUMNAS_HABITACION; j++) 
		{
			//fscanf ignora los espacios y saltos de lineas 
        	fscanf(archivoMapa, " %c", &mapa[i][j]);

			//Ubicar posicion personaje
			if (mapa[i][j]=='@')
			{
				personaje->posX = j * TAMANHO;
				personaje->posY = i * TAMANHO;
			}
			
			//ubicar posicion enemigo
			if (mapa[i][j]=='s')
			{
				if(enemigo[slimeActual].activa == 0)
				{
					enemigo[slimeActual].activa = 1;
					enemigo[slimeActual].posX = j * TAMANHO;
					enemigo[slimeActual].posY = i * TAMANHO;
					slimeActual ++;
				}
			}

			if (slimeActual > MAX_ENEMIGOS - 1)
			{
				slimeActual = 0;
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

			if (mapa[i][j] == 'a')
			{
				al_draw_bitmap_region(spriteSheet, 5 * TAMANHO, 15 * TAMANHO, TAMANHO, TAMANHO, j * TAMANHO, i * TAMANHO, 0);
			}

			if (mapa[i][j] == 'p')
			{
				al_draw_bitmap_region(spriteSheet, 4 * TAMANHO, 15 * TAMANHO, TAMANHO, TAMANHO, j * TAMANHO, i * TAMANHO, 0);
			}

		}
		printf("\n");
	}
}

void Render(char mapa[FILAS_HABITACION][COLUMNAS_HABITACION], ALLEGRO_BITMAP *spriteSheet, ALLEGRO_BITMAP *spriteSheetBalas, ALLEGRO_BITMAP *spriteSheetCaminarCaballero, ALLEGRO_BITMAP *spriteSheetIcons, ALLEGRO_FONT *fuenteJuego)
{

	al_clear_to_color(al_map_rgb(0, 0, 0));
	//////////////////////////////////////////////// Dibujar en este espacio

	DibujarMapa(mapa, spriteSheet);

	//Jugador de SpriteSheet
	AnimacionPersonaje(spriteSheet, spriteSheetCaminarCaballero);

	//Monedas Jugador
	al_draw_bitmap_region(spriteSheetIcons, 3 * TAMANHO, 8 * TAMANHO, TAMANHO, TAMANHO, TAMANHO, TAMANHO + TAMANHO / 2, 0);
	al_draw_textf(fuenteJuego, al_map_rgb(192, 192, 192), TAMANHO + 80, TAMANHO + 50, 0, "= %d", personaje.cantidadMonedas);

	//Vidas jugador
	for (int j = 0; j < personaje.vidas; j++)
	{
		al_draw_bitmap_region(spriteSheetIcons, 4 * TAMANHO, 134 * TAMANHO, TAMANHO, TAMANHO, j * TAMANHO + TAMANHO, TAMANHO / 2, 0);
	}
	
	//Dibujo de balas
	for (int i = 0; i < MAX_BALAS; i++)
	{
		if (bala[i].activa != 0)
		{
			//Bala circular
			//al_draw_filled_circle(bala[i].posX, bala[i].posY, 8, al_map_rgb(0, 255, 255));

			//Bala cuadrada
			//al_draw_filled_rectangle(bala[i].posX, bala[i].posY, bala[i].posX + (TAMANHO / 4), bala[i].posY + (TAMANHO / 4), al_map_rgb(0, 255, 255));

			//Bala sprite 
			//al_draw_bitmap_region(spriteSheetBalas, 2 * 16, 0 * 16, 16, 16, bala[i].posX, bala[i].posY, 0);

			al_draw_scaled_bitmap(spriteSheetBalas, 2 * 16, 0 * 16, 16, 16, bala[i].posX - 24, bala[i].posY - 24, TAMANHO, TAMANHO, 0); 
		}
	}
	
	//Dibujo enemigos
	AnimacionEnemigos(spriteSheet);
	
	//Puntero del mouse
	al_draw_filled_rectangle(mouse.posX - (mouse.tamanho/ 2), mouse.posY - (mouse.tamanho / 2), mouse.posX + (mouse.tamanho / 2), mouse.posY + (mouse.tamanho / 2), al_map_rgb(0, 255, 255));

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

	//Inicializando enemigos
	for (int i = 0; i < MAX_ENEMIGOS; i++)
	{
		slime[i].posX = 0;
		slime[i].posY = 0;
		slime[i].velocidad = 3;
		slime[i].activa = 0;
		slime[i].tipo = 0;
		slime[i].direccion = 0;
		slime[i].vida = 4;
	}

	//Inicializando balas
	for (int i = 0; i < MAX_BALAS; i++)
	{
		bala[i].posX = 0;
		bala[i].posY = 0;
		bala[i].velocidad = 10;
		bala[i].activa = 0;
		bala[i].dirBala.abajo = 0;
		bala[i].dirBala.arriba = 0;
		bala[i].dirBala.derecha = 0;
		bala[i].dirBala.izquierda = 0;
		bala[i].danho = 1;
		bala[i].anguloBalaX = 0;
		bala[i].anguloBalaY = 0;
	}

	for (int i = 0; i < FILAS_MAPA; i++)
	{
		for (int j = 0; i < COLUMNAS_MAPA; i++)
		{
			mapa[i][j] = NULL;
		}
	}

	mapa[3][3] = "habBase.txt";
	mapa[3][2] = "mapaTest2.txt";
	mapa[4][3] = "mapaSurTest.txt";
	mapa[2][3] = "mapaNorteTest.txt";
	mapa[3][4] = "mapaEsteTest.txt";
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
	}
}

void LogicaEnemigos()
{
	//Colicion slime y pared
	int auxXSlime = 0;
	int auxYSlime = 0;

	ColisionEnemigos();

	//Slime persiguiendo al jugador
	for (int i = 0; i < MAX_ENEMIGOS; i++)
	{		
		if (slime[i].activa != 0)
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

void ColisionEnemigos()
{
	for (int i = 0; i < MAX_ENEMIGOS; i++)
	{
		if (slime[i].activa != 0)
		{
			//Colision slime y bala
			for (int j = 0; j < MAX_BALAS; j++)
			{
				if (Colicion(slime[i].posX, slime[i].posY, TAMANHO, TAMANHO, bala[j].posX, bala[j].posY, TAMANHO / 4, TAMANHO / 4))
				{
					if(bala[j].activa != 0)
					{
						slime[i].vida -= bala[j].danho;
						bala[j].activa = 0;
						printf("Vida de slime: %d", slime[i].vida);
					}

					if(slime[i].vida <= 0)
					{
						slime[i].activa = 0;
						personaje.cantidadMonedas ++;
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
}

void CambioDeHabitaciones()
{
	FILE *archivoHabitacion = NULL;
	
	if (personaje.posX < 0)
	{
		if (mapa[actualMapaY][actualMapaX - 1] != NULL)
		{
			actualMapaX --;
			cargarMapa(mapa[actualMapaY][actualMapaX], archivoHabitacion, sala, &personaje, slime);

			personaje.posX = LARGO_PANTALLA - TAMANHO;
		}
		else
		{
			personaje.posX = 0;
		}
	}

	if (personaje.posX > LARGO_PANTALLA - TAMANHO)
	{
		if (mapa[actualMapaY][actualMapaX + 1] != NULL)
		{
			actualMapaX ++;
			cargarMapa(mapa[actualMapaY][actualMapaX], archivoHabitacion, sala, &personaje, slime);

			personaje.posX = TAMANHO;
		}
		else
		{
			personaje.posX = LARGO_PANTALLA - TAMANHO;
		}
	}

	if (personaje.posY > ANCHO_PANTALLA - TAMANHO)
	{
		if (mapa[actualMapaY + 1][actualMapaX] != NULL)
		{
			actualMapaY ++;
			cargarMapa(mapa[actualMapaY][actualMapaX], archivoHabitacion, sala, &personaje, slime);

			personaje.posY = TAMANHO;
		}
		else
		{
			personaje.posY = ANCHO_PANTALLA - TAMANHO;
		}
	}
	
	if (personaje.posY < 0)
	{
		if (mapa[actualMapaY - 1][actualMapaX] != NULL)
		{
			actualMapaY --;
			cargarMapa(mapa[actualMapaY][actualMapaX], archivoHabitacion, sala, &personaje, slime);

			personaje.posY = ANCHO_PANTALLA - TAMANHO;
		}
		else
		{
			personaje.posY = 0;
		}
	}
}

void RenderMenu(ALLEGRO_FONT *fuenteJuego)
{
	al_clear_to_color(al_map_rgb(0, 0, 0));
	//////////////////////////////////////////////// Dibujar en este espacio

	al_draw_text(fuenteJuego, al_map_rgb(0, 255, 255), LARGO_PANTALLA / 2, ANCHO_PANTALLA / 2 + 200, ALLEGRO_ALIGN_CENTRE, "Jugar");

	////////////////////////////////////////////////
	al_flip_display();
}
