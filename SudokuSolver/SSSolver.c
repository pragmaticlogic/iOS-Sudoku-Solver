//
//  SSSolver.c
//  SudokuSolver
//
//  Created by Denis Lebedev on 3/26/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include <stdio.h>
#include "SSSolver.h"
int g[81][11];

//int t[9][9] = {
//    {1,0,0,0,0,7,0,9,0},
//    {0,3,0,0,2,0,0,0,8},
//    {0,0,9,6,0,0,5,0,0},
//    {0,0,5,3,0,0,9,0,0},
//    {0,1,0,0,8,0,0,0,2},
//    {6,0,0,0,0,4,0,0,0},
//    {3,0,0,0,0,0,0,1,0},
//    {0,4,0,0,0,0,0,0,7},
//    {0,0,7,0,0,0,3,0,0}};

int tile(int x) {
	int a;
	for(a=1; a<10 && x<81 && !g[x][10]; a++)
		g[x][a] = 1;
    
	for(a=0; a<9 && x<81 && !g[x][10]; a++)
		g[x][g[x/9*9+a][0]] = g[x][g[x%9+9*a][0]] = g[x][g[x%9/3*3+x/27*27+a/3*9+a%3][0]] = 0;
    
	for(a=0; a<10 && x<81; a++)
		if(g[x][a])
		{
			g[x][0] = a?a:g[x][10];
            
			if(tile(x+1))
                return printf(x%9?"%d ":"%d \n",g[x][0])|1;
		}
    
	g[x][0] = g[x][10];
	return x>80;
}

void SolveSudoku (int sudokuArray[9][9]) {
        int a = 0;
    	for(int i=0; i<9; i++)
            for (int j = 0; j < 9; j++) {
                g[80-a][10] = g[80-a][0] = sudokuArray[i][j];
                a++;
            }
    tile(0);
    a = 0;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            sudokuArray[i][j] = g[80-a++][0];
        }
    }
}
//int main()
//{
//	int x;
//	char c[9];
//	for(x=0; x<81 && (x%9 || scanf("%s",c)|1); x++)
//		g[80-x][10] = g[80-x][0] = (c[x%9]-48);
//   
//	return tile(0);
//}