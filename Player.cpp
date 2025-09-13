#include "Player.h"


bool Player::Moved() const
{
  if(coords.x == old_coords.x && coords.y == old_coords.y)
    return false;
  else
    return true;
}

void Player::ProcessInput(MovementDir dir, int MaxRight, int MaxUp, int** levelmatrix)
{
  int move_dist = move_speed * 1;

  switch(dir)
  {
    case MovementDir::DOWN:
      old_coords.y = coords.y;
       coords.y -= move_dist;
       if(coords.y<0)
       coords.y = 0;

       if(levelmatrix[MaxUp/tileSize-coords.y/tileSize-1][coords.x/tileSize]=='#'||
          levelmatrix[MaxUp/tileSize-coords.y/tileSize-1][coords.x/tileSize]=='%'||
          levelmatrix[MaxUp/tileSize-coords.y/tileSize-1][(coords.x+player_sprite.Width()-1)/tileSize]=='#'||
          levelmatrix[MaxUp/tileSize-coords.y/tileSize-1][(coords.x+player_sprite.Width()-1)/tileSize]=='%')

          coords.y=coords.y+(tileSize - coords.y%tileSize);

      break;
    case MovementDir::UP:
      old_coords.y = coords.y;
      coords.y += move_dist;
      if(coords.y>MaxUp-1)
      coords.y=MaxUp-1;

       if(levelmatrix[MaxUp/tileSize-coords.y/tileSize-1][coords.x/tileSize]=='#'||
          levelmatrix[MaxUp/tileSize-coords.y/tileSize-1][coords.x/tileSize]=='%'||
          levelmatrix[MaxUp/tileSize-coords.y/tileSize-1][(coords.x+player_sprite.Width()-1)/tileSize]=='#'||
          levelmatrix[MaxUp/tileSize-coords.y/tileSize-1][(coords.x+player_sprite.Width()-1)/tileSize]=='%')
        coords.y=coords.y-(coords.y%tileSize)-1;

      break;
    case MovementDir::LEFT://set flag FacingRight=false;
      FacingLeft=true;
      old_coords.x = coords.x;
      coords.x -= move_dist;
     if(coords.x<0)
      coords.x=0;

       if(levelmatrix[MaxUp/tileSize-coords.y/tileSize-1][coords.x/tileSize]=='#'||//-1???
          levelmatrix[MaxUp/tileSize-coords.y/tileSize-1][coords.x/tileSize]=='%')
        coords.x=coords.x+(tileSize - coords.x%tileSize);

      break;
    case MovementDir::RIGHT://FacingRight=true;
      FacingLeft=false;
      old_coords.x = coords.x;
      coords.x += move_dist;
   if(coords.x>MaxRight-player_sprite.Width()-1)
     coords.x=MaxRight-player_sprite.Width()-1;

        if(levelmatrix[MaxUp/tileSize-coords.y/tileSize-1][(coords.x+player_sprite.Width()-1)/tileSize]=='#'||
           levelmatrix[MaxUp/tileSize-coords.y/tileSize-1][(coords.x+player_sprite.Width()-1)/tileSize]=='%')
          coords.x=coords.x-((coords.x+player_sprite.Width()-1)%tileSize)-1;//w is >1 but <2 tS

      break;
    default:
      break;
  }
}

void Player::SetSpawnPosition(Point pt)
{
   coords.x=pt.x;coords.y=pt.y;
}

PlayerStates Player::PositionState(int MaxUp, int**levelmatrix)
{

//is on lava?
if (levelmatrix[MaxUp/tileSize-coords.y/tileSize-1][coords.x/tileSize]==' '||
    levelmatrix[MaxUp/tileSize-coords.y/tileSize-1][coords.x/tileSize+1]==' ')
return lavadeath;
//second priority, has reached the level exit?
if (levelmatrix[MaxUp/tileSize-coords.y/tileSize-1][(coords.x-1)/tileSize]=='x'||
    levelmatrix[MaxUp/tileSize-coords.y/tileSize-1][(coords.x+1)/tileSize+1]=='x'||
    levelmatrix[MaxUp/tileSize-(coords.y+1)/tileSize-1][coords.x/tileSize]=='x'||
    levelmatrix[MaxUp/tileSize-(coords.y+1)/tileSize-1][coords.x/tileSize+1]=='x'||
    levelmatrix[MaxUp/tileSize-(coords.y-1)/tileSize-1][coords.x/tileSize]=='x'||
    levelmatrix[MaxUp/tileSize-(coords.y-1)/tileSize-1][coords.x/tileSize+1]=='x')
return levelcleared;
//passive state: is there a breakable wall near, just to be prepared?
if (levelmatrix[MaxUp/tileSize-coords.y/tileSize-1][(coords.x-1)/tileSize]=='%'||
    levelmatrix[MaxUp/tileSize-coords.y/tileSize-1][(coords.x+1)/tileSize+1]=='%'||
    levelmatrix[MaxUp/tileSize-(coords.y+1)/tileSize-1][coords.x/tileSize]=='%'||
    levelmatrix[MaxUp/tileSize-(coords.y+1)/tileSize-1][coords.x/tileSize+1]=='%'||
    levelmatrix[MaxUp/tileSize-(coords.y-1)/tileSize-1][coords.x/tileSize]=='%'||
    levelmatrix[MaxUp/tileSize-(coords.y-1)/tileSize-1][coords.x/tileSize+1]=='%')
return nearbreakablewall;

//no=> nothing is special(of immediate interest)
return normal;


}

