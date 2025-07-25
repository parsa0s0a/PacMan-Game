#include "raylib.h"
#include <map.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <gameover.h>
#include <filehandling.h>
#define block 40 // object sizes are block
#define telorance 10

int pacman_moving_speed = block / 5;
int ghost_moving_speed = block / 5;

int pacman_is_catcher = 0;
int star_score = 10;
int score;
int should_wait;
int cnt;

struct object
{
    int health;
    int x;
    int y;
    Texture2D alltexture[5]; // 0 : right 1 : up 2 : left 3 : down 4 : otherframe
    int direction;           // 0 : right 1 : up 2 : left 3 : down
    int prev_direction;      //used only in ghosts
    int eaten_counter;
};

struct object create_object(int health, int x, int y, char *pic_ad[5])
{
    struct object object;
    object.x = x;
    object.y = y;
    object.health = health;
    for (int i = 0; i < 5; i++)
    {
        Image image = LoadImage(pic_ad[i]); // Load image data into CPU memory (RAM)
        ImageResize(&image, (block - 8), (block - 8));
        Texture2D texture = LoadTextureFromImage(image); // Image converted to texture, GPU memory (RAM -> VRAM)
        object.alltexture[i] = texture;
        UnloadImage(image);
    }
    object.direction = 0;
    return object;
}

int check_collision(struct object object1, struct object object2)
{
    // object 1 is pacman
    if (object2.health != 0)
    {
        if (object1.x < object2.x + block / 2 && object2.x + block / 2 < object1.x + block)
        {
            if (object1.y < object2.y + block / 2 && object2.y + block / 2 < object1.y + block)
            {
                return 1;
            }
        }
    }
    return 0;
}

int all_stars_are_collected(struct object stars[10])
{
    for (int i = 0; i < 10; i++)
    {
        if (stars[i].health != 0)
        {
            return 0;
        }
    }
    return 1;
}

void generate_random_xy_on_map(int *x, int *y)
{
    srand(clock());
    do
    {
        // Generate random values between 4 and 23 (inclusive) because 3 and 24 are wall
        *x = rand() % 20 + 4;
        *y = rand() % 20 + 4;
    } while (map[*y][*x] == 1 || (*y != 14 && 12 <= *x && *x <= 17));
}

void regerate_stars(struct object stars[10])
{
    stars[0].eaten_counter++;
    int new_x_star, new_y_star;
    for (int i = 0; i < 10; i++)
    {
        generate_random_xy_on_map(&new_x_star, &new_y_star);
        stars[i].health = 1;
        stars[i].x = new_x_star * block;
        stars[i].y = new_y_star * block;
        WaitTime(0.001);
    }
}

void correct_pacman_position(struct object *object,int new_direction){
    if(abs(object->direction-new_direction) != 2){
        if(new_direction%2==0){
        object->y = ((object->y+block/2)/block)*block;
        }
        else{
        object->x = ((object->x+block/2)/block)*block;
        }
    }
}

