#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdio.h>
#include <GL/glew.h>    //you must include glew before glut
#include <GL/freeglut.h>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "types.h"
#include "util.h"

using namespace std;
using namespace cv;

void print_log(GLuint object);
GLuint make_shader(char* fileName, GLenum type);
GLuint make_program( int count, string fileNames[], GLenum type[]);
GLint get_attribute( GLuint program, const char *name);
void get_error(void);
GLuint make_shader(char* fileName, GLenum type);
GLuint make_program( int count, string fileNames[], GLenum type[]);
GLint get_attribute( GLuint program, const char *name);
void get_error(void);
bool make_texture(GLuint& textureObj, ImageData data);
bool cvLoad_image( string fileName, Mat dataContainer );
bool cvMake_texture(GLuint& textureObj, Mat dataContainer, GLuint& texBuffer);
void cv_update_texture( Mat dataContainer, GLuint texBuffer);
