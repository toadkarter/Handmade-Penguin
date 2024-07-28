#include <SDL2/SDL.h>
#include <sys/mman.h>

#define internal static
#define local_persist static
#define global_variable static

global_variable SDL_Texture* Texture;
global_variable void* BitmapMemory;
global_variable int BitmapWidth;
global_variable int BitmapHeight;
global_variable int BytesPerPixel = 4;

internal void
RenderWeirdGradient(int XOffset, int YOffset)
{
    Uint8* Row = (Uint8*)BitmapMemory;
    int Pitch = BitmapWidth* BytesPerPixel;

    for (int Y = 0; Y < BitmapHeight; ++Y)
    {
        Uint8* Pixel = (Uint8*)Row;

        for (int X = 0; X < BitmapWidth; ++X)
        {
            *Pixel = (Uint8)(X + XOffset);
            ++Pixel;
            *Pixel = (Uint8)(Y + YOffset);
            ++Pixel;
            *Pixel = 0;
            ++Pixel;
            *Pixel = 0;
            ++Pixel;
        }

        Row += Pitch;
    }
}

internal void
SDLResizeTexture(SDL_Renderer* Renderer, int Width, int Height)
{
    // The size of memory will be different - therefore undo all the memory stuff.
    if (BitmapMemory != NULL)
    {
        munmap(BitmapMemory, BitmapWidth * BitmapHeight * BytesPerPixel);
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
    BitmapHeight = Height;
    BitmapMemory = mmap(0,
                        Width * Height * 4,
                        PROT_READ | PROT_WRITE,
                        MAP_ANONYMOUS | MAP_PRIVATE,
                        -1,
                        0);
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

    int Width, Height;
    SDL_GetWindowSize(Window, &Width, &Height);
    SDLResizeTexture(Renderer, Width, Height);

    bool Running = true;
    int XOffset = 0;
    int YOffset = 0;

    while (Running)
    {
        SDL_Event Event;
        while (SDL_PollEvent(&Event))
        {
            if (HandleEvent(&Event))
            {
                Running = false;
            }
        }

        RenderWeirdGradient(XOffset, YOffset);
        ++XOffset;
        YOffset += 2;

        SDLUpdateWindow(Window, Renderer);
    }

    SDL_Quit();
    return 0;

}
