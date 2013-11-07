#include "SDL2/SDL.h"
#include "SDL2/SDL_opengl.h"
#include "SDL2/SDL_image.h"
#include <iostream>
#include "Texture.h"
using namespace std;

extern SDL_Window *window;
bool SDL_GL_Enter2DMode();

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

//Initialize SDL and other libraries///////////////////////////////////
int Initialize()
{
   //OpenGL context
   SDL_GLContext context;
   
   //Initialize SDL
   SDL_Init( SDL_INIT_VIDEO );
   
   //Use OpenGL 2.1
   SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 2 );
   SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );

   window = SDL_CreateWindow("Test...",
                                SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED,
                                SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL);// | SDL_WINDOW_FULLSCREEN); //SDL_WINDOW_SHOWN);
   
   context = SDL_GL_CreateContext(window);
   
   if ( window == NULL ) {
      fprintf(stderr, "Couldn't create window: %s\n", SDL_GetError());
      exit(1);
   }

   if ( context == NULL ) {
      fprintf(stderr, "Couldn't create OpenGL context: %s\n", SDL_GetError());
      exit(1);
   }

   //Initialize OpenGL
   SDL_GL_Enter2DMode();
   
   //Use Vsync
   if( SDL_GL_SetSwapInterval( 1 ) < 0 )
   {
      printf( "Warning: Unable to set VSync! SDL Error: \n");//, SDL_GetError() );
   }

	return true;
}
///////////////////////////////////////////////////////////////////////

//Initialize Open GL///////////////////////////////////////////////////
bool SDL_GL_Enter2DMode()
{
   // Note, there may be other things you need to change,
   //   depending on how you have your OpenGL state set up.
   glPushAttrib(GL_ENABLE_BIT);
   glDisable(GL_DEPTH_TEST);
   glDisable(GL_CULL_FACE);
   glEnable(GL_TEXTURE_2D);

   //This allows alpha blending of 2D textures with the scene 
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();

   glOrtho(0.0, (GLdouble)SCREEN_WIDTH, (GLdouble)SCREEN_HEIGHT, 0.0, 0.0, 1.0);

   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glLoadIdentity();

   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
   
   if( glGetError() != GL_NO_ERROR )
   {
      cerr<<"Unable to init GL"<<endl;
      exit(1);
   }

	return true;

}
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
void SDL_GL_Leave2DMode()
{
   glMatrixMode(GL_MODELVIEW);
   glPopMatrix();

   glMatrixMode(GL_PROJECTION);
   glPopMatrix();

   glPopAttrib();
}
///////////////////////////////////////////////////////////////////////

// Quick utility function for texture creation/////////////////////////
static int power_of_two(int input)
{
   int value = 1;

   while ( value < input ) {
      value <<= 1;
   }
   return value;
}
///////////////////////////////////////////////////////////////////////

//Load an image file and optimize it (also set color transparency key)/
Texture loadImage(std::string name)
{
   SDL_Surface *surface;
   surface = IMG_Load(name.c_str());

   GLuint texture;
   int w, h;
   SDL_Surface *image;
   SDL_Rect area;
   Uint8  saved_alpha;
   SDL_BlendMode saved_mode;

   /* Use the surface width and height expanded to powers of 2 */
   w = power_of_two(surface->w);
   h = power_of_two(surface->h);

   image = SDL_CreateRGBSurface(
         SDL_SWSURFACE,
         w, h,
         32,
#if SDL_BYTEORDER == SDL_LIL_ENDIAN /* OpenGL RGBA masks */
         0x000000FF, 
         0x0000FF00, 
         0x00FF0000, 
         0xFF000000
#else
         0xFF000000,
         0x00FF0000, 
         0x0000FF00, 
         0x000000FF
#endif
             );
   if ( image == NULL ) {
      cerr<<"Failure";
      exit(1);
   }

   /* Save the alpha blending attributes */
   SDL_GetSurfaceAlphaMod(surface, &saved_alpha);
   SDL_SetSurfaceAlphaMod(surface, 0xFF);
   SDL_GetSurfaceBlendMode(surface, &saved_mode);
   SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_NONE);

   /* Copy the surface into the GL texture image */
   area.x = 0;
   area.y = 0;
   area.w = surface->w;
   area.h = surface->h;
   SDL_BlitSurface(surface, &area, image, &area);

   /* Restore the alpha blending attributes */
   SDL_SetSurfaceAlphaMod(surface, saved_alpha);
   SDL_SetSurfaceBlendMode(surface, saved_mode);

   /* Create an OpenGL texture for the image */
   glGenTextures(1, &texture);
   glBindTexture(GL_TEXTURE_2D, texture);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexImage2D(GL_TEXTURE_2D,
           0,
           GL_RGBA,
           w, h,
           0,
           GL_RGBA,
           GL_UNSIGNED_BYTE,
           image->pixels);
   SDL_FreeSurface(image); /* No longer needed */

   Texture t(texture, w, h, surface->w, surface->h);

   SDL_FreeSurface(surface);

   return t;
 
}
//////////////////////////////////////////////////////////////////////


//Shutdown and free all SDL_Surfaces///////////////////////////////////
int Shutdown()
{
	SDL_Quit();
	return 0;
}
///////////////////////////////////////////////////////////////////////
