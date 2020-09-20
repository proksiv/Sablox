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
    memset(world, Air, WORLD_W*WORLD_H);
}

bool world_get_updated(int cell_x, int cell_y)
{
    return (world[cell_x][cell_y] >> 7) & 1;
}

void world_set_updated(int cell_x, int cell_y, bool b)
{
    if(b)
        world[cell_x][cell_y] |= (1 << 7);
    else
        world[cell_x][cell_y] &= (~(1 << 7));
}

MATERIAL world_get_cell(int cell_x, int cell_y)
{
    return (world[cell_x][cell_y] & 127);
}

void world_set_cell(int cell_x, int cell_y, MATERIAL m)
{
    world[cell_x][cell_y] = world_get_updated(cell_x, cell_y) | m;
}

bool world_move_cell(int src_x, int src_y, int dst_x, int dst_y)
{
    if(world[dst_x][dst_y])
        return false;

    world_set_cell(dst_x, dst_y, world[src_x][src_y]);
    world_set_updated(dst_x, dst_y, true);
    world_set_cell(src_x, src_y, Air);
    return true;
}

void world_update()
{
    int i, j;
    for(i = 1; i < WORLD_W - 1; i++)
    {
        for(j = 1; j < WORLD_H - 1; j++)
        {
            if(world_get_updated(i, j)) // get 7th bit
            {
                world_set_updated(i, j, false); // clear 7th bit
                continue;
            }

            switch(world[i][j])
            {
                case Sand:
                    if(world[i][j + 1])
                    {
                        if(!world[i - 1][j + 1])
                            world_move_cell(i, j, i - 1, j + 1);
                        else if(!world[i + 1][j + 1])
                            world_move_cell(i, j, i + 1, j + 1);
                    }
                    else
                        world_move_cell(i, j, i, j + 1);
                    break;
            }
        }
    }
}

void world_render()
{
    ALLEGRO_COLOR color;

    int i, j;
    for(i = 1; i < WORLD_W - 1; i++)
    {
        for(j = 1; j < WORLD_H - 1; j++)
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
                    continue;
            }

            float x1 = (i - 1)*4;
            float y1 = (j - 1)*4;
            float x2 = x1 + 4;
            float y2 = y1 + 4;
            al_draw_filled_rectangle(x1, y1, x2, y2, color);
        }
    }
}

int main()
{
    must_init(al_init(), "allegro");
    must_init(al_install_keyboard(), "keyboard");
    must_init(al_install_mouse(), "mouse");

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
    al_register_event_source(queue, al_get_mouse_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    world_init();

    bool done = false;
    bool redraw = true;
    ALLEGRO_EVENT event;

    ALLEGRO_MOUSE_STATE state;

    al_start_timer(timer);
    while(1)
    {
        al_wait_for_event(queue, &event);

        switch(event.type)
        {
            case ALLEGRO_EVENT_TIMER:
                al_get_mouse_state(&state);
                if(state.buttons & 1)
                    world_set_cell(state.x / 4, state.y / 4, Sand);

                world_update();

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

            al_draw_bitmap(mysha, 0, 0, 0);
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