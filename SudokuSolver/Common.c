//
//  Common.c
//  SudokuSolver
//
//  Created by Denis Lebedev on 12/6/11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#include <stdio.h>
#include "Common.h"

#define BLACK_PIXEL 0
#define WHITE_PIXEL 255
#define DELETED_PIXEL 99
#define YES 1
#define NO 0


unsigned char firstCondition(IplImage *source, int i, int j);
unsigned char secondCondition(IplImage *source, int i, int j);
unsigned char thirdCondition(IplImage *source, int i, int j);
unsigned char fourthCondition(IplImage *source, int i, int j);
unsigned char fifthCondition(IplImage *source, int i, int j);
unsigned char sixthCondition(IplImage *source, int i, int j);


int countOfPairs(IplImage *source, int i, int j);
int countOfNeighbourPixels(int searchValue, IplImage * source, int i, int j);

unsigned char p2p4Check(IplImage *source, int i, int j, int k, int l);

const int boundingBoxTreshhold = 2; //if count of black pixels in line is less than this value, the line is marked as the border


//void DlIntegral(IplImage *source, CvMat *result) {
//    for (int i = 0; i < 10; i++) {
//        for (int j = 0; j < 10; j++) {
//            printf("%.0f ",cvGet2D(source, i, j).val[0]);
//        }
//        printf("\n");
//    }
//    printf("///////////\n"); 
//    
//    cvSet2D(result, 0, 0, cvScalarAll(cvGet2D(source, 0, 0).val[0]));
//    
//    for (int x = 1; x < source->width; x++) {
//        double sum = cvGet2D(source, x, 0).val[0] + cvGet2D(result, x-1, 0).val[0];
//        cvSet2D(result, x, 0, cvScalarAll(sum));
//    }
//        
//        for (int y = 1; y < 360/*img_hsv->height*/; y++) {
//            double sum = cvGet2D(source, 0, y).val[0] + cvGet2D(result, 0, y-1).val[0];
//            cvSet2D(result, 0, y, cvScalarAll(sum));
//        }
//       
//       for (int y = 1; y <360/*img_hsv->height*/; y++) {
//            for (int x = 1; x<source->width; x++) {
//                double sum = cvGet2D(source, x, y).val[0] + cvGet2D(result, x-1, y).val[0] +
//                cvGet2D(source, x, y-1).val[0] - cvGet2D(result, x-1, y-1).val[0];
//                cvSet2D(result, x, y, cvScalarAll(sum));
//            }
//        }
//        
//        for (int i = 0; i < 10; i++) {
//            for (int j = 0; j < 10; j++) {
//                printf("%.0f ",cvGet2D(result, i, j).val[0]);
//            }
//            printf("\n");
//        }
//
//}
void AdaptiveThreshold(IplImage *source, IplImage *result, int size) {
    CvScalar pixel;
    
    IplImage *img_gray = cvCreateImage(cvGetSize(source),IPL_DEPTH_8U,1);
    cvCvtColor(source, img_gray, CV_RGB2GRAY);
    
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
    int currentLineLength;
    int countOfStripes = 0;
    int start = 0;
    
    for (int i = 0; i < source->width; i++) {
        double val = cvGet2D(img_bw, 5, i).val[0];
        if (val == 0.0) {
            start = i; break;
        }
    }

    if (start > source->width / 9) {
        start = 0;
    }
    int prevLineX = start;

    for (int i = start ; i < source->width; i++) {
        currentLineLength = 0;
        for (int j = 0 ; j < source->height; j++) {
            double val = cvGet2D(img_bw, j, i).val[0];
            if (val == 0.0) {
                currentLineLength++; 
            }
        }
        
        int magicCropGap = 5;
        if (currentLineLength >= cutThreshold) {
            if (i - prevLineX < averageRowWidth) {
                continue;
            }
            CvRect rect = cvRect(prevLineX + magicCropGap, 0, i - prevLineX - magicCropGap, source->height);
            prevLineX = i;
            IplImage *resultImage = cvCreateImage(cvSize(rect.width, rect.height),IPL_DEPTH_8U,1);
            GetSubImage(img_bw, resultImage, rect);
            stripes[countOfStripes++] = resultImage;
            if (countOfStripes >= 9) {
                return;
            }
            i+=2; //to avoid one-pixel black lines
        }
    }
}

