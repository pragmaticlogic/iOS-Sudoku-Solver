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

- (IBAction)onBinarizeButtonTap:(id)sender;
- (IBAction)onAddPhotoTap:(id)sender;
- (IBAction)onRotateButtonTap:(id)sender;
- (IBAction)onDetectRectangleButtonTap:(id)sender;
- (IBAction)onSplitButtonTap:(id)sender;
- (IBAction)onThinTap:(id)sender;



@end
