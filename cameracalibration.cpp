
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/video.hpp>
#include "opencv2/core/opengl_interop.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/core/opengl_interop.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
//#include "cvaux.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>

using namespace std;
using namespace cv;

Mat imagePoint, temp1, temp2;

vector <Point2f> imagePoints;
vector <Point3f> realPoints;
vector <vector <Point2f> > imagePoints_vector;
vector <vector <Point3f> > realPoints_vector;
Point current;

Mat intrinsic = Mat(3, 3, CV_32FC1);
Mat rotationMatrix = Mat(3, 3, CV_32FC1);
Mat distCoeffs;
vector<Mat> rvecs;
vector<Mat> tvecs;
double error1;
int counter=0;


void imageToReal(Point2f current, Point3f* real)
{
	imagePoint = cv::Mat::ones(3,1,cv::DataType<double>::type);
	imagePoint.at<double>(0,0) = current.x;
	imagePoint.at<double>(1,0) = current.y;

	//Mat temp1, temp2;

	double s;

	temp1 = rotationMatrix.inv() * intrinsic.inv() * imagePoint;
	temp2 = rotationMatrix.inv() *tvecs[0];

	s = 0 + temp2.at<double>(2,0);
	s= s/temp1.at<double>(2,0);

	imagePoint = rotationMatrix.inv() * (s * intrinsic.inv() * imagePoint - tvecs[0]);
	imagePoint.at<double>(2,0) = 0;
	// Ypu might have to use pointers because it involves some 
	// kind of operations and assigning
	// recall the use of pointers in function funda

	// This is constructor to define a new object newpoint with argument being a 3Dpoint
	Point3f newpoint(imagePoint);
	*real = newpoint;

}

static void onMouse (int event, int x, int y, int, void* parameter)
{
	Mat* image = (Mat*) parameter;
	switch(event){

		case CV_EVENT_LBUTTONDOWN: {
			current = Point(x,y);
			imagePoints.push_back(current);
			counter++;
			circle(*image, current, 0, Scalar( 0, 0, 0 ), 4, 8, 0);
			imshow("Original frame",*image);

		}
		case CV_EVENT_RBUTTONDOWN :
		{	

			break;
		}

	}

} 

int main(){

	
	//realPoints.push_back(Point3f(0.0f,0.0f,0.0f));
	//realPoints.push_back(Point3f(0.0f,2.5,0.0f));
	//realPoints.push_back(Point3f(0.5,2.5,0.0f));
	//realPoints.push_back(Point3f(0.5,0.0f,0.0f));
	//realPoints.push_back(Point3f(16.0,0.0f,0.0f));
	//realPoints.push_back(Point3f(16.0,2.5,0.0f));
	//realPoints.push_back(Point3f(16.5,2.5,0.0f));
	//realPoints.push_back(Point3f(16.5,0.0f,0.0f));
	//realPoints_points.push_back(realPoints);
	realPoints.push_back(Point3f(0.0f,0.0f,0.0f));
	realPoints.push_back(Point3f(3.5f,0.0f,0.0f));

	//realPoints.push_back(Point3f(0.0f,4.0f,0.0f));
	//realPoints.push_back(Point3f(3.5f,4.0f,0.0f));

	realPoints.push_back(Point3f(0.0f,8.0f,0.0f));
	realPoints.push_back(Point3f(3.5f,8.0f,0.0f));


	Point2f current1(100,100);
	Point3f real1;


	Mat frame1 = imread("/home/itslab/Downloads/cfiles/3FOB.jpg");

	//Mat frame1 = imread("bg.jpg");

	//for it videos 60,130 .. . ..
    //Mat frame = frame1(Rect(60,130,240,230));
   	// for nift videos
	//Mat frame = frame1(Rect(160,130,150,230));
	//Mat frame = frame1(Rect(60,130,240,230));

	//for third fob videos
	Mat frame = frame1(Rect(70,130, 500, 300));

	imshow("Original frame",frame);
	setMouseCallback("Original frame", onMouse, &frame);
	int key = waitKey(0);
	cout << counter << endl;
	cout<<realPoints.size()<<endl;

	realPoints_vector.push_back(realPoints);
	imagePoints_vector.push_back(imagePoints);

	if (key ==99){

		error1=calibrateCamera(realPoints_vector,imagePoints_vector,frame.size(),intrinsic, distCoeffs, rvecs, tvecs);
		Rodrigues(rvecs[0], rotationMatrix); 

		cout<< "exit" <<endl;
		cout<<error1<<endl;
		cout<<rvecs[0]<<endl;
		cout<<tvecs[0]<<endl;
		//cout << error <<endl;
		cout << intrinsic <<endl;

		imageToReal(current1, &real1);
		cout<< real1 << endl;
	}
	// for peak videos
	//FileStorage fs("calibrationData2.xml", FileStorage::WRITE);


	FileStorage fs("/home/itslab/Downloads/cfiles/calibrationDatathirdfobdelete1333.xml", FileStorage::WRITE);

	fs<<"intrinsic"<<intrinsic;
	fs<<"rvecs" << rvecs[0];
	fs<<"tvecs" << tvecs[0];
	fs<<"rotationMatrix" << rotationMatrix;
	fs<<"error"<<error1;
	fs<<"imagePoints" << imagePoints_vector;

	fs.release();

	/*if(counter > 4) {
	Mat H = findHomography(realPoints, imagePoints, CV_RANSAC);
	cout<<"hello"<<endl;	
	}*/

}

// Reading the points from the xml file 

/*FileStorage fsObject("calibrationData.xml", FileStorage::READ);

//declare the variables intrinsic2, rvecs2 ...etc

// object["name of the dataset stored in the xml file"] >> objectname you want to store the data in
fsObject["intrinsic"] >> intrinsic2; 
fsObject["rvecs"] >> rvecs2;
fsObject["tvecs"] >> tvecs2;
fsObject["rotationMatrix"] >> rotationMatrix2;
*/


// All of Mat, Point2f, Point3f are classes. They have constructors and a few 
//methods in the class declaration. 
//Point3f real;
