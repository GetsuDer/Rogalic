#include <iostream>
#include <fstream>
#include <vector>
#include "Image.h"
#include "Player.h"

Image doors_opened_img("resources/elems/doors_opened.png");
Image doors_closed_img("resources/elems/doors_closed.png");
Key::Key(int x, int y) {
    coords.x = x;
    coords.y = y;
    obtained = false;
}

Maze::Maze(std::string &_path, std::string &_type): key_animation("resources/elems/key/", 9, 30),
    exit_animation("resources/elems/rooms/" + _type + "/exit/", 13, 7) {
    path = _path;
    type = _type;
    std::string pref = "resources/elems/rooms/" + _type;
    
    std::string elem_path = pref + "/wall.png";
    wall = new Image(elem_path);

    elem_path = pref + "/empty.png";
    empty = new Image(elem_path);

    elem_path = pref + "/floor.png";
    floor = new Image(elem_path);

    std::ifstream input;
    input.open(path);
    int height = 0;
    size = 0;
    char c = 0;
    exit_point.x = -1;
    exit_point.y = -1;
    bool first_elem = true;
    std::string line;
    Monster *monster = NULL;
    while (!input.eof()) {
        getline(input, line);
        if (line.empty()) break;
        height++;
        field.push_back(std::vector<Maze_Point>());
        size = line.size();
        for (int i = 0; i < size; i++) { 
            char c = line[i];
            switch (c) {
                case ' ': 
                    field[height - 1].push_back(Maze_Point::EMPTY);
                    break;
                case '#':
                    field[height - 1].push_back(Maze_Point::WALL);
                    break;
                case '.':
                    field[height - 1].push_back(Maze_Point::FLOOR);
                    break;
                case '@':
                    field[height - 1].push_back(Maze_Point::FLOOR);
                    Start_Pos.x = i * tileSize;
                    Start_Pos.y = (size - height) * tileSize;
                    break;
                case 'K':
                    field[height - 1].push_back(Maze_Point::FLOOR);
                    keys.push_back(Key(i, size - height));
                    break;
                case 'x':
                    field[height - 1].push_back(Maze_Point::DOOR_OPENED);
                    break;
                case 'X':
                    field[height - 1].push_back(Maze_Point::DOOR_CLOSED);
                    break;
                case 'Q':
                    field[height - 1].push_back(Maze_Point::EXIT);
                    exit_point.x = i;
                    exit_point.y = size - height;
                    break;
                case 'e':
                    field[height - 1].push_back(Maze_Point::FLOOR);
                    monster = new Monster(i, size - height, 1);
                    monsters.push_back(monster);
                    break;
                case 'E':
                    field[height - 1].push_back(Maze_Point::FLOOR);
                    monster = new Monster(i, size - height, 2);
                    monsters.push_back(monster);
                    break;
                default:
                    break;
            }
        }
    }
    std::vector <std::vector <Maze_Point> > tmp_vec(size);
    for (int i = 0; i < size; i++) tmp_vec[i].resize(size);
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            tmp_vec[j][size - i - 1] = field[i][j];
        }
    }
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            field[i][j] = tmp_vec[i][j];
        }
    }
    input.close();

};

bool
Maze::free(Point coords) {
    int x = coords.x / tileSize;
    int y = coords.y / tileSize;
    if (x < 0 || y < 0) {
        return false;
    }
    if (x >= field.size() || y >= field[x].size()) {
        return false;
    }

    return field[x][y] == Maze_Point::FLOOR ||
        field[x][y] == Maze_Point::DOOR_OPENED || field[x][y] == Maze_Point::EMPTY || 
        field[x][y] == Maze_Point::EXIT;
}

Point Maze::Get_Player() {
    return Start_Pos;
}

int
Maze::update_keys(Point coords) {
    int res = 0;
    for (int key = 0; key < keys.size(); key++) {
        if (keys[key].obtained) continue;
        Point key_point;
        key_point.x = keys[key].coords.x * tileSize;
        key_point.y = keys[key].coords.y * tileSize;
        if (dist(coords, key_point) < tileSize / 3) {
            keys[key].obtained = true;
            res++;
        }
    }    
    return res;
}

