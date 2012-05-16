//
//  PictureViewController.m
//  SudokuSolver
//
//  Created by Denis Lebedev on 12/6/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#import "PictureViewController.h"
#import "SplitSudokuViewController.h"
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


#pragma mark - View lifecycle

- (void)viewDidLoad {
    [super viewDidLoad];
    UIBarButtonItem *item = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemCamera target:self action:@selector(onAddPhotoTap:)];
    self.navigationItem.rightBarButtonItem = item;
    self.imageView.image = [[UIImage imageNamed:@"3.jpg"] scaleAndRotateImage];
}

- (void)viewDidUnload {
    [self setToolBar:nil];
    [super viewDidUnload];
}


#pragma mark - Image Processsing

- (IBAction)onBinarizeButtonTap:(id)sender {
    self.imageProcessor = [[SSImageProcessor alloc] initWithImage:self.imageView.image];
    self.imageView.image = [_imageProcessor binarizeImage:self.imageView.image];
}

- (IBAction)onAddPhotoTap:(id)sender {
    UIActionSheet *actionSheet = [[UIActionSheet alloc] initWithTitle:@"Take Photo" delegate:self cancelButtonTitle:@"Cancel" destructiveButtonTitle:nil otherButtonTitles:@"From Camera", @"From Library", nil];
    [actionSheet showInView:self.view];
}

- (IBAction)onRotateButtonTap:(id)sender {
    self.imageView.image = [self.imageProcessor rotateImage:self.imageView.image];
}

- (IBAction)onDetectRectangleButtonTap:(id)sender {
    self.imageView.image = [self.imageProcessor detectLines:self.imageView.image];
}

- (IBAction)onSplitButtonTap:(id)sender {
    SplitSudokuViewController *splitVC = [[SplitSudokuViewController alloc] init];
    splitVC.itemsToDisplay = [self.imageProcessor splitImages];
    splitVC.imgProcessor = self.imageProcessor;
    [self.navigationController pushViewController:splitVC animated:YES];
}

- (IBAction)onThinTap:(id)sender {
    self.imageView.image = [self.imageProcessor thinImage:self.imageView.image];
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
