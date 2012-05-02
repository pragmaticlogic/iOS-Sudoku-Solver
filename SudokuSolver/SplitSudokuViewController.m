//
//  SplitSudokuViewController.m
//  SudokuSolver
//
//  Created by Denis Lebedev on 4/30/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "SplitSudokuViewController.h"

@interface SplitSudokuViewController ()

@end

@implementation SplitSudokuViewController

@synthesize itemsToDisplay;

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
            UIImageView *imgView = [[UIImageView alloc] initWithFrame:CGRectMake(xOffset, yOffset, size, size)];
            imgView.tag = i * 9 + j;
            [self.view addSubview:imgView];
            xOffset += (offset + size);
        }
        yOffset += (offset + size);
    }
    for (int i = 0; i < [self.itemsToDisplay count]; i++) {
        UIImageView *imgView = (UIImageView *)[self.view viewWithTag:i];
        [imgView setImage:[self.itemsToDisplay objectAtIndex:i]];
    }
    // Do any additional setup after loading the view from its nib.
}

- (void)viewDidUnload {
    [super viewDidUnload];
}
@end
