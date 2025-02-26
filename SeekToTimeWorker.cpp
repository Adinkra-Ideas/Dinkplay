#include "SeekToTimeWorker.hpp"

SeekToTimeWorker::SeekToTimeWorker(Media * media, bool & threadLoopState) :
        media_{media},
        threadLoopState_{threadLoopState}
{}

SeekToTimeWorker::~SeekToTimeWorker() {}

void SeekToTimeWorker::startTheSeekToTimethreadLoop() {
    while (threadLoopState_) {
        // Set the Breakpoint to end of one lapDuration + now()
        gettimeofday(&clock_, 0);
        clkBreakPoint_ = clock_.tv_sec + 1;
        // Wait here until either clkBreakPoint_ time
        // elapses or threadLoopState_ becomes false
        while (threadLoopState_ && clock_.tv_sec < clkBreakPoint_) {
            gettimeofday(&clock_, 0);
        }
        // emit the signal every 1 second
        emit oneSecondReached();
    }
}

/**
  * Simply sets the threadLoopState_ to false
  * so that any blocking loop will be released.
  * @returns void
  */
void SeekToTimeWorker::stopTheSeekToTimethreadLoop() {
    threadLoopState_ = false;
}
