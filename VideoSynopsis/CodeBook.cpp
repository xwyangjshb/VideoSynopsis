#include "CodeBook.h"
#include "ForegroundCleanUp.h"

int UpdateCodeBook(unsigned char *p, codeBook &c, unsigned int *cbBounds, int numChannels) {
	unsigned int high[3], low[3];
	for (int n = 0; n < numChannels; n++) {
		high[n] = *(p + n) + *(cbBounds + n);
		if (high[n] > 255)
			high[n] = 255;
		low[n] = *(p + n) - *(cbBounds + n);
		if (low[n] < 0)
			low[n] = 0;
	}
	int matchChannel;
	int i;
	for (i = 0; i < c.numEntries; i++) {
		matchChannel = 0;
		for (int n = 0; n < numChannels; n++) {
			if ((c.cb[i]->learnLow[n] <= *(p + n)) && (*(p + n) <= c.cb[i]->learnHigh[n])) {
				matchChannel++;
			}
		}
		if (matchChannel == numChannels) {
			c.cb[i]->t_last_update = c.t;
			for (int n = 0; n < numChannels; n++) {
				if (c.cb[i]->max[n] < *(p + n)) {
					c.cb[i]->max[n] = *(p + n);
				}
				else if (c.cb[i]->min[n]>*(p + n)) {
					c.cb[i]->min[n] = *(p + n);
				}
			}
		}
		break;
	}

	for (int s = 0; s < c.numEntries; s++) {
		int negRun = c.t - c.cb[s]->t_last_update;
		if (c.cb[s]->stale < negRun)
			c.cb[s]->stale = negRun;
	}

	if (i == c.numEntries) {
		codeElement **foo = new codeElement*[c.numEntries + 1];
		for (int ii = 0; ii < c.numEntries; ii++) {
			foo[ii] = c.cb[ii];
		}
		foo[c.numEntries] = new codeElement;
		if (c.numEntries)
			delete[] c.cb;
		c.cb = foo;
		for (int n = 0; n < numChannels; n++) {
			c.cb[c.numEntries]->learnHigh[n] = high[n];
			c.cb[c.numEntries]->learnLow[n] = low[n];
			c.cb[c.numEntries]->max[n] = *(p + n);
			c.cb[c.numEntries]->min[n] = *(p + n);
		}
		c.cb[c.numEntries]->t_last_update = c.t;
		c.cb[c.numEntries]->stale = 0;
		c.numEntries += 1;
	}

	for (int n = 0; n < numChannels; n++) {
		if (c.cb[i]->learnHigh[n] < high[n])
			c.cb[i]->learnHigh[n] += 1;
		if (c.cb[i]->learnLow[n] > low[n])
			c.cb[i]->learnLow[n] -= 1;
	}
	return i;
}

int ClearStaleEntries(codeBook &c) {
	int staleThresh = c.t >> 1;
	int *keep = new int[c.numEntries];
	int keepCnt = 0;
	for (int i = 0; i < c.numEntries; i++) {
		if (c.cb[i]->stale>staleThresh)
			keep[i] = 0;
		else {
			keep[i] = 1;
			keepCnt += 1;
		}
	}
	c.t = 0;
	codeElement **foo = new codeElement *[keepCnt];
	int k = 0;
	for (int ii = 0; ii < c.numEntries; ii++) {
		if (keep[ii]) {
			foo[k] = c.cb[ii];
			foo[k]->t_last_update = 0;
			k++;
		}
	}
	delete[] keep;
	delete[] c.cb;
	c.cb = foo;
	int numCleared = c.numEntries - keepCnt;
	c.numEntries = keepCnt;
	return numCleared;
}

unsigned char BackgroundDiff(unsigned char *p, codeBook &c, int numChannels, int *minMod, int *maxMod) {
	int i;
	int matchChannel;
	for (i = 0; i < c.numEntries; i++) {
		matchChannel = 0;
		for (int n = 0; n < numChannels; n++) {
			if ((c.cb[i]->min[n] - minMod[n] <= *(p + n)) && (*(p + n) <= c.cb[i]->max[n] + maxMod[n])) {
				matchChannel++;
			}
			else {
				break;
			}
		}
		if (matchChannel == numChannels) {
			break;
		}
	}
	if (i >= c.numEntries)
		return 255;
	return 0;
}

void test() {
	CvCapture *capture = cvCreateFileCapture("D:\\≤‚ ‘ ”∆µ\\1344.avi");
	int frameWidth = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
	int frameHeight = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
	codeBook *cb = new codeBook[frameWidth * frameHeight];
	memset(cb, 0, frameWidth * frameHeight * sizeof(codeBook));
	unsigned int bound[3] = { 10 ,10,10 };
	int minMod[3] = { 10 ,10,10 };
	int maxMod[3] = { 10 ,10,10 };
	IplImage *frame[1000];

	for (int i = 0; i < 1000; i++) {
		frame[i] = cvCreateImage(cvSize(frameWidth, frameHeight), 8, 3);
		cvCopy(cvQueryFrame(capture), frame[i]);
	}

	for (int i = 0; i < frameWidth * frameHeight; i++) {
		for (int j = 0; j < 1000; j++) {
			UpdateCodeBook((unsigned char*)frame[j]->imageData + i * 3, cb[i], bound, 3);
		}
	}

	IplImage *mask = cvCreateImage(cvSize(frameWidth, frameHeight), 8, 1);

	for (int i = 0; i < 1000; i++) {
		for (int j = 0; j < frameWidth * frameHeight; j++) {
			mask->imageData[j] = BackgroundDiff((unsigned char*)frame[i]->imageData + j * 3, cb[j], 3, minMod, maxMod);
		}
		FindConnectedComponents(mask);
		cvShowImage("1", mask);
		cvShowImage("2", frame[i]);
		cvWaitKey(30);
	}
}