#include <cstdint>
#include "handmade.h"

internal void
RenderWeirdGradient(game_offscreen_buffer* Buffer, int XOffset, int YOffset)
{
    uint8_t* Row = (uint8_t*)Buffer->Memory;

    for (int Y = 0; Y < Buffer->Height; ++Y)
    {
        uint32_t* Pixel = (uint32_t*)Row;

        for (int X = 0; X < Buffer->Width; ++X)
        {
            uint8_t Blue = (uint8_t)(X + XOffset);
            uint8_t Green = (uint8_t)(Y + YOffset);

            *Pixel++ = ((Green << 8) | Blue);
        }

        Row += Buffer->Pitch;
    }
}

internal void GameUpdateAndRender(game_offscreen_buffer* Buffer)
{
    int BlueOffset = 0;
    int GreenOffset = 0;
    RenderWeirdGradient(Buffer, BlueOffset, GreenOffset);
}