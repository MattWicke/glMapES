/*
   I think i fixed the sticky handle problem
   need to impliment a file reader to 
      a) save configs
      b) load playlists
*/
#include "surface.h"
#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <locale>
#include <vector>

#include <math.h>
#include "util.h"

using namespace cv; 
//global vars 
const int FRAMERATE = 15;
bool fullScreen = false;
bool play = false;
int Surface::surfaceCount = 0;
int Surface::activeSurfaceIndex = 0;
bool masterDragState = false;
vector<Surface> surfaces;

int initialize(int _argc, char* _argv[])
{
   const unsigned char* versionString;
   glutInit( &_argc, _argv);
   glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA);
   glutInitWindowSize(1024, 1024);
   glutCreateWindow("junk");
   //glutDisplayFunc(renderCallback);
   glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
   glutSwapBuffers();
   versionString = glGetString(GL_VERSION);
   cerr << "gl version: "<<  versionString << endl;
}


void display_callback(void)
{
   glClear(GL_COLOR_BUFFER_BIT);
   
   for(int i = 0; i < surfaces.size(); i++)
   {
      surfaces[i].draw();
   }

   glutSwapBuffers();
   //glFlush();
}

//void idle_callback(void)
void timer_callback(int value)
{
   if(play)
   {
      for(int i = 0; i < surfaces.size(); i++)
      {
         surfaces[i].update();
      }
   }

   glutPostRedisplay();
   glutTimerFunc(1000/FRAMERATE, timer_callback, 0);
}

double pix2cart(float inval, float max)
{
   return inval* 2.0 /max - 1;
}

int cart2pix(float inval, float max)
{
   return (inval * max  + max) / 2.0;
}

void mouseButtonCB( int _button, int _state, int _x, int _y)
{
   _y = glutGet(GLUT_WINDOW_HEIGHT) - _y;
   int bigI = -1;
   int bigJ = -1;
   
   
//   double tempX = (double)_x / glutGet(GLUT_WINDOW_WIDTH);
//   double tempY = (double)_y / glutGet(GLUT_WINDOW_HEIGHT);

   if( _button == GLUT_LEFT_BUTTON && _state == GLUT_DOWN)
   {
      //cout << "_x = " << _x << " _y = " << _y <<  endl;
      //look for collisions
      for(int i = 0; i < surfaces.size(); i++)
      {
         for(int j = 0; j < Surface::numHandles; j+=2 )
         {
            double tempX = _x - cart2pix(surfaces[i].vertexData[j], glutGet(GLUT_WINDOW_WIDTH));
            double tempY = _y - cart2pix(surfaces[i].vertexData[j+1], glutGet(GLUT_WINDOW_HEIGHT));

            if( sqrt(tempX * tempX +  tempY * tempY) <= surfaces[i].handleRad )
            {
               bigI = i;
               bigJ = j;
               //cout << "caught surface " << i << " handle " << j/2 << endl;
               //cout << "handle x = " << cart2pix(surfaces[i].vertexData[j],
               //                                     glutGet(GLUT_WINDOW_WIDTH));
               //cout << " handle y = " << cart2pix(surfaces[i].vertexData[j+1],
               //                                     glutGet(GLUT_WINDOW_HEIGHT)) << endl;
            }
            if( bigI > -1 && bigJ > -1)
            {
               Surface::activeSurfaceIndex = bigI;
               surfaces[bigI].activeHandleIndex = bigJ;
               masterDragState = true;
            }
         }
      }
            cout << "activeSurfaceIndex " << bigI << endl;
            cout << "surface[bigI].activeHandleIndex " << bigJ << endl;
   }//end mouse down

   if( _button == GLUT_LEFT_BUTTON && _state == GLUT_UP)
   {
      masterDragState = false;
//      surfaces[Surface::activeSurfaceIndex].releaseHandle();
   }
   glutPostRedisplay();
}

void mouseMoveCB( int _x, int _y)
{
   _y = glutGet(GLUT_WINDOW_HEIGHT) - _y; //invert y because 
//   cout << "Moved x " << _x  << " xn " << pix2cart(_x, glutGet(GLUT_WINDOW_WIDTH));
//   cout << " y " << _y << " yn " << pix2cart(_y, glutGet(GLUT_WINDOW_HEIGHT)) << endl;

   if(masterDragState)
   surfaces[Surface::activeSurfaceIndex].dragHandle(pix2cart(_x, glutGet(GLUT_WINDOW_WIDTH)),
                                                   pix2cart(_y, glutGet(GLUT_WINDOW_HEIGHT)));

   glutPostRedisplay();
}

