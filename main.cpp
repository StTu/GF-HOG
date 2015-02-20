////////////////////////////////////////////////////////////////////////////////
// main (.cpp)
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

#include "GFHOG.h"
#include <iostream>
#include <fstream>

#ifdef WIN32
#ifdef _DEBUG
	#pragma comment(lib, "SuperLUd.lib")
	#pragma comment(lib, "opencv_calib3d249d.lib")
	#pragma comment(lib, "opencv_core249d.lib")
	#pragma comment(lib, "opencv_contrib249d.lib")
	#pragma comment(lib, "opencv_features2d249d.lib")
	#pragma comment(lib, "opencv_flann249d.lib")
	#pragma comment(lib, "opencv_highgui249d.lib")
	#pragma comment(lib, "opencv_imgproc249d.lib")
	#pragma comment(lib, "opencv_legacy249d.lib")
	#pragma comment(lib, "opencv_ml249d.lib")
	#pragma comment(lib, "opencv_nonfree249d.lib")
	#pragma comment(lib, "opencv_objdetect249d.lib")
	#pragma comment(lib, "opencv_photo249d.lib")
	#pragma comment(lib, "opencv_stitching249d.lib")
	#pragma comment(lib, "opencv_ts249d.lib")
	#pragma comment(lib, "opencv_video249d.lib")
	#pragma comment(lib, "opencv_videostab249d.lib")
#else
	#pragma comment(lib, "SuperLU.lib")
	#pragma comment(lib, "opencv_core249.lib")
	#pragma comment(lib, "opencv_highgui249.lib")
	#pragma comment(lib, "opencv_video249.lib")
	#pragma comment(lib, "opencv_ml249.lib")
	#pragma comment(lib, "opencv_legacy249.lib")
	#pragma comment(lib, "opencv_imgproc249.lib")
	#pragma comment(lib, "opencv_features2d249.lib")
	#pragma comment(lib, "opencv_contrib249.lib")
	#pragma comment(lib, "opencv_flann249.lib")
	#pragma comment(lib, "opencv_objdetect249.lib")
	#pragma comment(lib, "opencv_nonfree249.lib")
#endif
	#pragma comment(lib, "libatlas.lib")
	#pragma comment(lib, "libcblas.lib")
	#pragma comment(lib, "libf77blas.lib")
	#pragma comment(lib, "liblapack.lib")
	#pragma comment(lib, "libmetis.lib")
#endif

void writeVector(std::vector<double>& v, std::ofstream &str);

int main (int argc, const char *  argv [])
{
	std::string image_path;
	std::string mask_path;
	std::string grad_path;
	std::string out_path;
	int type = 0;
	int setSize = 100;
	// Display Help Message
	if (argc == 1)
	{
		std::cout << "GFHOG.[ln/exe] Options:" << std::endl;
		std::cout << "	Parameters Types:" << std::endl;
		std::cout << "		-i [IN: image path]" << std::endl;
		std::cout << "		-m [IN: mask image path]" << std::endl;
		std::cout << "		-t [IN: 0(Sketch)/1(Image)]" << std::endl;
		std::cout << "		-s [IN: Scale image to value(in pixels), maintaining aspect ratio]" << std::endl;
		std::cout << "		-g [OUT: gradient image path]" << std::endl;
		std::cout << "		-o [OUT: descriptors path]" << std::endl;
		
		return 0;
	}
	
	for (int i = 1 ; i < argc ; i++){
		std::string val = argv[i];
		// Dataset Options
		if (val == "-i"){
			image_path = std::string(argv[i+1]);
			i++;
		} else if (val =="-m"){
			mask_path = std::string(argv[i+1]);
			i++;
		} else if (val =="-g"){
			grad_path = std::string(argv[i+1]);
			i++;
		} else if (val =="-s"){
			int t = atoi(argv[i+1]);
			if (t < 1 )
				setSize = 100;
			else
				setSize = t;
			i++;
		} else if (val =="-t"){
			int t = atoi(argv[i+1]);
			if (t)
				t = 1;
			i++;
		} else if (val =="-o"){
			out_path = std::string(argv[i+1]);
			i++;
		}else{
			std::cout << "Uknown Parameter: " << argv[i] << std::endl;
			return 0;
		}
	}
	
	IplImage* img = cvLoadImage(image_path.c_str(),0);
	if (!img){
		std::cout <<"Error Loading Image" << std::endl;
		return 0;
	}
	// Resize to maximum size
	if (std::max(img->width,img->height) > setSize){
		// Change in size
		CvSize s;
		if (img->width > setSize){
			float r = (float)setSize / img->width ;
			s.width = setSize;
			s.height = (float)(img->height) * r;
		}else{
			float r = (float)setSize / img->height ;
			s.height = setSize;
			s.width = (float)(img->width) * r;
		}
		IplImage* resize = cvCreateImage(s,8,1);
		cvResize(img,resize);
		cvReleaseImage(&img);
		img = resize;
	}

	IplImage* mask = NULL;
	if (mask_path.length()){
		mask = cvLoadImage(image_path.c_str(),0);
		if (!mask){
			std::cout <<"Error Loading Mask" << std::endl;
			return 0;
		}
		cvZero(mask);
		cvCopyMakeBorder(mask,mask,cvPoint(setSize/100,setSize/100),IPL_BORDER_CONSTANT);
	}else{
		mask = cvCreateImage(cvGetSize(img),8,1);
		cvZero(mask);
		cvNot(mask,mask);
		//cvCopyMakeBorder(mask,mask,cvPoint(setSize/10,setSize/10),IPL_BORDER_CONSTANT);
	}

	GFHOG descriptor;

	descriptor.Compute(img,(GFHOGType)type,mask);
	
	if (out_path.length()){
		//descriptor.saveToFile(out_path.c_str());
		std::ofstream str;
		str.open(out_path.c_str());
		if (str.is_open()){
			GFHOG::iterator it = descriptor.begin();
			for ( ; it < descriptor.end() ; it++){
				writeVector(*it,str);
				str << std::endl;
			}
		}else{
			std::cout << "Failed to Save Descriptor" << std::endl;
		}
	}
		
	if (grad_path.length()){
		IplImage* g = descriptor.Gradient();
		IplImage* g8bit = cvCreateImage(cvGetSize(g),8,1);
		cvConvertScale(g,g8bit,255);
		cvSaveImage(grad_path.c_str(),g8bit);
	}


}

void writeVector(std::vector<double>& v, std::ofstream &str){
	std::vector<double>::iterator it = v.begin();
	str << *it;
	it++;
	for ( ; it < v.end() ; it++){
		str  << ',' << *it;
	}
}