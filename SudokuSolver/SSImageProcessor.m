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

@end

@implementation SSImageProcessor

@synthesize image = _image;
@synthesize grayscaleImage = _grayscaleImage;
@synthesize binaryImage = _binaryImage;

- (void)dealloc {
    cvReleaseImage(&_binaryImage);
    cvReleaseImage(&_grayscaleImage);
}

-(id)initWithImage:(UIImage *)image {
    if (self = [super init]) {
        assert(image);
        self.image = image;
        IplImage *img_rgb = [_image IplImage];
        _grayscaleImage = cvCreateImage(cvGetSize(img_rgb),IPL_DEPTH_8U,1);
        cvCvtColor(img_rgb, _grayscaleImage, CV_RGB2GRAY);
    }
    return self;
}

- (UIImage *)binarizeImage {
    IplImage* im_bw = cvCreateImage(cvGetSize(_grayscaleImage),IPL_DEPTH_8U,1);
    //cvAdaptiveThreshold(im_gray, im_bw, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 25, 0);
    AdaptiveThreshold(_grayscaleImage, im_bw, 11);
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
@end
