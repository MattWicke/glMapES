#pragma once
#include <chrono>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "util.h"
#include "types.h"
#include "ogl.h"
class Surface
{
    enum Mode{ STILL, VIDEO }mode;
    static int surfaceCount;

    public:
    //local variables
    static const int numHandles = 8;
    static int activeSurfaceIndex;
    int activeHandleIndex;
    int width, height;
    int surfaceID;
    int handleRad;
    int frameIndex;
    Myglpoint srcCorners[numHandles];
    VideoCapture vidCap;
    Mat frame;
    std::vector<Mat> frameVec;
    std::vector<string> plist;
    int plistIndex;
    int repeatCount;
    GLuint vertexArrayObj;
    GLuint texCoordObj;
    GLuint texBuffer;
    GLuint textureObj;
    GLuint vboVert;
    GLuint vboTexcoords;
    GLuint program; 
    GLfloat vertexData[8];

    Surface(std::vector<string> _plist, GLfloat* _vertexData, GLuint _program, int _handleRad=35);
    void draw();
    void update();
    void dragHandle(double _x, double _y);
    void releaseHandle();
    float* getVertData();
    void reset();
};