IplImage* cropImage(IplImage *source) {
    int startY = 0, stopY = source -> height, startX = 0, stopX = source->width;
    
    //top
    for (int i = 1 ; i < source->height / 2; i++) {
        int lenghtsTop = 0;
        for (int j = 0 ; j < source->width; j++) {
            double val = cvGet2D(source, i, j).val[0];
            if (val == 0.0) {
                lenghtsTop++; 
            }
        }
        if (lenghtsTop != 0) {
            startY = i - 1;
            break;
        }
    }
//    
    //bottom
    for (int i = source->height - 2 ; i > source->height /2; i--) {
        int lenghtsBottom = 0;
        for (int j = 0 ; j < source->width; j++) {
            double val = cvGet2D(source, i, j).val[0];
            if (val == 0.0) {
                lenghtsBottom++; 
            }
        }
        if (lenghtsBottom != 0) {
            stopY = i + 1;
            break;
        }
    }
    
    //left
    for (int i = 1 ; i < source->width / 2; i++) {
        int lenghtsLeft = 0;
        for (int j = 0 ; j < source->height; j++) {
            double val = cvGet2D(source, j, i).val[0];
            if (val == 0.0) {
                lenghtsLeft++; 
            }
        }
        if (lenghtsLeft != 0) {
            startX = i - 1;
            break;
        }
    }
    
    //right
    for (int i = source->width - 3; i > source->width / 2; i--) {
        int lenghtsRight = 0;
        for (int j = 0 ; j < source->height; j++) {
            double val = cvGet2D(source, j, i).val[0];
            if (val == 0.0) {
                lenghtsRight++; 
            }
        }
        if (lenghtsRight != 0) {
            stopX = i + 1;
            break;
        }
    }
    CvRect rect = cvRect(startX, startY, stopX - startX, stopY - startY);
    IplImage *cropped_dest = cvCreateImage(cvSize(rect.width, rect.height),IPL_DEPTH_8U,1);
    GetSubImage(source, cropped_dest, rect);
    return cropped_dest;

}

IplImage* cropGarbageFromTopAndBottom(IplImage *source) {
    IplImage *img_bw = cvCreateImage(cvGetSize(source),IPL_DEPTH_8U,1);
    cvCvtColor(source, img_bw, CV_RGB2GRAY);

    int startY = 0, stopY = source -> height;
    
    //top
    for (int i = 1 ; i < source->height / 2; i++) {
        int lenghtsTop = 0;
        for (int j = 0 ; j < source->width; j++) {
            double val = cvGet2D(img_bw, i, j).val[0];
            if (val == 0.0) {
                lenghtsTop++; 
            }
        }
        if (lenghtsTop == 0) {
            startY = i;
            break;
        }
    }
    
    //bottom
    for (int i = source->height /2 ; i < source->height - 1; i++) {
        int lenghtsBottom = 0;
        for (int j = 0 ; j < source->width; j++) {
            double val = cvGet2D(img_bw, i, j).val[0];
            if (val == 0.0) {
                lenghtsBottom++; 
            }
        }
        if (lenghtsBottom == 0) {
            stopY = i + 1;
            break;
        }
    }
    
    CvRect rect = cvRect(0, startY, source->width, stopY - startY);
    IplImage *cropped_dest = cvCreateImage(cvSize(rect.width, rect.height),IPL_DEPTH_8U,1);
    GetSubImage(img_bw, cropped_dest, rect);
    return cropped_dest;
}
void splitVerticalLineIntoDigits(IplImage *source, IplImage *array[9]) {

    int countOfSquares = 0;
    
    int start = 0;
    int stop = 0;
    
    int i = 0;
    do {
        double val = cvGet2D(source, i, source->width/2).val[0];
        if (val == 0.0)
            start = i;
        i++;
    } while (!start);
    
    i = source->height-1;
    do {
        double val = cvGet2D(source, i, source->width/2).val[0];
        if (val == 0.0)
            stop = i;
        i--;
    } while (!stop);

    int prevLineY = start;

    for (int i = start ; i < stop; i++) {
        if (i - prevLineY > (stop-start)/ 9) {
            CvRect rect = cvRect(0, prevLineY, source->width, i - prevLineY);
            prevLineY = i;
            IplImage *resultImage = cvCreateImage(cvSize(rect.width, rect.height),IPL_DEPTH_8U,1);
            GetSubImage(source, resultImage, rect);
            array[countOfSquares++] = resultImage;
            i+=2; //to avoid one-pixel black lines
        }
        CvRect rect = cvRect(0, prevLineY, source->width, stop - prevLineY);
        IplImage *resultImage = cvCreateImage(cvSize(rect.width, rect.height),IPL_DEPTH_8U,1);
        GetSubImage(source, resultImage, rect);
        array[countOfSquares] = resultImage;
    }
}

