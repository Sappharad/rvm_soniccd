//
//  SetupViewController.h
//  rvm
//

#import <Cocoa/Cocoa.h>

NS_ASSUME_NONNULL_BEGIN

@interface SetupViewController : NSViewController<NSOpenSavePanelDelegate>
@property (weak) IBOutlet NSWindow *wndSetup;
@property bool ReadyToPlay;
@property NSString* RsdkPath;
@end

NS_ASSUME_NONNULL_END