void keyboardCB( unsigned char _key, int _x, int _y)
{
   if(_key == ' ')
   {
      if(fullScreen)
      {
         glutReshapeWindow(1024,1024);
         glutPositionWindow(0,0);
         fullScreen = false;
      }
      else
      {
         glutFullScreen();
         fullScreen = true;
      }
   }
   if(_key == 'p')
   {
      play = !play;
   }
   if(_key == 'r')
   {
      for(int i = 0; i < surfaces.size(); i++)
      {
         surfaces[i].reset();
      }
   }
   if(_key == 's')
   {
      vector<vector<float> > coordData;
      for( int i = 0; i < surfaces.size(); i++)
      {
         vector<float>  tempCoords(surfaces[i].getVertData(), surfaces[i].getVertData()+8);
         coordData.push_back(tempCoords);
      }
      writeCoordsFile( "outfile.clist", coordData);
   }
}

int main(int argc, char* argv[])
{
   char* shaderCode;
   int length;
   GLuint program;
   vector<vector<float> > coordData;
   std::vector<std::vector<std::string>> plists;
   string singleVidName("frac.avi");
   bool usePlaylist = false;
   bool useCoordlist = false;
   bool usePlist = false;

   initialize(argc, argv);
   GLenum ret = glewInit();
   if( ret != GLEW_OK)
   {
      cerr << "main: glew did not init " << glewGetErrorString(ret) << endl;
      return 1;
   }

   glutMouseFunc(mouseButtonCB);
   glutMotionFunc(mouseMoveCB);
   // name off some shaders, and build a program
   // fileNames & shaderTypes are a strict 1to1
   string fileNames[] = { "shader.v.glsl", "shader.f.glsl" };
   GLenum shaderTypes[] = { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER };
   program = make_program( 2, fileNames, shaderTypes );
   glUseProgram(program);

   /*********************** start ******************************/

   // parse the args
   
   if(argc < 2)
   {
      cout << "-v" << endl;
      cout << "-c" << endl;
      return 1;
   }

   for(int i = 1; i < argc; i++)
   {
      string argstring(argv[i]);
      size_t dotIdx = argstring.find_first_of('.');
      string suffix = argstring.substr(dotIdx+1);
      if(suffix == "jpg" || suffix == "bmp" || suffix == "mp4" || suffix == "avi" )
      {
         //cout << "bang" << endl;
//         surfaces.push_back(Surface(argstring, program));
      }
      if(argstring == "-c")
      {
         int length; 
         string coordFilename(argv[i+1]);
         getCoords(coordData, argv[i+1]);         
         cout << "coord data: " << endl;
         useCoordlist = true;
      }
      if(argstring == "-v")
      {
         singleVidName = argv[i+1];
      }
      if(argstring == "-p")
      {
          usePlist = true;
          std::string filename(argv[i+1]);
          getPlist(filename, plists);
          for(size_t ii= 0; ii < plists.size(); ii++)
          {
              for(size_t jj= 0; jj < plists[ii].size(); jj++)
              {
                  std::cout << plists[ii][jj] << std::endl;
              }
              std::cout << std::endl;
          }
      }
   }

   if(!useCoordlist)
   {
   }

   if(usePlist)
   {
       for(int i = 0; i < coordData.size(); i++)
       {
          surfaces.push_back(Surface(plists[i], coordData[i].data(), program));
       }
   }
   else
   {
       std::vector<std::string> tempPlist;
       tempPlist.push_back(singleVidName);
       for(int i = 0; i < coordData.size(); i++)
       {
          surfaces.push_back(Surface(tempPlist, coordData[i].data(), program));
       }
   }


   glutKeyboardFunc(keyboardCB);
   glutDisplayFunc(display_callback);
//   glutIdleFunc(idle_callback);
    std::cout << "loaded" << std::endl;
   glutTimerFunc(100, timer_callback, 0);
   glutMainLoop();
   return 0;
}
