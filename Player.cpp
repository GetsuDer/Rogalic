#include "Player.h"
#include <iostream>
Image player_img("resources/player/0.png");

bool Player::Moved() const
{
  if(coords.x == old_coords.x && coords.y == old_coords.y)
    return false;
  else
    return true;
}

void Player::ProcessInput(MovementDir dir, Maze **maze, int mouseX, int mouseY)
{
  int move_dist = move_speed * 1;
  Point new_coords = {.x = coords.x, .y = coords.y};
  Point check_cage_1 = {.x = coords.x, .y = coords.y};
  Point check_cage_2 = {.x = coords.x, .y = coords.y};
  
  active = false;

  Fire *fire = NULL;
  int firex, firey;
  double xspeed, yspeed, speed;
  switch(dir)
  {
    case MovementDir::UP:
      active = true;
      new_coords.y += move_dist;
      check_cage_1.y += tileSize + move_dist - 1;
      check_cage_2.y += tileSize + move_dist - 1;
      check_cage_2.x += tileSize - 1;
      break;
    case MovementDir::DOWN:
      active = true;
      new_coords.y -= move_dist;
      check_cage_1.y -= move_dist;
      check_cage_2.y -= move_dist;
      check_cage_2.x += tileSize - 1;
      break;
    case MovementDir::LEFT:
      active = true;
      look = MovementDir::LEFT;
      new_coords.x -= move_dist;
      check_cage_1.x -= move_dist;
      check_cage_2.x -= move_dist;
      check_cage_2.y += tileSize - 1;
      break;
    case MovementDir::RIGHT:
      active = true;
      look = MovementDir::RIGHT;
      new_coords.x += move_dist;
      check_cage_1.x += tileSize + move_dist - 1;
      check_cage_2.x += tileSize + move_dist - 1;
      check_cage_2.y += tileSize - 1;      
      break;
    case MovementDir::ACTION:
      if (keys_obtained > 0) {
        if ((*maze)->open_nearest_door(coords)) {
            keys_obtained--;
        }
      }
      break;
    case MovementDir::FIRE:
      firex = coords.x + tileSize / 2;
      firey = coords.y + tileSize / 2;
    
      xspeed = mouseX - coords.x;
      yspeed = (1024 - mouseY) - coords.y;
      speed = sqrt(xspeed * xspeed + yspeed * yspeed);
      fire = new Fire(firex, firey, (xspeed / speed) * 7, (yspeed / speed) * 7);
      (*maze)->fires.push_back(fire);
      break;
    case MovementDir::NONE:
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
  int change = (*maze)->opened_door(coords);
  if (change != -1) {
      *maze = (*maze)->others[change];
      coords = (*maze)->from_door((change + 2) % 4);
      coords.x *= tileSize;
      coords.y *= tileSize;
      return;
  }
  keys_obtained += (*maze)->update_keys(coords);
  if ((*maze)->fell(coords)) {
      state = PlayerState::DEAD;
      return;
  }
  if ((*maze)->reach_exit(coords)) {
      state = PlayerState::WIN;
      return;
  }

  int strike = (*maze)->attack(coords);
  if (strike > 0) {
      hitpoints -= strike;
      if (hitpoints <= 0) {
          state = PlayerState::DEAD;
          hitpoints = 0;
      }
      return;
  }
}

Point
Player::placed() {
    return coords;
}
constexpr Pixel red {.r = 255, .g = 0, .b = 0, .a = 0};

void Player::Draw(Image &screen)
{
    Image *img = (active) ? walk_animation.Draw() : walk_animation.Draw(0);
    for (int i = 0; i < tileSize; i++) {
        for (int j = 0; j < tileSize; j++) {
            int x = coords.x + i;
            int y = coords.y + j;
            screen.PutPixel(x, y, 
                    blend(screen.GetPixel(x, y), 
                        img->GetPixel((look == MovementDir::RIGHT ? i : tileSize - i - 1), 
                                        tileSize - j - 1)));
        }
    }
    for (int i = 0; i * 6 < hitpoints; i++) {
        for (int j = 0; j < 5; j++) {
            screen.PutPixel(coords.x + tileSize / 2 + i, coords.y + tileSize + tileSize / 3 + j, red);
            screen.PutPixel(coords.x + tileSize / 2 - i, coords.y + tileSize + tileSize / 3 + j, red);
        }
    }
}