struct object update_pacman(struct object object, struct object stars[10], struct object ghosts[5],
                            struct object *apple, struct object *poison, struct object *pepper,struct object *cherry,
                            Sound sstar,Sound slosing ,Sound sapple,Sound spoison,Sound scherry,Sound spepper,int level)
{
    //*****object is pacman******//

    // check collision with stars
    for (int i = 0; i < 10; i++)
    {
        if (check_collision(object, stars[i]))
        {
            PlaySound(sstar);
            stars[i].health = 0;
            score += star_score * level;
            break;
        }
    }
    if (all_stars_are_collected(stars))
    {
        regerate_stars(stars);
    }

    // check collision with ghost
    for (int i = 0; i < 4 + level; i++)
    {
        if (check_collision(object, ghosts[i]))
        {
            if(!pacman_is_catcher){
                PlaySound(slosing);
                WaitTime(2);
                should_wait = 1;
                cnt = 0;
                object.health--;
                object.x = 14 * block;
                object.y = 16 * block;
                object.direction = 0;
                // replace ghosts
                for (int j = 0; j < 4 + level; j++)
                {
                    ghosts[j].x = (12 + j) * block;
                    ghosts[j].y = 12 * block;
                }
                // replace stars
                regerate_stars(stars);

                // replace apple
                apple->eaten_counter=0;
                apple->health=0;
                int new_apple_x, new_apple_y;
                generate_random_xy_on_map(&new_apple_x, &new_apple_y);
                WaitTime(0.02);
                apple->x = new_apple_x * block;
                apple->y = new_apple_y * block;

                // replace poison
                poison->health=0;
                poison->eaten_counter = 0;
                int new_poison_x, new_poison_y;
                generate_random_xy_on_map(&new_poison_x, &new_poison_y);
                WaitTime(0.02);
                poison->x = new_poison_x * block;
                poison->y = new_poison_y * block;

                // replace pepper
                pepper->eaten_counter=0;
                pepper->health=0;
                pacman_moving_speed = block / 5;
                int new_pepper_x, new_pepper_y;
                generate_random_xy_on_map(&new_pepper_x, &new_pepper_y);
                WaitTime(0.02);
                pepper->x = new_pepper_x * block;
                pepper->y = new_pepper_y * block;

                // replace cherry
                cherry->eaten_counter=0;
                cherry->health=0;
                int new_cherry_x, new_cherry_y;
                generate_random_xy_on_map(&new_cherry_x, &new_cherry_y);
                WaitTime(0.02);
                cherry->x = new_cherry_x * block;
                cherry->y = new_cherry_y * block;
                break;
            }
            if(pacman_is_catcher){
                PlaySound(sstar);
                //send ghosts[i] to the starting point
                int new_ghost_x,new_ghost_y;
                generate_random_xy_on_map(&new_ghost_x,&new_ghost_y);
                WaitTime(0.02);
                ghosts[i].x = new_ghost_x*block;
                ghosts[i].y = new_ghost_y*block;
                score+=25*level;
            }
        }
    }

    // check collision with apple
    if (check_collision(object, *apple))
    {
        PlaySound(sapple);
        score += 15*level;
        apple->health = 0;
        if (object.health < 3)
            object.health++;
    }
    // check collision with poison
    if (check_collision(object, *poison))
    {
        PlaySound(spoison);
        score -= 10*level;
        poison->health = 0;
        poison->eaten_counter = 0;
        object.health--;
    }

    // check collision with pepper
    if (check_collision(object, *pepper))
    {
        PlaySound(spepper);
        score += 20*level;
        pepper->health = 0;
        pacman_moving_speed = block / 3;
    }

    // check collision with cherry
    if (check_collision(object, *cherry))
    {
        PlaySound(scherry);
        cherry->health = 0;
        cherry->eaten_counter=0;
        pacman_is_catcher = 1;
    }

    // moving the pacman
    if (object.direction == 0)
    {
        int nextX = object.x + block + block / 10;
        int nextYUpper = (object.y + telorance) / block;
        int nextYLower = (object.y + block - telorance) / block;
        int nextXBlock = nextX / block;

        bool withinHorizontalBounds = (object.x + block < 1200 ||
                                       (13 * block < object.y + telorance && object.y + block - telorance < 15 * block && object.x < 1200));

        bool isNextXBlockPassable = (map[nextYUpper][nextXBlock] != 1 || nextXBlock == 30) &&
                                    (map[nextYLower][nextXBlock] != 1 || nextXBlock == 30);

        if (withinHorizontalBounds && isNextXBlockPassable)
        {
            object.x += pacman_moving_speed;

            if (object.x >= 1200)
            {
                object.x = 0;
            }
        }
    }
    else if (object.direction == 1)
    {
        if (object.y > 0 && map[(object.y - block / 10) / block][(object.x + telorance) / block] != 1 && map[(object.y - block / 10) / block][(object.x + block - telorance) / block] != 1)
        {
            object.y -= pacman_moving_speed;
        }
    }
    else if (object.direction == 2)
    {
        int nextX = object.x - block / 10;
        int nextYUpper = (object.y + telorance) / block;
        int nextYLower = (object.y + block - telorance) / block;
        int nextXBlock = nextX / block;

        bool withinHorizontalBounds = (object.x > 0 ||
                                       (13 * block < object.y + telorance && object.y + block - telorance < 15 * block && object.x + block > 0));

        bool isNextXBlockPassable = (map[nextYUpper][nextXBlock] != 1 || nextXBlock == -1) &&
                                    (map[nextYLower][nextXBlock] != 1 || nextXBlock == -1);

        if (withinHorizontalBounds && isNextXBlockPassable)
        {
            object.x -= pacman_moving_speed;

            if (object.x + block <= 0)
            {
                object.x = 1200;
            }
        }
    }
    else if (object.direction == 3)
    {
        if (object.y + block < 1000 && map[(object.y + block + block / 10) / block][(object.x + telorance) / block] != 1 && map[(object.y + block + block / 10) / block][(object.x + block - telorance) / block] != 1)
        {
            object.y += pacman_moving_speed;
        }
    }

    

    // next direction of pacman
    if (IsKeyPressed(KEY_RIGHT))
    {
        correct_pacman_position(&object,0);
        object.direction = 0;
    }
    if (IsKeyPressed(KEY_UP))
    {
        correct_pacman_position(&object,1);
        object.direction = 1;
    }
    if (IsKeyPressed(KEY_LEFT))
    {
        correct_pacman_position(&object,2);
        object.direction = 2;
    }

    if (IsKeyPressed(KEY_DOWN))
    {
        correct_pacman_position(&object,3);
        object.direction = 3;
    }

    return object;
}

