//
//  SplitSudokuViewController.m
//  SudokuSolver
//
//  Created by Denis Lebedev on 4/30/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "SplitSudokuViewController.h"
#import "SSImageProcessor.h"
#import "ResultViewController.h"

@interface SplitSudokuViewController ()

@property (nonatomic, retain) NSMutableArray *sudokuDigits;

@end

@implementation SplitSudokuViewController

@synthesize itemsToDisplay;
@synthesize imgProcessor;
@synthesize sudokuDigits;

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
            imgView.tag = i * 9 + j + 100;
            [self.view addSubview:imgView];
            xOffset += (offset + size);
        }
        yOffset += (offset + size);
    }
    for (int i = 0; i < [self.itemsToDisplay count]; i++) {
        UIImageView *imgView = (UIImageView *)[self.view viewWithTag:i + 100];
        [imgView setImage:[self.itemsToDisplay objectAtIndex:i]];
        
        //debug
        
        UILabel *label = [[UILabel alloc] initWithFrame:imgView.frame];
        label.backgroundColor = [UIColor clearColor];
        label.textColor = [UIColor redColor];
        label.text = [NSString stringWithFormat:@"%d",[imgProcessor recognizeDigit:[self.itemsToDisplay objectAtIndex:i]]];
        [self.view addSubview:label];
         NSString *documentsDirectory = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
        NSData *data = UIImagePNGRepresentation(imgView.image);
        [data writeToFile:[NSString stringWithFormat:@"%@/%d.png", documentsDirectory,i] atomically:YES];
        //debug
    }
}

- (IBAction)onPrepareImagesTap:(id)sender {
    
}

- (IBAction)onRecognizeTap:(id)sender {
    self.sudokuDigits = [NSMutableArray array];
    for (UIImage *img in itemsToDisplay) {
        int value = [self.imgProcessor recognizeDigit:img];
        [self.sudokuDigits addObject:[NSNumber numberWithInt:value]];
    }
    ResultViewController *rVC = [[ResultViewController alloc] init];
    rVC.solvedSudoku = self.sudokuDigits;
    [self.navigationController pushViewController:rVC animated:YES];
}
@end
