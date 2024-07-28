#include <SDL2/SDL.h>
#include <sys/mman.h>

#define internal static
#define local_persist static
#define global_variable static

#define MAX_CONTROLLERS 4
SDL_GameController* ControllerHandles[MAX_CONTROLLERS];
SDL_Haptic* RumbleHandles[MAX_CONTROLLERS];

struct sdl_offscreen_buffer
{
    SDL_Texture* Texture;
    void* Memory;
    int Width;
    int Height;
    int BytesPerPixel;
};

struct sdl_window_dimension
{
    int Width;
    int Height;
};

global_variable sdl_offscreen_buffer GlobalBackbuffer;

internal sdl_window_dimension
SDLGetWindowDimension(SDL_Window* window)
{
    sdl_window_dimension Result;
    SDL_GetWindowSize(window, &Result.Width, &Result.Height);
    return Result;
}

internal void
RenderWeirdGradient(sdl_offscreen_buffer Buffer, int XOffset, int YOffset)
{
    Uint8* Row = (Uint8*)Buffer.Memory;
    int Pitch = Buffer.Width * Buffer.BytesPerPixel;

    for (int Y = 0; Y < Buffer.Height; ++Y)
    {
        Uint32* Pixel = (Uint32*)Row;

        for (int X = 0; X < Buffer.Width; ++X)
        {
            Uint8 Blue = (Uint8)(X + XOffset);
            Uint8 Green = (Uint8)(Y + YOffset);

            *Pixel++ = ((Green << 8) | Blue);
        }

        Row += Pitch;
    }
}

internal void
SDLResizeTexture(sdl_offscreen_buffer* Buffer, SDL_Renderer* Renderer, int Width, int Height)
{
    Buffer->BytesPerPixel = 4;

    // The size of memory will be different - therefore undo all the memory stuff.
    if (Buffer->Memory != NULL)
    {
        munmap(Buffer->Memory, Buffer->Width * Buffer->Height * Buffer->BytesPerPixel);
    }

    if (Buffer->Texture != NULL)
    {
        SDL_DestroyTexture(Buffer->Texture);
    }

    Buffer->Texture = SDL_CreateTexture(Renderer,
                                        SDL_PIXELFORMAT_ARGB8888,
                                        SDL_TEXTUREACCESS_STREAMING,
                                        Width,
                                        Height);

    Buffer->Width = Width;
    Buffer->Height = Height;
    Buffer->Memory = mmap(0,
                          Width * Height * 4,
                          PROT_READ | PROT_WRITE,
                          MAP_ANONYMOUS | MAP_PRIVATE,
                          -1,
                          0);
}

