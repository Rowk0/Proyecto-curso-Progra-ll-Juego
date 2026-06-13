#include <stdio.h>
#include <allegro5/allegro.h> 
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#define FILAS 19
#define COLUMNAS 30
#define TAMANHO 64
#define TAMANHO_MAPA 9

ALLEGRO_KEYBOARD_STATE estado; //Estructura donde se guarda el estado del teclado
ALLEGRO_MOUSE_STATE estadoMouse;

//Leer el mapa desde un archivo (prox. jueves).
//Crear funcion que cargue el archivo.
//Cargar un sprite, al personaje.

//Hacer una estructura al jugador

//gcc juego.c -o juego -lallegro -lallegro_main -lallegro_primitives -lallegro_image
//para compilar

//./juego 
//para correr programa

//Si no funciona la pantalla, poner "wsl --shutdown" en powershell y luego "code ."

//flujo git (antes ctrl + s para guardar archivo local)
//git add . -> git commit -m "cambios" -> git push.

//hacer un makefile para abrir el ejecutable de inmediato

void leerMapa(char mapa[FILAS][COLUMNAS]);
void InicializarHabitaciones();

//Primeros cuatro parametros representan un cuadrado (x1,y1) esquina superior izquierda (w1,h1) esquina inferior derecha
//Ultimo cuatro representa otro cuadrado con otros parametros
//Compara si hay entre colicion entre ambos, y si hay devuelve true
bool Colicion(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
	return ( (x1 < x2 + w2) && (x2 < x1 + w1) && (y1 < y2 + h2) && (y2 < y1 + h1));
}

char mapa2[FILAS][COLUMNAS] = 
{
	"##############  ##############",
	"#                            #",
	"#                            #",
	"#                            #",
	"#                            #",
	"#                            #",
	"#                            #",
	"#                            #",
	"#                            #",
	"#                            #",
	"#                            #",
	"#                            #",
	"#                            #",
	"#                            #",
	"#                            #",
	"#                            #",
	"##############  ##############"
};

typedef struct
{
	bool activa; //Las activas se muestran en pantalla
	bool visitada; //Más adelante para hacer un minimapa
	int tipo; //0 = normal, 1 = especial, 2 = jefe, por ejemplo. Cada tipo tendrá su habitacion propia
} habitacion; 

habitacion mazmorra[TAMANHO_MAPA][TAMANHO_MAPA];

typedef struct 
{
	int posX;
	int posY;
	int velocidad;
	bool movimiento;
} jugador;

jugador personaje;

int main(int argc, char **argv)
{ 
	int posXMouse = 0, posYMouse = 0, tamaño = 7;

	personaje.posX = 500;
	personaje.posY = 500;
	personaje.velocidad = 5;
	personaje.movimiento = true;


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

	//Guardar imagenes
	ALLEGRO_BITMAP *fondo = al_load_bitmap("test.png");

	while (1)
	{
		al_get_keyboard_state(&estado); //Llena la estructura con el estado actual del taclado
		al_get_mouse_state(&estadoMouse);

		posXMouse = estadoMouse.x;
		posYMouse = estadoMouse.y;

		if (personaje.movimiento)
		{
			if (al_key_down(&estado, ALLEGRO_KEY_ESCAPE))
			{
				break;
			}
			
			if(al_key_down(&estado, ALLEGRO_KEY_W))
			{
				personaje.posY -= personaje.velocidad; 
			}

			if(al_key_down(&estado, ALLEGRO_KEY_S))
			{
				personaje.posY += personaje.velocidad; 
			}

			if(al_key_down(&estado, ALLEGRO_KEY_D))
			{
				personaje.posX += personaje.velocidad; 
			}

			if(al_key_down(&estado, ALLEGRO_KEY_A))
			{
				personaje.posX -= personaje.velocidad; 
			}
		}

		al_get_mouse_num_axes();

		al_clear_to_color(al_map_rgb(0, 0, 0));
		//////////////////////////////////////////////// Dibujar en este espacio

		leerMapa(mapa2);

		//Jugador
		al_draw_filled_rectangle(personaje.posX, personaje.posY, personaje.posX + 64, personaje.posY + 64, al_map_rgb(0, 255, 255));
		
		//Puntero del mouse
		al_draw_filled_rectangle(posXMouse - (tamaño / 2), posYMouse - (tamaño / 2), posXMouse + (tamaño / 2), posYMouse + (tamaño / 2), al_map_rgb(0, 255, 255));

		////////////////////////////////////////////////
        al_flip_display();

		al_rest(0.016); //Hacer descansar el cpu
	}

	return 0;
}

void InicializarHabitaciones()
{
	for (int i = 0; i < TAMANHO_MAPA; i++)
	{
		for (int j = 0; j < TAMANHO_MAPA; j++)
		{
			mazmorra[i][j].activa = false;
			mazmorra[i][j].visitada = false;
			mazmorra[i][j].tipo = 0;
		}
	}

	//Como el mapa es 9x9, la habitacion central es la 5,5
	//Sala de spawn, por lo que se activa desde el inicio
	mazmorra[(TAMANHO_MAPA + 1) / 2][(TAMANHO_MAPA + 1) / 2].activa = true;
	mazmorra[(TAMANHO_MAPA + 1) / 2][(TAMANHO_MAPA + 1) / 2].visitada = true;
	mazmorra[(TAMANHO_MAPA + 1) / 2][(TAMANHO_MAPA + 1) / 2].tipo = 0;

	//Hacer un codigo donde busque caminos alrededor de la habitacion central y genere habitaciones segun la cantidad deseada
}

void leerMapa(char mapa[FILAS][COLUMNAS])
{

	for (int i = 0; i < FILAS; i++)
	{
		for (int j = 0; j < COLUMNAS; j++)
		{
			if (mapa[i][j] == '#')
			{
				al_draw_filled_rectangle(j * TAMANHO, i * TAMANHO, j * TAMANHO + TAMANHO, i * TAMANHO + TAMANHO, al_map_rgb(245, 73, 39));

				//Movimiento tosco al tocar el limite.
				if (Colicion(personaje.posX, personaje.posY, TAMANHO, TAMANHO, j * TAMANHO, i * TAMANHO, TAMANHO, TAMANHO))
				{
					if(al_key_down(&estado, ALLEGRO_KEY_W))
					{
						personaje.posY += personaje.velocidad; 
					}

					if(al_key_down(&estado, ALLEGRO_KEY_S))
					{
						personaje.posY -= personaje.velocidad; 
					}

					if(al_key_down(&estado, ALLEGRO_KEY_D))
					{
						personaje.posX -= personaje.velocidad; 
					}

					if(al_key_down(&estado, ALLEGRO_KEY_A))
					{
						personaje.posX += personaje.velocidad; 
					}

					personaje.movimiento = false;
				}
				else 
				{
					personaje.movimiento = true;
				}
			}

		}
		printf("\n");
	}
}