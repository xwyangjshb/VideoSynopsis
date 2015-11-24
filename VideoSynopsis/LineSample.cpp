#include <highgui.h>
#include "LineSample.h"

void LineSample(char *videoName) {
	CvCapture *capture = cvCreateFileCapture(videoName);
	CvLineIterator iterator;
	IplImage *frame;
	FILE *fpb = fopen("b.csv", "w");
	FILE *fpg = fopen("g.csv", "w");
	FILE *fpr = fopen("r.csv", "w");
	int pointNum;
	while (frame = cvQueryFrame(capture)) {
		pointNum = cvInitLineIterator(frame, cvPoint(0, 0), cvPoint(10, 0), &iterator);
		for (int i = 0; i < pointNum; i++) {
			fprintf(fpb, "%d,", iterator.ptr[0]);
			fprintf(fpg, "%d,", iterator.ptr[1]);
			fprintf(fpr, "%d,", iterator.ptr[2]);
			CV_NEXT_LINE_POINT(iterator);
		}
		fprintf(fpb, "\n");
		fprintf(fpg, "\n");
		fprintf(fpr, "\n");
	}
	fclose(fpb);
	fclose(fpg);
	fclose(fpr);
	cvReleaseCapture(&capture);
}