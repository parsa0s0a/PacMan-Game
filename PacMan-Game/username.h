#include <stdio.h>
#include "raylib.h"
#include <game.h>
#include "gameover.h"
#include "difficality.h"
#include "filehandling.h"
#define MAX_INPUT_CHARS 10
extern char name[MAX_INPUT_CHARS + 1] = "\0";      // NOTE: One extra space required for null terminator char '\0'
int letterCount = 0;

void drawusername_opt(int n,int opt_num,Sound type){
    if(n==1){
            int height = 70;
            int width = 300;
            int first_up_height = 100;
            int mid_height = 50;
            int x = 600-width/2;
            int y = height*(n-1)+mid_height*(n-1)+first_up_height;
            //if we are on an option it shoulb be blue
            DrawRectangle(x,y,width,height,YELLOW);
            DrawText(name,x+width/5-5,y+height/4,35,BLACK);
            if(n==opt_num){
                int rectanglelinesNumber = 10;
                for(int i=0;i<rectanglelinesNumber;i++){
                DrawRectangleLines(x-i,y-i,width+2*i,height+2*i,BLUE);
                }
            }
            // Get char pressed (unicode character) on the queue
            int key = GetCharPressed();
            if(opt_num==1){
            // Check if more characters have been pressed on the same frame
            while (key > 0){
                // NOTE: Only allow keys in range [32..125]
                PlaySound(type);
                if ((key >= 32) && (key <= 125) && (letterCount < MAX_INPUT_CHARS))
                {
                    name[letterCount] = (char)key;
                    name[letterCount+1] = '\0'; // Add null terminator at the end of the string.
                    letterCount++;
                }
                key = GetCharPressed();  // Check next character in the queue
            }
            if (IsKeyPressed(KEY_BACKSPACE))
            {
            PlaySound(type);
            letterCount--;
                if (letterCount < 0) letterCount = 0;
                name[letterCount] = '\0';
            }
        }
    }
    else{
        int height = 70;
        int width = 300;
        int first_up_height = 100;
        int mid_height = 50;
        int x = 600-width/2;
        int y = height*(n-1)+mid_height*(n-1)+first_up_height;
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
        case 2:
            DrawText("Start Game",x+width/5-5,y+height/4,35,BLACK);
            break;
        case 3:
            DrawText("Quit to main menu",x+width/5-35,y+height/4,30,BLACK);
            break;
        }
    }
}

void DrawUserNameMenu(){
    Image poster_img = LoadImage("images\\poster.png"); // Load image data into CPU memory (RAM)
    ImageResize(&poster_img,300,400);
    Texture2D poster = LoadTextureFromImage(poster_img); // Image converted to texture, GPU memory (RAM -> VRAM)
    UnloadImage(poster_img);
    Sound tap = LoadSound("sounds\\tap.wav");
    Sound type = LoadSound("sounds\\type.wav");
    int opt_num = 1;
    int UserNameMenuIsOpen = 1; //handling closing with exit option
    int warning = 0;
    while (!WindowShouldClose()&&UserNameMenuIsOpen){
        BeginDrawing();
        ClearBackground(BLACK);
        for(int i=1;i<=3;i++){
            drawusername_opt(i,opt_num,type);
        }
        if(warning)
            DrawText("please enter your username!!!",800,175,26,RED);
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
        //choosing username menu option
        else if (IsKeyPressed(KEY_ENTER)) { 
            //second UserNameMenu option (start game)
            if(opt_num==2&&strlen(name)==0){
                warning = 1;
            }
            else if(opt_num==2&&strlen(name)!=0){
                opt_num = 1;
                warning = 0;
                int restart;
                do
                {
                    int level =  draw_difficality_menu();
                    int score = GameLoop(name,level);
                    add_new_score(name,score);
                    restart = gameover_menu(name,score);
                } while (restart==1);
                
                name[0]='\0';
                letterCount = 0;
            }
            //third menu option(exit)
            else if(opt_num==3){
                name[0]='\0';
                letterCount = 0;
                UserNameMenuIsOpen = 0;
                opt_num = 1;

            }
        }
    }
    UnloadSound(tap);
    UnloadSound(type);
}
