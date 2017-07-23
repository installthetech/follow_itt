#include <iostream>
#include "opencv2/opencv.hpp"
#include "opencv2/nonfree/nonfree.hpp"

using namespace cv;

void readme();

int main( int argc, char** argv )
{
  if( argc != 3 )
  { readme(); return -1; }

  Mat img_object = imread( argv[1], CV_LOAD_IMAGE_GRAYSCALE );
  VideoCapture cap;
  string deviceID = argv[2];

  if(deviceID.compare("0") == 0)
  	cap.open(0);
  else if(deviceID.compare("-1") == 0)
  	cap.open(-1);
  else if(deviceID.compare("1") == 0)
  	cap.open(1);
  else if(deviceID.compare("2") == 0)
  	cap.open(2);
  else
  	cap.open(argv[2]);
  
  float steer = 0;
  float filSteer = 0;
  if(!cap.isOpened())
  {
  	std::cout<<"Video/ Camera not found\n";
  	readme(); return -1;

  }
  while(1)
  {
    Mat img_scene;
    cap>>img_scene;
    resize(img_scene, img_scene, Size(img_scene.cols*480.0/img_scene.rows, 480));
    Mat orig = img_scene.clone();
    cvtColor(img_scene, img_scene, CV_BGR2GRAY);
    if( !img_object.data || !img_scene.data )
    { 
    	std::cout<< " --(!) Error reading images " << std::endl; return -1; }

    int minHessian = 400;

    SurfFeatureDetector detector( minHessian );

    std::vector<KeyPoint> keypoints_object, keypoints_scene;

    detector.detect( img_object, keypoints_object );
    detector.detect( img_scene, keypoints_scene );

    SurfDescriptorExtractor extractor;

    Mat descriptors_object, descriptors_scene;

    extractor.compute( img_object, keypoints_object, descriptors_object );
    extractor.compute( img_scene, keypoints_scene, descriptors_scene );

    FlannBasedMatcher matcher;
    std::vector< DMatch > matches;
    matcher.match( descriptors_object, descriptors_scene, matches );

    double max_dist = 0; double min_dist = 100;

    for( int i = 0; i < descriptors_object.rows; i++ )
    { double dist = matches[i].distance;
      if( dist < min_dist ) min_dist = dist;
      if( dist > max_dist ) max_dist = dist;
    }

    std::vector< DMatch > good_matches;

    for( int i = 0; i < descriptors_object.rows; i++ )
    { if( matches[i].distance < 3*min_dist )
       { good_matches.push_back( matches[i]); }
    }

    Mat img_matches;
    drawMatches( img_object, keypoints_object, img_scene, keypoints_scene,
                 good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
                 vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

    std::vector<Point2f> obj;
    std::vector<Point2f> scene;

    for( int i = 0; i < good_matches.size(); i++ )
    {
      obj.push_back( keypoints_object[ good_matches[i].queryIdx ].pt );
      scene.push_back( keypoints_scene[ good_matches[i].trainIdx ].pt );
    }

    Mat H = findHomography( obj, scene, CV_RANSAC );

    std::vector<Point2f> obj_corners(4);
    obj_corners[0] = cvPoint(0,0); obj_corners[1] = cvPoint( img_object.cols, 0 );
    obj_corners[2] = cvPoint( img_object.cols, img_object.rows ); obj_corners[3] = cvPoint( 0, img_object.rows );
    std::vector<Point2f> scene_corners(4);

    perspectiveTransform( obj_corners, scene_corners, H);

    line( img_matches, scene_corners[0] + Point2f( img_object.cols, 0), scene_corners[1] + Point2f( img_object.cols, 0), Scalar(0, 255, 0), 4 );
    line( img_matches, scene_corners[1] + Point2f( img_object.cols, 0), scene_corners[2] + Point2f( img_object.cols, 0), Scalar( 0, 255, 0), 4 );
    line( img_matches, scene_corners[2] + Point2f( img_object.cols, 0), scene_corners[3] + Point2f( img_object.cols, 0), Scalar( 0, 255, 0), 4 );
    line( img_matches, scene_corners[3] + Point2f( img_object.cols, 0), scene_corners[0] + Point2f( img_object.cols, 0), Scalar( 0, 255, 0), 4 );

    resize(img_matches, img_matches, Size(), 0.5, 0.5);


    Rect r;
    r.x = (scene_corners[0].x+scene_corners[3].x)/2.0;
    r.y = (scene_corners[1].y+scene_corners[0].y)/2.0;
    r.width = (scene_corners[1].x+scene_corners[2].x)/2.0 - r.x ;
    r.height = (scene_corners[3].y+scene_corners[2].y)/2.0 - r.y ;

    cvtColor(img_scene, img_scene, CV_GRAY2BGR);
    if(r.width*r.height > 5000)
    { 
      
      rectangle(orig, r, Scalar(0,255,0), 2);
      // if((r.x+r.width/2) - (img_scene.cols/2) <= 0)
      steer = (((r.x+r.width/2) - (img_scene.cols/2))*1.0/(img_scene.cols/2))*100;
      if(steer<-100)
        steer = -100;
      if(steer>100)
        steer = 100;
      
    }
    else
    {
      steer = 0;
    }

    filSteer = 0.2*steer + 0.8*filSteer;

    Mat redbar = imread("data/redbar.png");
    
    resize(redbar, redbar, Size(img_scene.cols, 20));
    std::cout<<"Predicted steering value: "<<steer<<" percent"<<std::endl;
    circle(redbar, Point(img_scene.cols/2 + img_scene.cols*filSteer/200.0, redbar.rows/2), redbar.rows/2-1, Scalar(255,200,255), redbar.rows/2);
    addWeighted(redbar, 0.6, orig(Rect(0, 0, redbar.cols, redbar.rows)), 0.4, 1.0, orig(Rect(0, 0, redbar.cols, redbar.rows)));
    imshow( "Raw image", orig );

    waitKey(1);

  }
  return 0;
}

void readme()
{ std::cout << " Usage: ./run.sh <pattern> <video/cam>" << std::endl; }