#  Readme
## Acknowledgement
To use this code please acknowlege where this code was used from:

James S, Collomosse J. "Interactive Video Asset Retrieval using Sketched Queries". CVMP'14

See http://stuart-james.com/Publications/ for more details


## Info
Developed By:           Stuart James

Start Date:             04 July 2012

Version Publish Date:   04 March 2013

Version Number:         1.0

Development Info:       Requires SuperLu, OpenCV

Credit:                 John Collomosse (Base Code), Rui Hu (Algorithm[1])

References:

        [1] Rui Hu; Barnard, M.; Collomosse, J.;
        "Gradient field descriptor for sketch based retrieval and localization,"
        Image Processing (ICIP), 2010 17th IEEE International Conference on ,
        vol., no., pp.1025-1028, 26-29 Sept. 2010 doi: 10.1109/ICIP.2010.5649331

License:

        BSD License, Copyright (c) 2012, Stuart James

                See: http://stuartjames.info/license.aspx


## Running
1. Modify Makefile or VS Project File for library paths
        Dependencies
                - SuperLU
                - OpenCV
2. Compile
3. Execute on a single image,

        Parameters:

                -i [IN: image path]*

                -m [IN: mask image path]

                -t [IN: 0(Sketch)/1(Image)]*

                -s [IN: Scale image to value(in pixels), maintaining aspect ratio]

                -g [OUT: gradient image path]

                -o [OUT: descriptors path]*

        * = required

e.g.
        GFHOG.exe -i "myImage.jpg" -t 1 -s 100 -o "myImage.des"
