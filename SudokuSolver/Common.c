//
//  Common.c
//  SudokuSolver
//
//  Created by Denis Lebedev on 12/6/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include <stdio.h>
#include "Common.h"

const int boundingBoxTreshhold = 5; //if count of black pixels in line is less than this value, the line is marked as the border

void AdaptiveThreshold(IplImage *source, IplImage *result, int size) {
    CvScalar pixel;
    for (int i = 0; i < source->height; i++) {
        for (int j = 0; j < source->width; j++) {
            int threshold = 0;
            for (int n = i - size/2; n < i + size/2; n++) {
                for (int m = j - size/2; m < j + size/2; m++) {
                    if (n>=0 && m>=0 && m < source->width - size/2 && n < source->height - size/2) {
                        pixel = cvGet2D(source, n, m);
                        threshold += pixel.val[0];
                    }
                }
            }
            threshold /= size*size;
            pixel = cvGet2D(source, i, j);
            pixel.val[0] = pixel.val[0] > threshold ? 255 : 0; 
            cvSet2D(result, i, j, pixel);
        }
    }
}

IplImage *rotateImage(const IplImage *img, int angle) 
{
    IplImage *newImg;
    int newWidth, newHeight;
    int rectX, rectY;
    if (angle == -45 || angle == 45) {
        newWidth = (int) ((img->width + img->height) / sqrt(2.0));
        newHeight = (int) ((img->width + img->height) / sqrt(2.0));
        
    } else if (angle == -90 || angle == 90) {
        if (img->width > img->height) {
            newWidth = img->width;
            newHeight = img->width;
        } else {
            newWidth = img->height;
            newHeight = img->height;
        }
    } else {
        newWidth = img->width;
        newHeight = img->height;
    }
    
    newImg = cvCreateImage(cvSize(newWidth, newHeight), img->depth,
                           img->nChannels);
    cvSetZero(newImg);
    
    rectX = (int) ((newWidth - img->width) / 2);
    rectY = (int) ((newHeight - img->height) / 2);
    
    CvRect rect = cvRect(rectX, rectY, img->width, img->height);
    
    cvSetImageROI(newImg, rect);
    
    cvResize(img, newImg, CV_INTER_LINEAR);
    cvResetImageROI(newImg);
    
    IplImage *rotatedImg = cvCreateImage(cvGetSize(newImg), IPL_DEPTH_8U,
                                         img -> nChannels);
    
    CvPoint2D32f center;
    int xPos, yPos;
    
    xPos = (int) newWidth / 2;
    yPos = (int) newHeight / 2;
    
    CvMat *mapMatrix = cvCreateMat(2, 3, CV_32FC1);
    
    center.x = xPos;
    center.y = yPos;
    
    cv2DRotationMatrix(center, angle, 1.0, mapMatrix);
    cvWarpAffine(newImg, rotatedImg, mapMatrix, CV_INTER_LINEAR | CV_WARP_FILL_OUTLIERS, cvScalarAll(255.f));
    return rotatedImg;
}

CvPoint intersectionBetweenLines(CvPoint p1, CvPoint p2, CvPoint p3, CvPoint p4)
{
    float x, y;
    int A1 = p2.y - p1.y;
    int B1 = p1.x - p2.x;
    int C1 = A1*p1.x + B1*p1.y;
    
    int A2 = p3.y - p4.y;
    int B2 = p3.x - p4.x;
    int C2 = A2*p3.x + B2*p3.y;
    
    float det = A1*B2 - A2*B1;
    if(det == 0){
        //Lines are parallel
    }else{
        x = (B2*C1 - B1*C2)/det;
        y = (A1*C2 - A2*C1)/det;
    }
    
    return cvPoint(x, y);
}

void detectSudokuBoundingBox(IplImage *image, CvPoint box[])
{
    CvScalar pixel;
    rgb_color rgb;
    int darkPixels = 0;
    
    int horizontalIndexes[image->height];
    int verticalIndexes[image->width];
    
    int x, y;
    
    for(x = 0; x < image->height; x++)
    {
        darkPixels = 0;
        for(y = 0; y < image->width; y++)
        {
            pixel = cvGet2D(image, x, y);
            
            rgb.r = pixel.val[0] / 255.0;
            if (rgb.r == 0) {
                darkPixels++;
            }
        }
        horizontalIndexes[x] = darkPixels;
    }
    
    for(x = 0; x < image->width; x++)
    {
        darkPixels = 0;
        for(y = 0; y < image->height; y++)
        {
            pixel = cvGet2D(image, y, x);
            
            rgb.r = pixel.val[0] / 255.0;
            if (rgb.r == 0) {
                darkPixels++;
            }
        }
        verticalIndexes[x] = darkPixels;
    }
    
    int topBorder = 0, bottomBorder = 0;
    int leftBorder = 0, rightBorder = 0;
    
    
    for (int i = image->height / 2 , j = 0; i > 0; i--, j++) 
    {
        if (horizontalIndexes[i] < boundingBoxTreshhold && topBorder == 0) 
        {
            topBorder = i;
        }
        if (horizontalIndexes[image->height / 2 + j] < boundingBoxTreshhold && bottomBorder == 0) {
            bottomBorder = image->height / 2 + j;
        }
        
    }
    
    for (int i = image->width / 2, j = 0; i > 0; i--, j++) 
    {
        if (verticalIndexes[i] < boundingBoxTreshhold && leftBorder == 0) 
        {
            leftBorder = i;
        }

        if (verticalIndexes[image->width / 2 + j] < boundingBoxTreshhold && rightBorder == 0) {
            rightBorder = image->width / 2 + j;
        }
        
    }
    
    CvPoint topLeftcorner = intersectionBetweenLines(cvPoint(0, topBorder), 
                                                     cvPoint(image->width, topBorder), 
                                                     cvPoint(leftBorder, 0), 
                                                     cvPoint(leftBorder, image->height));
    
    CvPoint bottomRightCorner = intersectionBetweenLines(cvPoint(0, bottomBorder), 
                                                         cvPoint(image->width, bottomBorder), 
                                                         cvPoint(rightBorder, 0), 
                                                         cvPoint(rightBorder, image->height));
    
    box[0] = topLeftcorner;
    box[1] = bottomRightCorner;
    //return box;
}