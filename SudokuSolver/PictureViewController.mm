//
//  PictureViewController.m
//  SudokuSolver
//
//  Created by Denis Lebedev on 12/6/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#import "PictureViewController.h"
#import "SSImageProcessor.h"
#import "UIImage+Additions.h"

#ifdef __cplusplus
extern "C" {
#endif
#import "Common.h"
#import "SSSolver.h"
#ifdef __cplusplus
}
#endif

int testSudoku[9][9] = {
    {1,0,0,0,0,7,0,9,0},
    {0,3,0,0,2,0,0,0,8},
    {0,0,9,6,0,0,5,0,0},
    {0,0,5,3,0,0,9,0,0},
    {0,1,0,0,8,0,0,0,2},
    {6,0,0,0,0,4,0,0,0},
    {3,0,0,0,0,0,0,1,0},
    {0,4,0,0,0,0,0,0,7},
    {0,0,7,0,0,0,3,0,0}};

@interface PictureViewController () <UIActionSheetDelegate, UIImagePickerControllerDelegate, UINavigationControllerDelegate>

@property (nonatomic, retain) SSImageProcessor *imageProcessor;

@end
@implementation PictureViewController

@synthesize imageView;
@synthesize toolBar;
@synthesize imageProcessor = _imageProcessor;

- (void)detectLines {
    assert(self.imageView.image);

    IplImage* dst = 0;
        
    CvMemStorage* storage = cvCreateMemStorage(0);
    CvSeq* lines = 0;
    int i = 0;
    
    //detect endges
    //cvCanny( im_bw, dst, 50, 200, 3 );

    //convert to RGB
    //cvCvtColor( dst, color_dst, CV_GRAY2RGB);
    
    
    //find lines
    lines = cvHoughLines2( dst, storage, CV_HOUGH_PROBABILISTIC, 1, CV_PI/180, 50, 50, 10);
    
    double imageRotationAngle = 0;
    
    //draw the lines
    for( i = 0; i < lines->total; i++ ){
        CvPoint* line = (CvPoint*)cvGetSeqElem(lines, i);
        double tempAngle = atan2(line[1].y - line[0].y, line[1].x - line[0].x) * 180.0 / CV_PI;
        if (tempAngle > - 30 && tempAngle < 30 && imageRotationAngle == 0) {
            imageRotationAngle = tempAngle;
        }
        //cvLine( img_rgb, line[0], line[1], CV_RGB(255,255,0), 1, CV_AA, 0 );
    }
    
    cvReleaseMemStorage(&storage);
    cvReleaseImage(&dst);
}


#pragma mark - View lifecycle

- (void)viewDidLoad {
    [super viewDidLoad];
    self.imageView.image = [[UIImage imageNamed:@"test1.jpg"] scaleAndRotateImage];
    
    
    //[self detectLines];
    //SolveSudoku(testSudoku);
}

- (void)viewDidUnload {
    [self setToolBar:nil];
    [super viewDidUnload];
}


#pragma mark - Image Processsing

- (IBAction)onBinarizeButtonTap:(id)sender {
    self.imageProcessor = [[SSImageProcessor alloc] initWithImage:self.imageView.image];

    self.imageView.image = [_imageProcessor binarizeImage];
    //detect borders
}

- (IBAction)onAddPhotoTap:(id)sender {
    UIActionSheet *actionSheet = [[UIActionSheet alloc] initWithTitle:@"Take Photo" delegate:self cancelButtonTitle:@"Cancel" destructiveButtonTitle:nil otherButtonTitles:@"From Camera", @"From Library", nil];
    [actionSheet showFromToolbar:self.toolBar];
}

- (IBAction)onRotateButtonTap:(id)sender {
    self.imageView.image = [_imageProcessor normalizeImageRotation];
}

- (IBAction)onDetectRectangleButtonTap:(id)sender {
    self.imageView.image = [_imageProcessor detectBoundingRectangle];
}

#pragma mark - UIActionSheetDelegate

- (void)actionSheet:(UIActionSheet *)actionSheet clickedButtonAtIndex:(NSInteger)buttonIndex {
    UIImagePickerController *imgPickerController = [[UIImagePickerController alloc] init];
    imgPickerController.delegate = self;
    switch (buttonIndex) {
        case 0:
            imgPickerController.sourceType = UIImagePickerControllerSourceTypeCamera;
            [self presentModalViewController:imgPickerController animated:YES];
            break;
        case 1:
            [self presentModalViewController:imgPickerController animated:YES];
        default:
            break;
    }
}

#pragma mark - UIImagePickerControllerDelegate

- (void)imagePickerController:(UIImagePickerController *)picker didFinishPickingMediaWithInfo:(NSDictionary *)info {
    UIImage *result = [info objectForKey:UIImagePickerControllerOriginalImage];    
    result = [result scaleAndRotateImage];
    self.imageView.image = result;
    [picker dismissModalViewControllerAnimated:YES];
}
- (void)imagePickerControllerDidCancel:(UIImagePickerController *)picker {
    [picker dismissModalViewControllerAnimated:YES];
}

@end
