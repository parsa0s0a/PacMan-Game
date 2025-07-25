#include "raylib.h"
void draw_difficality_opt(int n,int opt_num){
    int height = 98;
    int width = 300;
    int mid_height = 50;
    int x = 600-width/2;
    int y = height*(n-1)+mid_height*n;
    //if we are on an option it shoulb be blue
    if(n==opt_num){
    DrawRectangle(x,y,width,height,BLUE);
    }
    else{
    DrawRectangle(x,y,width,height,RED);
    }

    //each option has different text
    switch (n)
    {
    case 1:
        DrawText("Easy",x+width/5+40,y+height/4+5,39,BLACK);
        break;
    case 2:
        DrawText("Medium",x+width/5+30,y+height/4+5,39,BLACK);
        break;
    case 3:
        DrawText("Hard",x+width/5+40,y+height/4+5,39,BLACK);
        break;
    }
}
int draw_difficality_menu(){
    Image poster_img = LoadImage("images\\poster.png"); // Load image data into CPU memory (RAM)
    ImageResize(&poster_img,300,400);
    Texture2D poster = LoadTextureFromImage(poster_img); // Image converted to texture, GPU memory (RAM -> VRAM)
    UnloadImage(poster_img);
    Sound tap = LoadSound("sounds\\tap.wav");
    int opt_num = 1;
    int DifficalityIsOpen = 1; //handling closing with exit option
    while (!WindowShouldClose()&&DifficalityIsOpen){

        BeginDrawing();
        ClearBackground(BLACK);
        for(int i=1;i<=3;i++){
            draw_difficality_opt(i,opt_num);
        }
        DrawTexture(poster,450,525,WHITE);
        EndDrawing();

        if (IsKeyPressed(KEY_UP)) { 
            //opt_num should not be less than 1
            if(opt_num>1){
                PlaySound(tap);
                opt_num--;
            }
        } 
        else if (IsKeyPressed(KEY_DOWN)) { 
            //opt_num should not be more than 3
            if(opt_num<3){
                PlaySound(tap);
                opt_num++;
            }
        } 
        //choosing menu option
        else if (IsKeyPressed(KEY_ENTER)) { 
            //first menu option (new game)
            if(opt_num==1){
                return 1;
            }
            if(opt_num==2){
                return 2;
            }
            //first menu option(exit)
            if(opt_num==3){
                return 3;
            }
        }
    }
    UnloadSound(tap);
}