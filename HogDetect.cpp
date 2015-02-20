#include "HogDetect.h"
#include <vector>
#include <fstream> 

HogDetect::HogDetect(IplImage* img,IplImage* avoid) {

	_avoidImage=cvCloneImage(avoid);
	_sourceImage=cvCloneImage(img);
	Construct();

}

HogDetect::HogDetect(IplImage* img) {

	_avoidImage=NULL;
	_sourceImage=cvCloneImage(img);
	Construct();

}

HogDetect::~HogDetect() {

if (_convx)
	cvReleaseImage(&_convx);
if (_convy)
	cvReleaseImage(&_convy);
if (_avoidImage)
	cvReleaseImage(&_avoidImage);
	cvReleaseImage(&_sourceImage);
	cvReleaseImage(&_gradientMag);
	cvReleaseImage(&_gradientAngle);

}

IplImage* HogDetect::HorizDiff(IplImage* img) {

	IplImage* result=cvCreateImage(cvGetSize(img),32,1);
	cvZero(result);
	int rowsum=0;
	for (int j=1; j<img->height-1; j++) {
		for (int i=1; i<img->width-1; i++) {
			float val=((float*)img->imageData)[(i-1)+rowsum]-((float*)img->imageData)[(i+1)+rowsum];
			((float*)(result->imageData))[i+rowsum]=val;
		}
		rowsum+=img->width;
	}

	return result;

}



#ifdef ALREADY_GRADIENT
void HogDetect::Construct() {

 // printf("WARNING:  Assuming input image is already gradient\n");
	_gradientMag=cvCreateImage(cvGetSize(_sourceImage),32,1);
	if (_avoidImage) {
		cvThreshold(_avoidImage,_avoidImage,1,255,CV_THRESH_BINARY);
		cvDilate(_avoidImage,_avoidImage);		
		cvConvertScale(_avoidImage,_gradientMag,1.0/255.0,0);
	}
	else {
        cvSet(_gradientMag,cvScalarAll(1.0),NULL);
	}
	_gradientAngle=cvCreateImage(cvGetSize(_sourceImage),32,1);
        cvConvertScale(_sourceImage,_gradientAngle,TWOPI,0);
	_convx=NULL;
	_convy=NULL;

}

#else

void HogDetect::Construct() {
	
	// Perform convolutions for gradient
	_convx=HorizDiff(_sourceImage);
	IplImage* rot90=ImageFlipXY(_sourceImage);
	IplImage* convytmp=HorizDiff(rot90);
	_convy=ImageFlipXY(convytmp);
	cvReleaseImage(&convytmp);
	cvReleaseImage(&rot90);

	_gradientMag=cvCreateImage(cvGetSize(_sourceImage),32,1);
	_gradientAngle=cvCreateImage(cvGetSize(_sourceImage),32,1);

	IplImage* gradmagtmp=cvCreateImage(cvGetSize(_sourceImage),32,1);
	cvCartToPolar(_convx,_convy,gradmagtmp,_gradientAngle,0);
	cvConvertScale(gradmagtmp,_gradientMag,1.0f/SQRT2,0);

	cvReleaseImage(&gradmagtmp);

}

#endif

void HogDetect::GetLocalHOG(int x, int y, HOGPARAMS* hogparams, float* localhisto) {

	int halfwin=hogparams->winsize >> 1;

	memset(localhisto,0,hogparams->hogchannels*sizeof(float));

	if (x+halfwin > _gradientAngle->width-1 ||
	    y+halfwin > _gradientAngle->height-1 || x<halfwin || y<halfwin) {
		std::cout << "WARNING: Local HoG out of range" << std::endl;
		return;
	}

	for (int j=y-halfwin; j<=y+halfwin;  j++) {
		for (int i=x-halfwin; i<=x+halfwin;  i++) {

			float ang=((float*)_gradientAngle->imageData)[i+j*_gradientAngle->width];
			float mag=((float*)_gradientMag->imageData)[i+j*_gradientMag->width];

			float weight=mag; // TODO - better func

			int bin=quantize_radian(ang,hogparams->hogchannels-1);
			
		       	localhisto[bin]+=weight;

		}
	}

}

