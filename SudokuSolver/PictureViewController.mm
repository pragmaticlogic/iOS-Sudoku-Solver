//
//  PictureViewController.m
//  SudokuSolver
//
//  Created by Denis Lebedev on 12/6/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#import "PictureViewController.h"
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

@implementation PictureViewController

@synthesize imageView;

- (void)detectLines
{
    assert(self.imageView.image);

    IplImage* dst = 0;
    IplImage* color_dst = 0;
    
    IplImage *img_rgb = [self.imageView.image IplImage];
    
    IplImage *im_gray = cvCreateImage(cvGetSize(img_rgb),IPL_DEPTH_8U,1);
    cvCvtColor(img_rgb,im_gray,CV_RGB2GRAY);
    
    IplImage* im_bw = cvCreateImage(cvGetSize(im_gray),IPL_DEPTH_8U,1);
    cvThreshold(im_gray, im_bw, 128, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
    //cvAdaptiveThreshold(im_gray, im_bw, 255, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 25, 0);
    
    CvMemStorage* storage = cvCreateMemStorage(0);
    CvSeq* lines = 0;
    int i = 0;
    
    dst = cvCreateImage( cvGetSize(im_bw), 8, 1 );
    color_dst = cvCreateImage( cvGetSize(im_bw), 8, 3 );
    
    
    //test
    //cvCvtColor(im_bw, color_dst, CV_GRAY2RGB);
    //color_dst = rotateImage(color_dst, 15);
    //self.imageView.image = [UIImage imageFromIplImage:color_dst];
    //return;
    
    
    //detect endges
    cvCanny( im_bw, dst, 50, 200, 3 );

    //convert to RGB
    //cvCvtColor( dst, color_dst, CV_GRAY2RGB);
    //self.imageView.image = [UIImage imageFromIplImage:color_dst];
    //return;
    
    
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
        cvLine( img_rgb, line[0], line[1], CV_RGB(255,255,0), 1, CV_AA, 0 );
    }
    
    //img_rgb = rotateImage(img_rgb, imageRotationAngle);
    
    //detect borders
    CvPoint  box[2];    
    detectSudokuBoundingBox(im_bw, box);

    cvRectangle(img_rgb, box[0], box[1], CV_RGB(0,255,255), 6, CV_AA, 0);
    
    self.imageView.image = [UIImage imageFromIplImage:img_rgb];
    cvReleaseMemStorage(&storage);
    cvReleaseImage(&im_bw);
    cvReleaseImage(&dst);
    cvReleaseImage(&color_dst);
}


#pragma mark - View lifecycle

- (void)viewDidLoad
{
    SolveSudoku(testSudoku);
    
    [super viewDidLoad];
    self.imageView.image = [UIImage imageNamed:@"sudoku1.jpg"];
    
    [self detectLines];
}

- (void)viewDidUnload
{
    [super viewDidUnload];
}

#pragma mark -

- (NSString *) applicationDocumentsDirectory
{
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES); 
	NSString *documentsDirectoryPath = [paths objectAtIndex:0];
	return documentsDirectoryPath;
}

#pragma mark -
#pragma mark Image Processsing



- (IBAction)launchOCR:(id)sender
{

}

- (void)dealloc {
}




@end
