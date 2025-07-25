#include <stdio.h>
#include "raylib.h"
#include <menu.h>
 
int main()
{
    const int screenWidth = 1200;
    const int screenHeight = 1000;
    InitWindow(screenWidth, screenHeight, "pacman");
    InitAudioDevice();      // Initialize audio device
    SetTargetFPS(30);
    drawmenu();
    CloseAudioDevice();     // Close audio device
    CloseWindow();
    return 0;
}