void thinImage(IplImage *source, IplImage *destination) {
    for (int i = 0; i < 6; i++) {
        //cleanup
        
        for (int i = 1; i < source->height - 2; i++) {
            for (int j = 1; j < source->width - 2; j++) {
                if (cvGet2D(destination, i, j).val[0] == BLACK_PIXEL) {
                    if (firstCondition(destination, i, j) &&
                        secondCondition(destination, i, j) &&
                        thirdCondition(destination, i, j) &&
                        fourthCondition(destination, i, j) &&
                        fifthCondition(destination, i, j) &&
                        sixthCondition(destination, i, j)
                        ) 
                    {
                        cvSet2D(destination, i, j, cvScalarAll(DELETED_PIXEL));
                    }
                }
            }
        }
        
        for (int i = 0; i < source->height - 5; i++) {
            for (int j = 0; j < source->width - 5; j++) {
                if (cvGet2D(destination, i, j).val[0] == DELETED_PIXEL) {
                    cvSet2D(destination, i, j, cvScalarAll(WHITE_PIXEL));
                }
            }
        }

    }
}

int countOfNeighbourPixels(int searchValue, IplImage * source, int i, int j) {
    int countOfEightLinkedNeighbours = 0;
    for (int k = -1; k < 2; k++) {
        for (int l = -1; l < 2; l++) { {
                if (k == 0 && l == 0) { continue; }
                if (cvGet2D(source, i + k, j + l).val[0] == searchValue) {
                    countOfEightLinkedNeighbours++;
                }
            }
        }
    }
    return countOfEightLinkedNeighbours;
}

int countOfPairs(IplImage *source, int i, int j) {
    int countOfLinkedPairs = 0;
    int p[9];
    
    p[0] = cvGet2D(source, i, j +1).val[0] > 0 ? 1 : 0;//matrix[i, j + 1];
    p[1] = cvGet2D(source, i - 1, j + 1).val[0] > 0 ? 1 : 0;//matrix[i - 1, j + 1];
    p[2] = cvGet2D(source, i - 1, j).val[0] > 0 ? 1 : 0;//matrix[i - 1, j];
    p[3] = cvGet2D(source, i - 1, j - 1).val[0] > 0 ? 1 : 0;//matrix[i - 1, j - 1];
    p[4] = cvGet2D(source, i, j - 1).val[0] > 0 ? 1 : 0;//matrix[i, j - 1];
    p[5] = cvGet2D(source, i + 1, j + 1).val[0] > 0 ? 1 : 0;//matrix[i + 1, j - 1];
    p[6] = cvGet2D(source, i + 1, j).val[0] > 0 ? 1 : 0;// matrix[i + 1, j];
    p[7] = cvGet2D(source, i + 1, j + 1).val[0] > 0 ? 1 : 0;//matrix[i + 1, j + 1];
    p[8] = p[0] > 0 ? 1 : 0;
    
    for (int m = 0; m < 9 - 1; m++) {
        countOfLinkedPairs += p[m] * p[m + 1];
    }
    
    return countOfLinkedPairs;
}

unsigned char p2p4Check(IplImage *source, int i, int j, int k, int l) {
    unsigned char pixelShouldBeDeleted = YES;
    if (cvGet2D(source, i + k, j + l).val[0] == DELETED_PIXEL) {
        pixelShouldBeDeleted = NO;
        cvSet2D(source, i + k, j + l, cvScalarAll(WHITE_PIXEL));
        if (fourthCondition(source, i, j) == YES)
        {
            pixelShouldBeDeleted = YES;
        }
        cvSet2D(source, i+k, j+l, cvScalarAll(DELETED_PIXEL));
    }
    return pixelShouldBeDeleted;
}

unsigned char firstCondition(IplImage *source, int i, int j) {
//    if (i == 1 || j == 1) {
//        return YES;
//    }
    int countOfFourLinkedSibilings = 0;
    if (cvGet2D(source, i, j + 1).val[0] == WHITE_PIXEL) {
        countOfFourLinkedSibilings++;
    }
    if (cvGet2D(source, i - 1 , j).val[0] == WHITE_PIXEL) {
        countOfFourLinkedSibilings++;
    }
    if (cvGet2D(source, i, j - 1).val[0] == WHITE_PIXEL) { 
        countOfFourLinkedSibilings++;
    }
    if (cvGet2D(source, i + 1, j).val[0] == WHITE_PIXEL) { 
        countOfFourLinkedSibilings++;
    }
    return countOfFourLinkedSibilings > 1 ? YES : NO;
}

