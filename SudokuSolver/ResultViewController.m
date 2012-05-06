//
//  ResultViewController.m
//  SudokuSolver
//
//  Created by Denis Lebedev on 5/6/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "ResultViewController.h"

@interface ResultViewController ()

@end

@implementation ResultViewController

@synthesize solvedSudoku;

- (void)viewDidLoad
{
    [super viewDidLoad];
    int xOffset = 8;
    int yOffset = 36;
    int offset = 2;
    int size = 32;
    for (int i = 0; i < 9; i++) {
        xOffset = 8;
        for (int j = 0; j < 9; j++) {
            UILabel *imgView = [[UILabel alloc] initWithFrame:CGRectMake(xOffset, yOffset, size, size)];
            imgView.tag = i * 9 + j + 100;
            [self.view addSubview:imgView];
            xOffset += (offset + size);
        }
        yOffset += (offset + size);
    }
    for (int i = 0; i < [self.solvedSudoku count]; i++) {
        UILabel *label = (UILabel *)[self.view viewWithTag:i + 100];
            label.textColor = [UIColor redColor];
            label.backgroundColor = [UIColor blueColor];
            label.text = [[self.solvedSudoku objectAtIndex:i] stringValue];
    }
}


@end
