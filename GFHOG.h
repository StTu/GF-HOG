////////////////////////////////////////////////////////////////////////////////
// GFHOG (.h/.cpp)
// Developed By: 			Stuart James
// Start Date: 				04 July 2012
// Version Publish Date:	04 March 2013
// Version Number:			1.0
// Notes:
// Development Info:
//		Requires SuperLu, OpenCV
// Development ToDo:
//
// Credit:				John Collomosse (Base Code), Rui Hu (Algorithm[1])
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
#include <vector>
#include <opencv/cxcore.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>

#define SETUP_SCALES 		scales.push_back(7);	scales.push_back(11);	scales.push_back(25);

#define POISSON_SPEEDUP_DEGRADE (1)
#define RESIZE_IMAGES_PAD 100

//#define SKIP_GRADIENT_FIELD_INTERPOLATION
//#define SOFTASSIGN
//#define NOHOGZONE
#define SOFTASSIGN_SIGMA (0.1)
#define DO_COSINE
enum GFHOGType{
	Image,
	Sketch
};

class GFHOG : public std::vector<std::vector<double> >
{
public:
	GFHOG(void);
	~GFHOG(void);
	void Compute(IplImage* src,GFHOGType t, IplImage* mask = NULL);
	void ComputeImage(IplImage* src,IplImage* mask = NULL);
	void ComputeSketch(IplImage* src,IplImage* mask = NULL);
	void ComputeEdge(IplImage* edge,IplImage* mask = NULL);
	void ComputeGradient(IplImage* Edge8bit,IplImage* mask);

	IplImage* Gradient() { return _gradient; };
	static IplImage* ResizeToFaster(IplImage* image,int maxdim);
private:
	IplImage* _gradient;
	void gradientField(IplImage* inpmask32,IplImage* filtermask32);
	void histogramOfGradients(IplImage* edge,IplImage* gradient);
	IplImage* poissoncompute(IplImage* src, IplImage* mask);
	CvScalar sub(CvScalar a, CvScalar b);
};
