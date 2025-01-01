#import <Foundation/Foundation.h>
#import <MediaPlayer/MediaPlayer.h>
#import <UIKit/UIKit.h>
#include <stdio.h> // because nslog aint printing to qt debug

#import "MyMPRemoteCC.h"

// Un-initialized pointer to cpp class.
// This should be a the very top because
// it will be called from within @implementation
// and also within c-style functions.
// It will be inited from initAllObjectiveCBridges
// at app launch.
Top *cppObject;

/**
  * MyMPRemoteCC class...
  * @param none
  * @returns none
  */
@interface MyMPRemoteCC : NSObject
{
  MPRemoteCommandCenter *myCommandCenter;   // For holding the control center instance
  MPNowPlayingInfoCenter *myPlayingCenter;

@public
}
- (void)listenForInterruptions;
- (void)setupMPRemoteCommandCenter;
- (MPRemoteCommandHandlerStatus)previousAudio: (MPRemoteCommandHandlerStatus *)event;
- (MPRemoteCommandHandlerStatus)nextAudio: (MPRemoteCommandHandlerStatus *)event;
- (MPRemoteCommandHandlerStatus)playAudio: (MPRemoteCommandHandlerStatus *)event;
- (MPRemoteCommandHandlerStatus)pauseAudio: (MPRemoteCommandHandlerStatus *)event;
- (MPRemoteCommandHandlerStatus)playPauseAudio: (MPRemoteCommandHandlerStatus *)even;

@end
@implementation MyMPRemoteCC
// class constructor (id means instancetype AKA MyMPRemoteCC)
-(id)init
{
  return self;
}

// class destructor
-(void)dealloc
{
  [super dealloc];  // call the destructor of NSObject AKA parent that we inherited from
}


// will comment this
- (void)listenForInterruptions {
  @try {
    printf("%s", "enterrrrr\n");
    // Had to comment this part out coz it messes up the whole audio playback in media control center
    // AVAudioSession *session = [AVAudioSession sharedInstance];
    // [session setCategory:AVAudioSessionCategoryPlayback withOptions:AVAudioSessionCategoryOptionDuckOthers error:nil];
    // [session setActive:YES error:nil];

    // Listen for system interrupts AKA phone call et al, and suspend/unsuspend accordingly
    NSNotificationCenter *notifCenter = [NSNotificationCenter defaultCenter];
    [notifCenter  addObserver:
                  self
                  selector:@selector(onAudioSessionInterrupted:) //wow! if the callback method takes an arg and you fail to add the appending ":" to its name, app will crash at runtime when it gets here
                  name:AVAudioSessionInterruptionNotification
                  object:[AVAudioSession sharedInstance]];
  }
  @catch (NSException *exception) {
    // NSLog(@"%@", exception);
  }
}


// will also comment this
- (void) onAudioSessionInterrupted: (NSNotification *) notification
{
    //Check the type of notification, especially if you are sending multiple AVAudioSession events here
    printf("%s", "Interruption start\n");

    if ([notification.name isEqualToString:AVAudioSessionInterruptionNotification]) {
        printf("%s", "Interruption try\n");

        //Check to see if it was a Begin interruption
        if ([[notification.userInfo valueForKey:AVAudioSessionInterruptionTypeKey] isEqualToNumber:[NSNumber numberWithInt:AVAudioSessionInterruptionTypeBegan]]) {
            printf("%s", "Interruption enterrrrr\n");
            //Suspend your audio
            cppObject->suspendAudio();

        } else if([[notification.userInfo valueForKey:AVAudioSessionInterruptionTypeKey] isEqualToNumber:[NSNumber numberWithInt:AVAudioSessionInterruptionTypeEnded]]){
            printf("%s", "Interruption end\n");
            //Resume your audio
            cppObject->unsuspendAudio();

        }
    }
}
// - (void)onAudioSessionInterrupted: (NSNotification *)notif {
//   NSDictionary *userInfo = [notif userInfo];
//   NSNumber *typeValue = [userInfo valueForKey:AVAudioSessionInterruptionTypeKey];

//   AVAudioSessionInterruptionType type = (AVAudioSessionInterruptionType)[typeValue unsignedIntegerValue];

// //Check the type of notification
//   if ([notif.name isEqualToString:AVAudioSessionInterruptionNotification]) {
//       NSLog(@"Interruption notification received!");

//       //Check to see if it was a Begin interruption
//       if ([[notif.userInfo valueForKey:AVAudioSessionInterruptionTypeKey] isEqualToNumber:[NSNumber numberWithInt:AVAudioSessionInterruptionTypeBegan]]) {
//           NSLog(@"Interruption began!");
//           printf("%s", "Interruption enterrrrr\n");

//       } else {
//           NSLog(@"Interruption ended!");
//           printf("%s", "Interruption end\n");
//           //Resume your audio
//       }
//   }
// }

