#pragma once

#define CHANNELS 3
typedef struct codeElement {
	unsigned char learnHigh[CHANNELS];
	unsigned char learnLow[CHANNELS];
	unsigned char max[CHANNELS];
	unsigned char min[CHANNELS];
	int t_last_update;
	int stale;
}codeElement;

typedef struct codeBook {
	codeElement **cb;
	int numEntries;
	int t;
}codeBook;

int UpdateCodeBook(unsigned char *p, codeBook &c, unsigned int *cbBounds, int numChannels);

int ClearStaleEntries(codeBook &c);

unsigned char BackgroundDiff(unsigned char *p, codeBook &c, int numChannels, int *minMod, int *maxMod);

void test();