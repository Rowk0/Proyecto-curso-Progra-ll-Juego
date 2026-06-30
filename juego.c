#include <stdio.h>
#include <allegro5/allegro.h> 
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#define FILAS 17
#define COLUMNAS 30
#define TAMANHO 64
#define LARGO_TEXTO 30
#define LARGO_SPRITES 30
#define LARGO_PANTALLA 1920
#define ANCHO_PANTALLA 1088

////////////////////////////////////////////////////////////////  tareas

//Terminar Camara

//Mejorar Cambios de habitacion

//Más interacciones (vida, dinero, disparar)
//Hacer una especie de HUD

/////////////////////////////////////////////////////////////////  flujo trabajo

//gcc juego.c -o juego -lallegro -lallegro_main -lallegro_primitives -lallegro_image
//para compilar

//./juego 
//para correr programa

//Si no funciona la pantalla, poner "wsl --shutdown" en powershell y luego "code ."

//flujo git (antes ctrl + s para guardar archivo local)
//git add . -> git commit -m "cambios" -> git push.

////////////////////////////////////////////////////////////////// Estructuras

typedef struct 
{
	int posX;
	int posY;
	int velocidad;
	int movimientoJugador;
} jugador;

jugador personaje;

typedef struct 
{
	int posX;
	int posY;
	int tamanho;
} mouse_;

mouse_ mouse;

typedef	struct 
{
	int posX;
	int posY;
	int velocidad;
	int activa;
} bala_;

bala_ bala;

///////////////////////////////////////////////////////////////// Variables globales

//Estructura donde se guarda el estado del teclado y del mouse
ALLEGRO_KEYBOARD_STATE estado; 
ALLEGRO_MOUSE_STATE estadoMouse;

//En este arreglo se carga el mapa, y en base a él, se dibuja
char sala[FILAS][COLUMNAS];

//Cuando JUEGO = 0, el while termina y se cierra el programa
int JUEGO = 1;

//Control de sprites del personaje
int controlSprites = 0;

/////////////////////////////////////////////////////////////////  Funciones

void DibujarMapa(char mapa[FILAS][COLUMNAS], ALLEGRO_BITMAP *sprites[LARGO_SPRITES], ALLEGRO_BITMAP *spriteSheet);
char cargarMapa(char nombreMapa[LARGO_TEXTO], FILE *varMapa, char mapa[FILAS][COLUMNAS], jugador *personaje);
bool ColisionMapa(char mapa[FILAS][COLUMNAS], int jugadorPosXProximo, int jugadorPosYProximo);
void Logica();
void MovimientoJugador();
void InitAllegro();
int InitGameComponents(ALLEGRO_DISPLAY *ventana, ALLEGRO_BITMAP *sprites[LARGO_SPRITES], mouse_ *mouse);
void InputHandle();
void Render(char mapa[FILAS][COLUMNAS], ALLEGRO_BITMAP *sprites[LARGO_SPRITES], ALLEGRO_BITMAP *spriteSheet);
void Disparo();

//Primeros cuatro parametros representan un cuadrado (x1,y1) esquina superior izquierda (w1,h1) esquina inferior derecha
//Ultimo cuatro representa otro cuadrado con otros parametros
//Compara si hay entre colicion entre ambos, y si hay devuelve true
bool Colicion(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);

//////////////////////////////////////////////////////////////////////////////////////////// main

int main(int argc, char **argv)
{ 
	/////////////////////////////////////////////////////////////// Declaraciones

	bala.posX = 0;
	bala.posY = 0;
	bala.velocidad = 10;
	bala.activa = 0;

	//Inicializando jugador
	personaje.velocidad = 5;
	personaje.movimientoJugador = 1;

	FILE *archivoMapas = NULL;
	char nombreHabitacion[LARGO_TEXTO] = "mapas.txt";
	char nombreHabitacion2[LARGO_TEXTO] = "mapaTest2.txt";

	//Crear ventana
    ALLEGRO_DISPLAY *ventana;

	//Arreglo de sprites
	ALLEGRO_BITMAP *sprites[LARGO_SPRITES];

	ALLEGRO_BITMAP *spriteSheet;
	
	///////////////////////////////////////////////////////////////

	cargarMapa(nombreHabitacion, archivoMapas, sala, &personaje);

	InitAllegro();
	
	InitGameComponents(ventana, sprites, &mouse);

	spriteSheet = al_load_bitmap("64x64.png");

	while (JUEGO)
	{
		//Funcion que actualiza el estado del teclado y mouse
		InputHandle();

		//Logica del juego. Ej: movimientos del jugador
		Logica();

		//Cuando funcione correctamente lo pondré en logica
		if (personaje.posX < 0) 
		{
			cargarMapa(nombreHabitacion2, archivoMapas, sala, &personaje);
		}

		//Dibujar aqui
		Render(sala, sprites, spriteSheet);

		al_rest(0.016); //Hacer descansar el cpu
	}

	return 0;
}