Point
Maze::from_door(int door) {
    Point res;
    switch (door) {
        case 0:
            res.x = 1;
            for (int i = 0; i < size; i++) {
                if (field[0][i] == Maze_Point::DOOR_OPENED) {
                    res.y = i;
                }
            }
            return res;
        case 1:
            res.y = size - 2;
            for (int i = 0; i < size; i++) {
                if (field[i][size - 1] == Maze_Point::DOOR_OPENED) {
                    res.x = i;
                }
            }
            return res;
        case 2:
            res.x = size - 2;
            for (int i = 0; i < size; i++) {
                if (field[size - 1][i] == Maze_Point::DOOR_OPENED) {
                    res.y = i;
                }
            }
            return res;
        default:
            res.y = 1;
            for (int i = 0; i < size; i++) {
                if (field[i][0] == Maze_Point::DOOR_OPENED) {
                    res.x = i;
                }
            }
            return res;
    }
}

Point
Maze::left_door() {
    Point res;
    res.x = -1;
    res.y = -1;
    for (int i = 0; i < size; i++) {
        if (field[0][i] == Maze_Point::DOOR_OPENED || field[0][i] == Maze_Point::DOOR_CLOSED) {
            res.x = 0;
            res.y = i;
            return res;
        }
    }
    return res;
}

Point
Maze::right_door() {
    Point res;
    res.x = -1;
    res.y = -1;
    for (int i = 0; i < size; i++) {
        if (field[size - 1][i] == Maze_Point::DOOR_OPENED || field[size - 1][i] == Maze_Point::DOOR_CLOSED) {
            res.x = size - 1;
            res.y = i;
            return res;
        }
    }
    return res;
}

Point 
Maze::up_door() {
    Point res;
    res.x = -1;
    res.y = -1;
    for (int i = 0; i < size; i++) {
        if (field[i][size - 1] == Maze_Point::DOOR_OPENED || field[i][size - 1] == Maze_Point::DOOR_CLOSED) {
            res.x = i;
            res.y = size - 1;
            return res;
        }
    }
    return res;
}

Point
Maze::down_door() {
    Point res;
    res.x = -1;
    res.y = -1;
    for (int i = 0; i < size; i++) {
        if (field[i][0] == Maze_Point::DOOR_OPENED || field[i][0] == Maze_Point::DOOR_CLOSED) {
            res.x = i;
            res.y = 0;
            return res;
        }
    }
    return res;
}

bool
Maze::open_nearest_door(Point coords) {
    coords.x /= tileSize;
    coords.y /= tileSize;

    Point other_door;
    Point door = left_door();
    if (door.x >= 0 && (field[door.x][door.y] == Maze_Point::DOOR_CLOSED) && dist(door, coords) < 2) {
        field[door.x][door.y] = Maze_Point::DOOR_OPENED;
        other_door = others[0]->right_door();
        others[0]->field[other_door.x][other_door.y] = Maze_Point::DOOR_OPENED;
        return true;
    }
    door = up_door();
    if (door.x >= 0 && (field[door.x][door.y] == Maze_Point::DOOR_CLOSED) && dist(door, coords) < 2) {
        field[door.x][door.y] = Maze_Point::DOOR_OPENED;
        other_door = others[1]->down_door();
        others[1]->field[other_door.x][other_door.y] = Maze_Point::DOOR_OPENED;
        return true;
    }
    door = right_door();
    if (door.x >= 0 && (field[door.x][door.y] == Maze_Point::DOOR_CLOSED) && dist(door, coords) < 2) {
        field[door.x][door.y] = Maze_Point::DOOR_OPENED;
        other_door = others[2]->left_door();
        others[2]->field[other_door.x][other_door.y] = Maze_Point::DOOR_OPENED;
        return true;
    }
    door = down_door();
    if (door.x >= 0 && (field[door.x][door.y] == Maze_Point::DOOR_CLOSED) && dist(door, coords) < 2) {
        field[door.x][door.y] = Maze_Point::DOOR_OPENED;
        other_door = others[3]->up_door();
        others[3]->field[other_door.x][other_door.y] = Maze_Point::DOOR_OPENED;
        return true;
    }
    return false;
}

int 
Maze::opened_door(Point coords) {
    int x = coords.x / tileSize;
    int y = coords.y / tileSize;
    if (field[x][y] == Maze_Point::DOOR_OPENED) {
        if (x == 0) {
            return 0;
        }
        if (y == 0) {
            return 3;
        }
        if (y > x) {
            return 1;
        }
        return 2;
    } else {
        return -1;
    }
}

