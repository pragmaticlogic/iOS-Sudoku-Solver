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

@end

@implementation SSImageProcessor

@synthesize image = _image;


-(id)initWithImage:(UIImage *)image {
    if (self = [super init]) {
        assert(image);
        self.image = image;
    }
    return self;
}

- (UIImage *)binarizeImage:(UIImage *)source {
    IplImage *img_rgb = [source IplImage];
    IplImage *img_gray = cvCreateImage(cvGetSize(img_rgb),IPL_DEPTH_8U,1);
    cvCvtColor(img_rgb, img_gray, CV_RGB2GRAY);
    IplImage* im_bw = cvCreateImage(cvGetSize(img_gray),IPL_DEPTH_8U,1);
    AdaptiveThreshold(img_gray, im_bw, 11);
    
    IplImage* color_dst = cvCreateImage( cvGetSize(im_bw), IPL_DEPTH_8U, 3);
    cvCvtColor(im_bw, color_dst, CV_GRAY2RGB);
    UIImage *result = [UIImage imageFromIplImage:color_dst];

    cvReleaseImage(&color_dst);
    cvReleaseImage(&im_bw);
    return result;
}

- (UIImage *)normalizeImageRotation:(UIImage *)source {
    IplImage *img = [source IplImage];
    img = rotateImage(img, -15);
    return [UIImage imageFromIplImage:img];
}

- (UIImage *)detectBoundingRectangle:(UIImage *)source {
    IplImage *img_rgb = [source IplImage];
    IplImage *img_gray = cvCreateImage(cvGetSize(img_rgb),IPL_DEPTH_8U,1);
    cvCvtColor(img_rgb, img_gray, CV_RGB2GRAY);
    
    CvPoint  box[2];    
    detectSudokuBoundingBox(img_gray, box);    
    cvRectangle(img_rgb, box[0], box[1], CV_RGB(0,255,0), 1, CV_AA, 0);
    return [UIImage imageFromIplImage:img_rgb];
    //dealloc iplimages
}
@end
