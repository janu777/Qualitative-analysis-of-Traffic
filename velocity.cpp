#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/videoio/videoio.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <vector>
#include "opencv2/core/opengl_interop.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
//#include "cvaux.hpp"
#include <iostream>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <numeric>
#include <fstream>

using namespace cv;
using namespace std;


Mat intrinsic = Mat(3, 3, CV_32FC1);
Mat rotationMatrix = Mat(3, 3, CV_32FC1);
Mat distCoeffs;
vector<Mat> rvecs;
vector<Mat> tvecs;
double error1;
Mat imagePoint;
Mat temp1, temp2;


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


Point2f point;

int main( int argc, char** argv )
{

    // for peak videos
    //FileStorage fsObject("calibrationData2.xml", FileStorage::READ);

    ofstream myfile;
    myfile.open("/home/itslab/Downloads/cfiles/velnewthirdfob5.csv");

   // myfile<<"hello";

    int velcounter=0;

    FileStorage fsObject("/home/itslab/Downloads/cfiles/calibrationDatathirdfobdelete1333.xml", FileStorage::READ);


//declare the variables intrinsic2, rvecs2 ...etc

// object["name of the dataset stored in the xml file"] >> objectname you want to store the data in
    fsObject["intrinsic"] >> intrinsic; 
    fsObject["rvecs"] >> rvecs;
    fsObject["tvecs"] >> tvecs;
    fsObject["rotationMatrix"] >> rotationMatrix;
    //Rodrigues(rvecs, rotationMatrix);
    fsObject.release();


   bool check = true; int counter=0;
   bool featuresAreFound;

   //for nift videos 
   //Mat sample(230,150,CV_8UC1);

   //for it videos
   //Mat sample(230,240,CV_8UC1);


    //for third fob videos .... 
    Mat sample(250,300,CV_8UC1);  

   
   //VideoCapture cap("videoitcut5.asf");
   VideoCapture cap("/home/itslab/Downloads/cfiles/QueueLength2.mp4");

    //VideoWriter outputvideo("output",CV_FOURCC('M','J','P','G'), 30, sample.size(), true);

    //VideoWriter outputvideo("output.asf",CV_FOURCC('J','P','E','G'), 20, sample.size(),10);

    TermCriteria termcrit(TermCriteria::COUNT|TermCriteria::EPS,20,0.03);
    Size subPixWinSize(10,10), winSize(31,31);

    const int MAX_COUNT = 500;
    int Min_Req = 200;
    bool needToInit = true;
    bool nightMode = false;
    bool initializing = true;
    bool calcVelocity;
    double numberOfPoints = 0;
    double averagenumberOfPoints = 0;
    double consolidatenumberOfPoints=0;


    Mat gray, prevGray, image;
    vector<Point2f> points[3];

    vector<double> dist(MAX_COUNT);
    vector<double> nframes(MAX_COUNT);
    int totalDistance = 0;

    int pointsSize, distSize;

    int fc=0;
    long int fc1=0;
    double consolidatevelocity = 0;
    double consolidatevelocityaverage =0;
   
    for(;;)
    {
        //myfile<<"hello"<<endl;
/*
        fc = fc+1;
        
        fc1=fc1+1;
        cout<<"fc="<<fc1<<endl;*/

        if (fc>417){

        //consolidatevelocityaverage = consolidatevelocity/velcounter;
        //averagenumberOfPoints = consolidatenumberOfPoints/velcounter;

                //cout<<pointsSize<<endl;

        cout<<velcounter<<endl;

        myfile<<consolidatevelocityaverage<<";"<<averagenumberOfPoints<<";"<<pointsSize<<";"<<velcounter<<endl;
        cout<<consolidatevelocityaverage<<endl;
        fc=0;
        consolidatevelocityaverage=0;
        consolidatevelocity = 0;
        consolidatenumberOfPoints = 0;
        averagenumberOfPoints = 0;
        velcounter = 0;

        }
        else{
            fc=fc+1;
        }
        Mat frame;
        cap >> frame;
        if( frame.empty() )   
            break;

        // fc = fc+1;
        // cout<<"fc"<<fc<<endl;

        frame.copyTo(image);

        //Mat image1 = image(Rect(132,180,310,230));
        //Mat image1 = image(Rect(50,250,310,230));
        //for it videos 60,130 .. . ..
        //Mat image1 = image(Rect(60,130,240,230));
        //for nift videos .. . . 
       //Mat image1 = image(Rect(160,130,150,230));
       // Mat image1 = image(Rect(60,130,240,230));

        //Mat image1 = image(Rect(0,0,100,300));
        //Mat image1 = image;

        //for third fob videos ....

        //Mat image1 = image(Rect(240,120, 300, 250));
		Mat image1 = image(Rect(70,130, 500, 300));

      //  Mat image1 = image;

        cvtColor(image1, gray, COLOR_BGR2GRAY);

        if( nightMode )
            image = Scalar::all(0);

       /* if (points[0].size() < 5)
        {
            needToInit = true;
            gray.copyTo(prevGray);
        }*/
            int counter1;


        if( needToInit )
        {
            counter =0;
            counter1 = 0;
            calcVelocity = true;

            nframes.erase(nframes.begin(), nframes.end());
            dist.erase(dist.begin(), dist.end());

            goodFeaturesToTrack(gray, points[0], MAX_COUNT, 0.01, 10, Mat(), 3, 0, 0.04);
            cornerSubPix(gray, points[0], subPixWinSize, Size(-1,-1), termcrit);

            //adding new features discovered to the old features to be tracked which are in points[0]

            //points[0].insert(points[0].end(), points[2].begin(), points[2].end());

            featuresAreFound = true;

            // Here resizing is another problem when we keep adding elements into vectors beyond their limits, 
            // they do not get stored .. so we need to increase the size of the dist and points vectors.
            // few features are lost when status =0 for few points and hence we resize (decrease from imax to k) the vector to save memory
            // but when we add elements beyond they do not get stored in the memory...i.e we cant add elements beyond the size of the vector and then
            // resize the vector (increase) to accomodate for the extra elements. 

            pointsSize = points[0].size();




            //nframes.erase();
           
            dist.resize(pointsSize);
            nframes.resize(pointsSize);

        }


        if (!points[0].empty())
        {   
            counter1++;

            vector<uchar> status;
            vector<float> err;
            if(prevGray.empty())
            gray.copyTo(prevGray);

            calcOpticalFlowPyrLK(prevGray, gray, points[0], points[1], status, err, winSize,
                                 3, termcrit, 0, 0.001);

            int sum_of_elems = std::accumulate(status.begin(),status.end(),0);

            if (featuresAreFound)
            {
            int noOfPoints = points[1].size();
            cout<<noOfPoints;
            featuresAreFound = false;
            }      
            
            size_t i, k;
            cout<<"printing..";
            for( i = k = 0; i < points[1].size(); i++ )
            { 
                if (status[i] == 0)
                {
                    totalDistance = totalDistance + dist[i];
                }

                else 
                {
                //dist[1][k++] = norm(points[0][i] - points[1][i]);

                Point2f pointtemp;

                Point3f pointtemp2, real1, real2;

                cout<<"image to real";

                imageToReal(points[0][i], &real1); 

                cout<<points[0][i]<<endl;

                imageToReal(points[1][i], &real2);

                pointtemp = points[0][i] - points[1][i];

                pointtemp2 = real1-real2;

                //for it videos and freeflow --- 0.1 
                //if (sqrt(pointtemp2.x * pointtemp2.x + pointtemp2.y * pointtemp2.y) < 0.1 & counter > 1)
                //for third fob vidoes

                //if (sqrt(pointtemp2.x * pointtemp2.x + pointtemp2.y * pointtemp2.y) < 0.1 & counter > 1)
                if (sqrt(pointtemp2.x * pointtemp2.x + pointtemp2.y * pointtemp2.y) < 0.1 & counter > 1)
               // if (sqrt(pointtemp.x * pointtemp.x + pointtemp.y * pointtemp.y) < 1 & counter > 1)
               {
                status[i]=0;
               }

               double sum_of_elems = std::accumulate(status.begin(),status.end(),0);

            
               if (status[i]== 1)
               {

                nframes[k] = nframes[i]+1;

                // Point3f pointtemp2, real1, real2;

                // imageToReal(points[0][i], &real1);

                // imageToReal(points[1][i], &real2);

                // //cout<<real1<<endl;

                // pointtemp2 = real1-real2;

                //cout<<pointtemp2<<endl;

                dist[k] = dist[i] + sqrt(pointtemp2.x * pointtemp2.x + pointtemp2.y * pointtemp2.y);

                //cout<<dist[k]<<endl;
                //cout<<dist[k]<<endl;

                points[1][k] = points[1][i];

                k++;

                if (counter1 > 1)
                {

                circle( image1, points[1][i], 3, Scalar(0,255,0), -1, 8);

               /* if (counter1 > 20 & calcVelocity)
                {       


                    size_t m;
                    vector<double> velocity(MAX_COUNT);
                    double size = dist.size();
                    double averageVelocity=0;
                    for (m=0; m<sum_of_elems;m++){
                        //velocity[m] = dist[m]/nframes[m];
                        velocity[m] = dist[m]/nframes[m];
                        }
                    averageVelocity = accumulate(velocity.begin(),velocity.end(),0)/sum_of_elems;
                    //cout<<velocity[9]<<endl;
                    //cout<<averageVelocity<<endl;
                    //cout<<size<<endl;
                    //cout<<sum_of_elems<<endl;
                    //cout<<dist.size()<<endl;
                    cout<<nframes[0]<<endl;
                    calcVelocity = false;

                }*/

                }
            }

            
            }

    }
            cout<<"success";
            points[1].resize(k);
            dist.resize(k);
            nframes.resize(k);
            

            if (counter1 > 1 & calcVelocity)
                {       
                    size_t m;
                    velcounter = velcounter + 1;

                    
                    double size = dist.size();
                    vector<double> velocity(size);
                    double averageVelocity=0;
                    double actualVelocty = 0;
                    for (m=0; m<dist.size();m++)
                    {
                        //velocity[m] = dist[m]/nframes[m];
                        velocity[m] = dist[m]/nframes[m];

                        //cout<<velocity[m]<<"and"<<nframes[m]<<endl;

                        }
                        //cout<<velocity[3]<<endl;


                        // Original Print command
                        //cout<<size<<endl;

                         numberOfPoints = size;
                         



                        //myfile<<size<<endl;
                        // cout<<velocity[8];

                        for(m=0; m<velocity.size();m++){

                            averageVelocity = averageVelocity+velocity[m];

                         }
                         averageVelocity = averageVelocity/size;
                        //averageVelocity = std::accumulate(velocity.begin(),velocity.end(),0);

                        //cout<<averageVelocity<<endl;
                        //actualVelocty = averageVelocity*25*18/5;
                        actualVelocty = averageVelocity*28*18/5;

                        consolidatenumberOfPoints = numberOfPoints + consolidatenumberOfPoints;
                        averagenumberOfPoints = consolidatenumberOfPoints/velcounter;
                        consolidatevelocity = consolidatevelocity + actualVelocty;
                        consolidatevelocityaverage = consolidatevelocity/velcounter;

                        // Original Print Command
                        //cout<<actualVelocty<<endl;
                        



                    //cout<<actualVelocty<<endl;
                    velocity.erase(velocity.begin(), velocity.end());
                   //averageVelocity = accumulate(velocity.begin(),velocity.end(),0)/size;
                    //averageVelocity = accumulate(velocity.begin(),velocity.end(),0);
                    //cout<<velocity[5]<<endl;
                    //cout<<averageVelocity<<endl;
                    //cout<<actualVelocty<<endl;
                    //cout<<size<<endl;
                    //cout<<sum_of_elems<<endl;
                    //cout<<dist.size()<<endl;
                   //cout<<nframes[0]<<endl;
                    //cout<<dist.size()<<endl;
                    //cout<<dist[0]<<endl;
                    calcVelocity = false;
                    //cout<<dist[0]<<endl;

                }
            
        }
        
 
        needToInit = false;
        imshow("LK Demo", image1);
        imshow("actual", image);

       char c = (char)waitKey();
        if (c==27) 
        {
            waitKey(0);
        }

        // This is to copy the features first detected to points[1] and this points[1] is swapped to points[0] 
        // and then new features dectected i.e. points[2] will be added to points[0] and then these are tracked to points[1] and 
        // swapped to points[0]

        if (initializing)
        {
            points[1] = points[2];
            initializing = false;
        }
       
        std::swap(points[1], points[0]);
      
        if (points[0].size() < 5 & counter > 2)

        {
            needToInit = true;
        }
        else 
        {
        cv::swap(prevGray, gray);
        }
         
        counter++;
        //outputvideo.write(image1);
    }

    myfile.close();
    return 0;
   
}


// Reading the points from the xml file 


// onmouse function has been changed to the deleting regions functionality
// hence uncomment the static onmouse thing and also the function implementation