static void Draw_Square(int x, int y, Image *object, Image &screen) {
    for (int i = 0; i < tileSize; i++) {
        for (int j = 0; j < tileSize; j++) {
            screen.PutPixel(x * tileSize + i, y * tileSize + j, object->GetPixel(i, tileSize - j - 1));
        }
    }
}

void 
Maze::Draw_Lower(Image &screen) {
    for (int x = 0; x < size; x++) {
        for (int y = 0; y < size; y++) {
            switch(field[x][y]) {
                case Maze_Point::FLOOR:
                    Draw_Square(x, y, floor, screen);
                    break;
                case Maze_Point::WALL:
                    Draw_Square(x, y, wall, screen); 
                    break;
                case Maze_Point::EMPTY:
                    Draw_Square(x, y, empty, screen);
                    break;
                case Maze_Point::DOOR_CLOSED:
                    for (int i = 0; i < tileSize; i++) {
                        for (int j = 0; j < tileSize; j++) {
                            screen.PutPixel(x * tileSize + i, y * tileSize + j,
                              blend(wall->GetPixel(i, j), doors_closed_img.GetPixel(i, tileSize - j - 1)));
                        }
                    }
                    break;
                case Maze_Point::DOOR_OPENED:
                    for (int i = 0; i < tileSize; i++) {
                        for (int j = 0; j < tileSize; j++) {
                            screen.PutPixel(x * tileSize + i, y * tileSize + j,
                              blend(wall->GetPixel(i, j), doors_opened_img.GetPixel(i, tileSize - j - 1)));
                        }
                    }
                    break;
                case Maze_Point::EXIT:
                    Draw_Square(x, y, floor, screen);
                    break;
                default:
                    break;
            }
        }
    }
    if (exit_point.x != -1) {
        Image *img = exit_animation.Draw();
        for (int x = 0; x < tileSize; x++) {
            for (int y = 0; y < tileSize; y++) {
                screen.PutPixel(exit_point.x * tileSize + x, exit_point.y * tileSize + y,
                        blend(floor->GetPixel(x, tileSize - y - 1), img->GetPixel(x, tileSize - y - 1)));
            }
        }
    }
    for (int key = 0; key < keys.size(); key++) {
        if (keys[key].obtained) continue;
        int x = keys[key].coords.x;
        int y = keys[key].coords.y;
        Image *img = key_animation.Draw();
        for (int i = 0; i < keySize; i++) {
            for (int j = 0; j < keySize; j++) {
                screen.PutPixel(x * tileSize + i,
                                y * tileSize + j,
                                blend(floor->GetPixel(i, tileSize - j - 1), 
                                      img->GetPixel(i, keySize - j - 1)));
            }
        }
    
    }

    for (int monster = 0; monster < monsters.size(); monster++) {
        monsters[monster]->Draw(screen, this);
    }

    for (int i = 0; i < fires.size(); i++) {
        fires[i]->Draw(screen, this);
    }
}
Pixel
Maze::Get_Pixel(int x, int y) {
    switch (field[x / tileSize][y / tileSize]) {
        case Maze_Point::FLOOR:
            return floor->GetPixel(x % tileSize, y % tileSize);
        case Maze_Point::EMPTY:
            return empty->GetPixel(x % tileSize, y % tileSize);
        case Maze_Point::WALL:
            return wall->GetPixel(x % tileSize, y % tileSize);
        case Maze_Point::DOOR_OPENED:
            return doors_opened_img.GetPixel(x % tileSize, y % tileSize);
        case Maze_Point::DOOR_CLOSED:
            return doors_closed_img.GetPixel(x % tileSize, y % tileSize);
        case Maze_Point::EXIT:
            return exit_animation.Current()->GetPixel(x % tileSize, y % tileSize);
        default:
            return floor->GetPixel(x % tileSize, y % tileSize);
       }
}

bool
Maze::fell(Point coords) {
    coords.x += tileSize / 2;
    Point center;
    int x = coords.x / tileSize;
    int y = coords.y / tileSize;
    center.x = x * tileSize + tileSize / 2;
    center.y = y * tileSize + tileSize / 2;
    if (field[x][y] == Maze_Point::EMPTY && dist(coords, center) < (((double)tileSize) / 2.5)) {
        return true;
    }
    return false;
}