- (void)setupMPRemoteCommandCenter {
  // This line was used with the old ApI.
  // Will leave it here though, in case I need it for future learnings.
  // [[UIApplication sharedApplication] beginReceivingRemoteControlEvents];

  // MPRemoteCommandCenter.sharedCommandCenter returns a pointer to the running instance.
  // This by itself wont turn on the lockscreen media control board.
  // At least one command must be setEnabled:YES and having a valid action:@selector
  // What it does is it seizes control of the lockscreen media control board for your app.
  myCommandCenter = [MPRemoteCommandCenter sharedCommandCenter];

  // resetting any previous hold of this app on control center
  [myCommandCenter.playCommand removeTarget:self];
  [myCommandCenter.pauseCommand removeTarget:self];

  // Add handler for Commands we intend to support
  [myCommandCenter.previousTrackCommand setEnabled:YES];
  [myCommandCenter.previousTrackCommand addTarget:self action:@selector(previousAudio:)];
  //
  [myCommandCenter.nextTrackCommand setEnabled:YES];
  [myCommandCenter.nextTrackCommand addTarget:self action:@selector(nextAudio:)];
  //
  [myCommandCenter.playCommand setEnabled:YES];
  [myCommandCenter.playCommand addTarget:self action:@selector(playPauseAudio:)];
  //
  [myCommandCenter.pauseCommand setEnabled:YES];
  [myCommandCenter.pauseCommand addTarget:self action:@selector(pauseAudio:)];
  // Play and pause buttons (earphone control)
  [myCommandCenter.togglePlayPauseCommand setEnabled:YES];
  [myCommandCenter.togglePlayPauseCommand addTarget:self action:@selector(playPauseAudio:)];

  // explicitly disabling the skip forward and backward commands
  [myCommandCenter.skipBackwardCommand setEnabled:NO];
  [myCommandCenter.skipForwardCommand setEnabled:NO];

  // // Drag the progress bar https://programmersought.com/article/44296352753/
  // if (@available(iOS 9.1, *)) {
  //     MPRemoteCommand *changePlaybackPositionCommand = commandCenter.changePlaybackPositionCommand;
  //     changePlaybackPositionCommand.enabled = NO;
  //     [changePlaybackPositionCommand addTargetWithHandler:^MPRemoteCommandHandlerStatus(MPRemoteCommandEvent * _Nonnull event) {
  //         return MPRemoteCommandHandlerStatusCommandFailed;
  //     }];
  // } else {
  //     // Fallback on earlier versions
  // }
}

// Not yet implemented
- (void)setupLockScreenMediaInfo {
  // just Pass empty information to MPNowPlayingInfoCenter for now
  myPlayingCenter = [MPNowPlayingInfoCenter defaultCenter];
  NSMutableDictionary *playingInfo = [NSMutableDictionary new];
  [myPlayingCenter setNowPlayingInfo:playingInfo];
  [playingInfo release];
}

- (MPRemoteCommandHandlerStatus)previousAudio: (MPRemoteCommandHandlerStatus *)event {
  cppObject->changePlay(false);
  return MPRemoteCommandHandlerStatusSuccess;
}

- (MPRemoteCommandHandlerStatus)nextAudio: (MPRemoteCommandHandlerStatus *)event {
  cppObject->changePlay(true);
  return MPRemoteCommandHandlerStatusSuccess;
}

- (MPRemoteCommandHandlerStatus)playAudio: (MPRemoteCommandHandlerStatus *)event {
  cppObject->play();
  return MPRemoteCommandHandlerStatusSuccess;
}

- (MPRemoteCommandHandlerStatus)pauseAudio: (MPRemoteCommandHandlerStatus *)event {
  cppObject->pause();
  return MPRemoteCommandHandlerStatusSuccess;
}

- (MPRemoteCommandHandlerStatus)playPauseAudio: (MPRemoteCommandHandlerStatus *)event {
  cppObject->playOrPause();
  return MPRemoteCommandHandlerStatusSuccess;
}

@end


/************* C functions for controlling this objective-c classes from cpp BEGINS *****/

// Un-initialized pointer to this objective-c class
MyMPRemoteCC *mpRmObject;


/**
  * This function is called when app launches on iOS.
  * It will simply initialize MyMPRemoteCC pointer.
  * MyMPRemoteCC is a pointer to this class and
  * through this pointer, we can call the methods
  * of this class from cpp side of codes using
  * c-style functions forward-declared inside
  * MyMPRemoteCC.h header.
  * @param instance of cpp part of this app
  * @returns void
  */
void initMyMPRemoteCC(Top *cppObj) {
  mpRmObject = [MyMPRemoteCC new];
  cppObject = cppObj;

  [mpRmObject listenForInterruptions];
}

void destroyMyMPRemoteCC() {
  [mpRmObject release];
}


void turnonMPRemoteCC() {
  [mpRmObject setupMPRemoteCommandCenter];
}





