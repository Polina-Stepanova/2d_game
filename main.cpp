#include "common.h"
#include "Image.h"
#include "Player.h"
#include <stdio.h>
#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <string.h>
constexpr GLsizei WINDOW_WIDTH = 480, WINDOW_HEIGHT = 480; //used as width and height of the visible through the camera part of a level

struct InputState
{
  bool keys[1024]{}; //массив состояний кнопок - нажата/не нажата
  GLfloat lastX = 400, lastY = 300; //исходное положение мыши
  bool firstMouse = true;
//  bool captureMouse         = true;  // Мышка захвачена нашим приложением или нет?
  bool captureMouse         = false;  // Мышка захвачена нашим приложением или нет?
  bool capturedMouseJustNow = false;
} static Input;


GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;


void OnKeyboardPressed(GLFWwindow* window, int key, int scancode, int action, int mode)
{
   switch (key)
   {
      case GLFW_KEY_ESCAPE:
         if (action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GL_TRUE);
      break;
      case GLFW_KEY_1:
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      break;
      case GLFW_KEY_2:
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
      break;
      default:
            if (action == GLFW_PRESS)
               Input.keys[key] = true;
            else if (action == GLFW_RELEASE)
               Input.keys[key] = false;
   }
}

void processPlayerMovement(Player &player, int WinW, int WinH,int** levelmatrix)
{
  if (Input.keys[GLFW_KEY_UP])
     player.ProcessInput(MovementDir::UP, WinW, WinH,levelmatrix);
  else if (Input.keys[GLFW_KEY_DOWN])
     player.ProcessInput(MovementDir::DOWN,WinW, WinH,levelmatrix);
  if (Input.keys[GLFW_KEY_LEFT])
     player.ProcessInput(MovementDir::LEFT,WinW, WinH,levelmatrix);
  else if (Input.keys[GLFW_KEY_RIGHT])
     player.ProcessInput(MovementDir::RIGHT,WinW, WinH,levelmatrix);
}

