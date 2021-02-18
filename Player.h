#ifndef MAIN_PLAYER_H
#define MAIN_PLAYER_H

#include <vector>
#include "Image.h"
struct Point
{
  int x;
  int y;
};

enum class Maze_Point {
    EMPTY,
    WALL,
    FLOOR,
    PLAYER
};

struct Maze {
        Maze(std::string &path);
        void Draw(Image &screen);
        Point Get_Player();
        bool free(Point coords);
    private:
        Point Start_Pos;
        int size;
        std::vector <std::vector <Maze_Point>> field;
};

enum class MovementDir
{
  UP,
  DOWN,
  LEFT,
  RIGHT,
};

struct Player
{
  explicit Player(Point pos = {.x = 10, .y = 10}) :
                 coords(pos), old_coords(coords) {};

  bool Moved() const;
  void ProcessInput(MovementDir dir, Maze &maze);
  void Draw(Image &screen);

private:
  Point coords {.x = 10, .y = 10};
  Point old_coords {.x = 10, .y = 10};
  Pixel color {.r = 255, .g = 255, .b = 0, .a = 255};
  int move_speed = 4;

};

#endif //MAIN_PLAYER_H
