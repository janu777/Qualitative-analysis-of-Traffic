#include <opencv2/opencv.hpp>
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/core/core.hpp>
#include "opencv2/highgui/highgui.hpp"
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <ctype.h>
#include <limits>
#include <fstream>

using namespace cv;
using namespace std;

bool track=false;
Point current;
Mat duplicate;

const Point** r;
Point* p;
int* PinR;
int input=1;
int pcount=0;					//point count
int rcount=0;					// region count
int i,j,nop;
CvMemStorage* pstorage = cvCreateMemStorage(0);
CvSeq* pseq = cvCreateSeq( CV_SEQ_ELTYPE_POINT, sizeof(CvSeq), sizeof(CvPoint), pstorage);
CvMemStorage* rstorage = cvCreateMemStorage(0);
CvSeq* rseq = cvCreateSeq( 0, sizeof(CvSeq), sizeof(CvPoint*), rstorage);
CvMemStorage* nstorage = cvCreateMemStorage(0);
CvSeq* nseq = cvCreateSeq( CV_32SC1, sizeof(CvSeq), sizeof(int), nstorage);
CvPoint **tempp;
CvPoint *t;
static void onMouse( int event, int x, int y, int, void* parameter)
{
	if(input) {
		Mat* image = (Mat*) parameter;
		switch( event ){
			case CV_EVENT_LBUTTONDOWN: {
				
				printf("%d %d\n",x,y);
				current = Point(x,y);
				pcount++;
				cvSeqPush( pseq, &current );
				*image = duplicate.clone();
				track=false;
				current = Point(x,y);
				
				circle(*image, current, 0, Scalar( 0, 0, 0 ), 4, 8, 0);
				imshow("Original frame",*image);
			break;	}
		
			case CV_EVENT_LBUTTONUP:
				track=true;
				printf("%d %d\n",x,y);
			break;

			case CV_EVENT_MOUSEMOVE: {
			if(track) {
				duplicate = (*image).clone();
				
				Point temp = Point(x,y);
				line(duplicate, current, temp, Scalar( 0, 0, 0 ), 1, 8, 0);
				imshow("Original frame",duplicate);
			}
			break; }
		}
	}
}