void drawpacman(struct object pacman, int cnt)
{
    // draw pacman
    if (cnt % 7 != 6 && cnt % 7 != 5)
    { // this is set so that the face will change twice each 7 farmes so 8 times in a second ولی چون دو فریم پشت هم هستن بنظر یه بار ولی طولانی تر بنظر میاد
        // draw pacman
        DrawTexture(pacman.alltexture[pacman.direction], pacman.x, pacman.y, WHITE);
    }
    else
    {
        // draw pacman
        DrawTexture(pacman.alltexture[4], pacman.x, pacman.y, WHITE);
    }
}

void drawstars(struct object stars[10], int cnt)
{
    // draw stars
    if (cnt % 10 < 5)
    {
        // draw stars
        for (int i = 0; i < 10; i++)
        {
            if (stars[i].health != 0)
            {
                    DrawTexture(stars[i].alltexture[0], stars[i].x, stars[i].y + cnt % 10, WHITE);
            }
        }
    }
    else
    {
        // draw stars
        for (int i = 0; i < 10; i++)
        {
            if (stars[i].health != 0)
            {
                DrawTexture(stars[i].alltexture[1], stars[i].x, stars[i].y + 10 - cnt % 10, BLUE);
            }
        }
    }
}

void drawghosts(struct object ghosts[5], int cnt, int level)
{
    for (int i = 0; i < 4 + level; i++)
    {
        if (cnt % 8 == 0 || cnt % 8 == 1)
        {
            DrawTexture(pacman_is_catcher?ghosts[i].alltexture[4]:ghosts[i].alltexture[ghosts[i].direction], ghosts[i].x, ghosts[i].y - 1, WHITE);
        }
        else if (cnt % 8 == 4 || cnt % 8 == 5)
        {
            DrawTexture(pacman_is_catcher?ghosts[i].alltexture[4]:ghosts[i].alltexture[ghosts[i].direction], ghosts[i].x, ghosts[i].y + 1, WHITE);
        }
        else
        {
            DrawTexture(pacman_is_catcher?ghosts[i].alltexture[4]:ghosts[i].alltexture[ghosts[i].direction], ghosts[i].x, ghosts[i].y, WHITE);
        }
    }
}

void drawfruit(struct object fruit, int cnt)
{
    // draw fruit

    if ((cnt + 5) % 10 < 5)
    {
        if (fruit.health != 0)
        {

            DrawTexture(fruit.alltexture[0], fruit.x, fruit.y + cnt % 10, WHITE);
        }
    }
    else
    {
        if (fruit.health != 0)
        {
            DrawTexture(fruit.alltexture[0], fruit.x, fruit.y + 10 - cnt % 10, WHITE);
        }
    }
}

