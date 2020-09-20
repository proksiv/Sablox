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
    Air, Stone, Sand, Wood, Fire, MaterialCount
} MATERIAL;

const char *material_name(MATERIAL m)
{
    switch(m)
    {
        case Air:
            return "Air";
        case Stone:
            return "Stone";
        case Sand:
            return "Sand";
        case Wood:
            return "Wood";
        case Fire:
            return "Fire";
        default:
            return "Undefined";
    }
}

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

void world_clear_updates()
{
    int i, j;
    for(i = 1; i < WORLD_W - 1; i++)
    {
        for(j = 1; j < WORLD_H - 1; j++)
        {
            world_set_updated(i, j, false);
        }
    }
}

MATERIAL world_get_cell(int cell_x, int cell_y)
{
    return (world[cell_x][cell_y] & 127);
}

void world_set_cell(int cell_x, int cell_y, MATERIAL m)
{
    world[cell_x][cell_y] = (world_get_updated(cell_x, cell_y) << 7) | m;
}

void world_paint(int cell_x, int cell_y, MATERIAL m)
{
    int i, j;
    for(i = cell_x - 1; i <= cell_x + 1; i++)
    {
        for(j = cell_y - 1; j <= cell_y + 1; j++)
        {
            world_set_cell(i, j, m);
        }
    }
}

bool world_move_cell(int src_x, int src_y, int dst_x, int dst_y)
{
    if(world_get_cell(dst_x, dst_y))
        return false;

    world_set_cell(dst_x, dst_y, world_get_cell(src_x, src_y));
    world_set_updated(dst_x, dst_y, true);
    world_set_cell(src_x, src_y, Air);
    return true;
}

void world_update()
{
    world_clear_updates();

    int i, j, k, l;
    for(i = 1; i < WORLD_W - 1; i++)
    {
        for(j = 1; j < WORLD_H - 1; j++)
        {
            if(world_get_updated(i, j))
                continue;

            switch(world_get_cell(i, j))
            {
                case Sand:
                    if(world_get_cell(i, j + 1))
                    {
                        if(!world_get_cell(i - 1, j + 1))
                            world_move_cell(i, j, i - 1, j + 1);
                        else if(!world_get_cell(i + 1, j + 1))
                            world_move_cell(i, j, i + 1, j + 1);
                    }
                    else
                        world_move_cell(i, j, i, j + 1);
                    break;

                case Fire:
                    for(k = i - 1; k <= i + 1; k++)
                    {
                        for(l = j - 1; l <= j + 1; l++)
                        {
                            if(k == i && l == j)
                                continue;
                            if(world_get_cell(k, l) == Wood)
                            {
                                world_set_cell(k, l, Fire);
                                world_set_updated(k, l, true);
                            }
                        }
                    }
                    world_set_cell(i, j, Air);
                    break;

                default:
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
            switch(world_get_cell(i, j))
            {
                case Stone:
                    color = al_map_rgb_f(0.5, 0.5, 0.5);
                    break;
                case Sand:
                    color = al_map_rgb_f(0.7, 0.7, 0.5);
                    break;
                case Wood:
                    color = al_map_rgb_f(0.5, 0.3, 0.2);
                    break;
                case Fire:
                    color = al_map_rgb_f(0.9, 0.3, 0.1);
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

    MATERIAL mat;

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
                    world_paint(state.x / 4, state.y / 4, mat);
                
                world_update();

                redraw = true;
                break;

            // case ALLEGRO_EVENT_KEY_DOWN:
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
            al_draw_text(font, al_map_rgb_f(1.0, 1.0, 1.0), 0, 0, 0, material_name(mat));
            
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