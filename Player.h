#ifndef MAIN_PLAYER_H
#define MAIN_PLAYER_H

#include <iostream>
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
    DOOR_CLOSED,
    EXIT
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

enum class MovementDir {
    UP,
    DOWN,
    LEFT, 
    RIGHT,
    ACTION,
    NONE
};

enum class MonsterState {
    SLEEP,
    WALK,
    ATTACK,
    DEAD,
    DYING
};

struct Monster;

struct Maze {
        std::string type;
        std::string path;
        Image *wall, *floor, *empty;

        Maze(std::string &_path, std::string &_type);
        void processPlayer(Point coords);
        
        void Draw_Lower(Image &screen);
        void Draw_Higher(Image &screen);
        Point Get_Player();
        Pixel Get_Pixel(int x, int y);
        bool free(Point coords);
        bool open_nearest_door(Point coords);
        bool fell(Point coords);
        bool reach_exit(Point coords);
        int update_keys(Point coords);
        int opened_door(Point coords);
        Point left_door();
        Point right_door();
        Point up_door();
        Point down_door();
        Point from_door(int door);
        std::vector <Key> keys;
        Point Start_Pos;
        Point exit_point;
        int size;
        Maze *others[4];
        std::vector <std::vector <Maze_Point>> field;
        std::vector <Image *> key_img;
        int key_img_ind;
        int key_img_times;
        bool up;

        std::vector <Image *> exit_img;
        int exit_img_ind;
        int exit_img_times;
        bool exit_up;

        std::vector <Monster *> monsters;
};

struct Monster{
    Monster(int x, int y);
    void Draw(Image &screen, Maze *maze);
    void MoveTo(Point player, Maze *maze);
    MonsterState state;
    Point coords;
    Point start;
    MovementDir look;
    std::vector <Image *> walk;
    int walk_img_ind;
    int walk_img_times;
    bool walk_up;
};

enum class PlayerState
{
    DEAD,
    ALIVE,
    WIN
};

struct Player
{
  explicit Player(Point pos = {.x = 10, .y = 10}) :
                 coords(pos), old_coords(coords), keys_obtained(0) {
                    state = PlayerState::ALIVE;
                    look = MovementDir::RIGHT;
                    for (int i = 0; i < 3; i++) {
                        std::string name = "resources/player/" + std::to_string(i) + ".png";
                        Image *img = new Image(name);
                        walk.push_back(img);
                    }
                    active = false;
                    walk_ind = 0;
                    walk_times = 0;
                 };

  bool Moved() const;
  void ProcessInput(MovementDir dir, Maze **maze);
  void Draw(Image &screen);
  PlayerState state;
  int keys_obtained;
  Point placed();
private:
  MovementDir look;
  bool active;
  std::vector <Image *> walk;
  int walk_ind;
  int walk_times;
  Point coords {.x = 10, .y = 10};
  Point old_coords {.x = 10, .y = 10};
  Pixel color {.r = 255, .g = 255, .b = 0, .a = 255};
  int move_speed = 4;

};

#endif //MAIN_PLAYER_H
