#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace cv;
int main(int argc, char* argv[])
{
    Mat frame1, frame2;
    VideoCapture cap1("cdRoll.avi");
    VideoCapture cap2("greenTriangles.avi");

    namedWindow("a", 1);
    namedWindow("b", 1);

    while(true)
    {
        cap1 >> frame1;
        cap2 >> frame2;
        imshow("a", frame1);
        imshow("b", frame2);
        waitKey(30);
    }
    return 0;
}
