
#include <stdio.h>
#include<utility>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <cmath>
#include <string>
#include <fstream>

using namespace cv;
using namespace std;


int cCount = 0;
int snd = 0;
vector<Point> Pvec;
vector<Mat> Mvec;
float histo[33] = { 0, };
float histo2[33] = { 0, };
int Pcount = 0;
int maxX = -1;
int minX = 65535;
int maxY = -1;
int minY = 65535;

float intensity(int r, int g, int b) {
	return  0.2125 * r + 0.7152 * g + 0.0722 * b;
}

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
	Mat im = *(Mat*)(userdata);

	if (cCount < 4) {
		if (event == EVENT_LBUTTONDOWN)
		{
			cCount++;
			cout << "왼쪽 마우스 버튼 클릭.. 좌표 = (" << x << ", " << y << ")" << endl;
			if (x < 8)
				x = 8;
			if (y < 8)
				y = 8;
			if (x > 591)
				x = 591;
			if (y > 791)
				y = 791;
			rectangle(im, Rect(Point(x - 7, y - 7), Point(x + 7, y + 7)), Scalar(0, 254, 0), 2, 8, 0);
			
			//Mat gx, gy;
			//Sobel(im(Rect(Point(x - 3, y - 3), Point(x + 3, y + 3))), gx, CV_32F, 1, 0, 1);
			//Sobel(im(Rect(Point(x - 3, y - 3), Point(x + 3, y + 3))), gy, CV_32F, 0, 1, 1);
			//Mat mag, angle;
			//cartToPolar(gx, gy, mag, angle, 1);
			//imshow("new1", mag);
			//imshow("new2", angle);
			if (snd == 0) {
				Pvec.push_back(Point(x, y));
				Pcount++;
			
			}
			else if (snd == 1) {
				Pvec.push_back(Point(x, y));
				if (x > maxX)
					maxX = x;
				if (x < minX)
					minX = x;
				if (y > maxY)
					maxY = y;
				if (y < minY)
					minY = y;
				Pcount++;
			}


			//Mvec.push_back(mag);
			//Mvec.push_back(angle);
			//rectangle(im, Rect(400, 400, 100, 100), Scalar(0, 0, 255), 1, 8, 0);
			cv::imshow("image", im);

		}
	}
	
}
float BilinearInterpolation(float q11, float q12, float q21, float q22, float x1, float x2, float y1, float y2, float x, float y)
{
	float x2x1, y2y1, x2x, y2y, yy1, xx1;
	x2x1 = x2 - x1;
	y2y1 = y2 - y1;
	x2x = x2 - x;
	y2y = y2 - y;
	yy1 = y - y1;
	xx1 = x - x1;
	return 1.0 / (x2x1 * y2y1) * (
		q11 * x2x * y2y +
		q21 * xx1 * y2y +
		q12 * x2x * yy1 +
		q22 * xx1 * yy1
		);
}
void displayType(cv::Mat& img, const std::string type, std::vector<cv::Point>& contour)
{
	cv::Rect rect = cv::boundingRect(contour);
	cv::Point top_left(rect.x, rect.y);
	cv::putText(img, type, top_left, 0, 0.5, CV_RGB(0, 0, 0));
}
vector<Point> linePoints(int x0, int y0, int x1, int y1)
{
	vector<Point> pointsOfLine;

	int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
	int err = (dx > dy ? dx : -dy) / 2, e2;

	for (;;)
	{
		pointsOfLine.push_back(Point(x0, y0));
		if (x0 == x1 && y0 == y1) break;
		e2 = err;
		if (e2 > -dx)
		{
			err -= dy;
			x0 += sx;
		}
		if (e2 < dy)
		{
			err += dx;
			y0 += sy;
		}
	}
	return pointsOfLine;
}
int ccw(pair<int, int> a, pair<int, int> b, pair<int, int> c) {
	int op = a.first * b.second + b.first * c.second + c.first * a.second;
	op -= (a.second * b.first + b.second * c.first + c.second * a.first);
	if (op > 0)return 1;
	else if (op == 0)return 0;
	else return -1;
}
int isIntersect(pair<pair<int, int>, pair<int, int>> x, pair<pair<int, int>, pair<int, int>> y) {
	pair<int, int> a = x.first;
	pair<int, int> b = x.second;
	pair<int, int> c = y.first;
	pair<int, int> d = y.second;
	int ab = ccw(a, b, c) * ccw(a, b, d);
	int cd = ccw(c, d, a) * ccw(c, d, b);
	if (ab == 0 && cd == 0) {
		if (a > b)swap(a, b);
		if (c > d)swap(c, d);
		return c <= b && a <= d;
	}
	return ab <= 0 && cd <= 0;
}
double computeCosine(cv::Point pointA, cv::Point pointB, cv::Point anglePoint)
{
	std::pair<double, double> v1(pointA.x - anglePoint.x, pointA.y - anglePoint.y);
	std::pair<double, double> v2(pointB.x - anglePoint.x, pointB.y - anglePoint.y);
	return (v1.first * v2.first + v1.second * v2.second) /
		sqrt((pow(v1.first, 2) + pow(v1.second, 2)) * (pow(v2.first, 2) + pow(v2.second, 2)));
}

