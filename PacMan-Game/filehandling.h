#ifndef FILEHANDLING.H
#define FILEHANDLING.H
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <time.h> 
#include "raylib.h"
#define MAX_PLAYERS 100
#define MAX_NAME_LENGTH 12
#define MAX_DATE_LENGTH 100

typedef struct { 
    char name[MAX_NAME_LENGTH]; 
    int score; 
    char date[MAX_DATE_LENGTH]
}Player; 
void readScores(Player players[], int *count) { 
        FILE *file = fopen("scores.txt", "r"); 
        if (file == NULL) { 
            *count = 0; 
            return; 
        } 
        *count = 0; 
        while (fscanf(file, "%s %d %s", players[*count].name, &players[*count].score,players[*count].date) == 3) { 
            (*count)++; 
        } 
        fclose(file); 
} 
void writeScores(Player players[], int count) { 
    FILE *file = fopen("scores.txt", "w"); 
    for (int i = 0; i < count; i++) { 
        fprintf(file, "%s %d %s\n", players[i].name, players[i].score,players[i].date); 
    } 
    fclose(file); 
} 
void addOrUpdateScore(Player players[], int *count, const char *name, int score) { 
    int found = 0; 
    for (int i = 0; i < *count; i++) { 
        if (strcmp(players[i].name, name) == 0) { 
            if (score > players[i].score) { 
                players[i].score = score; 
                time_t now = time(NULL); 
                strftime(players[i].date, MAX_DATE_LENGTH, "%Y-%m-%d-%H:%M", localtime(&now));
            } 
            found = 1; 
            break; 
        } 
    } 
    if (!found) {
        if (*count == MAX_PLAYERS) { 
            int minScore = players[0].score; 
            int minIndex = 0; 
            for (int i = 1; i < *count; i++) { 
                if (players[i].score < minScore) { 
                    minScore = players[i].score; 
                    minIndex = i; 
                } 
            } 
            for (int i = minIndex; i < *count - 1; i++) {
                players[i] = players[i + 1]; 
            } 
            (*count)--; 
        }
        strcpy(players[*count].name, name); 
        players[*count].score = score; 
        time_t now = time(NULL); 
        strftime(players[*count].date, MAX_DATE_LENGTH, "%Y-%m-%d-%H:%M", localtime(&now));
        (*count)++; 
    } 
} 
int comparePlayers(const void *a, const void *b) { 
    Player *playerA = (Player *)a; 
    Player *playerB = (Player *)b; 
    return playerB->score - playerA->score;
}

void add_new_score(char name[MAX_NAME_LENGTH],int score){
    Player players[MAX_PLAYERS];
    int count;
    readScores(players, &count);
    addOrUpdateScore(players, &count, name, score);
    qsort(players, count, sizeof(Player), comparePlayers);
    writeScores(players, count);
    return;
}

void drawhighscore(){
    Player players[MAX_PLAYERS];
    int count;
    readScores(players, &count);
    char high_score_text[10000];
    snprintf(high_score_text, 9999, "%d", players[0].score);
    DrawText("HIGH SCORE",250,5,35,WHITE);
    DrawText(high_score_text,325,40,35,WHITE);
}

void top_10(){
    Player players[MAX_PLAYERS];
    int count;
    readScores(players, &count);
    char score_text[10000];
    int ScoreIsOpen = 1;
    while(!WindowShouldClose()&&ScoreIsOpen){
        BeginDrawing();
        ClearBackground(BLACK);
        if(count!=0){
            for(int i=0;i<count;i++){
                snprintf(score_text, 9999, "%d", players[i].score);
                DrawRectangleLines(90,90+i*80,570,50,RED);
                DrawRectangleLines(89,89+i*80,572,52,RED);
                DrawText(players[i].name,100,100+i*80,30,RED);
                DrawText(score_text,300,100+i*80,30,RED);
                DrawText(players[i].date,400,100+i*80,30,RED);

            }
            DrawText("click BackSpace to exit",100,100+count*80,30,RED);
        }
        else{
            DrawText("There are no scores",100,100,30,RED);
            DrawText("click BackSpace to exit",100,180,30,RED);
        }
        EndDrawing();
        if (IsKeyPressed(KEY_BACKSPACE)) { 
        ScoreIsOpen = 0;
        }
    }
    return;
}
#endif // GAMEOVER_H
