#include <SDL2/SDL.h>

#define internal static
#define local_persist static
#define global_variable static

global_variable SDL_Texture* Texture;
global_variable void* BitmapMemory;
global_variable int BitmapWidth;
global_variable int BytesPerPixel = 4;

internal void
SDLResizeTexture(SDL_Renderer* Renderer, int Width, int Height)
{
    if (BitmapMemory != NULL)
    {
        free(BitmapMemory);
    }

    if (Texture != NULL)
    {
        SDL_DestroyTexture(Texture);
    }

    Texture = SDL_CreateTexture(Renderer,
                                SDL_PIXELFORMAT_ARGB8888,
                                SDL_TEXTUREACCESS_STREAMING,
                                Width,
                                Height);

    BitmapWidth = Width;
    BitmapMemory = malloc(Width * Height * BytesPerPixel);
}

internal void
SDLUpdateWindow(SDL_Window* Window, SDL_Renderer* Renderer)
{
    SDL_UpdateTexture(Texture,
                      NULL,
                      BitmapMemory,
                      BitmapWidth * BytesPerPixel);

    SDL_RenderCopy(Renderer,
                   Texture,
                   NULL,
                   NULL);

    SDL_RenderPresent(Renderer);
}

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
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                {
                    SDL_Window* Window = SDL_GetWindowFromID(Event->window.windowID);
                    SDL_Renderer* Renderer = SDL_GetRenderer(Window);
                    printf("SDL_WINDOWEVENT_SIZE_CHANGED (%d, %d)\n", Event->window.data1, Event->window.data2);
                    SDLResizeTexture(Renderer, Event->window.data1, Event->window.data2);
                } break;

                case SDL_WINDOWEVENT_EXPOSED:
                {
                    SDL_Window* Window = SDL_GetWindowFromID(Event->window.windowID);
                    SDL_Renderer* Renderer = SDL_GetRenderer(Window);
                    SDLUpdateWindow(Window, Renderer);
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

    SDL_Quit();
    return 0;

}
