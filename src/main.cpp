#include "SDL2/SDL.h"
#include "SDL2/SDL_opengl.h"
#include "SDL2/SDL_image.h"
#include <iostream>
#include "Texture.h"
#include <chrono>
#include <thread>

using namespace std;

//ControlFunctions.cpp Function Declarations/////////
Texture loadImage(std::string name);
int Initialize();
int Shutdown();
bool SDL_GL_Enter2DMode();
void SDL_GL_Leave2DMode();
/////////////////////////////////////////////////////

SDL_Window *window = NULL;

Texture background;
float bgx = 0;
float bgy = 0;

bool done = false;


void Draw(const Texture& texture, float x, float y, SDL_Rect* clip = NULL)
{  // Draw -- //////////////////////////
   
   //Texture coordinates
   GLfloat texTop = 0.f;
   GLfloat texBottom = 1.f;
   GLfloat texLeft = 0.f;
   GLfloat texRight = 1.f;

   //Vertex coordinates
   GLfloat quadWidth = texture.w;
   GLfloat quadHeight = texture.h; 

   if( clip != NULL )
   {   
      //Texture coordinates
      texLeft = clip->x / texture.w;
      texRight = ( clip->x + clip->w ) / texture.w;
      texTop = clip->y / texture.h;
      texBottom = ( clip->y + clip->h ) / texture.h;

      //Vertex coordinates
      quadWidth = clip->w;
      quadHeight = clip->h;
   }   

   glBegin( GL_QUADS );
      glTexCoord2f(  texLeft,    texTop ); glVertex2f( x,               y );
      glTexCoord2f( texRight,    texTop ); glVertex2f( x +quadWidth,    y );
      glTexCoord2f( texRight, texBottom ); glVertex2f( x +quadWidth,    y + quadHeight );
      glTexCoord2f(  texLeft, texBottom ); glVertex2f( x,               y +quadHeight );
   glEnd();

/*ALTERNATIVE METHOD - push matrix, translate to the point, draw, pop matrix back
   //Move to rendering point
   glPushMatrix();
   glTranslatef( x, y, 0.f );

   // Bind the texture to which subsequent calls refer to
   glBindTexture( GL_TEXTURE_2D, texture.tex);

   //Render textured quad
   glBegin( GL_QUADS );
      glTexCoord2f(  texLeft,    texTop ); glVertex2f(       0.f,        0.f );
      glTexCoord2f( texRight,    texTop ); glVertex2f( quadWidth,        0.f );
      glTexCoord2f( texRight, texBottom ); glVertex2f( quadWidth, quadHeight );
      glTexCoord2f(  texLeft, texBottom ); glVertex2f(       0.f, quadHeight );
   glEnd();

   glPopMatrix();*/
}

void update()
{
   bgx++;
   bgy++;

   SDL_Event event;

   while(SDL_PollEvent(&event))
   {   
      switch(event.type)
      {   
         case SDL_KEYDOWN:
            switch(event.key.keysym.sym)
            {
               case SDLK_ESCAPE:
                  done = true;
                  break;
            }
      }
   }

}

void render()
{
   Draw(background, bgx, bgy);
}


using namespace std::chrono;
int main(int argc, char* argv[])
{
   Initialize(); //Start SDL

   background = loadImage("images/cloudTile.png");

   std::chrono::time_point<std::chrono::system_clock> start, end;
   
   while(done != true) //game loop logic
   { 
      start = std::chrono::system_clock::now();
      update();
      render();

      //Display rendered data to the screen
      SDL_GL_SwapWindow(window);
      glClear( GL_COLOR_BUFFER_BIT );


      end = std::chrono::system_clock::now();
      std::chrono::duration<double> elapsed_seconds = end-start;
      float timer = elapsed_seconds.count() * 1000;
      std::cout<< "Frame Time: " << timer << "ms\n";

   /*   if(timer < 16.66)
      {
         float t = (16.66666666 - timer) * 1000000;
         cout<<"Sleeping for :"<<16.66 - timer<<" ms"<<endl;
         this_thread::sleep_for(nanoseconds((long)t));
      }*/
   }

   Shutdown();

   return 0;
}
             