void OnMouseButtonClicked(GLFWwindow* window, int button, int action, int mods)
{
  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    Input.captureMouse = !Input.captureMouse;

  if (Input.captureMouse)
  {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    Input.capturedMouseJustNow = true;
  }
  else
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void OnMouseMove(GLFWwindow* window, double xpos, double ypos)
{
  if (Input.firstMouse)
  {
    Input.lastX = float(xpos);
    Input.lastY = float(ypos);
    Input.firstMouse = false;
  }

  GLfloat xoffset = float(xpos) - Input.lastX;
  GLfloat yoffset = Input.lastY - float(ypos);

  Input.lastX = float(xpos);
  Input.lastY = float(ypos);
}


void OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{
  // ...
}


int initGL()
{
   if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
   {
      std::cout << "Failed to initialize OpenGL context" << std::endl;
      return -1;
   }
   std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
   std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
   std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
   std::cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
   std::cout << "Controls: "<< std::endl;
   std::cout << "up/down/left/right arrows - movement  "<< std::endl;
   std::cout << "keyboard 'e' - destroy a breakable wall segment "<< std::endl;
   std::cout << "press ESC to exit" << std::endl;
   return 0;
}

void FadeinFadeout(Image &img,double percent)
{
   Pixel tp;

   if((percent>-1)&&(percent<1)){
      if(percent<0) percent=(-1)*percent;

      for(int y=0;y<img.Height();y++){
         for(int x=0;x<img.Width();x++){
            tp=img.GetPixel(x,y);
            tp.r=(uint8_t)tp.r*percent;
            tp.g=(uint8_t)tp.g*percent;
            tp.b=(uint8_t)tp.b*percent;
            img.PutPixel(x,y,tp);
         }
      }
   }
}

Point LoadBkgBuffer(int**levelmatrix,Image &screenBufferBackground,Image &img_empty_lava,Image &img_floor,Image &img_wall, Image &img_breakable_wall,Image &img_door)
{
   int playerspawn_x=0,playerspawn_y=0,i,j;
   for(int x2=0;x2<screenBufferBackground.Width();x2++){//pixels of the window, 0--WinW/H
      for(int y2=0;y2<screenBufferBackground.Height();y2++){
        i=(screenBufferBackground.Height()/tileSize)-(y2/tileSize)-1;j=x2/tileSize;
        switch(levelmatrix[i][j]){
           case ' '://lava=empty
             screenBufferBackground.PutPixel(x2,y2,img_empty_lava.GetPixel((x2%tileSize),tileSize-(y2%tileSize)-1));
           break;
           case '@'://draw door in the nearest wall and floor under it, save coords for player initialization?spawnpoint
              screenBufferBackground.PutPixel(x2,y2,img_floor.GetPixel((x2%tileSize),tileSize-(y2%tileSize)-1));
              playerspawn_x=j;playerspawn_y=i;
           break;
           case '.'://floor
              screenBufferBackground.PutPixel(x2,y2,img_floor.GetPixel((x2%tileSize),tileSize-(y2%tileSize)-1));
           break;
           case '#'://wall
              screenBufferBackground.PutPixel(x2,y2,img_wall.GetPixel((x2%tileSize),tileSize-(y2%tileSize)-1));
           break;
           case '%'://breakable wall
              screenBufferBackground.PutPixel(x2,y2,img_breakable_wall.GetPixel((x2%tileSize),tileSize-(y2%tileSize)-1));
           break;
           case 'x'://the door - exit
              screenBufferBackground.PutPixel(x2,y2,img_door.GetPixel((x2%tileSize),tileSize-(y2%tileSize)-1));
           break;
           default:  break;
        }
      }
   }

  Point rez{.x = playerspawn_x*tileSize, .y = screenBufferBackground.Height()-(playerspawn_y+1)*tileSize};
  return rez;
}

int ReadLevelList(char **&levels)
{
   FILE* f;
   int c=0,N_Levels=0,i=0,j=0;
   //get names of level map files from a special txt file
   f=fopen("./resources/labyrinth_map_list.txt","r");
   if(!f){perror("labyrinth_map_list.txt");exit(1);}
   while (c!=EOF){
      c=fgetc(f);
      if(c=='\n')N_Levels++;
      }

   levels = new char*[N_Levels];

   char* s=(char*)malloc(1),*st;s[0]=0;
   std::string s1="./resources/.txt";

   fseek(f,0,SEEK_SET);c=0;
   while(c!=EOF)
   {
      c=fgetc(f);
      if(c!='\n'&&c!='\t'&&c!='\r'){

         st=(char*)malloc(strlen(s)+1+1);
         for(int k=0;k<strlen(s);k++)
            st[k]=s[k];
          st[strlen(s)]=c;
          st[strlen(s)+1]=0;
          free(s);
          s=st;
      }
      if(c=='\n'){

        levels[i]=(char*)malloc(strlen(s)+1+16);//16 is length of extra additions to the name from s1

        for(int k=0;k<12;k++) levels[i][k]=s1[k];
        for(int k=0;k<strlen(s);k++) levels[i][k+12]=s[k];
        for(int k=0;k<4;k++) levels[i][strlen(s)+k+12]=s1[k+12];
        levels[i][strlen(s)+16]=0;
        free(s);
        s=(char*)malloc(1);s[0]=0;
        i++;
      }
   }

   free(s);

   fclose(f);//read all level map file names
   return N_Levels;

}

Point LoadLevelMap(int**&levelmatrix,char**levels,int Level_Counter)
{
   FILE* f;
   int Nlines=0,linelength=0,c=0,i=0,j=0;

   f =fopen(levels[Level_Counter-1],"r");
   if(!f){perror(levels[Level_Counter-1]);exit(1);}

   while (c!=EOF){
     c=fgetc(f);
     if((c!='\n')&&(c!='\r')&&(c!='\t')&&(!Nlines))
        linelength++;
     if(c=='\n')Nlines++;
   }

   levelmatrix=new int*[Nlines];
   for(i=0;i<Nlines;i++)
      levelmatrix[i]=new int[linelength];

   fseek(f,0,SEEK_SET);
   c=0,i=0;
   while (c!=EOF){

     c=fgetc(f);
     if((c!='\n')&&(c!='\r')&&(c!='\t'))  levelmatrix[i][j]=c;

     if(j==linelength){j=0;i++;}
     else j++;
     if(i==Nlines) break;
   }

   fclose(f);
   //got the map
   Point sizepair{.x=linelength*tileSize,.y=Nlines*tileSize};
   return sizepair;
}

void DrawMessage(Image &cameraBuffer,Image &message)
{
   for(int x2=0;x2<message.Width();x2++){
      for(int y2=0;y2<message.Height();y2++)
         cameraBuffer.PutPixel(x2+WINDOW_WIDTH/2-(message.Width()/2),WINDOW_HEIGHT/2+(message.Height()/2)-y2,message.GetPixel(x2,y2));
   }
}

int main(int argc, char** argv)
{
   if(!glfwInit())
   return -1;

   glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

   int N_Levels=0,i=0,j=0;

   char **levels;
   N_Levels=ReadLevelList(levels);//a special txt file in resources stores all names of level map files in order of level progression

   int Level_Counter=1;

   int** levelmatrix;
   Point dimensions=LoadLevelMap(levelmatrix,levels,Level_Counter);
   int WinH=dimensions.y, WinW=dimensions.x;

   GLFWwindow*  window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Escape the labyrinth", nullptr, nullptr);
   if (window == nullptr)
   {
      std::cout << "Failed to create GLFW window" << std::endl;
      glfwTerminate();
      return -1;
   }
   glfwMakeContextCurrent(window);
   glfwSetKeyCallback(window, OnKeyboardPressed);
   glfwSetCursorPosCallback(window, OnMouseMove);
   glfwSetMouseButtonCallback(window, OnMouseButtonClicked);
   glfwSetScrollCallback(window, OnMouseScroll);

   if(initGL() != 0)
      return -1;

   //Reset any OpenGL errors which could be present for some reason
   GLenum gl_error = glGetError();
   while (gl_error != GL_NO_ERROR)
	gl_error = glGetError();


   Image img_empty_lava("./resources/lava_pustota2.bmp");
   Image img_floor("./resources/pol.bmp");
   Image img_wall("./resources/nelomabelnaya_stena.bmp");
   Image img_breakable_wall("./resources/lomabelnaya_stena2.bmp");
   Image img_door("./resources/dver.bmp");
   Image img_death("./resources/youdied.bmp");
   Image img_completion("./resources/gamefinished.bmp");
   Image img_levelcleared("./resources/levelcleared.bmp");

   Image cameraBuffer(WINDOW_WIDTH, WINDOW_HEIGHT, 4);
   Image screenBufferBackground(WinW, WinH, 4);

   Point starting_pos=LoadBkgBuffer(levelmatrix,screenBufferBackground,img_empty_lava,img_floor,img_wall,img_breakable_wall,img_door);
   Player player{starting_pos,"./resources/hero2.bmp"};
   PlayerStates PlayerState=normal;//PlayerStates is a enum in Player.h

   glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);  GL_CHECK_ERRORS;
   glClearColor(0.0f, 0.0f, 0.0f, 1.0f); GL_CHECK_ERRORS;

   double Timer;//for fadein-fadeout level transition effect

   //game loop
   while (!glfwWindowShouldClose(window))
   {
      GLfloat currentFrame = glfwGetTime();
      deltaTime = currentFrame - lastFrame;
      lastFrame = currentFrame;
      glfwPollEvents();

      if(PlayerState==normal){//only paying attention to player movement if the game is still in active progress
         processPlayerMovement(player,WinW,WinH,levelmatrix);
         PlayerState=player.PositionState(WinH,levelmatrix);
         player.Draw(screenBufferBackground,cameraBuffer);


         switch(PlayerState){
            case lavadeath://hit lava(empty) tiles
               DrawMessage(cameraBuffer,img_death);
            break;
            case levelcleared://if this is last level in level list taken from another txt file=>full stop reaction, like the death one, if there is still a next level - different
               if(Level_Counter<N_Levels){//not the last level
                  DrawMessage(cameraBuffer,img_levelcleared);
               }
               else {//== means game finished, from this point on will only react to esc, for exiting
                  DrawMessage(cameraBuffer,img_completion);
                  PlayerState=gamefinished;//game finished, analog. -1 but different screen message
               }
            break;
            case nearbreakablewall://breakable wall is nearby
               if(Input.keys[GLFW_KEY_E]){
                  /*destroy(replace w\ floor) nearest breakable wall tile - in background buffer and in the matrix-map*/
                  Input.keys[GLFW_KEY_E]=false;
                  Point pt=player.NearBreakableWall(WinW,WinH,levelmatrix);
                  if((pt.x>=0)&&(pt.y>=0)){
                     levelmatrix[pt.y][pt.x]='.';
                     for(int i=0;i<tileSize;i++){
                        for(int j=0;j<tileSize;j++){
                           screenBufferBackground.PutPixel(tileSize*pt.x+j,WinH-tileSize*(pt.y+1)+i,img_floor.GetPixel(j,tileSize-i-1));
                        }
                     }
                  }
               }
               PlayerState=normal;
            break;
            default:
            break;
         }
      }
      else{
         if(PlayerState==levelcleared){//awaiting space pressed
            if(Input.keys[GLFW_KEY_SPACE]){
               Timer=-2;PlayerState=leveltransition;//begin fadein-fadeout process with loading next level's map in the middle. Two seconds for fadeout(Timer=-2 to 0), two seconds for fadein(0 to 2)
            }
         }
         else
            if(PlayerState==leveltransition){
               //fade level transition
               Timer+=deltaTime;
               if((Timer-deltaTime<=0)&&(Timer>=0)){
               //load map of the level, refill background and  camera buffers
                  Level_Counter++;
                  dimensions=LoadLevelMap(levelmatrix,levels,Level_Counter);
                  WinH=dimensions.y; WinW=dimensions.x;

                  screenBufferBackground=Image(WinW,WinH,4);
                  starting_pos=LoadBkgBuffer(levelmatrix,screenBufferBackground,img_empty_lava,img_floor,img_wall,img_breakable_wall,img_door);
                  player.SetSpawnPosition(starting_pos);
                  PlayerState=leveltransition;

                  player.Draw(screenBufferBackground,cameraBuffer);
                  FadeinFadeout(cameraBuffer,0);
               }
               else if(Timer>2){
                  PlayerState=normal;
                  player.Draw(screenBufferBackground,cameraBuffer);
               }
               else{//<0 and between 0+d(load) and 2-d(>2,finished transition) - refilling camera buffer and dimming its color according to the timer
                  player.Draw(screenBufferBackground,cameraBuffer);
                  FadeinFadeout(cameraBuffer,Timer/2);
               }
            }
      }

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;
      glDrawPixels (WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, cameraBuffer.Data()); GL_CHECK_ERRORS;
      glfwSwapBuffers(window);
   }
   glfwTerminate();

   return 0;
}
