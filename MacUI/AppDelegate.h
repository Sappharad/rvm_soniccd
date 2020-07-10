//
//  AppDelegate.h
//  rvm
//

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>
#import "SetupViewController.h"

NS_ASSUME_NONNULL_BEGIN

@interface AppDelegate : NSObject <NSApplicationDelegate, NSWindowDelegate>
@property (weak) IBOutlet NSWindow *wndSetup;
@property (weak) IBOutlet SetupViewController *SetupVC;

@end

NS_ASSUME_NONNULL_END
