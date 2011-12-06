//
//  Common.h
//  SudokuSolver
//
//  Created by Denis Lebedev on 12/6/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#import <OpenCV/opencv2/imgproc/imgproc_c.h>
#import <OpenCV/opencv2/objdetect/objdetect.hpp>

typedef struct  
{
    double r, g, b;    /* Channel intensities between 0.0 and 1.0 */
} rgb_color;

IplImage *rotateImage(const IplImage *img, int angle);
CvPoint intersectionBetweenLines(CvPoint p1, CvPoint p2, CvPoint p3, CvPoint p4);
void detectSudokuBoundingBox(IplImage *image, CvPoint box[]);