void update_apple(struct object *apple, int pacman_health)
{
    if (pacman_health == 3)
    {
        apple->health = 0;
    }
    if (apple->health == 0 && pacman_health != 3)
    {
        apple->eaten_counter++;
    }
    if (apple->eaten_counter == 90)
    {
        apple->eaten_counter = 0;
        apple->health = 1;
        int new_apple_x, new_apple_y;
        generate_random_xy_on_map(&new_apple_x, &new_apple_y);
        apple->x = new_apple_x * block;
        apple->y = new_apple_y * block;
    }
}

void update_poison(struct object *poison)
{
    if (poison->health == 1)
    {
        if (poison->eaten_counter == 120)
        { // stay for 120 farames(4 seconds)
            poison->health = 0;
            poison->eaten_counter = 0;
        }
        poison->eaten_counter++;
    }
    else if (poison->health == 0)
    {
        if (poison->eaten_counter == 60)
        { // disappear for 60 frames (2 seconds)
            poison->health = 1;
            poison->eaten_counter = 0;
            int new_poison_x, new_poison_y;
            generate_random_xy_on_map(&new_poison_x, &new_poison_y);
            poison->x = new_poison_x * block;
            poison->y = new_poison_y * block;
        }
        poison->eaten_counter++;
    }
}

void update_pepper(struct object *pepper)
{
    if (pepper->health == 0)
    {
        pepper->eaten_counter++;
    }
    if (pepper->health == 0 && pepper->eaten_counter == 90)
    {
        pacman_moving_speed = block / 5;
    }
    if (pepper->health == 0 && pepper->eaten_counter == 150)
    {
        pepper->health = 1;
        pepper->eaten_counter = 0;
        int new_pepper_x, new_pepper_y;
        generate_random_xy_on_map(&new_pepper_x, &new_pepper_y);
        pepper->x = new_pepper_x * block;
        pepper->y = new_pepper_y * block;
    }
}

void update_cherry(struct object *cherry)
{
    if (cherry->health == 1)
    {
        if (cherry->eaten_counter == 120)
        { // stay for 120 farames(4 seconds)
            cherry->health = 0;
            cherry->eaten_counter = 0;
        }
        cherry->eaten_counter++;
    }
    else if (cherry->health == 0&&pacman_is_catcher==0)
    {
        if (cherry->eaten_counter == 180)
        { // disappear for 180 frames (6 seconds)
            cherry->health = 1;
            cherry->eaten_counter = 0;
            int new_cherry_x, new_cherry_y;
            generate_random_xy_on_map(&new_cherry_x, &new_cherry_y);
            cherry->x = new_cherry_x * block;
            cherry->y = new_cherry_y * block;
        }
        cherry->eaten_counter++;
    }
    else if (cherry->health == 0&&pacman_is_catcher==1)
    {
        if (cherry->eaten_counter == 150) // 5 seconds to catch the ghosts
        {   
            pacman_is_catcher = 0 ;
            cherry->eaten_counter = 0 ;
        }
        cherry->eaten_counter++;
    }
}

int least_moving_direction(struct object ghosts[5])
{
    int directions[4] = {0, 0, 0, 0};
    for (int i = 0; i < 5; i++)
    {
        directions[ghosts[i].direction]++;
    }
    int min_index = 0;
    for (int i = 0; i < 4; i++)
    {
        if (directions[i] < directions[min_index])
        {
            min_index = i;
        }
    }
    return min_index;
}

