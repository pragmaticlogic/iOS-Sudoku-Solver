//
//  UIImage+Additions.h
//  SudokuSolver
//
//  Created by Denis Lebedev on 12/6/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <OpenCV/opencv2/imgproc/imgproc_c.h>

@interface UIImage (Additions)

- (IplImage *)IplImage;
+ (UIImage *)imageFromIplImage:(IplImage *)image;

@end
