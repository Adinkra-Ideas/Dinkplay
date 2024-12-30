
// Used #import instead of the regular #include.
// The advantage in using #import is, the compiler
// will make sure that it includes the header file
// only once during compilation irrespective of how
// many modules is importing ( i.e including ) the
// same header file repeatedly. So you can avoid
// boiler-plate code like this:
// #ifndef MPREMOTECOMMANDCENTER_H
// #define MPREMOTECOMMANDCENTER_H
// #include "MPRemoteCommandCenter.h"
// #endif

// extending the NSObject class enable
// us to use methods like new, alloc,free etc.


#ifndef MPREMOTECOMMANDCENTER_H
#define MPREMOTECOMMANDCENTER_H

#include <QString>

void helloim();
QString localizedHostName();

#endif

// so, as a last resort for maintaining the instance alive, we might have to declare a pointer
// to the instance from this header, then assign the pointer to the class from its .mm,
// then when we want to do something inside the class, we call a function, that will execute that
// method. we will have to test this by incrementing and decrementing the value to see if it works.
// But how do we instantiate and destroy an objective-c pointer on exit ?
