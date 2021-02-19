#include "Player.h"
#include <iostream>
Image player_img("resources/player/1.png");

bool Player::Moved() const
{
  if(coords.x == old_coords.x && coords.y == old_coords.y)
    return false;
  else
    return true;
}

void Player::ProcessInput(MovementDir dir, Maze **maze)
{
  int move_dist = move_speed * 1;
  Point new_coords = {.x = coords.x, .y = coords.y};
  Point check_cage_1 = {.x = coords.x, .y = coords.y};
  Point check_cage_2 = {.x = coords.x, .y = coords.y};
  switch(dir)
  {
    case MovementDir::UP:
      new_coords.y += move_dist;
      check_cage_1.y += tileSize + move_dist - 1;
      check_cage_2.y += tileSize + move_dist - 1;
      check_cage_2.x += tileSize - 1;
      break;
    case MovementDir::DOWN:
      new_coords.y -= move_dist;
      check_cage_1.y -= move_dist;
      check_cage_2.y -= move_dist;
      check_cage_2.x += tileSize - 1;
      break;
    case MovementDir::LEFT:
      new_coords.x -= move_dist;
      check_cage_1.x -= move_dist;
      check_cage_2.x -= move_dist;
      check_cage_2.y += tileSize - 1;
      break;
    case MovementDir::RIGHT:
      new_coords.x += move_dist;
      check_cage_1.x += tileSize + move_dist - 1;
      check_cage_2.x += tileSize + move_dist - 1;
      check_cage_2.y += tileSize - 1;      
      break;
    default:
      break;
  }
  old_coords.x = coords.x;
  old_coords.y = coords.y;
  coords.x = new_coords.x;
  coords.y = new_coords.y;
  if (!(*maze)->free(check_cage_1) || !(*maze)->free(check_cage_2)) {
    coords.x = old_coords.x;
    coords.y = old_coords.y;
  }
}

void Player::Draw(Image &screen)
{
    for (int i = 0; i < tileSize; i++) {
        for (int j = 0; j < tileSize; j++) {
            int x = coords.x + i;
            int y = coords.y + j;
            screen.PutPixel(x, y, 
                    blend(screen.GetPixel(x, y), player_img.GetPixel(i, tileSize - j - 1)));
    }
  }
}