void update_ghost(struct object ghosts[5], int additioan_speed, int level)
{
    for (int i = 0; i < 4 + level; i++)
    {
        struct object object = ghosts[i];
        int last_x = object.x;
        int last_y = object.y;
        
        int can_move_in_direction[4] = {0};

                {
                    int nextX = object.x + block + block / 10;
                    int nextXBlock = nextX / block;
                    int nextYUpper = (object.y + telorance) / block;
                    int nextYLower = (object.y + block - telorance) / block;

                    bool withinHorizontalBounds = (object.x + block < 1200 ||
                                                (13 * block < object.y + telorance && object.y + block - telorance < 15 * block && object.x < 1200));

                    bool isNextXBlockPassable = (map[nextYUpper][nextXBlock] != 1 || nextXBlock == 30) &&
                                                (map[nextYLower][nextXBlock] != 1 || nextXBlock == 30);
                    if (withinHorizontalBounds && isNextXBlockPassable)
                        can_move_in_direction[0] = 1;

                }
            
                {   
                    if (object.y > 0 && map[(object.y - block / 10) / block][(object.x + telorance) / block] != 1 && map[(object.y - block / 10) / block][(object.x + block - telorance) / block] != 1)
                        can_move_in_direction[1] = 1;
                }

                {
                    int nextX = object.x - block / 10;
                    int nextYUpper = (object.y + telorance) / block;
                    int nextYLower = (object.y + block - telorance) / block;
                    int nextXBlock = nextX / block;

                    bool withinHorizontalBounds = (object.x > 0 ||
                                                (13 * block < object.y + telorance && object.y + block - telorance < 15 * block && object.x + block > 0));

                    bool isNextXBlockPassable = (map[nextYUpper][nextXBlock] != 1 || nextXBlock == -1) &&
                                                (map[nextYLower][nextXBlock] != 1 || nextXBlock == -1);

                    if (withinHorizontalBounds && isNextXBlockPassable)
                        can_move_in_direction[2] = 1;
                }

                {
                    if (object.y + block < 1000 && map[(object.y + block + block / 10) / block][(object.x + telorance) / block] != 1 && map[(object.y + block + block / 10) / block][(object.x + block - telorance) / block] != 1)
                        can_move_in_direction[3] = 1;
                }

        // moving the ghost
        if (object.direction == 0&&can_move_in_direction[0]==1)
        {
                object.x += ghost_moving_speed + additioan_speed;

                if (object.x >= 1200)
                {
                    object.x = 0;
                }
        }
        else if (object.direction == 1&&can_move_in_direction[1]==1)
        {
                object.y -= ghost_moving_speed + additioan_speed;
        }
        else if (object.direction == 2&&can_move_in_direction[2]==1)
        {
                object.x -= ghost_moving_speed + additioan_speed;

                if (object.x + block <= 0)
                {
                    object.x = 1200;
                }
        }
        else if (object.direction == 3&&can_move_in_direction[3]==1)
        {
                object.y += ghost_moving_speed + additioan_speed;
        }
        

        //choose new direction
        //if ((last_x == object.x && last_y == object.y))
        srand(clock());
        if (can_move_in_direction[object.direction]==0)
        {
            int least_direction = least_moving_direction(ghosts);
            if (object.direction != least_direction && can_move_in_direction[least_direction])
            {
                object.prev_direction = object.direction;
                object.direction = least_direction;
            }
            else
            {
                int randomdirection;
                do
                {
                    randomdirection = rand() % 4;
                } while (randomdirection == object.direction && (!can_move_in_direction[randomdirection]));
                object.prev_direction = object.direction;
                object.direction = randomdirection;
            }
        }
        else{
            if(rand()%50==1){
                int randomdirection;
                do
                {
                    randomdirection = rand() % 4;
                } while (randomdirection == object.direction && (!can_move_in_direction[randomdirection]));
                object.prev_direction = object.direction;
                object.direction = randomdirection;
            }
        }

        ghosts[i] = object;
        WaitTime(0.002);
    }
}

void delay(int number_of_seconds)
{
    // Converting time into milli_seconds
    int milli_seconds = 1000 * number_of_seconds;

    // Storing start time
    clock_t start_time = clock();

    // looping till required time is not achieved
    while (clock() < start_time + milli_seconds)
        ;
}

