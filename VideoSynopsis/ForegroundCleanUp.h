#pragma once
#include <highgui.h>

void FindConnectedComponents(IplImage *mask, int poly1Hull0 = 1, float perimScale = 4, int *num = NULL, CvRect *bbs = NULL, CvPoint *centers = NULL);