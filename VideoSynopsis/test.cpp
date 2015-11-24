#include "ForegroundCleanUp.h"
#include <cv.hpp>

using namespace cv;

const int MAX_CORNERS = 500;

void OpticalFlow() {
	IplImage *imgA = cvLoadImage("D:\\测试文件\\daytime\\daytime050.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	IplImage *imgB = cvLoadImage("D:\\测试文件\\daytime\\daytime051.jpg", CV_LOAD_IMAGE_GRAYSCALE);

	CvSize imgSize = cvGetSize(imgA);
	int winSize = 10;

	IplImage *imgC = cvCreateImage(imgSize, 8, 3);// cvLoadImage("", CV_LOAD_IMAGE_UNCHANGED);
	cvSetZero(imgC);

	IplImage *eigImage = cvCreateImage(imgSize, IPL_DEPTH_32F, 1);
	IplImage *tmpImage = cvCreateImage(imgSize, IPL_DEPTH_32F, 1);

	int cornerCount = MAX_CORNERS;
	CvPoint2D32f *cornersA = new CvPoint2D32f[MAX_CORNERS];

	cvGoodFeaturesToTrack(imgA, eigImage, tmpImage, cornersA, &cornerCount, 0.01, 5.0, 0, 3, 0, 0.04);
	cvFindCornerSubPix(imgA, cornersA, cornerCount, cvSize(winSize, winSize), cvSize(-1, -1), cvTermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.03));

	char featureFound[MAX_CORNERS];
	float featureErrors[MAX_CORNERS];

	CvSize pyrSize = cvSize(imgA->width + 8, imgB->height / 3);

	IplImage *pyrA = cvCreateImage(pyrSize, IPL_DEPTH_32F, 1);
	IplImage *pyrB = cvCreateImage(pyrSize, IPL_DEPTH_32F, 1);

	CvPoint2D32f *cornersB = new CvPoint2D32f[MAX_CORNERS];

	cvCalcOpticalFlowPyrLK(imgA, imgB, pyrA, pyrB, cornersA, cornersB, cornerCount, cvSize(winSize, winSize), 5, featureFound, featureErrors, cvTermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, 0.3), 0);

	for (int i = 0; i < cornerCount; i++) {
		if (featureFound[i] == 0 || featureErrors[i]>500) {
			printf("Error is %f\n", featureErrors[i]);
			continue;
		}
		printf("Got it\n");
		CvPoint p0 = cvPoint(cvRound(cornersA[i].x), cvRound(cornersA[i].y));

		CvPoint p1 = cvPoint(cvRound(cornersB[i].x), cvRound(cornersB[i].y));

		cvLine(imgC, p0, p1, CV_RGB(255, 0, 0), 2);
	}

	cvShowImage("ImageA", imgA);
	cvShowImage("ImageB", imgB);
	cvShowImage("LKpyr_OpticalFlow", imgC);
	cvWaitKey();
}

void VideoToPicture() {
	int n = 0;
	VideoCapture video("D:\\测试文件\\1344.avi");
	Mat frame;
	char fileName[256];
	video >> frame;
	while (frame.data) {
		sprintf(fileName, "D:\\测试文件\\1344\\%03d.bmp", n++);
		imwrite(fileName, frame);
		video >> frame;
	}
}

void CreateMask() {
	int n = 0;
	VideoCapture video("D:\\测试文件\\daytime.avi");
	Mat frame;
	Mat mask;
	Mat background;
	video >> frame;
	Ptr<BackgroundSubtractorMOG2> backgroundSubtractor = createBackgroundSubtractorMOG2();
	char foreFileName[256];
	while (frame.data) {
		backgroundSubtractor->apply(frame, mask);
		FindConnectedComponents(&IplImage(mask));
		//backgroundSubtractor->getBackgroundImage(background);
		imshow("mask", mask);
		//imshow("background", background);
		sprintf(foreFileName, "D:\\测试文件\\daytimemask\\%03d.bmp", n++);
		imwrite(foreFileName,mask);
		waitKey(1);
		video >> frame;
	}
}

void CreateForeground() {
	Mat mask;
	Mat image;
	Mat dst(Size(320, 240), CV_8UC3);
	char fileName[256];
	for (int i = 0; i < 1000; i++) {
		sprintf(fileName, "D:\\测试文件\\daytimemask\\%03d.bmp", i);
		mask = imread(fileName);
		sprintf(fileName, "D:\\测试文件\\daytime\\%03d.bmp", i);
		image = imread(fileName);
		cvAnd(&IplImage(mask), &IplImage(image), &IplImage(dst));
		imshow("foreground", dst);
		sprintf(fileName, "D:\\测试文件\\daytimefore\\%03d.bmp",i);
		imwrite(fileName, dst);
		waitKey(40);
	}
}

int main() {
	//CreateMask();
	CreateForeground();
	return 0;
}