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
@property (nonatomic, retain) IplImage *processedImage;

@end

@implementation SSImageProcessor

@synthesize image = _image;

-(id)initWithImage:(UIImage *)image {
    if (self = [super init]) {
    }
    return self;
}

- (UIImage *)binarizeImage {
    AdaptiveThreshold(<#IplImage *source#>, <#IplImage *result#>, 11);
}

@end
