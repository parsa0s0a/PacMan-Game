#ifndef GAMEOVER_H
#define GAMEOVER_H

#include "raylib.h"
int gameover_menu(char name[],int score){
        Sound tap = LoadSound("sounds\\tap.wav");
        char score_text[10000];
        snprintf(score_text, 9999, "%d", score);
        int opt_num = 1;
        int height = 70;
        int width = 300;
        int first_up_height = 150;
        int mid_height = 50;
        int x = 600-width/2;
        int y1 = first_up_height+100;
        int y2 = height+mid_height+first_up_height+100;
        int GameOverMenuIsOpen = 1; //handling closing with exit option
        while (!WindowShouldClose()&&GameOverMenuIsOpen)
        {
                BeginDrawing();
                ClearBackground(BLACK);
                //draw box
                for(int i=0;i<4;i++){
                DrawRectangleLines(350-i,100-i,550+2*i,50+2*i,RED);
                }
                //draw name and score in the box
                DrawText(name,380,110,30,RED);
                DrawText(score_text,800,110,30,RED);
                if(opt_num==1){
                        DrawRectangle(x,y1,width,height,BLUE);
                        DrawRectangle(x,y2,width,height,RED);
                }
                else if(opt_num==2){
                        DrawRectangle(x,y1,width,height,RED);
                        DrawRectangle(x,y2,width,height,BLUE);
                }
                DrawText("Play agian",520,y1+20,30,BLACK);
                DrawText("Return",540,y2+20,30,BLACK);
                EndDrawing();
                if (IsKeyPressed(KEY_UP)) { 
                        //opt_num should not be less than 1
                        if(opt_num>1){
                                PlaySound(tap);
                                opt_num--;
                        }
                }
                else if (IsKeyPressed(KEY_DOWN)) { 
                        //opt_num should not be more than 2
                        if(opt_num<2){
                                PlaySound(tap);
                                opt_num++;
                        }
                }
                
                else if (IsKeyPressed(KEY_ENTER)) { 
                        //second UserNameMenu option (start game)
                        if(opt_num==1){
                                return 1;
                        }
                        //third menu option(exit)
                        if(opt_num==2){
                                GameOverMenuIsOpen = 0;
                                return 0;
                        }
                
                }        
        }
        UnloadSound(tap);
}
#endif // GAMEOVER_H