void Logica()
{
	MovimientoJugador();

	Disparo();

	ALLEGRO_TRANSFORM camara;

	//Poner en objetivo el objeto a mover
	al_identity_transform(&camara);

	//Hacer los movimientos
	//Movimiento pantalla en eje x
	int camaraLimiteX = 1920;
	int camaraLimiteY = 1088;

	if (personaje.posX < camaraLimiteX * 0.25) //Izquierda
	{
		al_translate_transform(&camara, camaraLimiteX * 0.25 - personaje.posX, 0);
	}
	else if (personaje.posX > camaraLimiteX * 0.75) //Derecha
	{
		al_translate_transform(&camara, camaraLimiteX * 0.75 - personaje.posX, 0);
	}
	
	//Movimiento pantalla en eje y
	if (personaje.posY > ANCHO_PANTALLA * 0.75) //Arriba
	{
		al_translate_transform(&camara, 0, ANCHO_PANTALLA * 0.75 - personaje.posY);
	}
	else if (personaje.posY < ANCHO_PANTALLA * 0.25) //Abajo
	{
		al_translate_transform(&camara, 0, ANCHO_PANTALLA * 0.25 - personaje.posY);
	}

	//Usar los movimientos
	al_use_transform(&camara);
	
	//Axis del mouse
	al_get_mouse_num_axes();

}

void Disparo()
{
	

	if(al_mouse_button_down (&estadoMouse, ALLEGRO_MOUSE_BUTTON_LEFT))
	{
		bala.posX = personaje.posX + (TAMANHO/2);
		bala.posY = personaje.posY + (TAMANHO/2);
		bala.activa = 1;
	}

	if(bala.activa != 0)
	{
		bala.posX += bala.velocidad;
	}
}

void MovimientoJugador()
{
	//Guardamos la posicion del jugador en un axiliar
	int auxX = personaje.posX;
	int auxY = personaje.posY;

	if (personaje.movimientoJugador == 1)
	{
		if(al_key_down(&estado, ALLEGRO_KEY_W))
		{
			personaje.posY -= personaje.velocidad; 
		}

		if(al_key_down(&estado, ALLEGRO_KEY_S))
		{
			personaje.posY += personaje.velocidad; 
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
		}

		if(al_key_down(&estado, ALLEGRO_KEY_A))
		{
			personaje.posX -= personaje.velocidad; 
		}

		if (ColisionMapa(sala, personaje.posX, personaje.posY) || 
		ColisionMapa(sala, personaje.posX + TAMANHO - 1, personaje.posY) || 
		ColisionMapa(sala, personaje.posX + TAMANHO - 1, personaje.posY + TAMANHO - 1) ||
		ColisionMapa(sala, personaje.posX, personaje.posY + TAMANHO - 1))
		{
			personaje.posX = auxX;
		}
	}
	
}

char cargarMapa(char nombreMapa[LARGO_TEXTO], FILE *archivoMapa, char mapa[FILAS][COLUMNAS], jugador *personaje)
{

	if ((archivoMapa = fopen(nombreMapa,"r")) == NULL)
	{
		return 0;
	} 

	for (int i = 0; i < FILAS; i++) 
	{
    	for (int j = 0; j < COLUMNAS; j++) 
		{
			//fscanf ignora los espacios y saltos de lineas 
        	fscanf(archivoMapa, " %c", &mapa[i][j]);

			if (mapa[i][j]=='@')
			{
				personaje->posX = j * TAMANHO;
				personaje->posY = i * TAMANHO;
			}
    	}
	}
	
}

