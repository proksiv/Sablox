#include <stdio.h>

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
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

    al_set_new_display_flags(ALLEGRO_OPENGL|ALLEGRO_PROGRAMMABLE_PIPELINE);
    ALLEGRO_DISPLAY* disp = al_create_display(WINDOW_W, WINDOW_H);
    must_init(disp, "display");

    ALLEGRO_FONT* font = al_create_builtin_font();
    must_init(font, "font");

    ALLEGRO_PATH* path = al_get_standard_path(ALLEGRO_RESOURCES_PATH);

    ALLEGRO_SHADER* sh_brightness_filter = al_create_shader(ALLEGRO_SHADER_GLSL);
    must_init(sh_brightness_filter, "brightness_filter shader");
    al_set_path_filename(path, ASSETS_DIR"/shader/brightness_filter_v.glsl");
    must_init(al_attach_shader_source_file(sh_brightness_filter, ALLEGRO_VERTEX_SHADER, al_path_cstr(path, '/')), "brightness_filter vertex shader source");
    al_set_path_filename(path, ASSETS_DIR"/shader/brightness_filter_p.glsl");
    must_init(al_attach_shader_source_file(sh_brightness_filter, ALLEGRO_PIXEL_SHADER, al_path_cstr(path, '/')), "brightness_filter pixel shader source");
    must_init(al_build_shader(sh_brightness_filter), "brightness_filter shader build");

    ALLEGRO_SHADER* sh_blur_h = al_create_shader(ALLEGRO_SHADER_GLSL);
    must_init(sh_blur_h, "horizontal_blur shader");
    al_set_path_filename(path, ASSETS_DIR"/shader/horizontal_blur_v.glsl");
    must_init(al_attach_shader_source_file(sh_blur_h, ALLEGRO_VERTEX_SHADER, al_path_cstr(path, '/')), "horizontal_blur vertex shader source");
    al_set_path_filename(path, ASSETS_DIR"/shader/blur_p.glsl");
    must_init(al_attach_shader_source_file(sh_blur_h, ALLEGRO_PIXEL_SHADER, al_path_cstr(path, '/')), "horizontal_blur pixel shader source");
    must_init(al_build_shader(sh_blur_h), "horizontal_blur shader build");

    ALLEGRO_SHADER* sh_blur_v = al_create_shader(ALLEGRO_SHADER_GLSL);
    must_init(sh_blur_v, "vertical_blur shader");
    al_set_path_filename(path, ASSETS_DIR"/shader/vertical_blur_v.glsl");
    must_init(al_attach_shader_source_file(sh_blur_v, ALLEGRO_VERTEX_SHADER, al_path_cstr(path, '/')), "vertical_blur vertex shader source");
    al_set_path_filename(path, ASSETS_DIR"/shader/blur_p.glsl");
    must_init(al_attach_shader_source_file(sh_blur_v, ALLEGRO_PIXEL_SHADER, al_path_cstr(path, '/')), "vertical_blur pixel shader source");
    must_init(al_build_shader(sh_blur_v), "vertical_blur shader build");

    al_destroy_path(path);

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

            ALLEGRO_BITMAP* original = al_create_bitmap(WINDOW_W, WINDOW_H);
            ALLEGRO_BITMAP* bloom = al_create_bitmap(WINDOW_W, WINDOW_H);

            al_set_target_bitmap(original);
            world_render();

            al_set_target_bitmap(bloom);
            al_use_shader(sh_brightness_filter);
            al_draw_bitmap(original, 0, 0, 0);
            al_use_shader(sh_blur_v);
            al_draw_bitmap(bloom, 0, 0, 0);
            al_use_shader(sh_blur_h);
            al_draw_bitmap(bloom, 0, 0, 0);
            
            al_set_target_backbuffer(disp);
            al_draw_bitmap(original, 0, 0, 0);
            al_draw_bitmap(bloom, 0, 0, 0);

            al_destroy_bitmap(original);
            al_destroy_bitmap(bloom);
            
            al_use_shader(NULL);
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