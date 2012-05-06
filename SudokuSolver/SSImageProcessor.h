//
//  SSImageProcessor.h
//  SudokuSolver
//
//  Created by Denis Lebedev on 3/26/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

@interface SSImageProcessor : NSObject

-(id)initWithImage:(UIImage *)image;
- (UIImage *)binarizeImage:(UIImage *)source;
- (UIImage *)detectBoundingRectangle:(UIImage *)source;
- (UIImage *)rotateImage:(UIImage *)source;
- (UIImage *)detectLines:(UIImage *)source;
- (int)recognizeDigit:(UIImage *)source;

@property (nonatomic, readonly) NSArray *splitImages;
@end
