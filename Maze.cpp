#include <iostream>
#include <fstream>
#include <vector>
#include "Image.h"
#include "Player.h"

Image wall_img("/home/getsu/Data/Game/Rogalik/resources/wall.jpg");
Image floor_img("/home/getsu/Data/Game/Rogalik/resources/floor.jpg");
constexpr Pixel wall{255, 0, 0, 0};
constexpr Pixel floor{0, 255, 0, 0};
constexpr Pixel empty{200, 200, 200, 0};

Maze::Maze(std::string &path) {
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
    return field[coords.x / tileSize][coords.y / tileSize] == Maze_Point::FLOOR;
}

Point Maze::Get_Player() {
    std::cout << Start_Pos.x << ' ' << Start_Pos.y << std::endl;
    return Start_Pos;
}

static void Draw_Square(int x, int y, Pixel color, Image &screen) {
    for (int i = 0; i < tileSize; i++) {
        for (int j = 0; j < tileSize; j++) {
            screen.PutPixel(x * tileSize + i, y * tileSize + j, color);
        }
    }
}

void 
Maze::Draw(Image &screen) {
    for (int x = 0; x < size; x++) {
        for (int y = 0; y < size; y++) {
            switch(field[x][y]) {
                case Maze_Point::FLOOR:
                    for (int i = 0; i < tileSize; i++) {
                        for (int j = 0; j < tileSize; j++) {
                            screen.PutPixel(x * tileSize + i, y * tileSize + j, floor_img.GetPixel(i, j));
                    //        std::cout << (int)floor_img.GetPixel(i, j).r << ' ';
                        }
                      //  std::cout << std::endl;
                    }
                    break;
                case Maze_Point::WALL:
                    for (int i = 0; i < tileSize; i ++) {
                        for (int j = 0; j < tileSize; j++) {
                            screen.PutPixel(x * tileSize + i, y * tileSize + j, wall_img.GetPixel(i, j));
                        }
                    }
                    
                    break;
                case Maze_Point::EMPTY:
                    Draw_Square(x, y, empty, screen);
                    break;
                default:
                    break;
            }
        }
    }

};
