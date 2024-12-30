#import <Foundation/Foundation.h>
#import <MediaPlayer/MediaPlayer.h>
#import <UIKit/UIKit.h>
#import <AVKit/AVKit.h>

#import "MyMPRemoteCC.h"

@interface MyMPRemoteCC : NSObject
{
  int test; // default label = @protected but in cpp it is "private"

  MPRemoteCommandCenter *myCommandCenter;

@public
}

- (void)printTest;
- (void)setupMPRemoteCommandCenter;

@end


@implementation MyMPRemoteCC
// class constructor (id means instancetype AKA MyMPRemoteCC)
-(id)init
{
  NSLog(@"Constructor Called!");
  self->test = 203;
  return self;
}

// class destructor
-(void)dealloc
{
  NSLog(@"Destructor Called!");
  [super dealloc];
}

- (void)incrementor {
  self->test += 1;
}

- (void)printTest {
  NSLog(@"Value of hello testtttt = %d", self->test); // String == %@, int == d, float == %f, double == %lf
}

- (void)setupMPRemoteCommandCenter {
  NSLog(@"setuoooo Called!");
  myCommandCenter = [MPRemoteCommandCenter sharedCommandCenter];

  MPRemoteCommand *play = [myCommandCenter playCommand];
  [play setEnabled:YES];
  [play addTarget:self action:@selector(playPauseAudio:)];

  MPRemoteCommand *pause = [myCommandCenter pauseCommand];
  [pause setEnabled:NO];
  [pause addTarget:self action:@selector(playPauseAudio:)];

// [myCommandCenter.pauseCommand addTargetWithHandler:^MPRemoteCommandHandlerStatus(MPRemoteCommandEvent *event) {

//     // if ([self pauseAudioPlayback])
//         return MPRemoteCommandHandlerStatusSuccess;
//     // else
//         // return MPRemoteCommandHandlerStatusCommandFailed;
// } ];


}

- (MPRemoteCommandHandlerStatus)playPauseAudio: (MPRemoteCommandHandlerStatus *)event {
    // [self playAction];
    //playAction handles the audio pausing and toggling the play button on the app
  NSLog(@"Cal to handle plaing and pausing from control center!");
  return MPRemoteCommandHandlerStatusSuccess;
}

@end


/************* C functions for controlling this objective-c class from cpp BEGINS *****/

// Un-initialized pointer to class
MyMPRemoteCC *mpRmObject;

void initMyMPRemoteCC() {
  mpRmObject = [MyMPRemoteCC new];
}

void destroyMyMPRemoteCC() {
  [mpRmObject release];
}

void incrementMyMPRemoteCC() {
  [mpRmObject incrementor];
}

void printMyMPRemoteCC() {
  [mpRmObject printTest];
}

void turnonMPRemoteCC() {
  [mpRmObject setupMPRemoteCommandCenter];
}





