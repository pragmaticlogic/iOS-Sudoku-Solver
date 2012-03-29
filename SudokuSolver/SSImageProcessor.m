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
@property (nonatomic) IplImage *grayscaleImage;
@property (nonatomic) IplImage *binaryImage;
@property (nonatomic) IplImage *rgbImage;

@end

@implementation SSImageProcessor

@synthesize image = _image;
@synthesize grayscaleImage = _grayscaleImage;
@synthesize binaryImage = _binaryImage;
@synthesize rgbImage = _rgbImage;

- (void)dealloc {
    cvReleaseImage(&_binaryImage);
    cvReleaseImage(&_grayscaleImage);
    cvReleaseImage(&_rgbImage);
}

-(id)initWithImage:(UIImage *)image {
    if (self = [super init]) {
        assert(image);
        self.image = image;
        _rgbImage = [_image IplImage];
        _grayscaleImage = cvCreateImage(cvGetSize(_rgbImage),IPL_DEPTH_8U,1);
        cvCvtColor(_rgbImage, _grayscaleImage, CV_RGB2GRAY);
    }
    return self;
}

- (UIImage *)binarizeImage {
    IplImage* im_bw = cvCreateImage(cvGetSize(_grayscaleImage),IPL_DEPTH_8U,1);
    AdaptiveThreshold(_grayscaleImage, im_bw, 11);
    
    //////
//    //draws sudoku bounding box

    ///////
    
    IplImage* color_dst = cvCreateImage( cvGetSize(im_bw), IPL_DEPTH_8U, 3);
    cvCvtColor(im_bw, color_dst, CV_GRAY2RGB);
    _image = [UIImage imageFromIplImage:color_dst];

    cvReleaseImage(&color_dst);
    return _image;
}

- (UIImage *)normalizeImageRotation {
    IplImage *img = [_image IplImage];
    img = rotateImage(img, -15);
    return [UIImage imageFromIplImage:img];
}

- (UIImage *)detectBoundingRectangle {
    IplImage* im_bw = cvCreateImage(cvGetSize(_grayscaleImage),IPL_DEPTH_8U,1);
    CvPoint  box[2];    
    detectSudokuBoundingBox(im_bw, box);    
    cvRectangle(_rgbImage, box[0], box[1], CV_RGB(0,255,255), 6, CV_AA, 0);
    return [UIImage imageFromIplImage:_rgbImage];
}
@end
