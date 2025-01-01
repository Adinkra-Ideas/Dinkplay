#import <Foundation/Foundation.h>
#import <MediaPlayer/MediaPlayer.h>
#import <UIKit/UIKit.h>
#include <stdio.h> // because nslog aint printing to qt debug
#import <unistd.h>
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
  NSNotificationCenter *notifCenter;        // for adding and removing observers AKA notification for audio state-change listeners
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
  // Initialize base class before this AKA self
  // IF PROBLEM, SIMPLY COMMENT OUT COZ APP WORKED FINE WITHOUT IT.
  self = [super init];
  if (!self) {
    return nil;
  }

  return self;
}

// class destructor
-(void)dealloc
{
  // unplug the listeners.  we will comment this
  [notifCenter removeObserver:self];

  [super dealloc];  // call the destructor of NSObject AKA parent that we inherited from
}


/**
  * This method is called when app launches on iOS.
  * It is called from initMyMPRemoteCC() at the time
  * when the app is instantiating the objective-c
  * part of the app.
  * It simply sets up a listener to listen for:
  * (1) interrupts AKA phone calls, alarms or any
  * other system sound that will cause this app to
  * relinquish audiosession,
  * (2) Duckings AKA when another audio player takes
  * control of audiosession.
  * NOTES:
  * (1) This listener will pick up notifications about
  * System interruptions even if the app is not playing
  * a song.
  * (2) But this listener can only pick up notifications
  * about Duckings only if the app was playing an audio at the
  * time the other audio player takes control of audiosession.
  * @param none
  * @returns void
  */
- (void)listenForInterruptions {
  [[AVAudioSession sharedInstance] setCategory:
                  AVAudioSessionCategoryPlayback
                  withOptions:AVAudioSessionCategoryOptionDefaultToSpeaker | AVAudioSessionCategoryOptionAllowBluetooth | AVAudioSessionCategoryOptionAllowBluetoothA2DP | AVAudioSessionCategoryOptionAllowAirPlay
                  error:nil];

  @try {
    // Listen for system interrupts and duckings and suspend/unsuspend play accordingly
    notifCenter = [NSNotificationCenter defaultCenter];
    [notifCenter  addObserver:
                  self
                  selector:@selector(onAudioSessionInterrupted:) //wow! if the callback method takes an arg and you fail to add the appending ":" to its name, app will crash at runtime when it gets here
                  name:AVAudioSessionInterruptionNotification
                  object:[AVAudioSession sharedInstance]];
    //The below is for appending listener for bluetooth connection.
    // if you fail to add "withOptions:AVAudioSessionCategoryOptionDefaultToSpeaker | AVAudioSessionCategoryOptionAllowBluetoothA2DP | AVAudioSessionCategoryOptionAllowAirPlay"
    // beforenow, it will not fire properly besides firng on app launch.
    [notifCenter  addObserver:
                  self
                  selector:@selector(onAudioSessionInterrupted:)
                  name:AVAudioSessionRouteChangeNotification
                  object:[AVAudioSession sharedInstance]];


  }
  @catch (NSException *exception) {
    // NSLog(@"%@", exception);
  }
}

/**
  * This method is callback passed to
  * listenForInterruptions() above.
  * It reads the notification to see whether
  * it is begin or end of notification, then
  * decides whether audio should be
  * suspended or unsuspended.
  * @param none
  * @returns void
  */
- (void) onAudioSessionInterrupted: (NSNotification *) notification
{
  // This means it is either a system interrupt or ducking by other audio apps
  if ([notification.name isEqualToString:AVAudioSessionInterruptionNotification]) {
    NSDictionary *userInfo = notification.userInfo;

    //Check to see if it was a Begin interruption
    if ([[userInfo valueForKey:AVAudioSessionInterruptionTypeKey] isEqualToNumber:[NSNumber numberWithInt:AVAudioSessionInterruptionTypeBegan]]) {
      // Suspend your audio
      cppObject->suspendAudio();
    } else if([[userInfo valueForKey:AVAudioSessionInterruptionTypeKey] isEqualToNumber:[NSNumber numberWithInt:AVAudioSessionInterruptionTypeEnded]]){
      // This block is intended to resume play only if it is ok to do so.
      // AKA system interrupts can resume, but docking by z.B VLC shouldnt
      // because if it resumes after vlc ducked it, it will lose audio and the
      // only work around in that case is to wait for at least 6 seconds after
      // ducking interrupt ends before resuming. Even at that, it still wont
      // resume from background. That is why iOS system forbids it.
      NSNumber *optionsValue = userInfo[AVAudioSessionInterruptionOptionKey];
      AVAudioSessionInterruptionOptions options = (AVAudioSessionInterruptionOptions)[optionsValue unsignedIntegerValue];
      if (options & AVAudioSessionInterruptionOptionShouldResume) {
        // Unsuspend your audio
        cppObject->unsuspendAudio();
      }
    }
  }
  // This means a headset device (bluetooth or usb audio to speaker) interruption
  else if ([notification.name isEqualToString:AVAudioSessionRouteChangeNotification]) {
    NSDictionary *userInfo = notification.userInfo;
    NSNumber *reasonValue = userInfo[AVAudioSessionRouteChangeReasonKey];

    if (reasonValue) {
      AVAudioSessionRouteChangeReason reason = (AVAudioSessionRouteChangeReason)[reasonValue unsignedIntegerValue];
      switch (reason) {
        case AVAudioSessionRouteChangeReasonNewDeviceAvailable:
          // plugging in a headset, has made a preferred audio route available
          // Unsuspend your audio
          cppObject->unsuspendAudio();
          break;
        case AVAudioSessionRouteChangeReasonOldDeviceUnavailable:
          // unplugging a headset, now users audio privacy must be respected
          // Suspend your audio
          cppObject->suspendAudio();
          break;
        default:
          break;
      }
    }
  }
}


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
  // Play and pause buttons (bluetooth earphone control)
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