void DibujarMapa(char mapa[FILAS][COLUMNAS], ALLEGRO_BITMAP *sprites[LARGO_SPRITES], ALLEGRO_BITMAP *spriteSheet)
{
	for (int i = 0; i < FILAS; i++)
	{
		for (int j = 0; j < COLUMNAS; j++)
		{
			if (mapa[i][j] == '#')
			{
				//al_draw_bitmap(sprites[1], j * TAMANHO, i * TAMANHO, 0);
				al_draw_bitmap_region(spriteSheet, 1 * TAMANHO, 16 * TAMANHO, TAMANHO, TAMANHO, j * TAMANHO, i * TAMANHO, 0);
			}

			if (mapa[i][j] == '.')
			{
				//al_draw_bitmap(sprites[0], j * TAMANHO, i * TAMANHO, 0);
				al_draw_bitmap_region(spriteSheet, 2 * TAMANHO, 15 * TAMANHO, TAMANHO, TAMANHO, j * TAMANHO, i * TAMANHO, 0);
			}

			if (mapa[i][j] == '@')
			{
				//al_draw_bitmap(sprites[0], j * TAMANHO, i * TAMANHO, 0);
				al_draw_bitmap_region(spriteSheet, 2 * TAMANHO, 15 * TAMANHO, TAMANHO, TAMANHO, j * TAMANHO, i * TAMANHO, 0);
			}
		}
		printf("\n");
	}
}

void Render(char mapa[FILAS][COLUMNAS], ALLEGRO_BITMAP *sprites[LARGO_SPRITES], ALLEGRO_BITMAP *spriteSheet)
{

	al_clear_to_color(al_map_rgb(0, 0, 0));
	//////////////////////////////////////////////// Dibujar en este espacio

	DibujarMapa(mapa, sprites, spriteSheet);

	//Jugador escalado
	//al_draw_scaled_bitmap(sprites[2], 0, 0, 64, 64, personaje.posX, personaje.posY, 128, 128, 0);

	//Jugador de SpriteSheet
	controlSprites += 1;
	
	if (controlSprites >= 0)
	{
		al_draw_bitmap_region(spriteSheet, 0 * TAMANHO, 23 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, 0);	
	}
	if (controlSprites > 10)
	{
		al_draw_bitmap_region(spriteSheet, 1 * TAMANHO, 23 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, 0);	
	}
	if (controlSprites > 20)
	{
		al_draw_bitmap_region(spriteSheet, 2 * TAMANHO, 23 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, 0);	
	}
	if (controlSprites > 30)
	{
		al_draw_bitmap_region(spriteSheet, 3 * TAMANHO, 23 * TAMANHO, TAMANHO, TAMANHO, personaje.posX, personaje.posY, 0);	
	}
	if(controlSprites > 40)
	{
		controlSprites = 0;
	}

	if (bala.activa != 0)
	{
		al_draw_filled_circle(bala.posX, bala.posY, 8, al_map_rgb(0, 255, 255));
	}
	
	//Puntero del mouse
	//al_draw_filled_rectangle(mouse.posX - (mouse.tamanho/ 2), mouse.posY - (mouse.tamanho / 2), mouse.posX + (mouse.tamanho / 2), mouse.posY + (mouse.tamanho / 2), al_map_rgb(0, 255, 255));

	////////////////////////////////////////////////
	al_flip_display();
}

bool ColisionMapa(char mapa[FILAS][COLUMNAS], int jugadorPosXProximo, int jugadorPosYProximo)
{
	//Convertimos la posicion del jugador en indice del arreglo del mapa
	int columna = jugadorPosXProximo / TAMANHO;
	int fila = jugadorPosYProximo / TAMANHO;

	//Validamos que no nos salimos del arreglo
	if(fila >= 0 && fila < FILAS && columna >= 0 && columna < COLUMNAS)
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
}

int InitGameComponents(ALLEGRO_DISPLAY *ventana, ALLEGRO_BITMAP *sprites[LARGO_SPRITES], mouse_ *mouse)
{
	//Inicializar ventana
	al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
	ventana = al_create_display(640, 480);
	if(!ventana) return -1;

	//Cargar imagenes
	sprites[0] = al_load_bitmap("Suelo.png");
	sprites[1] = al_load_bitmap("Pared.png");
	sprites[2] = al_load_bitmap("Cesar.png");

	//Inicializando Mouse
	mouse->posX = 0;
	mouse->posY = 0;
	mouse->tamanho = 7;
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
	}
}