/* 
 * File:   Texture.h
 */

#ifndef TEXTURE_H
#define TEXTURE_H 

class Texture
{
public:
   Texture(){};
   Texture(GLuint texture, float width, float height, float imageWidth, float imageHeight) 
         { tex = texture; w = width; h = height; imageW = imageWidth; imageH = imageHeight;};

   GLuint tex;
   float w;
   float h;

   float imageW;
   float imageH;
};

#endif	/* TEXTURE_H */

