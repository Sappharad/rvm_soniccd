//
//  SetupViewController.m
//  rvm
//

#import "SetupViewController.h"
#include "FileIO.h"

@interface SetupViewController ()

@end

@implementation SetupViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do view setup here.
}

- (void)awakeFromNib {
    [super awakeFromNib];
    _ReadyToPlay = NO;
}

-(BOOL)panel:(id)sender shouldEnableURL:(NSURL *)url{
    if(url.hasDirectoryPath){
        return YES;
    }
    else if([url.lastPathComponent.lowercaseString isEqualToString:@"data.rsdk"]){
        return YES;
    }
    return NO;
}

- (IBAction)btnSelectData:(id)sender {
    NSOpenPanel* nop = [NSOpenPanel openPanel];
    nop.delegate = self;
    nop.allowedFileTypes = @[@"rsdk"];
    [nop beginSheetModalForWindow:_wndSetup completionHandler:^(NSModalResponse result) {
        if(result == NSModalResponseOK){
            const char* cPath = [nop.URL.path cStringUsingEncoding:NSString.defaultCStringEncoding];
            if(FileIO_IsValidDataRsdk(cPath)){
                self.RsdkPath = nop.URL.path;
                self.ReadyToPlay = YES;
                [self->_wndSetup close];
            }
            else{
                NSAlert *alert = [[NSAlert alloc] init];
                [alert setMessageText:@"Incorrect file"];
                [alert setInformativeText:@"The selected file is not game data compatible with Sonic CD 2011."];
                [alert addButtonWithTitle:@"OK"];
                [alert setAlertStyle:NSAlertStyleCritical];
                [alert beginSheetModalForWindow:self->_wndSetup completionHandler:^(NSModalResponse returnCode) { }];
            }
        }
    }];
}

@end