int GameLoop(char name[], int level)
{

    Sound s321 = LoadSound("sounds\\321.wav");
    Sound sstar = LoadSound("sounds\\star.wav");
    Sound slosing = LoadSound("sounds\\losing.wav");
    Sound sapple = LoadSound("sounds\\apple.wav");
    Sound spoison = LoadSound("sounds\\poison.wav");
    Sound spepper = LoadSound("sounds\\pepper.wav");
    Sound scherry = LoadSound("sounds\\cherry.wav");


    int PackManIsAlive = 1;
    score = 0;
    char score_text[10000];

    // load heart img ----------------
    Image heart_img = LoadImage("images\\heart.png"); // Load image data into CPU memory (RAM)
    ImageResize(&heart_img, (1.5 * block), (1.5 * block));
    Texture2D heart_texture = LoadTextureFromImage(heart_img); // Image converted to texture, GPU memory (RAM -> VRAM)
    UnloadImage(heart_img);
    //-------------------------------

    // pacman -------------
    // pacman images
    char *pacman_ad[5] = {
        "images\\rightpacman.png",
        "images\\uppacman.png",
        "images\\leftpacman.png",
        "images\\downpacman.png",
        "images\\closedpacman.png"};

    // create a pacman object
    struct object pacman = create_object(3, 14 * block, 16 * block, pacman_ad);
    //---------------------

    // ghost -------------
    // ghost images
    char *ghost_ad[5] = {
        "images\\ghosts\\redghostright.png",
        "images\\ghosts\\redghostup.png",
        "images\\ghosts\\redghostleft.png",
        "images\\ghosts\\redghostdown.png",
        "images\\ghosts\\chatchingpacmanghost.png"};

    // create ghosts object
    struct object ghosts[4 + level];
    for (int i = 0; i < 4 + level; i++)
    {
        ghosts[i] = create_object(1, (12 + i) * block, 12 * block, ghost_ad);
        ghosts[i].direction = 0 + 2 * (i % 2);
    }
    int additional_speed; // additional speed of ghosts based on level
    if (level == 1)
        additional_speed = 0;
    if (level == 2)
        additional_speed = 0.06 * block;
    if (level == 3)
        additional_speed = 0.14 * block;

    // create stars
    struct object stars[10];
    char *star_ad[5] = {
    "images\\star.png",
    "images\\starleft.png"
    , "", "", ""};
    int x_star, y_start;
    for (int i = 0; i < 10; i++)
    {
        generate_random_xy_on_map(&x_star, &y_start);
        stars[i] = create_object(1, x_star * block, y_start * block, star_ad);
    }

    // apple -------------
    // apple images
    char *apple_ad[5] = {
        "images\\apple.png",
        "",
        "",
        "",
        ""};
    int x_apple, y_apple;
    // create a apple object
    generate_random_xy_on_map(&x_apple, &y_apple);
    struct object apple = create_object(1, x_apple * block, y_apple * block, apple_ad);
    apple.eaten_counter = 0;
    apple.health = 0; // don't need apple at the begining
    //---------------------

    // poison -------------
    // poison images
    char *poison_ad[5] = {
        "images\\poison.png",
        "",
        "",
        "",
        ""};
    int x_poison, y_poison;
    // create a poison object
    generate_random_xy_on_map(&x_poison, &y_poison);
    struct object poison = create_object(1, x_poison * block, y_poison * block, poison_ad);
    poison.eaten_counter = 0;
    poison.health = 0; // don't need poison at the begining
    //---------------------

    // pepper -------------
    // pepper images
    char *pepper_ad[5] = {
        "images\\pepper.png",
        "",
        "",
        "",
        ""};
    int x_pepper, y_pepper;
    // create a pepper object
    generate_random_xy_on_map(&x_pepper, &y_pepper);
    struct object pepper = create_object(1, x_pepper * block, y_pepper * block, pepper_ad);
    pepper.eaten_counter = 0;
    pepper.health = 0; // don't need pepper at the begining
    //---------------------

    // cherry -------------
    // cherry images
    char *cherry_ad[5] = {
        "images\\cherry.png",
        "",
        "",
        "",
        ""};
    int x_cherry, y_cherry;
    // create a cherry object
    generate_random_xy_on_map(&x_cherry, &y_cherry);
    struct object cherry = create_object(1, x_cherry * block, y_cherry * block, cherry_ad);
    cherry.eaten_counter = 0;
    cherry.health = 0; // don't need cherry at the begining
    //---------------------

    // frame counter
    cnt = 0;
    should_wait = 1; // at the begining of the game
    int first_time = 1;

    while (!WindowShouldClose() && PackManIsAlive)
    {

        if (!should_wait)
        { // wait positions are at the begining and and collision by ghost
            BeginDrawing();
            ClearBackground(BLACK);
            drawmap();                       // draw map
            DrawText(name, 5, 5, 30, WHITE); // draw name
            snprintf(score_text, 9999, "%d", score);
            DrawText("SCORE", 525, 5, 35, WHITE);     // draw "score"
            DrawText(score_text, 550, 40, 35, WHITE); // draw score count
            drawhighscore();
            // draw hearts
            for (int i = 0; i < pacman.health; i++)
            {
                DrawTexture(heart_texture, 5 + 1.2 * block * i, 55, WHITE);
            }
            drawfruit(apple, cnt);  // draw apple
            drawfruit(poison, cnt); // draw poison
            drawfruit(pepper, cnt); // draw pepper
            drawfruit(cherry, cnt); // draw cherry
            char countertext[10000];
            snprintf(countertext, 9999, "%d", cherry.eaten_counter);
            DrawText(countertext, 50, 10, 35, WHITE);
            drawstars(stars, cnt);          // draw stars
            drawghosts(ghosts, cnt, level); // draw ghosts
            drawpacman(pacman, cnt);        // draw pacman
            EndDrawing();

            update_ghost(ghosts, additional_speed, level);
            pacman = update_pacman(pacman, stars, ghosts, &apple, &poison, &pepper, &cherry,
                                    sstar,slosing,sapple,spoison,scherry,spepper,level); // move and redirect pacman
            update_apple(&apple, pacman.health);
            update_poison(&poison);
            update_pepper(&pepper);
            update_cherry(&cherry);
            if (pacman.health == 0)
            {
                PackManIsAlive = 0;
                return score;
            }
        }

        else
        { // if we are at the begining of the game or after collision by a ghost we want to draw every thing and count 3 to 1 on top of it
            PlaySound(s321);
            for (int i = 0; i < 3; i++)
            {
                BeginDrawing();
                ClearBackground(BLACK);
                drawmap();                       // draw map
                DrawText(name, 5, 5, 30, WHITE); // draw name
                snprintf(score_text, 9999, "%d", score);
                DrawText("SCORE", 525, 5, 35, WHITE);     // draw "score"
                DrawText(score_text, 550, 40, 35, WHITE); // draw score count
                drawhighscore();
                // draw hearts
                for (int i = 0; i < pacman.health; i++)
                {
                    DrawTexture(heart_texture, 5 + 1.2 * block * i, 55, WHITE);
                }
                drawfruit(apple, cnt);          // draw apple
                drawfruit(poison, cnt);         // draw apple
                drawfruit(pepper, cnt);         // draw poison
                drawfruit(cherry, cnt);         // draw cherry
                drawstars(stars, cnt);          // draw stars
                drawghosts(ghosts, cnt, level); // draw ghosts
                drawpacman(pacman, cnt);        // draw pacman
                if (!first_time)
                { // we only do this when we have collision by ghost not at the begining
                    DrawRectangle(150, 260, 970, 80, BLACK);
                    DrawText("You lost one heart", 150, 250, 100, RED);
                }
                DrawRectangle(510, 400, 100, 150, BLACK);
                if (i == 0)
                    DrawText("3", 510, 400, 200, RED);
                if (i == 1)
                    DrawText("2", 510, 400, 200, ORANGE);
                if (i == 2)
                    DrawText("1", 560, 400, 200, GREEN);
                EndDrawing();
                WaitTime(1);
            }
            should_wait = 0;
        }

        if (first_time)
            first_time = 0;

        if (cnt == 120)
        {
            cnt = 0;
        }
        else
        {
            cnt++;
        }
    }

    UnloadSound(s321);
    UnloadSound (sstar);
    UnloadSound(slosing);
    UnloadSound(sapple);
    UnloadSound(spoison);
    UnloadSound(spepper);
    UnloadSound(scherry);

}