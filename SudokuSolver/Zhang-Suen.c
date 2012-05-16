//
//  Zhang-Suen.c
//  SudokuSolver
//
//  Created by Denis Lebedev on 5/15/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "Zhang-Suen.h"
#include <stdio.h>

/* Zhang-Suen thinning */

#define IMAGE_SIZE 600
void iputim (int image[IMAGE_SIZE][IMAGE_SIZE],int n,int m);
void thin_b(int image[IMAGE_SIZE][IMAGE_SIZE], int nn,int mm);
void subtr(int a[IMAGE_SIZE][IMAGE_SIZE], int b[IMAGE_SIZE][IMAGE_SIZE],int n,int m);
int t1a(int image[IMAGE_SIZE][IMAGE_SIZE], int i, int j, int a[8], int *b,int nn,int mm);
void iputim (int image[IMAGE_SIZE][IMAGE_SIZE], int n, int m);

int y [IMAGE_SIZE][IMAGE_SIZE];


void iputim (int image[IMAGE_SIZE][IMAGE_SIZE], int n, int m) {
    
	int i,j;
    
	for (i=0; i<n; i++) {
        for (j=0; j<m; j++)  {
            if (image[i][j] >= 100) image[i][j] = 9;
            printf ("%2d", image[i][j]);
        }
        printf ("\n");
	}
}

IplImage *zhangSuenThinning(IplImage *source) {
    int  imgMatrix[IMAGE_SIZE][IMAGE_SIZE];
    for (int i = 0 ; i < source->width; i++) {
        for (int j = 0 ; j < source->height; j++) {
            double val = cvGet2D(source, j, i).val[0];
            imgMatrix[i][j] = val > 0 ? 0 : 1;
        }
    }
    thin_b(imgMatrix, source->width, source->height);
    
    for (int i = 0 ; i < source->width; i++) {
        for (int j = 0 ; j < source->height; j++) {
            int val = imgMatrix[i][j] > 0 ? 0 : 255;
            cvSet2D(source, j, i, cvScalarAll(val));
        }
    }
    
    return source;
}

void thin_b(int image[IMAGE_SIZE][IMAGE_SIZE], int nn,int mm) {
    /*		Thinning algorithm: CACM 1984 march (Zhang and Suen)	*/
    
	int i,j, cont, br,ar,p1,p2, t1a();
	int a[8];
    
	cont = 1;
	while (cont) {
		cont = 0;
        
        /*	Sub-iteration 1: */
		for (i=0; i<nn; i++)
            for (j=0; j<mm; j++) {		/* Scan the entire image */
                if (image[i][j] == 0) {
                    y[i][j] = 0;
                    continue;
                }
                ar = t1a (image, i, j, a, &br,nn,mm);	/* Function A */
                p1 = a[0]*a[2]*a[4];
                p2 = a[2]*a[4]*a[6];
                if ( (ar == 1) && ((br>=2) && (br<=6)) &&
                    (p1 == 0) && (p2 == 0) )  {
					y[i][j] = 1;
					cont = 1;
                }
                else y[i][j] = 0;
            }
		subtr (y, image,nn,mm);
        
        /* Sub iteration 2: */
		for (i=0; i<nn; i++)
            for (j=0; j<mm; j++) {		/* Scan the entire image */
                if (image[i][j] == 0) {
                    y[i][j] = 0;
                    continue;
                }
                ar = t1a (image, i, j, a, &br,nn,mm);	/* Function A */
                p1 = a[0]*a[2]*a[6];
                p2 = a[0]*a[4]*a[6];
                if ( (ar == 1) && ((br>=2) && (br<=6)) &&
                    (p1 == 0) && (p2 == 0) )  {
					y[i][j] = 1;
					cont = 1;
                }
                else y[i][j] = 0;
            }
		subtr (y, image,nn,mm);
	}
}

void subtr(int a[IMAGE_SIZE][IMAGE_SIZE], int b[IMAGE_SIZE][IMAGE_SIZE],int n,int m) {
	int i,j;
    
	for (i=0; i<n; i++)
		for (j=0; j<m; j++) 
			b[i][j] -= a[i][j];
}


int t1a(int image[IMAGE_SIZE][IMAGE_SIZE], int i, int j, int a[8], int *b,int nn,int mm) {
    /*	Return the number of 01 patterns in the sequence of pixels
     P2 p3 p4 p5 p6 p7 p8 p9.					*/
    
	int n,m;
    
	for (n=0; n<8; n++) a[n] = 0;
	if (i-1 >= 0) {
		a[0] = image[i-1][j];
		if (j+1 < mm) a[1] = image[i-1][j+1];
		if (j-1 >= 0) a[7] = image[i-1][j-1];
	}
	if (i+1 < nn) {
		a[4] = image[i+1][j];
		if (j+1 < mm) a[3] = image[i+1][j+1];
		if (j-1 >= 0) a[5] = image[i+1][j-1];
	}
	if (j+1 < mm) a[2] = image[i][j+1];
	if (j-1 >= 0) a[6] = image[i][j-1];
    
	m= 0;	*b = 0;
	for (n=0; n<7; n++) {
		if ((a[n]==0) && (a[n+1]==1)) m++;
		*b = *b + a[n];
	}
	if ((a[7] == 0) && (a[0] == 1)) m++;
	*b = *b + a[7];
	return m;
}
