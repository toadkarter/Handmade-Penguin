#ifndef HANDMADE_H
#define HANDMADE_H

#define internal static
#define local_persist static
#define global_variable static

struct game_offscreen_buffer
{
    void* Memory;
    int Width;
    int Height;
    int Pitch;
};

internal void
GameUpdateAndRender(game_offscreen_buffer* Buffer);

#endif