bool
Maze::reach_exit(Point coords) {
    if (exit_point.x == -1) {
        return false;
    }
    coords.x = coords.x + tileSize / 2;
    coords.y = coords.y + tileSize / 2;
    Point center;
    center.x = exit_point.x * tileSize + tileSize / 2;
    center.y = exit_point.y * tileSize + tileSize / 2;
    return dist(coords, center) < tileSize / 2;
}

int
Maze::attack(Point coords) {
    int strike = 0;
    for (int i = 0; i < monsters.size(); i++) {
        if (monsters[i]->hit(coords)) {
            strike += monsters[i]->attack_points;
        }
    }
    return strike;
}


Monster::Monster(int x, int y, int type): 
    walk_animation("resources/monsters/" + std::to_string(type) + "/walk/", 4, 50),
    attack_animation("resources/monsters/" + std::to_string(type) + "/attack/", 
            (type == 1) ? 6 : 20, (type == 1) ? 100 : 5),
    dying_animation("resources/monsters/" + std::to_string(type) + "/dying/", (type == 1) ? 10 : 12, (type == 1) ? 20 : 3) {
   
    attack_points = ((type == 1) ? 50 : 20); 
    this->type = type;
    coords.x = x * tileSize;
    coords.y = y * tileSize;
    start.x = x * tileSize;
    start.y = y * tileSize;

    state = MonsterState::SLEEP;
    look = MovementDir::RIGHT;
    attacking = 0;
    attack_ind = 0;

    dying = false;
    dying_ind = 0;
    dying_times = 0;
    hitpoints = (type == 1) ? 75 : 50; 
}
constexpr Pixel blue {.r = 0, .g = 0, .b = 255, .a = 0};
void
Monster::Draw(Image &screen, Maze *maze) {
    Image *img;
    switch (state) {
        case MonsterState::DYING:
            img = dying_animation.Draw(dying_ind);
            dying_times++;
            if (dying_times > dying_animation.img_times_max) {
                dying_ind++;
                dying_times = 0;
            }
            if (dying_ind == dying_animation.len()) {
                state = MonsterState::DEAD;
            }
            break;
        case MonsterState::DEAD:
            return;
         //   img = dying_animation.Draw(dying_animation.len() - 1);
        case MonsterState::SLEEP:
            img = walk_animation.Draw(0);
            break;
        case MonsterState::WALK:
            img = walk_animation.Draw();
            break;
        case MonsterState::ATTACK:
            img = attack_animation.Draw(attack_ind);
            attack_ind += attacking;
            if (attack_ind == attack_animation.len() - 1) {
                attacking = -1;
            }
            if (attack_ind == 0) {
                attacking = 0;
            }
            break;
    }
    for (int x = 0; x < tileSize; x++) {
        for (int y = 0; y < tileSize; y++) {
            screen.PutPixel(coords.x + x, coords.y + y,
                    blend(screen.GetPixel(coords.x + x, coords.y + y),
                        img->GetPixel((look == MovementDir::LEFT) ? x : tileSize - x - 1, tileSize - y - 1)));
        }
    }

    for (int i = 0; i * 6 < hitpoints; i++) {
        for (int j = 0; j < 5; j++) {
            screen.PutPixel(coords.x + tileSize / 2 + i, coords.y + tileSize + tileSize / 3 + j, blue);
            screen.PutPixel(coords.x + tileSize / 2 - i, coords.y + tileSize + tileSize / 3 + j, blue);
        }
    }

}


void
Maze::processPlayer(Point player) {
    for (int i = 0; i < fires.size(); i++) {
        fires[i]->update(this);
    }
    int size = fires.size();
    for (int i = size - 1; i >= 0; i--) {
        if (!fires[i]->alive) {
            fires.erase(fires.begin() + i);
        }
    }


    double monster_attack_distance = tileSize / 2;
    double monster_awake_distance = 5 * tileSize;
    for (int i = 0; i < monsters.size(); i++) {
        Monster *monster = monsters[i];
        double player_dist = dist(player, monster->coords);
        switch (monster->state) {
            case MonsterState::SLEEP:
                if (player_dist < monster_awake_distance) {
                    monster->state = MonsterState::WALK;
                }
                break;

            case MonsterState::WALK:
                if (player_dist > monster_awake_distance) {
                    monster->state = MonsterState::SLEEP;
                    break;
                }
                if (player_dist <= monster_attack_distance) {
                    monster->state = MonsterState::ATTACK;
                    monster->attacking = 1;
                    break;
                }
                monster->MoveTo(player, this);
            case MonsterState::ATTACK:
                if (!monster->attacking) {
                    monster->state = MonsterState::WALK;
                    break;
                }
            default:
                break;
        }
    }
}

