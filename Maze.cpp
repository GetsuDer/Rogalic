#include <iostream>
#include <fstream>
#include <vector>
#include "Image.h"
#include "Player.h"

Image wall_img("resources/elems/wall.jpg");
Image floor_img("resources/elems/floor.jpg");
Image empty_img("resources/elems/empty.jpg");
Image doors_opened_img("resources/elems/doors_opened.jpg");
Image doors_closed_img("resources/elems/doors_closed.jpg");
Image key_img("resources/elems/key.png");

Key::Key(int x, int y) {
    coords.x = x;
    coords.y = y;
    obtained = false;
}

Maze::Maze(std::string &path) {
    name = path;
    std::ifstream input;
    input.open(path);
    int height = 0;
    size = 0;
    char c = 0;
    bool first_elem = true;
    std::string line;
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
    return field[x][y] == Maze_Point::FLOOR || field[x][y] == Maze_Point::DOOR_OPENED;
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

static void Draw_Square(int x, int y, Image &object, Image &screen) {
    for (int i = 0; i < tileSize; i++) {
        for (int j = 0; j < tileSize; j++) {
            screen.PutPixel(x * tileSize + i, y * tileSize + j, object.GetPixel(i, tileSize - j - 1));
        }
    }
}

void 
Maze::Draw(Image &screen) {
    for (int x = 0; x < size; x++) {
        for (int y = 0; y < size; y++) {
            switch(field[x][y]) {
                case Maze_Point::FLOOR:
                    Draw_Square(x, y, floor_img, screen);
                    break;
                case Maze_Point::WALL:
                    Draw_Square(x, y, wall_img, screen); 
                    break;
                case Maze_Point::EMPTY:
                    Draw_Square(x, y, empty_img, screen);
                    break;
                case Maze_Point::DOOR_CLOSED:
                    Draw_Square(x, y, doors_closed_img, screen);
                    break;
                case Maze_Point::DOOR_OPENED:
                    Draw_Square(x, y, doors_opened_img, screen);
                    break;
                default:
                    break;
            }
        }
    }
    for (int key = 0; key < keys.size(); key++) {
        if (keys[key].obtained) continue;
        int x = keys[key].coords.x;
        int y = keys[key].coords.y;
        int delta = (tileSize - keySize) / 2;
        for (int i = 0; i < keySize; i++) {
            for (int j = 0; j < keySize; j++) {
                screen.PutPixel(x * tileSize + delta + i,
                                y * tileSize + delta + j,
                                blend(floor_img.GetPixel(delta + i, tileSize - delta - j - 1), 
                                      key_img.GetPixel(i, keySize - j - 1)));
            }
        }
    
    }
};

Pixel
Maze::Get_Pixel(int x, int y) {
    switch (field[x / tileSize][y / tileSize]) {
        case Maze_Point::FLOOR:
            return floor_img.GetPixel(x % tileSize, y % tileSize);
        case Maze_Point::EMPTY:
            return empty_img.GetPixel(x % tileSize, y % tileSize);
        case Maze_Point::WALL:
            return wall_img.GetPixel(x % tileSize, y % tileSize);
        case Maze_Point::DOOR_OPENED:
            return doors_opened_img.GetPixel(x % tileSize, y % tileSize);
        case Maze_Point::DOOR_CLOSED:
            return doors_closed_img.GetPixel(x % tileSize, y % tileSize);
        default:
            return floor_img.GetPixel(x % tileSize, y % tileSize);
       }
}
