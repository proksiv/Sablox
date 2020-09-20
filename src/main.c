#include <stdio.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

#define ASSETS_DIR "../assets"

#define WORLD_W 162
#define WORLD_H 122

char world[WORLD_W][WORLD_H];
typedef enum
{
    Air, Stone, Sand
} MATERIAL;

void must_init(bool test, const char *description)
{
    if(test) return;

    printf("couldn't initialize %s\n", description);
    exit(1);
}

void world_init()
{
    int i, j;
    for(i = 0; i < WORLD_W; i++)
    {
        for(j = 0; j < WORLD_H; j++)
        {
            world[i][j] = Air;
        }
    }
}

void world_render()
{
    ALLEGRO_COLOR color;

    int i, j;
    for(i = 0; i < WORLD_W; i++)
    {
        for(j = 0; j < WORLD_H; j++)
        {
            switch(world[i][j])
            {
                case Stone:
                    color = al_map_rgb_f(0.5, 0.5, 0.5);
                    break;
                case Sand:
                    color = al_map_rgb_f(0.7, 0.7, 0.5);
                    break;
                default:
                    color = al_map_rgb_f(0.0, 0.0, 0.0);
                    break;
            }

            float x1 = (i - 1)*4;
            float y1 = (j - 1)*4;
            float x2 = x1 + 4;
            float y2 = x2 + 4;
            al_draw_filled_rectangle(x1, y1, x2, y2, color);
        }
    }
}

int main()
{
    must_init(al_init(), "allegro");
    must_init(al_install_keyboard(), "keyboard");

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 30.0);
    must_init(timer, "timer");

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    must_init(queue, "queue");

    ALLEGRO_DISPLAY* disp = al_create_display(640, 480);
    must_init(disp, "display");

    ALLEGRO_FONT* font = al_create_builtin_font();
    must_init(font, "font");

    must_init(al_init_image_addon(), "image addon");

    ALLEGRO_PATH* path = al_get_standard_path(ALLEGRO_RESOURCES_PATH);
    al_set_path_filename(path, ASSETS_DIR"/mysha.png");

    ALLEGRO_BITMAP* mysha = al_load_bitmap(al_path_cstr(path, '/'));
    must_init(mysha, "mysha");
    al_destroy_path(path);

    must_init(al_init_primitives_addon(), "primitives addon");

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    world_init();

    bool done = false;
    bool redraw = true;
    ALLEGRO_EVENT event;

    al_start_timer(timer);
    while(1)
    {
        al_wait_for_event(queue, &event);

        switch(event.type)
        {
            case ALLEGRO_EVENT_TIMER:
                // game logic goes here
                redraw = true;
                break;

            case ALLEGRO_EVENT_KEY_DOWN:
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                done = true;
                break;
        }

        if(done)
            break;

        if(redraw && al_is_event_queue_empty(queue))
        {
            al_clear_to_color(al_map_rgb(0, 0, 0));
            al_draw_text(font, al_map_rgb(255, 255, 255), 0, 0, 0, "Hello world!");
            
            // al_draw_bitmap(mysha, 100, 100, 0);
            world_render();
            
            al_flip_display();

            redraw = false;
        }
    }

    al_destroy_bitmap(mysha);

    al_destroy_font(font);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}