#include "ForegroundCleanUp.h"

#define CVCONTOUR_APPROX_LEVEL 2

void FindConnectedComponents(IplImage *mask, int poly1Hull0, float perimScale, int *num, CvRect *bbs, CvPoint *centers) {
	static CvMemStorage *memStorage = NULL;
	static CvSeq *contours = NULL;

	//cvMorphologyEx(mask, mask, 0, 0, CV_MOP_OPEN);	//¿ªÔËËã
	//cvMorphologyEx(mask, mask, 0, 0, CV_MOP_CLOSE);	//±ÕÔËËã

	if (memStorage == NULL) {
		memStorage = cvCreateMemStorage();
	}
	else {
		cvClearMemStorage(memStorage);
	}

	CvContourScanner scanner = cvStartFindContours(mask, memStorage, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	CvSeq *c;
	int numCont = 0;
	while ((c = cvFindNextContour(scanner)) != NULL) {
		double len = cvContourPerimeter(c);
		double q = (mask->height + mask->width) / perimScale;

		if (len < q) {
			cvSubstituteContour(scanner, NULL);
		}
		else {
			CvSeq *cNew;
			if (poly1Hull0) {
				cNew = cvApproxPoly(c, sizeof(CvContour), memStorage, CV_POLY_APPROX_DP, CVCONTOUR_APPROX_LEVEL, 0);
			}
			else {
				cNew = cvConvexHull2(c, memStorage, CV_CLOCKWISE, 1);
			}
			cvSubstituteContour(scanner, cNew);
			numCont++;
		}
	}

	contours = cvEndFindContours(&scanner);
	const CvScalar CVX_WHITE = cvScalar(255, 255, 255);
	const CvScalar CVX_BLACK = cvScalar(0, 0, 0);
	cvSetZero(mask);
	IplImage *maskTemp;

	if (num != NULL) {
		int N = *num, numFilled = 0, i = 0;
		CvMoments moments;
		double M00, M01, M10;
		maskTemp = cvCloneImage(mask);
		for (i = 0, c = contours; c != NULL; c = c->h_next, i++) {
			if (i < N) {
				cvDrawContours(maskTemp, c, CVX_WHITE, CVX_BLACK, -1, CV_FILLED, 8);
				if (centers != NULL) {
					cvMoments(maskTemp, &moments, 1);
					M00 = cvGetSpatialMoment(&moments, 0, 0);
					M10 = cvGetSpatialMoment(&moments, 1, 0);
					M01 = cvGetSpatialMoment(&moments, 0, 1);
					centers[i].x = (int)(M10 / M00);
					centers[i].y = (int)(M01 / M00);
				}
				if (bbs != NULL) {
					bbs[i] = cvBoundingRect(c);
				}
				cvSetZero(maskTemp);
				numFilled++;
			}
			cvDrawContours(mask, c, CVX_WHITE, CVX_WHITE, -1, CV_FILLED, 8);
		}
		*num = numFilled;
		cvReleaseImage(&maskTemp);
	}
	else {
		for (c = contours; c != NULL; c = c->h_next) {
			cvDrawContours(mask, c, CVX_WHITE, CVX_WHITE, -1, CV_FILLED, 8);
		}
	}
}