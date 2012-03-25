//
//  PictureViewController.m
//  SudokuSolver
//
//  Created by Denis Lebedev on 12/6/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#import "PictureViewController.h"
#ifdef __cplusplus
extern "C" {
#endif
#import "Common.h"
#ifdef __cplusplus
}
#endif

#include "baseapi.h"

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
- (void) startTesseract
{
	//code from http://robertcarlsen.net/2009/12/06/ocr-on-iphone-demo-1043
    
	NSString *dataPath = [[self applicationDocumentsDirectory] stringByAppendingPathComponent:@"tessdata"];
	/*
	 Set up the data in the docs dir
	 want to copy the data to the documents folder if it doesn't already exist
	 */
	NSFileManager *fileManager = [NSFileManager defaultManager];
	// If the expected store doesn't exist, copy the default store.
	if (![fileManager fileExistsAtPath:dataPath]) {
		// get the path to the app bundle (with the tessdata dir)
		NSString *bundlePath = [[NSBundle mainBundle] bundlePath];
		NSString *tessdataPath = [bundlePath stringByAppendingPathComponent:@"tessdata"];
		if (tessdataPath) {
			[fileManager copyItemAtPath:tessdataPath toPath:dataPath error:NULL];
		}
	}
	
	NSString *dataPathWithSlash = [[self applicationDocumentsDirectory] stringByAppendingString:@"/"];
	setenv("TESSDATA_PREFIX", [dataPathWithSlash UTF8String], 1);
	
	// init the tesseract engine.
	tess = new tesseract::TessBaseAPI();
	
	tess->Init([dataPath cStringUsingEncoding:NSUTF8StringEncoding], "eng"); 
	
	
}

- (NSString *) ocrImage: (UIImage *) uiImage
{
	
	//code from http://robertcarlsen.net/2009/12/06/ocr-on-iphone-demo-1043
	
	CGSize imageSize = [uiImage size];
	double bytes_per_line	= CGImageGetBytesPerRow([uiImage CGImage]);
	double bytes_per_pixel	= CGImageGetBitsPerPixel([uiImage CGImage]) / 8.0;
	
	CFDataRef data = CGDataProviderCopyData(CGImageGetDataProvider([uiImage CGImage]));
	const UInt8 *imageData = CFDataGetBytePtr(data);
	
	// this could take a while. maybe needs to happen asynchronously.
	char* text = tess->TesseractRect(imageData,(int)bytes_per_pixel,(int)bytes_per_line, 0, 0,(int) imageSize.height,(int) imageSize.width);
	
	// Do something useful with the text!
	NSLog(@"Converted text: %@",[NSString stringWithCString:text encoding:NSUTF8StringEncoding]);
    
	return [NSString stringWithCString:text encoding:NSUTF8StringEncoding];
}

- (IBAction)launchOCR:(id)sender
{

}

- (void)dealloc 
{
    tess->End(); // shutdown tesseract
}




@end
