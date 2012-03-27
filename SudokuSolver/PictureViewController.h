//
//  PictureViewController.h
//  SudokuSolver
//
//  Created by Denis Lebedev on 12/6/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//


@interface PictureViewController : UIViewController

@property (nonatomic, assign) IBOutlet UIImageView *imageView;
@property (unsafe_unretained, nonatomic) IBOutlet UIToolbar *toolBar;

- (IBAction)launchOCR:(id)sender;
- (IBAction)onAddPhotoTap:(id)sender;


@end
