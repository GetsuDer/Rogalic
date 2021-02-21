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

struct Animation {
    std::vector <Image *> img;
    int img_ind;
    int img_times;
    int img_times_max;
    bool up;

    Animation(std::string path_to_img, int img_number, int times_max);
    Image *Draw(int ind = -1);
    Image *Current();
    int len();

};

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
    FIRE,
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
struct Fire;

struct Maze {
        std::string type;
        std::string path;
        Image *wall, *floor, *empty;

        Maze(std::string &_path, std::string &_type);
        void processPlayer(Point coords);
        int attack(Point coords);
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
        
        Animation key_animation;
        Animation exit_animation;

        std::vector <Monster *> monsters;
        std::vector <Fire *> fires;
};

struct Monster{
    Monster(int x, int y, int type);
    void Draw(Image &screen, Maze *maze);
    void MoveTo(Point player, Maze *maze);
    bool hit(Point coords);
    MonsterState state;
    Point coords;
    Point start;
    MovementDir look;
    Animation walk_animation;
    Animation attack_animation;
    int attacking;
    int attack_ind;
    Animation dying_animation;
    bool dying;
    int dying_ind;
    int dying_times;

    int type;
    int attack_points;
    int hitpoints;
};

struct Fire {
    Point coords;
    Animation fire_animation;
    int x_speed;
    int y_speed;
    int time_to_live;
    bool alive;

    Fire(int x, int y, int _x_speed, int _y_speed);

    void Draw(Image &screen, Maze *maze);
    void update(Maze *maze);
};

enum class PlayerState
{
    DEAD,
    ALIVE,
    WIN,
};
constexpr int player_hitpoints = 100;
struct Player
{
  explicit Player(Point pos = {.x = 10, .y = 10}) :
                 coords(pos), old_coords(coords), keys_obtained(0),
                 walk_animation("resources/player/", 3, 10) {
                    state = PlayerState::ALIVE;
                    look = MovementDir::RIGHT;
                    active = false;
                    hitpoints = player_hitpoints;
                 };

  bool Moved() const;
  void ProcessInput(MovementDir dir, Maze **maze, int mouseX = -1, int mouseY = -1);
  void Draw(Image &screen);
  PlayerState state;
  int keys_obtained;
  Point placed();
  int hitpoints;
private:
  MovementDir look;
  bool active;
  Animation walk_animation; 
  Point coords {.x = 10, .y = 10};
  Point old_coords {.x = 10, .y = 10};
  Pixel color {.r = 255, .g = 255, .b = 0, .a = 255};
  int move_speed = 4;

};

#endif //MAIN_PLAYER_H