int main (int argc, char *argv[])
{


	ofstream myfile;

    myfile.open("/home/itslab/Downloads/cfiles/areanewthirdfobtest123.csv");

	bool check = true;
	
	int lowthershold=0;
    int maxthreshold = 200;
	int ratio =3;

	RNG rng(12345);

	Mat frame, fore, back, forewoshadows, shadows, foreFromMerge, foreground, finalfore,forewoshadows1, forewoshadows2,fore2 ,back2;

	//cv::VideoCapture cap("videopeakcut1.asf");
	//VideoCapture cap("/home/itslab/Downloads/cfiles/QueueLength2.mp4");
	VideoCapture cap(argv[1]);

	cv::BackgroundSubtractorMOG2 bg(1000, 16, true);    
	//cv::BackgroundSubtractorMOG2 bg2(100,16,true);

	//bg2.set("nmixtures",3);
	
	bg.set("nmixtures",3);
	bg.set("fTau",0.5);

	//Mat frame1 = imread("/home/itslab/Downloads/cfiles/3FOB.jpg");
	Mat frame1 = imread(argv[2]);
	//Mat frame1 = imread("bgit2.jpg");

	imshow("Original frame", frame1);
	duplicate =frame1.clone();
	setMouseCallback( "Original frame", onMouse, &frame1);


	int key;
	while(check) {
		key = waitKey(0);
	 

		printf("You pressed %d \n", key);
		
		if(key==99) 
		{                              // Small case C - 'c'
			if(pcount>2) {
				rcount++;
				p = new Point[pseq->total];
				cvCvtSeqToArray(pseq, p, CV_WHOLE_SEQ);
				track=false;
				duplicate =frame1.clone();
				line(duplicate, *(CV_GET_SEQ_ELEM(cv::Point, pseq, 0)), current, Scalar( 0, 0, 0 ), 1, 8, 0);

				imshow("Original frame",duplicate);
				cvSeqPush( nseq, &pcount );
				cvSeqPush( rseq, &p );
				p=NULL;
				pcount=0;
				cvClearSeq(pseq);
			}
		}
		else {
				cout << "You pressed Enter" << endl;
				check=false;
				input=0;
			}
	}


	r=(const Point**) new Point*[rcount];
	PinR=new int[rcount];
	cvCvtSeqToArray(rseq, r, CV_WHOLE_SEQ);
	cvCvtSeqToArray(nseq, PinR, CV_WHOLE_SEQ);

	Mat masked_frame;
	
	Mat mask = Mat(frame1.rows,frame1.cols,CV_8UC1);
	
	mask = Scalar(0);


	  if(pcount>0 || rcount>0) {
        
       
        fillPoly(mask, r, PinR, rcount, Scalar(1), 8, 0);
    }

	vector<vector<Point> > contours;
	vector<vector<Point> > contours1;
	vector<Vec4i> hierarchy;

	int totalarea = countNonZero(mask);

	int fc=0;
	double areaoccupancyaverage=0;
	double areaoccupancyconsolidate=0;

	

	for(;;)

	{
		fc = fc+1;
        int frameno=atoi(argv[3]);
		if (fc>frameno){
			fc =0;
			myfile << areaoccupancyaverage << endl;
			areaoccupancyconsolidate = 0;
			areaoccupancyaverage = 0;
			
		}

		cap >> frame;
		
	
		frame.copyTo(masked_frame,mask);

		bg.operator()(masked_frame,fore, 0.01);
	//	bg2.operator()(masked_frame,fore2,0.01);

		bg.getBackgroundImage(back);
	//	bg2.getBackgroundImage(back2);



		imshow("Initial Foreground",fore);
		imshow("Background", back);
	//	imshow("back2",back2);

		threshold(fore,forewoshadows,128,255,CV_THRESH_BINARY);

		absdiff(fore,forewoshadows,shadows);

		imshow("Shadown Removal", forewoshadows);

		Canny(forewoshadows, forewoshadows, lowthershold, ratio*lowthershold, 3);

		dilate(forewoshadows,forewoshadows,Mat());


		cv::Scalar color = cvScalar(rng.uniform(0,255), rng.uniform(0,255), rng.uniform(0,255));	

		cv::findContours(forewoshadows, contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);
		//cv::findContours(forewoshadows,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
		//cv::drawContours(forewoshadows, contours, -1, color, CV_FILLED, 8, hierarchy, 2);

		// the argument after contours indicates which contours to be drawn, if set negative
		// all the contours are drawn

		//cout<<contours[0].size()<<endl;

		double area;
		int i;
		for (i=0; i < contours.size(); i++){

			area = contourArea(contours[i],false);


			//cout<<"hello"<<endl;
			//cout<<area<<endl;
		
			//cout<<area<<endl;
			//if (area > 2000 & area < 4000)
			if(area > 100)
			//if(contours[i].size()>100)
			{
			//cout<<"thanks"<<endl;
			cv::drawContours(forewoshadows, contours, i, color , CV_FILLED);
				//cv::drawContours(forewoshadows, contours, i, color , 1);
			}
		}



		//cv::findContours(forewoshadows, contours1,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);
		//cv::drawContours(forewoshadows, contours1, -1, color , 1);


		//cv::drawContours(forewoshadows, contours, -1 , color, CV_FILLED,8 ,hierarchy, 2);
				
		//cv::drawContours(forewoshadows1, contours, -1, color, 1, 8, hierarchy, 2);

		//cout<<contours.size()<<endl;

		erode(forewoshadows,forewoshadows,Mat());		

		//erode(forewoshadows,forewoshadows,Mat());

		//imshow("test",finalfore);
		//imshow("Original Image1", frame);

		int whitepixels = countNonZero(forewoshadows);
		//int whitepixels = countNonZero(shadows);
		
		double areaoccupancy = whitepixels*100/totalarea;

		//imshow("Background Image", back);
		//imshow("Foreground Image", fore);
		
		imshow("Foreground Image without shadows", forewoshadows);
		imshow ("live video",frame);
		//imshow("contours", forewoshadows);
		//imshow("shadows", shadows);

		//cout<<whitepixels<<endl;

		//cout<<totalarea<<endl;

		//double areaoccupancy = whitepixels/totalarea;

		cout<<areaoccupancy<<endl;
		areaoccupancyconsolidate = areaoccupancyconsolidate + areaoccupancy;
		areaoccupancyaverage = areaoccupancyconsolidate/fc;

		//myfile << areaoccupancyaverage <<endl;


		//imshow("merge",foreground);
		//imshow("final",finalfore);
		//imshow("shandows",shadows);


		createTrackbar("lower threshold", "Foreground Image without shadows", &lowthershold, maxthreshold, NULL);

		//if(cv::waitKey(1) >= 0) break;
		char c = (char)waitKey(1);
		if (c==27) 
		{
			waitKey();
		}
		
	}
	myfile.close();
	return 0;
}