void
Monster::MoveTo(Point player, Maze *maze) {
    int movedist = (type == 1) ? 2 : 5;
    Point heart;
    heart.x = player.x + tileSize / 2;
    heart.y = player.y + tileSize / 2;
    Point tale;
    tale.x = (look == MovementDir::LEFT) ? coords.x + tileSize : coords.x;
    tale.y = coords.y + tileSize / 2;

    if (heart.x > tale.x) {
        coords.x += movedist;
        look = MovementDir::RIGHT;
    } else if (heart.x < tale.x) {
        coords.x -= movedist;
        look = MovementDir::LEFT;
    }
    if (heart.y > tale.y) {
        coords.y += movedist;
    } else if (heart.y < tale.y) {
        coords.y -= movedist;
    }
}

bool
Monster::hit(Point player) {
    Point heart;
    heart.x = player.x + tileSize / 2;
    heart.y = player.y + tileSize / 2;
    double hit_distance = tileSize / 2;
    if (attack_ind == attack_animation.img.size() - 1) {
        Point tale;
        tale.x = (look == MovementDir::RIGHT) ? coords.x + tileSize : coords.x;
        tale.y = coords.y + tileSize / 2;
        if (dist(heart, tale) < hit_distance) {
            return true;
        }
        return false;
    } else {
        return false;
    }
}

Animation::Animation(std::string path, int img_number, int times_max) {
    std::string name;
    img_ind = 0;
    img_times = 0;
    img_times_max = times_max;
    up = true;
    for (int i = 0; i < img_number; i++) {
        name = path + std::to_string(i + 1) + ".png";
        Image *new_img = new Image(name);
        img.push_back(new_img);
    }
}

Image *
Animation::Draw(int ind) {
    int draw = ind;
    if (ind == -1) {
        draw = img_ind;
        img_times++;
        if (img_times > img_times_max) {
            if (up) {
                img_ind++;
                if (img_ind == img.size() - 1) {
                    up = false;
                }
            } else {
                img_ind--;
                if (img_ind == 0) {
                    up = true;
                }
            }
            img_times = 0;
        }
    }
    return img[draw];
}

Image *
Animation::Current() {
    return img[img_ind];
}

int
Animation::len() {
    return img.size();
}

Fire::Fire(int x, int y, int _x_speed, int _y_speed): fire_animation("resources/player/fire/", 6, 3) {
    coords.x = x;
    coords.y = y;
    x_speed = _x_speed;
    y_speed = _y_speed;
    alive = true;
    time_to_live = 100;
}

void
Fire::Draw(Image &screen, Maze *maze) {
    Image *img = fire_animation.Draw();
    for (int x = 0; x < tileSize; x++) {
        for (int y = 0; y < tileSize; y++) {
            screen.PutPixel(coords.x + x, coords.y + y,
                    blend(screen.GetPixel(coords.x + x, coords.y + y),
                        img->GetPixel(x, tileSize - y - 1)));
        }
    }
}

void
Fire::update(Maze *maze) {
    time_to_live--;
    if (time_to_live <= 0) {
        alive = false;
        return;
    }

    coords.x += x_speed;
    coords.y += y_speed;

    Point center;
    center.x = coords.x + tileSize / 2;
    center.y = coords.y + tileSize / 2;
    if (!maze->free(center)) {
        alive = false;
        return;
    } else {
        if (maze->field[center.x / tileSize][center.y / tileSize] == Maze_Point::DOOR_OPENED) {
            alive = false;
            return;
        }
    }
    Point heart;
    for (int i = 0; i < maze->monsters.size(); i++) {

        if (maze->monsters[i]->state != MonsterState::WALK && 
                maze->monsters[i]->state != MonsterState::SLEEP) {
            continue;
        }
        heart.x = maze->monsters[i]->coords.x + tileSize / 2;
        heart.y = maze->monsters[i]->coords.y + tileSize / 2;
        if (dist(center, heart) < tileSize / 2) {
            alive = false;
            maze->monsters[i]->hitpoints -= 25;
            if (maze->monsters[i]->hitpoints <= 0) {
                maze->monsters[i]->state = MonsterState::DYING;
            }
        }
    }
}
