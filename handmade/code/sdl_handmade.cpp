#include <SDL2/SDL.h>

bool HandleEvent(SDL_Event* Event)
{
    bool ShouldQuit = false;
    uint32_t EventType = Event->type;

    switch (EventType)
    {
        case SDL_QUIT:
        {
            ShouldQuit = true;
        } break;

        case SDL_WINDOWEVENT:
        {
            Uint8 WindowEvent = Event->window.event;
            switch (WindowEvent)
            {
                case SDL_WINDOWEVENT_RESIZED:
                {
                    printf("Window has been resized (%d, %d)\n", Event->window.data1, Event->window.data2);
                } break;

                case SDL_WINDOWEVENT_EXPOSED:
                {
                    SDL_Window* Window = SDL_GetWindowFromID(Event->window.windowID);
                    SDL_Renderer* Renderer = SDL_GetRenderer(Window);
                    static bool IsWhite = true;

                    if (IsWhite)
                    {
                        SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
                        IsWhite = false;
                    }
                    else
                    {
                        SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
                        IsWhite = true;
                    }

                    SDL_RenderClear(Renderer);
                    SDL_RenderPresent(Renderer);
                } break;

                default:
                {
                } break;

            } break;
        }

        default:
        {
        } break;
    }

    return ShouldQuit;
}

int main(int argc, char **argv)
{

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        // TODO: SDL Initialisation failed.
        return 1;
    }

    SDL_Window* Window;
    Window = SDL_CreateWindow("Handmade Hero",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              640,
                              480,
                              SDL_WINDOW_RESIZABLE);

    if (Window == NULL)
    {
        // TODO: Handle error.
        return 1;
    }

    SDL_Renderer* Renderer;
    Renderer = SDL_CreateRenderer(Window,
                                  -1,
                                  0);

    if (Renderer == NULL)
    {
        // TODO: Handle error.
        return 1;
    }

    for (;;)
    {
        SDL_Event Event;
        SDL_WaitEvent(&Event);
        if (HandleEvent(&Event))
        {
            break;
        }

    }

    return 0;

}