int HogDetect::quantize_radian(float ang, int quant) {

	int res=floor(quant*(ang/TWOPI));
	if (res>=quant)
		res=quant;
	if (res<0)
		res=0;
	
	return res;

}

int HogDetect::CalcNumberDenseSamples(HOGPARAMS* hogparams) {

	int startx=-( (hogparams->superwinsize >> 1)*hogparams->winsize );
	int endx=( (hogparams->superwinsize >> 1)*hogparams->winsize );
	int starty=-( (hogparams->superwinsize >> 1)*hogparams->winsize );
	int endy=( (hogparams->superwinsize >> 1)*hogparams->winsize );

	//	return ((endx-startx)+1)*((endy-starty)+1);
	// Rui change

	return hogparams->superwinsize*hogparams->superwinsize;
}

void HogDetect::GetHOG(int x, int y, float* histo, HOGPARAMS* hogparams) {

	// mark invalid
	int histosize=CalcNumberDenseSamples(hogparams)*hogparams->hogchannels;

	for (int i=0; i<histosize; i++)
		histo[i]=-1;

	if (!pointInValidRange(x,y,hogparams))
		return;

	/*
	// Per pixel overlap
	int startx=x-( (hogparams->superwinsize >> 1)*hogparams->winsize );
	int endx=x+( (hogparams->superwinsize >> 1)*hogparams->winsize );
	int starty=y-( (hogparams->superwinsize >> 1)*hogparams->winsize );
	int endy=y+( (hogparams->superwinsize >> 1)*hogparams->winsize);
	int stepper=1;
	*/
	
	// Half block overlap	
	int stepper=hogparams->winsize;
	int startx=x-( (hogparams->superwinsize >> 1)*hogparams->winsize );
	int endx=x+( (hogparams->superwinsize >> 1)*hogparams->winsize );
	int starty=y-( (hogparams->superwinsize >> 1)*hogparams->winsize );
	int endy=y+( (hogparams->superwinsize >> 1)*hogparams->winsize);

	

	/* Get the local winsizexwinsize hog for each superwindow */
	std::vector <float*> localhogs;
	for (int supery=starty; supery<=endy; supery+=stepper) {
		for (int superx=startx; superx<=endx; superx+=stepper) {
			float* localhog=new float[hogparams->hogchannels];
//			memset(localhog,0,sizeof(float)*hogparams->hogchannels);
			GetLocalHOG(superx,supery,hogparams,localhog);
			localhogs.push_back(localhog);
		}	
	}

	/* Get total energy over all local hogs */
	float totalenergy=0;
	for (std::vector <float*>::iterator t=localhogs.begin(); t!=localhogs.end(); t++) {
		for (int c=0; c<hogparams->hogchannels; c++) {
			totalenergy+=((*t)[c])*((*t)[c]);
		}
	}
	if (totalenergy==0) // for div0
		totalenergy=1;

	totalenergy=sqrt(totalenergy);

	// Concatenate and normalise 
	int ctr=0;
	for (std::vector <float*>::iterator t=localhogs.begin(); t!=localhogs.end(); t++) {
		for (int c=0; c<hogparams->hogchannels; c++) {
			histo[ctr++]=(*t)[c]/totalenergy;
		}
		
		delete [] *t;
	}
	

}

bool HogDetect::pointInValidRange(int x, int y, HOGPARAMS* hogparams) {

  int side=(hogparams->winsize*hogparams->superwinsize) >> 1;

  if (x-side<0 || y-side<0 || x+side>=_sourceImage->width || y+side>=_sourceImage->height)
    return false;
  
  return true;

}

IplImage* HogDetect::ImageFlipXY(IplImage* in) {
	
	IplImage* out=cvCreateImage(cvSize(in->height, in->width),32,1);
	for (int j=0; j<in->height; j++) {
		for (int i=0; i<in->width; i++) {
			((float*)out->imageData)[j+i*out->width]=((float*)in->imageData)[i+j*in->width];
		}
	}

	return out;

}
