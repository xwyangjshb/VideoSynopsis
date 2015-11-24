#include <highgui.h>
#include "FrameDiff.h"
#include "ForegroundCleanUp.h"

void FrameDiff(char *videoName, char *backImage) {
	CvCapture *capture = cvCreateFileCapture(videoName);
	IplImage *frame;
	IplImage *back = cvLoadImage(backImage);
	IplImage *backR = cvCreateImage(cvGetSize(back), 8, 1);
	IplImage *backG = cvCreateImage(cvGetSize(back), 8, 1);
	IplImage *backB = cvCreateImage(cvGetSize(back), 8, 1);
	cvSplit(back, backR, backG, backB, 0);
	IplImage *frameR = cvCreateImage(cvGetSize(back), 8, 1);
	IplImage *frameG = cvCreateImage(cvGetSize(back), 8, 1);
	IplImage *frameB = cvCreateImage(cvGetSize(back), 8, 1);
	IplImage *foreR = cvCreateImage(cvGetSize(back), 8, 1);
	IplImage *foreG = cvCreateImage(cvGetSize(back), 8, 1);
	IplImage *foreB = cvCreateImage(cvGetSize(back), 8, 1);
	while (frame = cvQueryFrame(capture)) {
		cvSplit(frame, frameR, frameG, frameB, 0);
		cvAbsDiff(backR, frameR, foreR);
		cvAbsDiff(backG, frameG, foreG);
		cvAbsDiff(backB, frameB, foreB);
		cvThreshold(foreR, foreR, 15, 255, CV_THRESH_BINARY);
		cvThreshold(foreG, foreG, 15, 255, CV_THRESH_BINARY);
		cvThreshold(foreB, foreB, 15, 255, CV_THRESH_BINARY);
		cvOr(foreR, foreG, foreR);
		cvOr(foreR, foreB, foreR);
		FindConnectedComponents(foreR);
		cvShowImage("display", foreR);
		cvWaitKey(1);
	}
	cvReleaseImage(&back);
	cvReleaseImage(&backR);
	cvReleaseImage(&backG);
	cvReleaseImage(&backB);
	cvReleaseImage(&frameR);
	cvReleaseImage(&frameG);
	cvReleaseImage(&frameB);
	cvReleaseImage(&foreR);
	cvReleaseImage(&foreG);
	cvReleaseImage(&backB);
	cvReleaseCapture(&capture);
}