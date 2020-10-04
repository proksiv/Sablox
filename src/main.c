#include <stdio.h>

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

#include "world.h"

#define ASSETS_DIR "../assets"
#define WINDOW_W 384
#define WINDOW_H 216

void must_init(bool test, const char *description)
{
    if(test) return;

    printf("couldn't initialize %s\n", description);
    exit(1);
}

int main()
{
    srand(time(NULL));

    must_init(al_init(), "allegro");
    must_init(al_install_keyboard(), "keyboard");
    must_init(al_install_mouse(), "mouse");

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
    must_init(timer, "timer");

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    must_init(queue, "queue");

    ALLEGRO_DISPLAY* disp = al_create_display(WINDOW_W, WINDOW_H);
    must_init(disp, "display");

    ALLEGRO_FONT* font = al_create_builtin_font();
    must_init(font, "font");

    must_init(al_init_image_addon(), "image addon");

    /*ALLEGRO_PATH* path = al_get_standard_path(ALLEGRO_RESOURCES_PATH);
    al_set_path_filename(path, ASSETS_DIR"/mysha.png");

    ALLEGRO_BITMAP* mysha = al_load_bitmap(al_path_cstr(path, '/'));
    must_init(mysha, "mysha");
    al_destroy_path(path);*/

    must_init(al_init_primitives_addon(), "primitives addon");

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_mouse_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    world_init();

    bool done = false;
    bool redraw = true;
    bool paused = false;
    ALLEGRO_EVENT event;

    ALLEGRO_TRANSFORM trans;
    float sx = 1;
    float sy = 1;

    ALLEGRO_MOUSE_STATE state;

    MATERIAL mat = Air;

    al_start_timer(timer);
    while(1)
    {
        al_wait_for_event(queue, &event);

        switch(event.type)
        {
            case ALLEGRO_EVENT_TIMER:
                al_get_mouse_state(&state);
                mat = state.z % MaterialCount;

                if(state.buttons & 1)
                    world_paint(state.x/sx + 1, state.y/sy + 1, mat);
                
                if(!paused)
                    world_update();

                redraw = true;
                break;

            case ALLEGRO_EVENT_KEY_DOWN:
                if(event.keyboard.keycode == ALLEGRO_KEY_SPACE)
                    paused = !paused;
                else if(event.keyboard.keycode == ALLEGRO_KEY_F)
                {
                    al_set_display_flag(disp, ALLEGRO_FULLSCREEN_WINDOW, !(al_get_display_flags(disp) & ALLEGRO_FULLSCREEN_WINDOW));
                    al_acknowledge_resize(disp);
                    float w = al_get_display_width(disp);
                    float h = al_get_display_height(disp);
                    float aspect_ratio = w/h;
                    al_identity_transform(&trans);
                    sx = w/(WINDOW_H*aspect_ratio);
                    sy = h/WINDOW_H;
                    al_scale_transform(&trans, sx, sy);
                    al_use_transform(&trans);
                }
                break;

            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                done = true;
                break;
        }

        if(done)
            break;

        if(redraw && al_is_event_queue_empty(queue))
        {
            al_clear_to_color(al_map_rgb(0, 0, 0));

            // al_draw_bitmap(mysha, 0, 0, 0);
            world_render();

            const char* m_name = material_get_data(mat).name;
            al_draw_text(font, al_map_rgb_f(1.0, 1.0, 1.0), 0, 0, 0, m_name);

            if(paused)
                al_draw_text(font, al_map_rgb_f(1.0, 1.0, 1.0), 0, 16, 0, "<Paused>");
            
            al_flip_display();

            redraw = false;
        }
    }

    // al_destroy_bitmap(mysha);

    al_destroy_font(font);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}