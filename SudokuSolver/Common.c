//
//  Common.c
//  SudokuSolver
//
//  Created by Denis Lebedev on 12/6/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include <stdio.h>
#include "Common.h"

const int boundingBoxTreshhold = 2; //if count of black pixels in line is less than this value, the line is marked as the border


void DlIntegral(IplImage *source, CvMat *result) {
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            printf("%.0f ",cvGet2D(source, i, j).val[0]);
        }
        printf("\n");
    }
    printf("///////////\n"); 
    
    cvSet2D(result, 0, 0, cvScalarAll(cvGet2D(source, 0, 0).val[0]));
    
    for (int x = 1; x < source->width; x++) {
        double sum = cvGet2D(source, x, 0).val[0] + cvGet2D(result, x-1, 0).val[0];
        cvSet2D(result, x, 0, cvScalarAll(sum));
    }
        
        for (int y = 1; y < 360/*img_hsv->height*/; y++) {
            double sum = cvGet2D(source, 0, y).val[0] + cvGet2D(result, 0, y-1).val[0];
            cvSet2D(result, 0, y, cvScalarAll(sum));
        }
       
       for (int y = 1; y <360/*img_hsv->height*/; y++) {
            for (int x = 1; x<source->width; x++) {
                double sum = cvGet2D(source, x, y).val[0] + cvGet2D(result, x-1, y).val[0] +
                cvGet2D(source, x, y-1).val[0] - cvGet2D(result, x-1, y-1).val[0];
                cvSet2D(result, x, y, cvScalarAll(sum));
            }
        }
        
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                printf("%.0f ",cvGet2D(result, i, j).val[0]);
            }
            printf("\n");
        }

}
void AdaptiveThreshold(IplImage *source, IplImage *result, int size) {
    CvScalar pixel;
    
    IplImage *img_gray = cvCreateImage(cvGetSize(source),IPL_DEPTH_8U,1);
    cvCvtColor(source, img_gray, CV_RGB2GRAY);

    //IplImage *img_hsv = cvCreateImage(cvSize(source->width, source->height), source->depth, 3);
    //cvCvtColor( source,img_hsv,CV_RGB2HSV);

    //CvMat *integralSum = cvCreateMat(img_gray->width, img_gray->height, 10);
    //DlIntegral(img_gray, integralSum);
    
    for (int i = 0; i < img_gray->height; i++) {
        for (int j = 0; j < img_gray->width; j++) {
            int threshold = 0;
            for (int n = i - size/2; n < i + size/2; n++) {
                for (int m = j - size/2; m < j + size/2; m++) {
                    if (n>=0 && m>=0 && m < img_gray->width - size/2 && n < img_gray->height - size/2) {
                        pixel = cvGet2D(img_gray, n, m);
                        threshold += pixel.val[0];
                    }
                }
            }
            threshold /= size*size;
            pixel = cvGet2D(img_gray, i, j);
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

void GetSubImage(IplImage *source, IplImage *dest, CvRect roiRect) {
    cvSetImageROI(source, roiRect);
    cvCopy(source, dest, NULL);
    cvResetImageROI(source);
}

void splitSudokuIntoVerticalStripes(IplImage *source, IplImage *stripes[9]) {
    IplImage *img_bw = cvCreateImage(cvGetSize(source),IPL_DEPTH_8U,1);
    cvCvtColor(source, img_bw, CV_RGB2GRAY);
    
    cvThreshold(img_bw, img_bw, 125, 0, CV_THRESH_TOZERO);

    int cutThreshold = source->height * 0.3;
    int averageRowWidth = source->width / 9 * 0.7;
    int prevLineX = 0;
    int currentLineLength;
    int countOfStripes = 0;

    for (int i = 0 ; i < source->width; i++) {
        currentLineLength = 0;
        for (int j = 0 ; j < source->height; j++) {
            double val = cvGet2D(img_bw, j, i).val[0];
            if (val == 0.0) {
                currentLineLength++; 
            }
            //printf("%0.f ", val);
        }
        
        if (currentLineLength >= cutThreshold) {
            if (i - prevLineX < averageRowWidth) {
                continue;
            }
            CvRect rect = cvRect(prevLineX, 0, i - prevLineX, source->height);
            prevLineX = i;
            IplImage *resultImage = cvCreateImage(cvSize(rect.width, rect.height),IPL_DEPTH_8U,1);
            GetSubImage(img_bw, resultImage, rect);
            stripes[countOfStripes++] = resultImage;
            if (countOfStripes >= 9) {
                return;
            }
            i+=3; //to avoid one-pixel black lines
        }

    }
        
        //printf("\n");
    
}
void splitVerticalLineIntoDigits(IplImage *source, IplImage *array[9]) {
//    IplImage *img_bw = cvCreateImage(cvGetSize(source),IPL_DEPTH_8U,1);
//    cvCvtColor(source, img_bw, CV_RGB2GRAY);

//    cvThreshold(img_bw, img_bw, 125, 0, CV_THRESH_TOZERO);

    int cutThreshold = source->width * 0.7;
    int prevLineY = 0;
    int currentLineLength;
    int countOfSquares = 0;
    
    for (int i = 0 ; i < source->height; i++) {
        currentLineLength = 0;
        for (int j = 0 ; j < source->width; j++) {
            double val = cvGet2D(source, i, j).val[0];
            if (val == 0.0) {
                currentLineLength++;
            }
            //printf("%0.f ", val);
        }
        if (currentLineLength >= cutThreshold) {
            CvRect rect = cvRect(0, prevLineY, source->width, i - prevLineY);
            prevLineY = i;
            IplImage *resultImage = cvCreateImage(cvSize(rect.width, rect.height),IPL_DEPTH_8U,1);
            GetSubImage(source, resultImage, rect);
            array[countOfSquares++] = resultImage;
            if (countOfSquares >= 8) {
                return;
            }
            i+=5; //to avoid one-pixel black lines
        }

        //printf("\n");
    }
}