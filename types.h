#pragma once
#include <GL/glew.h>    //you must include glew before glut
#include <GL/freeglut.h>
#include <string>
using namespace std;

struct ImageData
{
   GLsizei width;
   GLsizei height;
   GLubyte* data;
};

struct shaderInfo{
   GLuint shader;
   string fileName;
   GLenum type;
};

struct Element
{
   unsigned char b;
   unsigned char g;
   unsigned char r;
};

struct Myglpoint
{
   GLfloat x;
   GLfloat y;
};