Point Player::NearBreakableWall(int MaxRight,int MaxUp,int**levelmatrix)
{
Point pt{.x=-1,.y=-1};

if  (levelmatrix[MaxUp/tileSize-(coords.y+1)/tileSize-1][coords.x/tileSize]=='%')//up 00
{pt.x=coords.x/tileSize;pt.y=MaxUp/tileSize-(coords.y+1)/tileSize-1;return pt;}

if  (levelmatrix[MaxUp/tileSize-(coords.y+1)/tileSize-1][coords.x/tileSize+1]=='%')//up 01
{pt.x=coords.x/tileSize+1;pt.y=MaxUp/tileSize-(coords.y+1)/tileSize-1;return pt;}

if  (levelmatrix[MaxUp/tileSize-(coords.y-1)/tileSize-1][coords.x/tileSize]=='%')//down 00
{pt.x=coords.x/tileSize;pt.y=MaxUp/tileSize-(coords.y-1)/tileSize-1;return pt;}

if  (levelmatrix[MaxUp/tileSize-(coords.y-1)/tileSize-1][coords.x/tileSize+1]=='%')//down 01
{pt.x=coords.x/tileSize+1;pt.y=MaxUp/tileSize-(coords.y-1)/tileSize-1;return pt;}

if  (levelmatrix[MaxUp/tileSize-coords.y/tileSize-1][(coords.x+1)/tileSize+1]=='%')//right 01
{pt.x=(coords.x+1)/tileSize+1;pt.y=MaxUp/tileSize-coords.y/tileSize-1;return pt;}

if  (levelmatrix[MaxUp/tileSize-coords.y/tileSize-1][(coords.x-1)/tileSize]=='%')//left 00
{pt.x=(coords.x-1)/tileSize;pt.y=MaxUp/tileSize-coords.y/tileSize-1;return pt;}

return pt;


}

void Player::Draw(Image &screenbkg,Image &camera)
{

  if(Moved())
    old_coords = coords;

//refill the buffer, in other words, and then put the player in there too

  Pixel tp;

//camera shift:
  int shiftup=0,shiftright=0;
  if(camera.Width()<screenbkg.Width()){
      if (coords.x>camera.Width()/2 && coords.x<screenbkg.Width()-camera.Width()/2) shiftright=coords.x-camera.Width()/2;
      else if (coords.x>=screenbkg.Width()-camera.Width()/2) shiftright=screenbkg.Width()-camera.Width();
  }

  if(camera.Height()<screenbkg.Height()){
     if (coords.y>camera.Height()/2 && coords.y<screenbkg.Height()-camera.Height()/2) shiftup=coords.y-camera.Height()/2;
     else if (coords.y>=screenbkg.Height()-camera.Height()/2)shiftup=screenbkg.Height()-camera.Height();
     if (coords.y+player_sprite.Height()>screenbkg.Height()) shiftup+=coords.y+player_sprite.Height()-screenbkg.Height();
  }

//filling camera shot-every time, cant just ride on old replacing minimal sprite-shaped space
 if (coords.y+player_sprite.Height()>screenbkg.Height()){

   for(int y=camera.Height()-(coords.y+player_sprite.Height()-screenbkg.Height());y<camera.Height();y++){
     for(int x=0;x<camera.Width();x++){
        camera.PutPixel(x,y,Pixel{0,0,0,255});
 }}
  for(int y=0;y<camera.Height()-(coords.y+player_sprite.Height()-screenbkg.Height());y++){
    for(int x=0;x<camera.Width();x++){
        camera.PutPixel(x,y,screenbkg.GetPixel(x+shiftright,y+shiftup));
    }
  }
 }
 else{
   for(int y=0;y<camera.Height();y++){
     for(int x=0;x<camera.Width();x++){
      camera.PutPixel(x,y,screenbkg.GetPixel(x+shiftright,y+shiftup));

     }
   }

 }


  for(int y = coords.y-shiftup; (y < coords.y-shiftup + player_sprite.Height())&&(y < camera.Height())/*tileSize*/; y++)
  {
    for(int x = coords.x-shiftright; (x < coords.x-shiftright + player_sprite.Width())&&(x < camera.Width())/*tileSize*/; x++)
    {

      if(FacingLeft){
         tp=player_sprite.GetPixel(x-(coords.x-shiftright),player_sprite.Height()-(y-(coords.y-shiftup))-1);
         if (! ( ((tp.r==255)&&(tp.g==255)&&(tp.b==255)) || (tp.a==0) ) )
              camera.PutPixel(x, y, tp);
      }
      else{
         tp=player_sprite.GetPixel(player_sprite.Width()-(x-(coords.x-shiftright))-1,player_sprite.Height()-(y-(coords.y-shiftup))-1);
         if (! ( ((tp.r==255)&&(tp.g==255)&&(tp.b==255)) || (tp.a==0) ) )
              camera.PutPixel(x, y, tp);

      }

    }
  }

}
