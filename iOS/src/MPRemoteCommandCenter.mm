// // IF WE WERE TO IMPLEMENT A MAIN FOR THIS CLASS
// // ./main.m
// // #import <objc/NSObject.h>
// // #import "MPRemoteCommandCenter.h"
// // int main(int argv, char* argc[])
// // {
// //   id myObject = [MPRemoteCommandCenter new];  // MPRemoteCommandCenter *myObject = new MPRemoteCommandCenter()
// //   [myObject helloer];                         // myObject.helloer()

// //   return 0;
// // }

#import <Foundation/Foundation.h>
// #import <CoreFoundation/CoreFoundation.h>
// #import <Foundation/NSHost.h>
#import "MPRemoteCommandCenter.h"

void helloim() {
  @autoreleasepool {

    NSLog(@"Helloim, World!");

  }
}

// QString localizedHostName() {
//   // return QString::fromNSString([[NSHost currentHost] localizedName]);
//   return QString::fromNSString(NSHost.currentHost.localizedName);
// }




#import <objc/NSObject.h>
#import <stdio.h>

@interface MPRemoteCommandCenter : NSObject
// {
    // member variables goes here
// }

- (void) helloer;

@end

@implementation MPRemoteCommandCenter

- (void)helloer {
  printf("%s\n", "Hello gra gra w World");
}

@end




