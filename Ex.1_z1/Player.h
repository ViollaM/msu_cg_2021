#ifndef MAIN_PLAYER_H
#define MAIN_PLAYER_H

#include "Image.h"
#include <math.h>

struct Point
{
    int x;
    int y;
};

enum class MovementDir
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

struct Player
{
    explicit Player(Point pos = {.x = 0, .y = 0}) :coords(pos), old_coords(coords) {};

    void NewRoom(int x, int y) {
        coords.x = x;
        old_coords.x = x;
        coords.y = y;
        old_coords.y = y;
    }



    bool Moved() const;
    void ProcessInput(MovementDir dir);
    void Draw(Image &screen);
    void DrawYouWin(Image &screen);
    void DrawGameOver(Image &screen);
    void DrawMap(Image &screen, char (&a)[32][32], int cl);
    void Light(Image &screen, int x, int y);
    void Coin(Image &screen, int x, int y, int t);
    void Ghost(Image &screen, int x, int y, int dx, int dy);
    void Enemy(Image &screen, int x, int y, int dx, int dy);
    void Tornado(Image &screen, int x, int y, int dx, int dy);
    void Fog(Image &screen);
    void Effect(Image &screen);

private:
    Point coords {.x = 0, .y = 0};
    Point old_coords {.x = 0, .y = 0};
    Pixel color {.r = 255, .g = 255, .b = 0, .a = 255};
    int move_speed = 1;

};

#endif //MAIN_PLAYER_H
