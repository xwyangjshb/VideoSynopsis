#pragma once

#include <highgui.h>

void AllocateImages(IplImage *I);

void AccumulateBackground(IplImage *I);

void SetHighThreshold(float scale);

void SetLowThreshold(float scale);

void CreateModelsfromStats();

void BackgroundDiff(IplImage *I, IplImage*Imask);

void DeallocateImages();

void AveragingMethodTest();