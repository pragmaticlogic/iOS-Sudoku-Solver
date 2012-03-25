//
//  PictureViewController.h
//  SudokuSolver
//
//  Created by Denis Lebedev on 12/6/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//


#import <UIKit/UIKit.h>
#import "UIImage+Additions.h"

namespace tesseract {
    class TessBaseAPI;
};

@interface PictureViewController : UIViewController
{
    tesseract::TessBaseAPI *tess;
}
@property (nonatomic, assign) IBOutlet UIImageView *imageView;


- (IBAction)launchOCR:(id)sender;


@end
