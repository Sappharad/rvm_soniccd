//
//  MpvPlayer.c
//  rvm
//

#include "MpvPlayer.h"

#ifdef ENABLE_MPV

static Uint32 wakeup_on_mpv_render_update, wakeup_on_mpv_events;

static void *get_proc_address_mpv(void *ctx, const char *name)
{
    return SDL_GL_GetProcAddress(name);
}

static void on_mpv_events(void *ctx)
{
    SDL_Event event = { .type = wakeup_on_mpv_events };
    SDL_PushEvent(&event);
}

static void on_mpv_render_update(void *ctx)
{
    SDL_Event event = { .type = wakeup_on_mpv_render_update };
    SDL_PushEvent(&event);
}

void MpvPlayer(char *fileName)
{
    // Check if fileName (with extension) exists
    char fullName[1024];
    snprintf(fullName, sizeof(fullName), "%s.mp4", fileName);
    if (access(fullName, F_OK) == -1)
    {
        return;
    }

#ifdef __SWITCH__
    // On Switch, MPV audio is managed by SDL
    // As it has been initialized already, it must be deinit
    AudioPlayback_ReleaseAudioPlayback();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
#endif

    // Retrieve current SDL_Window
    SDL_Window *window = NULL;
    for (int i = 0; i < 10; i++)
    {
        window = SDL_GetWindowFromID(i);
        if (window)
        {
            break;
        }
    }

    // MPV stuff
    mpv_handle *mpv = mpv_create();
    if (!mpv)
    {
        printf("MPV create failed\n");
        return;
    }

    if (mpv_initialize(mpv) < 0)
    {
        printf("MPV init failed\n");
        return;
    }

    mpv_render_param params[] =
    {
        { MPV_RENDER_PARAM_API_TYPE, MPV_RENDER_API_TYPE_OPENGL },
        { MPV_RENDER_PARAM_OPENGL_INIT_PARAMS, &(mpv_opengl_init_params)
            {
                .get_proc_address = get_proc_address_mpv,
            }
        },
        { MPV_RENDER_PARAM_ADVANCED_CONTROL, &(int){ 1 } },
        { 0 }
    };

    mpv_render_context *mpv_gl;
    if (mpv_render_context_create(&mpv_gl, mpv, params) < 0)
    {
        printf("Failed to initialize MPV GL context\n");
        return;
    }

    wakeup_on_mpv_render_update = SDL_RegisterEvents(1);
    wakeup_on_mpv_events = SDL_RegisterEvents(1);

    if (wakeup_on_mpv_render_update == (Uint32)-1 || wakeup_on_mpv_events == (Uint32)-1)
    {
        printf("Could not register MPV events\n");
        return;
    }

    mpv_set_wakeup_callback(mpv, on_mpv_events, NULL);
    mpv_render_context_set_update_callback(mpv_gl, on_mpv_render_update, NULL);

    const char *cmd[] = { "loadfile", fullName, NULL };
    mpv_command_async(mpv, 0, cmd);

    bool stop = false;

    while (!stop)
    {
        SDL_Event event;
        if (SDL_WaitEvent(&event) != 1)
        {
            printf("MPV event loop error\n");
            return;
        }

        bool redraw = false;

        switch (event.type)
        {
            case SDL_QUIT:
            case SDL_KEYDOWN:
            case SDL_JOYBUTTONDOWN:
                stop = true;
                break;
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_EXPOSED)
                {
                    redraw = true;
                }
                break;
            default:
                if (event.type == wakeup_on_mpv_render_update)
                {
                    uint64_t flags = mpv_render_context_update(mpv_gl);
                    if (flags & MPV_RENDER_UPDATE_FRAME)
                    {
                        redraw = true;
                    }
                }

                if (event.type == wakeup_on_mpv_events)
                {
                    bool haveEvents = true;
                    while (haveEvents)
                    {
                        mpv_event *mp_event = mpv_wait_event(mpv, 0);
                        switch (mp_event->event_id)
                        {
                            case MPV_EVENT_END_FILE:
                                stop = true;
                                break;
                            case MPV_EVENT_NONE:
                                haveEvents = false;
                                break;
                        }
                        printf("MPV event: %s\n", mpv_event_name(mp_event->event_id));
                    }
                }
                break;
        }

        if (redraw)
        {
            int w, h;
            SDL_GetWindowSize(window, &w, &h);
            mpv_render_param params[] =
            {
                { MPV_RENDER_PARAM_OPENGL_FBO, &(mpv_opengl_fbo)
                    {
                        .fbo = 0,
                        .w = w,
                        .h = h,
                    }
                },
                { MPV_RENDER_PARAM_FLIP_Y, &(int){ 1 } },
                { 0 }
            };
            mpv_render_context_render(mpv_gl, params);
            SDL_GL_SwapWindow(window);
        }
    }

    mpv_render_context_free(mpv_gl);
    mpv_detach_destroy(mpv);

#ifdef __SWITCH__
    // Reinit original audio playback
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
    InitAudioPlayback();
#endif
}
#endif
