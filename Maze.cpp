#include <iostream>
#include <fstream>
#include <vector>
#include "Image.h"
#include "Player.h"

Image doors_opened_img("resources/elems/doors_opened.jpg");
Image doors_closed_img("resources/elems/doors_closed.jpg");
Key::Key(int x, int y) {
    coords.x = x;
    coords.y = y;
    obtained = false;
}

Maze::Maze(std::string &_path, std::string &_type) {
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
                    monster = new Monster(i, size - height);
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
    int key_animation_number = 9;
    for (int i = 0; i < key_animation_number; i++) {
        std::string name = "resources/elems/key/" + std::to_string(i + 1) + ".png";
        Image *img = new Image(name);
        key_img.push_back(img);
    }
    key_img_ind = 0;
    key_img_times = 0;
    up = true;

    int exit_animation_number = 13;
    for (int i = 0; i < exit_animation_number; i++) {
        std::string name = "resources/elems/rooms/" + type + "/exit/" + std::to_string(i + 1) + ".png";
        Image *img = new Image(name);
        if (img->Width() == -1) {
            std::cout << "problem with " << name << ' ' << type << '\n';
        }
        exit_img.push_back(img);
    }
    exit_img_ind = 0;
    exit_img_times = 0;
    exit_up = true;
};

bool
Maze::free(Point coords) {
    int x = coords.x / tileSize;
    int y = coords.y / tileSize;
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

    Point door = left_door();
    if (door.x >= 0 && (field[door.x][door.y] == Maze_Point::DOOR_CLOSED) && dist(door, coords) < 2) {
        field[door.x][door.y] = Maze_Point::DOOR_OPENED;
        return true;
    }
    door = up_door();
    if (door.x >= 0 && (field[door.x][door.y] == Maze_Point::DOOR_CLOSED) && dist(door, coords) < 2) {
        field[door.x][door.y] = Maze_Point::DOOR_OPENED;
        return true;
    }
    door = right_door();
    if (door.x >= 0 && (field[door.x][door.y] == Maze_Point::DOOR_CLOSED) && dist(door, coords) < 2) {
        field[door.x][door.y] = Maze_Point::DOOR_OPENED;
        return true;
    }
    door = down_door();
    if (door.x >= 0 && (field[door.x][door.y] == Maze_Point::DOOR_CLOSED) && dist(door, coords) < 2) {
        field[door.x][door.y] = Maze_Point::DOOR_OPENED;
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
                    Draw_Square(x, y, &doors_closed_img, screen);
                    break;
                case Maze_Point::DOOR_OPENED:
                    Draw_Square(x, y, &doors_opened_img, screen);
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
        int img = exit_img_ind;
        exit_img_times++;
        if (exit_img_times > 10) {
            if (exit_up) {
                exit_img_ind++;
                if (exit_img_ind == exit_img.size() - 1) {
                    exit_up = false;
                }
            } else {
                exit_img_ind--;
                if (!exit_img_ind) {
                    exit_up = true;
                }
            }
            exit_img_times = 0;
        }
        for (int x = 0; x < tileSize; x++) {
            for (int y = 0; y < tileSize; y++) {
                screen.PutPixel(exit_point.x * tileSize + x, exit_point.y * tileSize + y,
                        blend(floor->GetPixel(x, tileSize - y - 1), exit_img[img]->GetPixel(x, tileSize - y - 1)));
            }
        }
    }
    for (int key = 0; key < keys.size(); key++) {
        if (keys[key].obtained) continue;
        int x = keys[key].coords.x;
        int y = keys[key].coords.y;
        int img = key_img_ind;
        key_img_times++;
        if (key_img_times > 30) {
            if (up) {
                key_img_ind++;
                if (key_img_ind == key_img.size() - 1) {
                    up = false;
                }
            } else {
                key_img_ind--;
                if (!key_img_ind) {
                    up = true;
                }
            }
            key_img_times = 0;
        }
        for (int i = 0; i < keySize; i++) {
            for (int j = 0; j < keySize; j++) {
                screen.PutPixel(x * tileSize + i,
                                y * tileSize + j,
                                blend(floor->GetPixel(i, tileSize - j - 1), 
                                      key_img[img]->GetPixel(i, keySize - j - 1)));
            }
        }
    
    }

    for (int monster = 0; monster < monsters.size(); monster++) {
        monsters[monster]->Draw(screen, this);
    }

}
void
Maze::Draw_Higher(Image &screen) {
// may be later

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
            return exit_img[exit_img_ind]->GetPixel(x % tileSize, y % tileSize);
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

Monster::Monster(int x, int y) {
    coords.x = x * tileSize;
    coords.y = y * tileSize;
    start.x = x * tileSize;
    start.y = y * tileSize;

    state = MonsterState::SLEEP;
    look = MovementDir::RIGHT;
    int walk_img_num = 4;
    for (int i = 0; i < walk_img_num; i++) {
        std::string path = "resources/monsters/1/walk/" + std::to_string(i + 1) + ".png";
        std::cout << path << '\n';
        Image *img = new Image(path);
        if (img->Width() == -1) {
            std::cout << "FAIL\n";
        }
        walk.push_back(img);
    }
}

void
Monster::Draw(Image &screen, Maze *maze) {
    switch (state) {
        case MonsterState::DEAD:
            return;
        case MonsterState::SLEEP:
            for (int x = 0; x < tileSize; x++) {
                for (int y = 0; y < tileSize; y++) {
                    screen.PutPixel(coords.x + x, coords.y + y,
                            blend(maze->Get_Pixel(coords.x + x, coords.y + y),
                                  walk[0]->GetPixel((look == MovementDir::LEFT) ? x : tileSize - x - 1, tileSize - y - 1)));
                }
            }
            break;
                    
        case MonsterState::WALK:
            int img = walk_img_ind;
            walk_img_times++;
            if (walk_img_times > 20) {
                if (walk_up) {
                    walk_img_ind++;
                    if (walk_img_ind == walk.size() - 1) {
                        walk_up = false;
                    }    
                } else {
                    walk_img_ind--;
                    if (!walk_img_ind) {
                        walk_up = true;
                    }
                }
                walk_img_times = 0;
            }
            for (int x = 0; x < tileSize; x++) {
                for (int y = 0; y < tileSize; y++) {
                    screen.PutPixel(coords.x + x, coords.y + y,
                            blend(maze->Get_Pixel(coords.x + x, coords.y + y),
                                  walk[img]->GetPixel((look == MovementDir::LEFT) ? x : tileSize - x - 1, tileSize - y - 1)));
                }
            }
            break;
    }

}


void
Maze::processPlayer(Point player) {
    double monster_attack_distance = tileSize / 2;
    double monster_awake_distance = 3 * tileSize;
    for (int i = 0; i < monsters.size(); i++) {
        Monster *monster = monsters[i];
        switch (monster->state) {

            case MonsterState::SLEEP:
                if (dist(player, monster->coords) < monster_awake_distance) {
                    monster->state = MonsterState::WALK;
                }
                return;

            case MonsterState::WALK:
                if (dist(player, monster->coords) > monster_awake_distance) {
                    monster->state = MonsterState::SLEEP;
                    return;
                }
                monster->MoveTo(player, this);
            default:
                break;
        }
    }
}

void
Monster::MoveTo(Point player, Maze *maze) {
    int movedist = 3;
    if (player.x > coords.x) {
        coords.x += movedist;
        look = MovementDir::RIGHT;
    } else if (player.x < coords.x) {
        coords.x -= movedist;
        look = MovementDir::LEFT;
    }
    if (player.y > coords.y) {
        coords.y += movedist;
    } else if (player.y < coords.y) {
        coords.y -= movedist;
    }
}
