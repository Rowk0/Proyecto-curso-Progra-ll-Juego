/*
 * Example program for the Allegro library.
 *
 * This program tests if the ALLEGRO_KEYBOARD_STATE `display' field
 * is set correctly to the focused display.
 */

#include <stdio.h> // Agregado para usar printf en lugar de common.c
#include <allegro5/allegro.h>

static ALLEGRO_DISPLAY *display1;
static ALLEGRO_DISPLAY *display2;

static void redraw(ALLEGRO_COLOR color1, ALLEGRO_COLOR color2)
{
   al_set_target_backbuffer(display1);
   al_clear_to_color(color1);
   al_flip_display();

   al_set_target_backbuffer(display2);
   al_clear_to_color(color2);
   al_flip_display();
}

int main(int argc, char **argv)
{
   ALLEGRO_COLOR black;
   ALLEGRO_COLOR red;
   ALLEGRO_KEYBOARD_STATE kbdstate;

   (void)argc;
   (void)argv;

   // Cambiamos abort_example por mensajes directos en consola
   if (!al_init()) {
      fprintf(stderr, "Error al inicializar Allegro.\n");
      return -1;
   }

   if (!al_install_keyboard()) {
      fprintf(stderr, "Error al instalar el teclado.\n");
      return -1;
   }

   // Forzar modo ventana para evitar bugs en el servidor gráfico de WSL
   al_set_new_display_flags(ALLEGRO_WINDOWED);
   display1 = al_create_display(300, 300);
   
   al_set_new_display_flags(ALLEGRO_WINDOWED);
   display2 = al_create_display(300, 300);

   if (!display1 || !display2) {
      fprintf(stderr, "Error al crear las ventanas.\n");
      return -1;
   }

   black = al_map_rgb(0, 0, 0);
   red = al_map_rgb(255, 0, 0);

   while (1) {
      al_get_keyboard_state(&kbdstate);
      if (al_key_down(&kbdstate, ALLEGRO_KEY_ESCAPE)) {
         break;
      }

      if (kbdstate.display == display1) {
         redraw(red, black);
      }
      else if (kbdstate.display == display2) {
         redraw(black, red);
      }
      else {
         redraw(black, black);
      }

      al_rest(0.1);
   }

   // Limpieza de memoria antes de salir
   al_destroy_display(display1);
   al_destroy_display(display2);

   return 0;
}