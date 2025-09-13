#ifndef MAIN_PLAYER_H
#define MAIN_PLAYER_H

#include "Image.h"

struct Point
{
  int x;
  int y;
};

enum class MovementDir
{
  UP,
  DOWN,
  LEFT,
  RIGHT
};

enum PlayerStates{normal,gamefinished,levelcleared,nearbreakablewall,leveltransition,lavadeath};

struct Player
{
  explicit Player(Point pos = {.x = 10, .y = 10}, const std::string &a_path=NULL) :
                 coords(pos), old_coords(coords), player_sprite(a_path), FacingLeft(true) {};

  bool Moved() const;
  void ProcessInput(MovementDir dir, int MaxRight, int MaxUp, int**levelmatrix);
  void SetSpawnPosition(Point pt);
  void Draw(Image &screenbkg, Image &camera);
  PlayerStates PositionState(int MaxUp, int**levelmatrix);
  Point NearBreakableWall(int MaxRight,int MaxUp,int**levelmatrix);

private:
  Point coords {.x = 10, .y = 10};
  Point old_coords {.x = 10, .y = 10};
  Pixel color {.r = 255, .g = 255, .b = 0, .a = 255};
  Image player_sprite;
  bool FacingLeft;
  int move_speed = 4;

};

#endif //MAIN_PLAYER_H