internal void
SDLUpdateWindow(SDL_Window* Window, SDL_Renderer* Renderer, sdl_offscreen_buffer* Buffer)
{
    SDL_UpdateTexture(Buffer->Texture,
                      NULL,
                      Buffer->Memory,
                      Buffer->Width * Buffer->BytesPerPixel);

    SDL_RenderCopy(Renderer,
                   Buffer->Texture,
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
        case SDL_KEYDOWN:
        case SDL_KEYUP:
        {
            SDL_Keycode KeyCode = Event->key.keysym.sym;
            bool IsDown = Event->key.state == SDL_PRESSED;
            bool WasDown = false;
            if (Event->key.state == SDL_RELEASED)
            {
                WasDown = true;
            }
            else if (Event->key.repeat != 0)
            {
                WasDown = true;
            }

            if (Event->key.repeat == 0)
            {
                if(KeyCode == SDLK_w)
                {
                }
                else if(KeyCode == SDLK_a)
                {
                }
                else if(KeyCode == SDLK_s)
                {
                }
                else if(KeyCode == SDLK_d)
                {
                }
                else if(KeyCode == SDLK_q)
                {
                }
                else if(KeyCode == SDLK_e)
                {
                }
                else if(KeyCode == SDLK_UP)
                {
                }
                else if(KeyCode == SDLK_LEFT)
                {
                }
                else if(KeyCode == SDLK_DOWN)
                {
                }
                else if(KeyCode == SDLK_RIGHT)
                {
                }
                else if(KeyCode == SDLK_ESCAPE)
                {
                    printf("ESCAPE: ");
                    if(IsDown)
                    {
                        printf("IsDown ");
                    }
                    if(WasDown)
                    {
                        printf("WasDown");
                    }
                    printf("\n");
                }
                else if(KeyCode == SDLK_SPACE)
                {
                }
            }

        } break;

        case SDL_QUIT:
        {
            ShouldQuit = true;
        } break;

        case SDL_WINDOWEVENT:
        {
            Uint8 WindowEvent = Event->window.event;
            switch (WindowEvent)
            {
                case SDL_WINDOWEVENT_EXPOSED:
                {
                    SDL_Window* Window = SDL_GetWindowFromID(Event->window.windowID);
                    SDL_Renderer* Renderer = SDL_GetRenderer(Window);
                    SDLUpdateWindow(Window, Renderer, &GlobalBackbuffer);
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

internal void SDLCloseGameControllers(void)
{
    for (int ControllerIndex = 0; ControllerIndex < MAX_CONTROLLERS; ++ControllerIndex)
    {
        if (ControllerHandles[ControllerIndex])
        {
            if (RumbleHandles[ControllerIndex])
            {
                SDL_HapticClose(RumbleHandles[ControllerIndex]);
                SDL_GameControllerClose(ControllerHandles[ControllerIndex]);
            }
        }
    }
}

int main(int argc, char **argv)
{

    if (SDL_Init(SDL_INIT_VIDEO |
                 SDL_INIT_GAMECONTROLLER |
                 SDL_INIT_HAPTIC) < 0)
    {
        // TODO: SDL Initialisation failed.
        return 1;
    }

    int MaxJoysticks = MAX_CONTROLLERS;
    int ControllerIndex = 0;
    for (int JoystickIndex = 0; JoystickIndex < MaxJoysticks; ++JoystickIndex)
    {
        if (!SDL_IsGameController(JoystickIndex))
        {
            continue;
        }

        if (ControllerIndex >= MAX_CONTROLLERS)
        {
            break;
        }

        ControllerHandles[ControllerIndex] = SDL_GameControllerOpen(JoystickIndex);
        RumbleHandles[ControllerIndex] = SDL_HapticOpen(JoystickIndex);
        if (RumbleHandles[ControllerIndex] && SDL_HapticRumbleInit(RumbleHandles[ControllerIndex]) != 0)
        {
            SDL_HapticClose(RumbleHandles[ControllerIndex]);
            RumbleHandles[ControllerIndex] = 0;
        }

        ControllerIndex++;
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

    sdl_window_dimension WindowDimension = SDLGetWindowDimension(Window);
    SDLResizeTexture(&GlobalBackbuffer, Renderer, WindowDimension.Width, WindowDimension.Height);

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

        for (int ControllerIndex = 0; ControllerIndex < MAX_CONTROLLERS; ++ControllerIndex)
        {
            if (ControllerHandles[ControllerIndex] != 0 && SDL_GameControllerGetAttached(ControllerHandles[ControllerIndex]))
            {
                bool Up = SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_DPAD_UP);
                bool Down = SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_DPAD_DOWN);
                bool Left = SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_DPAD_LEFT);
                bool Right = SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
                bool Start = SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_START);
                bool Back = SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_BACK);
                bool LeftShoulder = SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
                bool RightShoulder = SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
                bool AButton = SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_A);
                bool BButton = SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_B);
                bool XButton = SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_X);
                bool YButton = SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_Y);

                Uint16 StickX = SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_LEFTX);
                Uint16 StickY = SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_LEFTY);
            }
            else
            {
                // Note: This controller is not plugged in.
            }
        }

        RenderWeirdGradient(GlobalBackbuffer, XOffset, YOffset);
        ++XOffset;
        YOffset += 2;

        SDLUpdateWindow(Window, Renderer, &GlobalBackbuffer);
    }

    SDLCloseGameControllers();
    SDL_Quit();
    return 0;

}
