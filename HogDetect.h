////////////////////////////////////////////////////////////////////////////////
// HogDetect (.h/.cpp)
// Developed By: 			Stuart James
// Start Date: 				04 July 2012
// Version Publish Date:	04 March 2013
// Version Number:			1.0
// Notes:
// Development Info:
//		Requires OpenCV
// Development ToDo:
//
// Credit:					John Collomosse (Base Code), Rui Hu (Algorithm[1])
// References:
//		[1] Rui Hu; Barnard, M.; Collomosse, J.;
//			"Gradient field descriptor for sketch based retrieval and localization," 
//			Image Processing (ICIP), 2010 17th IEEE International Conference on , 
//			vol., no., pp.1025-1028, 26-29 Sept. 2010 doi: 10.1109/ICIP.2010.5649331  
// License:
//		BSD License, Copyright (c) 2012, Stuart James
//			See: http://stuartjames.info/license.aspx
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <opencv/cv.h>
#include <opencv/cxcore.h>
#define ALREADY_GRADIENT

#define SQRT2 (1.414213562373)
#define TWOPI (6.283185307180)


typedef struct _hogparams_struct {

	int winsize;
	int superwinsize;
	int hogchannels;

} HOGPARAMS;

class HogDetect {

public:
	HogDetect(IplImage* img);
	HogDetect(IplImage* img,IplImage* avoid);
	~HogDetect();

	void Construct();
	void GetHOG(int x, int y, float* histo, HOGPARAMS* hg);
	bool pointInValidRange(int x, int y, HOGPARAMS* hg);
	static int CalcNumberDenseSamples(HOGPARAMS* hogparams);

protected:
	IplImage* _sourceImage;
	IplImage* _convx;
	IplImage* _convy;
	IplImage* _gradientAngle;
	IplImage* _gradientMag;
	IplImage* _avoidImage;

	IplImage* HorizDiff(IplImage* img);
	IplImage* ImageFlipXY(IplImage* in);
	int quantize_radian(float ang, int quant);
	void GetLocalHOG(int x, int y, HOGPARAMS* hogparams, float* localhisto);

};

