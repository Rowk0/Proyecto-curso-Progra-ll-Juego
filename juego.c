#include <stdio.h>
#include <allegro5/allegro.h> 
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#define FILAS 17
#define COLUMNAS 30
#define TAMANHO 64
#define TAMANHO_MAPA 9
#define LARGO_TEXTO 30

ALLEGRO_KEYBOARD_STATE estado; //Estructura donde se guarda el estado del teclado
ALLEGRO_MOUSE_STATE estadoMouse;

////////////////////////////////////////////////////////////////  tareas

//Crear varios sprites, fondos. 
//Cambiar paredes, según convenga con hitbox
//Implementar camara
//mono, quieto y se mueve la camara
//Cargar más de una habitación
//Implementar spawn jugador por mapa

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
} jugador;

jugador personaje;

typedef struct 
{
	char sala1[FILAS][COLUMNAS];
} mapa;

mapa salas;

//IMPORTANTE
char sala[FILAS][COLUMNAS];

/////////////////////////////////////////////////////////////////  funciones

void DibujarMapa(char mapa[FILAS][COLUMNAS]);
char cargarMapa(char *nombreMapa[LARGO_TEXTO], FILE *varMapa, char mapa[FILAS][COLUMNAS]);
bool ColisionMapa(char mapa[FILAS][COLUMNAS], int jugadorPosXProximo, int jugadorPosYProximo);
void Logica();
void MovimientoJugador();

//Primeros cuatro parametros representan un cuadrado (x1,y1) esquina superior izquierda (w1,h1) esquina inferior derecha
//Ultimo cuatro representa otro cuadrado con otros parametros
//Compara si hay entre colicion entre ambos, y si hay devuelve true
bool Colicion(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);

//////////////////////////////////////////////////////////////////////////////////////////// main

int main(int argc, char **argv)
{ 
	//Inicializando mouse y jugador
	int posXMouse = 0, posYMouse = 0, tamaño = 7;
	personaje.posX = 500;
	personaje.posY = 500;
	personaje.velocidad = 7;

	FILE *archivoMapas = NULL;
	char *nombreHabitacion[LARGO_TEXTO];

	nombreHabitacion[LARGO_TEXTO] = "mapas.txt";  /*ver con Eloy!!!!!*/

	cargarMapa(nombreHabitacion, archivoMapas, sala);
/*	cargarMapa(nombreHabitacion, archivoMapas, sala,);*/
	//Pasar por puntero o personaje.posX
	//Para spawn personaje

	//Inicializar dibujos
	al_init_primitives_addon();

	//Inicializar imagenes
	al_init_image_addon();

	//Inicializar Allegro
	if(!al_init()) return -1;

	//Inicializar teclado
	if(!al_install_keyboard()) return -1;

	//Inicializar mouse
	if(!al_install_mouse()) return -1;

	//Crear ventana
	al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
    ALLEGRO_DISPLAY *ventana = al_create_display(640, 480);
    if(!ventana) return -1;

	//Cargar imagenes
	ALLEGRO_BITMAP *spriteJugador = al_load_bitmap("Cesar.png");

	//Si no hay nada en spriteJugador se devuelve
	if(!spriteJugador)
	{
		return -1;
	}

	while (1)
	{
		al_get_keyboard_state(&estado); //Llena la estructura con el estado actual del taclado
		al_get_mouse_state(&estadoMouse);

		posXMouse = estadoMouse.x;
		posYMouse = estadoMouse.y;
		
		/////////////////////////////////////////////////////// funciones jugador

		//Apagar programa con ESC
		if (al_key_down(&estado, ALLEGRO_KEY_ESCAPE))
		{
			break;
		}

		Logica();
		
		//////////////////////////////////////////////////
	
		al_get_mouse_num_axes();

		al_clear_to_color(al_map_rgb(0, 0, 0));
		//////////////////////////////////////////////// Dibujar en este espacio

		DibujarMapa(sala);

		//Jugador
		//al_draw_filled_rectangle(personaje.posX, personaje.posY, personaje.posX + 64, personaje.posY + 64, al_map_rgb(0, 255, 255));

		//Jugador
		//al_draw_bitmap(spriteJugador, personaje.posX, personaje.posY, 0);

		//Jugador
		al_draw_scaled_bitmap(spriteJugador, 0, 0, 64, 64, personaje.posX, personaje.posY, 128, 128, 0);
		
		//Puntero del mouse
		//al_draw_filled_rectangle(posXMouse - (tamaño / 2), posYMouse - (tamaño / 2), posXMouse + (tamaño / 2), posYMouse + (tamaño / 2), al_map_rgb(0, 255, 255));

		////////////////////////////////////////////////
        al_flip_display();

		al_rest(0.016); //Hacer descansar el cpu
	}

	return 0;
}

void Logica()
{
	MovimientoJugador();
}

void MovimientoJugador()
{

	//Guardamos la posicion del jugador en un axiliar
	int auxX = personaje.posX;
	int auxY = personaje.posY;

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

char cargarMapa(char *nombreMapa[LARGO_TEXTO], FILE *archivoMapa, char mapa[FILAS][COLUMNAS])
{

	if ((archivoMapa = fopen(nombreMapa[LARGO_TEXTO],"r")) == NULL)
	{
		return 0;
	} 

	for (int i = 0; i < FILAS; i++) 
	{
    	for (int j = 0; j < COLUMNAS; j++) 
		{
			//fscanf ignora los espacios y saltos de lineas 
        	fscanf(archivoMapa, " %c", &mapa[i][j]);
			/*if (mapa[i][j]=='@')
			{
			   
			}*/
    	}
	}
	
}

void DibujarMapa(char mapa[FILAS][COLUMNAS])
{

	for (int i = 0; i < FILAS; i++)
	{
		for (int j = 0; j < COLUMNAS; j++)
		{
			if (mapa[i][j] == '#')
			{
				al_draw_filled_rectangle(j * TAMANHO, i * TAMANHO, j * TAMANHO + TAMANHO, i * TAMANHO + TAMANHO, al_map_rgb(245, 73, 39));

			}
		}
		printf("\n");
	}
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