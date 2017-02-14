#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <cmath>
#include "enet.h"
#include <mutex>

#define HSV Scalar(57, 150, 100), Scalar(87, 255, 255)
using namespace cv;
using namespace std;

std::mutex sending_lock;

union S {
	enet_uint8 byte[sizeof(long double)];
	long double send;
};

S sending;

void connect(ENetPeer* peer, ENetHost* host, ENetEvent event){
	std::cout << "connect:" << peer->address.host << std::endl;
}

void recieve(ENetPeer* peer, ENetHost* host, ENetEvent event){
	sending_lock.lock();
	client::send(sending.byte, sizeof(long double), peer);
	sending_lock.unlock();
}

void disconnect(ENetPeer* peer, ENetHost* host, ENetEvent event){
	std::cout << "disconnect:" << peer->address.host << std::endl;
}

double sqr(double a){return a * a;}

double dist(Point a, Point b){
	return sqrt(sqr(a.x - b.x) + sqr(a.y + b.y));
}

double angle(Point a, Point b){
	return fabs(90-180.0*(acos((b.x - a.x) / dist(a,b)) / 3.1415926)); 
}

double targetDist(int one_l, int one_r, int two_l, int two_r){

	return ((one_r - one_l) + (two_r - two_l)) / 2.0;
}
	
void removeSmall(Mat* s, int size){
	Mat element = getStructuringElement( MORPH_RECT,
                                       Size( 2*size + 1, 2*size+1 ),
                                       Point( size, size ) );
	erode( *s, *s, element );
	dilate(*s, *s, element );
}

int main(int argc, char* argv[])
{
    VideoCapture cap(0); // open the video file for reading
	
    if ( !cap.isOpened() )  // if not success, exit program
    {
         cout << "Cannot open the video file" << endl;
         return -1;
    }	
 	
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	client::connect(connect);
	client::recieve(recieve);
	client::disconnect(disconnect);
	client::init("roborio-5572-frc.local", 25572);
	while(1)
    {
        Mat frame;
	
        bool bSuccess = cap.read(frame); // read a new frame from video/
	cvtColor(frame,frame,CV_BGR2HSV);
	
	Vec3b pixel = frame.at<Vec3b>(300, 300);

	inRange(frame, HSV, frame);
	removeSmall(&frame, 3);

	std::cout << pixel << std::endl;

	findContours(frame, contours, hierarchy, CV_RETR_LIST, CV_CHAIN_APPROX_TC89_L1);
	//std::cout << contours.size() << endl;
	if(contours.size() > 1){	
	for(int x = 0; x < contours.size(); x++)
		drawContours( frame, contours, x, Scalar(150,255,160), 2, 8, hierarchy, 0, Point() );	
	Point first, second;
	int ind1, ind2;
	int len1 = 0, len2 = 0;
	for(int t = 0; t < contours.size(); t++){
		int centerx = 0, centery = 0;
		for(int i = 0; i < contours[t].size(); i++){
			centerx += contours[t][i].x;
			centery += contours[t][i].y;
		}
		centerx /= contours[t].size();
		centery /= contours[t].size();
		Point center(centerx, centery), radius(center);
		for(int i = 0; i < contours[t].size(); i++){
			if(dist(center, contours[t][i]) > dist(center, radius)) radius = contours[t][i];
		}
		int length = dist(center, radius);
		if(length > len1){
			ind2 = ind1;
			second = first;
			len2 = len1;
			len1 = length;
			ind1 = t;
			first = center;
		} else if(length > len2){
			len2 = length;
			ind2 = t;
			second = center;
		}
	}
	int first_left = first.x, first_right = first.x, first_top = first.y, first_bottom = first.y;
	for(int i = 0; i < contours[ind1].size(); i++){
		if(contours[ind1][i].x<first_left){first_left = contours[ind1][i].x;}
		else if(contours[ind1][i].x>first_right){first_right = contours[ind1][i].x;}
		else if(contours[ind1][i].y<first_bottom){first_bottom = contours[ind1][i].y;}
		else if(contours[ind1][i].y>first_top){first_top = contours[ind1][i].y;}
	}
	int second_left = second.x, second_right = second.x, second_top = second.y, second_bottom = second.y;
	for(int i = 0; i < contours[ind2].size(); i++){
		if(contours[ind2][i].x<second_left){second_left = contours[ind2][i].x;}
		else if(contours[ind2][i].x>second_right){second_right = contours[ind2][i].x;}
		else if(contours[ind2][i].y<second_bottom){second_bottom = contours[ind2][i].y;}
		else if(contours[ind2][i].y>second_top){second_top = contours[ind2][i].y;}
	}
	
	int first_center_x = (first_right-first_left)/2+first_left, first_center_y = (first_top-first_bottom)/2+first_bottom;
	int second_center_x = (second_right-second_left)/2+second_left, second_center_y = (second_top-second_bottom)/2+second_bottom;

	circle(frame, Point(first_center_x, first_center_y), 10, Scalar(128,128,128), 10);
	circle(frame, Point(second_center_x, second_center_y), 10, Scalar(128,128,128), 10);

	rectangle(frame, Point(first_left, first_top), Point(first_right, first_bottom), Scalar(128,128,128), 2, 8 ,0);
	rectangle(frame, Point(second_left, second_top), Point(second_right, second_bottom), Scalar(128,128,128), 2, 8 ,0);

	Point center1(first_center_x, first_center_y), center2(second_center_x, second_center_y);


	sending_lock.lock();
	sending.send = targetDist(first_left, first_right, second_left, second_right);
	sending_lock.unlock();
	}
	
	circle(frame, Point(300, 300), 10, Scalar(128,128,128), 10);

	imshow("Frame", frame);
	
	int m;
	if((m = waitKey(1)) != -1){cout << m << endl;break;}

    }
	client::quit();
    return 0;

}
