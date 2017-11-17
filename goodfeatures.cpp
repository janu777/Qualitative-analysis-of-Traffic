
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/videoio/videoio.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <vector>

#include <iostream>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <numeric>

using namespace cv;
using namespace std;


int main( int argc, char** argv )
{



VideoCapture cap("/home/itslab/Downloads/cfiles/QueueLength2.mp4");

for(;;){

Mat image;
cap >> image;

    //VideoWriter outputvideo("output",CV_FOURCC('M','J','P','G'), 30, sample.size(), true);

    //VideoWriter outputvideo("output.asf",CV_FOURCC('J','P','E','G'), 20, sample.size(),10);

    TermCriteria termcrit(TermCriteria::COUNT|TermCriteria::EPS,20,0.03);
    Size subPixWinSize(10,10), winSize(31,31);

    const int MAX_COUNT = 500;
    int Min_Req = 200;


    Mat gray, prevGray;
    vector<Point2f> points[3];

    //Mat image = imread("bgit2.jpg");
    Mat image1 = image(Rect(70,130,500,300));

    cvtColor(image1, gray, COLOR_BGR2GRAY);


    goodFeaturesToTrack(gray, points[0], MAX_COUNT, 0.01, 10, Mat(), 3, 0, 0.04);
    //cornerSubPix(gray, points[0], subPixWinSize, Size(-1,-1), termcrit);

    int i,k;

    for( i = k = 0; i < points[0].size(); i++ )
    {
		circle( image1, points[0][i], 3, Scalar(0,255,0), -1, 8);


        }

        cout<<points[0].size()<<endl;

       imshow("points",image1);

       char c = (char)waitKey(10);
        if (c==27) 
        {
            waitKey();
        }
	
	}
    	

        return 0;

}
