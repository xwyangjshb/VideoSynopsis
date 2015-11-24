#include "AveragingMethod.h"
#include "ForegroundCleanUp.h"

IplImage *IavgF, *IdiffF, *IprevF, *IhiF, *IlowF;
IplImage *Iscratch, *Iscratch2;

IplImage *Igray1, *Igray2, *Igray3;
IplImage *Ilow1, *Ilow2, *Ilow3;
IplImage *Ihi1, *Ihi2, *Ihi3;
IplImage *Imaskt;

float Icount;

void AllocateImages(IplImage *I) {
	CvSize sz = cvGetSize(I);

	IavgF = cvCreateImage(sz, IPL_DEPTH_32F, 3);
	IdiffF = cvCreateImage(sz, IPL_DEPTH_32F, 3);
	IprevF = cvCreateImage(sz, IPL_DEPTH_32F, 3);
	IhiF = cvCreateImage(sz, IPL_DEPTH_32F, 3);
	IlowF = cvCreateImage(sz, IPL_DEPTH_32F, 3);
	Ilow1 = cvCreateImage(sz, IPL_DEPTH_32F, 1);
	Ilow2 = cvCreateImage(sz, IPL_DEPTH_32F, 1);
	Ilow3 = cvCreateImage(sz, IPL_DEPTH_32F, 1);
	Ihi1 = cvCreateImage(sz, IPL_DEPTH_32F, 1);
	Ihi2 = cvCreateImage(sz, IPL_DEPTH_32F, 1);
	Ihi3 = cvCreateImage(sz, IPL_DEPTH_32F, 1);
	cvSetZero(IavgF);
	cvSetZero(IdiffF);
	cvSetZero(IprevF);
	cvSetZero(IhiF);
	cvSetZero(IlowF);
	Icount = 0.00001;

	Iscratch = cvCreateImage(sz, IPL_DEPTH_32F, 3);
	Iscratch2 = cvCreateImage(sz, IPL_DEPTH_32F, 3);
	Igray1 = cvCreateImage(sz, IPL_DEPTH_32F, 1);
	Igray2 = cvCreateImage(sz, IPL_DEPTH_32F, 1);
	Igray3 = cvCreateImage(sz, IPL_DEPTH_32F, 1);
	Imaskt = cvCreateImage(sz, IPL_DEPTH_8U, 1);

	cvZero(Iscratch);
	cvZero(Iscratch2);
}

void AccumulateBackground(IplImage *I) {
	static int first = 1;
	cvConvertScale(I, Iscratch, 1, 0);
	if (!first) {
		cvAcc(Iscratch, IavgF);
		cvAbsDiff(Iscratch, IprevF, Iscratch2);
		cvAcc(Iscratch2, IdiffF);
		Icount += 1.0;
	}
	first = 0;
	cvCopy(Iscratch, IprevF);
}

void SetHighThreshold(float scale) {
	cvConvertScale(IdiffF, Iscratch, scale);
	cvAdd(Iscratch, IavgF, IhiF);
	cvSplit(IhiF, Ihi1, Ihi2, Ihi3, 0);
}

void SetLowThreshold(float scale) {
	cvConvertScale(IdiffF, Iscratch, scale);
	cvSub(IavgF, Iscratch, IlowF);
	cvSplit(IlowF, Ilow1, Ilow2, Ilow3, 0);
}

void CreateModelsfromStats() {
	cvConvertScale(IavgF, IavgF, (double)(1.0 / Icount));
	cvConvertScale(IdiffF, IdiffF, (double)(1.0 / Icount));

	cvAddS(IdiffF, cvScalar(1, 1, 1), IdiffF);
	SetHighThreshold(7.0);
	SetLowThreshold(6.0);
}

void BackgroundDiff(IplImage *I, IplImage*Imask) {
	cvConvertScale(I, Iscratch, 1, 0);
	cvSplit(Iscratch, Igray1, Igray2, Igray3, 0);
	cvInRange(Igray1, Ilow1, Ihi1, Imask);

	cvInRange(Igray2, Ilow2, Ihi2, Imaskt);
	cvOr(Imask, Imaskt, Imask);

	cvInRange(Igray3, Ilow3, Ihi3, Imaskt);
	cvOr(Imask, Imaskt, Imask);
	cvSubRS(Imask, 255, Imask);
}

void DeallocateImages() {
	cvReleaseImage(&IavgF);
	cvReleaseImage(&IdiffF);
	cvReleaseImage(&IprevF);
	cvReleaseImage(&IhiF);
	cvReleaseImage(&IlowF);
	cvReleaseImage(&Iscratch);
	cvReleaseImage(&Iscratch2);
	cvReleaseImage(&Igray1);
	cvReleaseImage(&Igray2);
	cvReleaseImage(&Igray3);
	cvReleaseImage(&Ilow1);
	cvReleaseImage(&Ilow2);
	cvReleaseImage(&Ilow3);
	cvReleaseImage(&Ihi1);
	cvReleaseImage(&Ihi2);
	cvReleaseImage(&Ihi3);
	cvReleaseImage(&Imaskt);
}

void AveragingMethodTest() {
	CvCapture *capture = cvCreateFileCapture("D:\\≤‚ ‘ ”∆µ\\daytime.avi");
	int fps = cvGetCaptureProperty(capture, CV_CAP_PROP_FPS);
	IplImage *frame;
	IplImage *back = cvLoadImage("back.jpg");
	IplImage *mask = cvCreateImage(cvGetSize(back), 8, 1);
	AllocateImages(back);
	while (frame = cvQueryFrame(capture)) {
		AccumulateBackground(frame);
	}
	CreateModelsfromStats();
	cvSetCaptureProperty(capture, CV_CAP_PROP_POS_FRAMES, 0);
	while (frame = cvQueryFrame(capture)) {
		BackgroundDiff(frame, mask);
		FindConnectedComponents(mask);
		cvShowImage("Display", mask);
		cvWaitKey(1000 / fps);
	}
	cvReleaseImage(&back);
	cvReleaseImage(&mask);
	cvReleaseCapture(&capture);
}