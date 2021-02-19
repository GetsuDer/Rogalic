#ifndef MAIN_PLAYER_H
#define MAIN_PLAYER_H

#include <cmath>
#include <vector>
#include "Image.h"
struct Point
{
  int x;
  int y;
};

static double 
dist(Point first, Point second) {
    double x = first.x - second.x;
    double y = first.y - second.y;
    return sqrt(x * x + y * y);
}

enum class Maze_Point {
    EMPTY,
    WALL,
    FLOOR,
    PLAYER,
    DOOR_OPENED,
    DOOR_CLOSED
};

enum class Key_Type {
    LEFT,
    RIGHT,
    UP,
    DOWN
};

struct Key {
    Key(int x, int y);
    bool obtained;
    Point coords;
};

struct Maze {
        std::string name;
        Maze(std::string &path);
        void Draw(Image &screen);
        Point Get_Player();
        Pixel Get_Pixel(int x, int y);
        bool free(Point coords);
        bool open_nearest_door(Point coords);
        int update_keys(Point coords);
        int opened_door(Point coords);
        Point left_door();
        Point right_door();
        Point up_door();
        Point down_door();
        Point from_door(int door);
        std::vector <Key> keys;
        Point Start_Pos;
        int size;
        Maze *others[4];
        std::vector <std::vector <Maze_Point>> field;
};

enum class MovementDir
{
  UP,
  DOWN,
  LEFT,
  RIGHT,
  ACTION
};

struct Player
{
  explicit Player(Point pos = {.x = 10, .y = 10}) :
                 coords(pos), old_coords(coords), keys_obtained(0) {};

  bool Moved() const;
  void ProcessInput(MovementDir dir, Maze **maze);
  void Draw(Image &screen);

private:
  int keys_obtained;
  Point coords {.x = 10, .y = 10};
  Point old_coords {.x = 10, .y = 10};
  Pixel color {.r = 255, .g = 255, .b = 0, .a = 255};
  int move_speed = 4;

};

#endif //MAIN_PLAYER_H