unsigned char secondCondition(IplImage *source, int i, int j) {
    return countOfNeighbourPixels(BLACK_PIXEL, source, i, j) >= 2 ? YES : NO;
}

unsigned char thirdCondition(IplImage *source, int i, int j) {
    return countOfNeighbourPixels(BLACK_PIXEL, source, i, j) - 
    countOfNeighbourPixels(DELETED_PIXEL, source, i, j) >= 1 ? YES : NO;
}

unsigned char fourthCondition(IplImage *source, int i, int j) {
    int result = countOfNeighbourPixels(WHITE_PIXEL, source, i, j) - countOfPairs(source, i, j);
    return result < 2 ? YES : NO;
}

unsigned char fifthCondition(IplImage *source, int i, int j) {
    return p2p4Check(source, i, j, 0, -1);
}

unsigned char sixthCondition(IplImage *source, int i, int j) {
    return p2p4Check(source, i, j, -1, 0);
}


kPixelType determinePixelType(int matrix[20][20], int m, int n) {
    
    kPixelType type;
    int countOfEightLinkedNeighbours = 0;
    for (int k = -1; k < 2; k++) {
        for (int l = -1; l < 2; l++) { {
            if (k == 0 && l == 0) {
                continue; 
            }
            if (matrix[m + k ][n + l] == 1) {
                countOfEightLinkedNeighbours++;
            }
        }
        }
    }
    switch (countOfEightLinkedNeighbours) {
        case 1:
            type = kPixelTypeEnd;
            break;
        case 2:
            type = kPixelTypeNormal;
            break;
        case 3:
            type = kPixelTypeNode3;
            break;
        case 4:
            type = kPixelTypeNode4;
            break;
        case 5:
            type = kPixelTypeNode5;
            break;
        default:
            type = kPixelTypeUndefined;
            break;
    }
    return type;
}

charStruct SSRezognizeNumericCharacter(IplImage *source) {
    charStruct result;
    result.g1 = 0;
    result.g2 = 0;
    result.g3 = 0;
#define SIZE 20
    int matrix[SIZE][SIZE];
    SSDebugOutput(source);
    for (int i = 0 ; i < source->height; i++) {
        for (int j = 0 ; j < source->width; j++) {
            matrix[i][j] = cvGet2D(source, i, j).val[0] > 0 ? 0: 1;
        }
    }
    
    
    //first zond
    for (int i = 0; i < SIZE; i++) {
        result.g1 += matrix[i][SIZE/2];
    }
    printf("g1=%d\n\n", result.g1);    
    if (result.g1 == 0) {
        result.value = 0;
        return result;
    }
    
    //second zond
    
    for (int i = 0; i < SIZE; i++) {
        result.g2 += matrix[SIZE/4 + 1][i];
        //matrix[SIZE/4][i] = 5;
    }
    
    for (int i = 0; i < SIZE; i++) {
        result.g3 += matrix[SIZE*3/4][i];
        //matrix[SIZE*3/4][i] = 5;
    }
    
    //third zond 
    if (result.g1 >= 3 && result.g2 == 2 && result.g3 == 2) {
        result.value = 8;
        return result;
    }
    
    if (result.g1 >=3 && result.g2 == 1 && result.g3 == 2) {
        result.value = 6;
        return result;
    }
    
    if (result.g1 >=3 && result.g2 == 2 && result.g3 == 1) {
        result.value = 9;
        return result;
    }
    

    
    for (int i = 1 ; i < SIZE - 1; i++) {
        for (int j = 1 ; j < SIZE - 1; j++) {
            
            if (matrix[i][j] == 1) {
                kPixelType type = determinePixelType(matrix, i, j);
                if (type == kPixelTypeEnd) {
                    printf("end pixel found\n");
                }
                if (type == kPixelTypeNode4) {
                    printf("node4 pixel found\n");
                }
                if (type == kPixelTypeNode5) {
                    printf("node5 pixel found\n");
                }

            }
        }
    }

    result.value = 1;
    return result;
}

void SSDebugOutput(IplImage *source) {
    printf ("-----------------\n");

    for (int i = 0 ; i < source->height; i++) {
        for (int j = 0 ; j < source->width; j++) {
            int val = cvGet2D(source, i, j).val[0];
            printf ("%d", val > 0 ? 0 : 1);
        }
        printf ("\n");
    }
    printf ("-----------------\n");
}