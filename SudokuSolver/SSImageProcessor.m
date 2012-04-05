//
//  SSImageProcessor.m
//  SudokuSolver
//
//  Created by Denis Lebedev on 3/26/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "SSImageProcessor.h"
#import "UIImage+Additions.h"

#ifdef __cplusplus
extern "C" {
#endif
#import "Common.h"
#ifdef __cplusplus
}
#endif

@interface SSImageProcessor ()

@property (nonatomic, retain) UIImage *image;

@property (nonatomic, retain) UIImage *binarizedImage;

@end

@implementation SSImageProcessor

@synthesize image = _image;
@synthesize binarizedImage = _binarizedImage;

-(id)initWithImage:(UIImage *)image {
    if (self = [super init]) {
        assert(image);
        self.image = image;
    }
    return self;
}

- (UIImage *)binarizeImage:(UIImage *)source {
    IplImage *img_rgb = [source IplImage];
    IplImage* im_bw = cvCreateImage(cvGetSize(img_rgb),IPL_DEPTH_8U,1);
    AdaptiveThreshold(img_rgb, im_bw, 11);
    
    IplImage* color_dst = cvCreateImage( cvGetSize(im_bw), IPL_DEPTH_8U, 3);
    cvCvtColor(im_bw, color_dst, CV_GRAY2RGB);
    self.binarizedImage = [UIImage imageFromIplImage:color_dst];

    cvReleaseImage(&color_dst);
    cvReleaseImage(&im_bw);
    return self.binarizedImage;
}

- (UIImage *)detectBoundingRectangle:(UIImage *)source {
    IplImage *img_rgb = [source IplImage];
    IplImage *img_gray = cvCreateImage(cvGetSize(img_rgb),IPL_DEPTH_8U,1);
    cvCvtColor(img_rgb, img_gray, CV_RGB2GRAY);
    
    CvPoint  box[2];    
    detectSudokuBoundingBox(img_gray, box);    
    cvRectangle(img_rgb, box[0], box[1], CV_RGB(0,255,0), 2, CV_AA, 0);
    return [UIImage imageFromIplImage:img_rgb];
    //dealloc iplimages
}


- (UIImage *)closeImage:(UIImage *)source {
    IplImage *img_rgb = [source IplImage];
    IplImage *img_gray = cvCreateImage(cvGetSize(img_rgb),IPL_DEPTH_8U,1);
    cvCvtColor(img_rgb, img_gray, CV_RGB2GRAY);
    
    int radius = 4;
    IplConvKernel* Kern = cvCreateStructuringElementEx(radius*2+1, radius*2+1, radius, radius, CV_SHAPE_RECT, NULL);
    cvErode(img_gray, img_gray, Kern, 1);
    cvDilate(img_gray, img_gray, Kern, 1);
    

    
    cvReleaseStructuringElement(&Kern);
    
    CvScalar pixel;
    
    IplImage* color_dst = cvCreateImage( cvGetSize(img_gray), IPL_DEPTH_8U, 3);
    cvCvtColor(img_gray, color_dst, CV_GRAY2RGB);

    return [UIImage imageFromIplImage:color_dst];

    for (int i = 1; i < img_rgb->height - 1; i++) {
        for (int j = 1; j < img_rgb->width - 1; j++) {
            pixel = cvGet2D(img_gray, i, j);
            
            if (pixel.val[0] == 0.0) {
                if (cvGet2D(img_gray, i - 1, j).val[0] == 0.0 && 
                    cvGet2D(img_gray, i + 1, j).val[0] == 0.0 && 
                    cvGet2D(img_gray, i, j - 1).val[0] == 0.0 &&
                    cvGet2D(img_gray, i, j + 1).val[0] == 0.0) {
                    //cvSet2D(img_rgb, i, j, pixel);
                    cvCircle(color_dst, cvPoint(j, i), 2, CV_RGB(255,255,0), 1, CV_AA, 0);
                }
            }
        }
    }

    UIImage *result = [UIImage imageFromIplImage:color_dst];

    return result;
}

- (double)imageRotationAngle:(UIImage *)source {
    IplImage *img_rgb = [source IplImage];
    IplImage *img_bw = cvCreateImage(cvGetSize(img_rgb),IPL_DEPTH_8U,1);
    IplImage* dst = cvCreateImage(cvGetSize(img_rgb),IPL_DEPTH_8U,1);
    IplImage* color_dst = cvCreateImage(cvGetSize(img_rgb),IPL_DEPTH_8U,3);
    
    cvCvtColor(img_rgb, img_bw, CV_RGB2GRAY);
    cvCanny( img_bw, dst, 50, 100, 3 );
    cvCvtColor( dst, color_dst, CV_GRAY2RGB);
    
    CvMemStorage* storage = cvCreateMemStorage(0);
    CvSeq* lines = 0;
    
    lines = cvHoughLines2( dst, storage, CV_HOUGH_PROBABILISTIC, 1, CV_PI/180, 50, 50, 10);
    
    double imageRotationAngle = 0;
    
    for(int i = 0; i < lines->total; i++ ){
        CvPoint* line = (CvPoint*)cvGetSeqElem(lines, i);
        double tempAngle = atan2(line[1].y - line[0].y, line[1].x - line[0].x) * 180.0 / CV_PI;
        if (tempAngle > - 20 && tempAngle < 20 && imageRotationAngle == 0) {
            imageRotationAngle = tempAngle;
        }
    }
    return imageRotationAngle;
}

- (UIImage *)rotateImage:(UIImage *)source {
    double rotationAngle = [self imageRotationAngle:source];
    return [UIImage imageFromIplImage:rotateImage([source IplImage], rotationAngle)];
}
            
- (UIImage *)detectLines:(UIImage *)source {
    IplImage *img_rgb = [source IplImage];
    IplImage *img_bw = cvCreateImage(cvGetSize(img_rgb),IPL_DEPTH_8U,1);
    IplImage* dst = cvCreateImage(cvGetSize(img_rgb),IPL_DEPTH_8U,1);
    IplImage* color_dst = cvCreateImage(cvGetSize(img_rgb),IPL_DEPTH_8U,3);
    
    cvCvtColor(img_rgb, img_bw, CV_RGB2GRAY);
    cvCanny( img_bw, dst, 50, 100, 3 );
    cvCvtColor( dst, color_dst, CV_GRAY2RGB);

    CvMemStorage* storage = cvCreateMemStorage(0);
    CvSeq* lines = 0;
    
    lines = cvHoughLines2( dst, storage, CV_HOUGH_PROBABILISTIC, 1, CV_PI/180, 50, 50, 10);
    
    UIImage *image = [self detectBoundingRectangle:[UIImage imageFromIplImage:img_rgb]];
     
    img_rgb = [image IplImage];
    
    for(int i = 0; i < lines->total; i++ ){
        CvPoint* line = (CvPoint*)cvGetSeqElem(lines, i);
        cvLine(img_rgb, line[0], line[1], CV_RGB(255,0,255), 1, CV_AA, 0 );
    }

    cvReleaseMemStorage(&storage);
    cvReleaseImage(&dst);
    
    return [UIImage imageFromIplImage:img_rgb];
}
@end
