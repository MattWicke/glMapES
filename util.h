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
#include "ogl.h"
using namespace cv;

using namespace std;


bool file_reader(const char* fileName, string& contents, int &length);

bool getCoords(vector<vector<float> >& data,  string _inString);
bool writeCoordsFile(string _fileName, const vector<vector<float> > _data);
bool getPlist(std::string filename, std::vector<std::vector<std::string>>& plists);