int main()
{
	

	

	//////////////////////////////
	string f_name;
	cout << "enter file name.. (1st.jpg)" << "\n";

	getline(cin, f_name, '\n');

	//cin << f_name;
	cv::Mat imgori = cv::imread(f_name);
	Mat img;
	
	GaussianBlur(imgori, imgori, Size(5, 5), 0.0);
	resize(imgori, img, Size(600, 800));
	Mat rimg;
	img.copyTo(rimg);

	if (img.empty())
		return EXIT_FAILURE;
	


	cv::imshow("image", img);
	setMouseCallback("image", CallBackFunc,&img);
	while (cCount != 4) {
		cv::waitKey(30);
	}
	cv::waitKey(500);
	destroyWindow("image");
	


	cout << "enter 2nd file name.. (target.jpg)" << "\n";
	cCount = 0;
	snd = 1;
	Pcount = 0;

	getline(cin, f_name, '\n');
	cv::Mat imgori2 = cv::imread(f_name);
	Mat img2;
	
	GaussianBlur(imgori2, imgori2, Size(5, 5), 0.0);
	resize(imgori2, img2, Size(600, 800));
	Mat rimg2;
	img2.copyTo(rimg2);

	if (img2.empty())
		return EXIT_FAILURE;
	cv::imshow("image", img2);
	setMouseCallback("image", CallBackFunc, &img2);
	while (cCount != 4) {
		cv::waitKey(30);
	}
	destroyWindow("image");
	cv::waitKey(500);
	//snd = 2;

	vector<Point2f> corners(4);
	corners[0] = Pvec[0];
	corners[1] = Pvec[1];
	corners[2] = Pvec[2];
	corners[3] = Pvec[3];

	Size warpSize(600, 800);

	Mat warpImg(warpSize, rimg.type());

	vector<Point2f> warpCorners(4);
	warpCorners[0] = Pvec[4];
	warpCorners[1] = Pvec[5];
	warpCorners[2] = Pvec[6];
	warpCorners[3] = Pvec[7];
	//Mat trans = getPerspectiveTransform(corners, warpCorners);
	int x1 = Pvec[0].x;
	int y1 = Pvec[0].y;
	int x2 = Pvec[1].x;
	int y2 = Pvec[1].y;
	int x3 = Pvec[2].x;
	int y3 = Pvec[2].y;
	int x4 = Pvec[3].x;
	int y4 = Pvec[3].y;
	int xt1 = Pvec[4].x;
	int yt1 = Pvec[4].y;
	int xt2 = Pvec[5].x;
	int yt2 = Pvec[5].y;
	int xt3 = Pvec[6].x;
	int yt3 = Pvec[6].y;
	int xt4 = Pvec[7].x;
	int yt4 = Pvec[7].y;
	Mat H = (Mat_<double>(8, 9) <<
		-x1, -y1, -1, 0, 0, 0, x1 * xt1, y1 * xt1, xt1,
		0, 0, 0, -x1, -y1, -1, x1 * yt1, y1 * yt1, yt1,
		-x2, -y2, -1, 0, 0, 0, x2 * xt2, y2 * xt2, xt2,
		0, 0, 0, -x2, -y2, -1, x2 * yt2, y2 * yt2, yt2,
		-x3, -y3, -1, 0, 0, 0, x3 * xt3, y3 * xt3, xt3,
		0, 0, 0, -x3, -y3, -1, x3 * yt3, y3 * yt3, yt3,
		-x4, -y4, -1, 0, 0, 0, x4 * xt4, y4 * xt4, xt4,
		0, 0, 0, -x4, -y4, -1, x4 * yt4, y4 * yt4, yt4);
	Mat U, S, VT;
	SVDecomp(H, U, S, VT, SVD::FULL_UV);
	transpose(VT, VT);
	Mat transeform(Size(3, 3), CV_64FC1);
	cout << "check\n";
	int lrow = 0;
	int lcols = VT.cols - 1;
	cout << "check\n\n";
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			transeform.at<double>(i, j) = VT.at<double>(lrow, lcols);
			cout << "check\n";
			lrow++;
		}
	}
	double dw = transeform.at<double>(2, 2);
	transeform = transeform / dw;

	// Warping
	warpPerspective(rimg, warpImg, transeform, warpSize);
	

	//vector<Point> line1 = linePoints(Pvec[4].x, Pvec[4].y, Pvec[5].x, Pvec[5].y);
	//vector<Point> line2 = linePoints(Pvec[5].x, Pvec[5].y, Pvec[6].x, Pvec[6].y);
	//vector<Point> line3 = linePoints(Pvec[6].x, Pvec[6].y, Pvec[7].x, Pvec[7].y);
	//vector<Point> line4 = linePoints(Pvec[7].x, Pvec[7].y, Pvec[4].x, Pvec[4].y);
	cout << "-=-=-=-=-\n";
	
	//img_result2.at<uchar>(y, x) = p;
	pair<int, int> p1 (Pvec[4].x,Pvec[4].y);
	pair<int, int> p2(Pvec[5].x, Pvec[5].y);
	pair<int, int> p3(Pvec[6].x, Pvec[6].y);
	pair<int, int> p4(Pvec[7].x, Pvec[7].y);
	//pair <pair, pair> line1 ();


	for (int i = minX; i <= maxX; i++) {
		for (int j = minY; j <= maxY; j++) {
			int cc = 0;
			pair<int, int> a;
			a = make_pair(i, j);
			pair<int, int> b = make_pair(maxX, j);
			pair<int, int> b2 = make_pair(minX, j);
			for (int p = 0; p < 4; p++) {

				pair<int, int> c = make_pair(Pvec[4 + p].x, Pvec[4 + p].y);
				pair<int, int> d = make_pair(Pvec[5 + p % 3].x, Pvec[5 + p % 3].y);
				if (p == 3) {
					d = make_pair(Pvec[4].x, Pvec[4].y);
				}
				int ab = ccw(a, b, c) * ccw(a, b, d);
				int cd = ccw(c, d, a) * ccw(c, d, b);
				if (ab == 0 && cd == 0) {
					if (a > b)swap(a, b);
					if (c > d)swap(c, d);
					if (c <= b && a <= d) {
						for (int p2 = 0; p2 < 4; p2++) {
							int ab2 = ccw(a, b2, c) * ccw(a, b2, d);
							int cd2 = ccw(c, d, a) * ccw(c, d, b2);
							if (ab2 == 0 && cd2 == 0) {
								if (a > b2)swap(a, b2);
								if (c > d)swap(c, d);
								if (c <= b && a <= d) {
									rimg2.at<Vec3b>(j, i)[0] = warpImg.at<Vec3b>(j, i)[0];
									rimg2.at<Vec3b>(j, i)[1] = warpImg.at<Vec3b>(j, i)[1];
									rimg2.at<Vec3b>(j, i)[2] = warpImg.at<Vec3b>(j, i)[2];
								}
							}
							if (ab2 <= 0 && cd2 <= 0) {
								rimg2.at<Vec3b>(j, i)[0] = warpImg.at<Vec3b>(j, i)[0];
								rimg2.at<Vec3b>(j, i)[1] = warpImg.at<Vec3b>(j, i)[1];
								rimg2.at<Vec3b>(j, i)[2] = warpImg.at<Vec3b>(j, i)[2];
							}
						}
						
					}
						
				}
				if (ab <= 0 && cd <= 0) {
					pair<int, int> a;
					a = make_pair(i, j);
					for (int p2 = 0; p2 < 4; p2++) {
						
						pair<int, int> b2 = make_pair(minX, j);
						pair<int, int> c = make_pair(Pvec[4 + p2].x, Pvec[4 + p2].y);
						pair<int, int> d = make_pair(Pvec[5 + p2 % 3].x, Pvec[5 + p2 % 3].y);
						if (p2 == 3) {
							d = make_pair(Pvec[4].x, Pvec[4].y);
						}


						int ab2 = ccw(a, b2, c) * ccw(a, b2, d);
						int cd2 = ccw(c, d, a) * ccw(c, d, b2);
						if (ab2 == 0 && cd2 == 0) {
							if (a > b2)swap(a, b2);
							if (c > d)swap(c, d);
							if (c <= b2 && a <= d) {
								rimg2.at<Vec3b>(j, i)[0] = warpImg.at<Vec3b>(j, i)[0];
								rimg2.at<Vec3b>(j, i)[1] = warpImg.at<Vec3b>(j, i)[1];
								rimg2.at<Vec3b>(j, i)[2] = warpImg.at<Vec3b>(j, i)[2];
							}
						}
						if (ab2 <= 0 && cd2 <= 0) {
							rimg2.at<Vec3b>(j, i)[0] = warpImg.at<Vec3b>(j, i)[0];
							rimg2.at<Vec3b>(j, i)[1] = warpImg.at<Vec3b>(j, i)[1];
							rimg2.at<Vec3b>(j, i)[2] = warpImg.at<Vec3b>(j, i)[2];
						}
					}
					
				}
			}
			
		}
	}
	imshow("resut", rimg2);
	cout << "end\n";

	waitKey(0);

	return EXIT_SUCCESS;
}