//
//  SplitSudokuViewController.h
//  SudokuSolver
//
//  Created by Denis Lebedev on 4/30/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

@class SSImageProcessor;

@interface SplitSudokuViewController : UIViewController

@property (nonatomic, retain) NSArray *itemsToDisplay;
@property (nonatomic, retain) SSImageProcessor *imgProcessor;

- (IBAction)onPrepareImagesTap:(id)sender;
- (IBAction)onRecognizeTap:(id)sender;

@end
