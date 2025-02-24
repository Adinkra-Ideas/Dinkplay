#ifndef SEEKTOTIMEWORKER_HPP
#define SEEKTOTIMEWORKER_HPP

#include <QObject>
#include <sys/time.h>

#include "Media.hpp"

class SeekToTimeWorker : public QObject
{
    Q_OBJECT

public:
    SeekToTimeWorker(Media * media, bool & threadLoopState);
    ~SeekToTimeWorker();

    void startTheSeekToTimethreadLoop();

signals:
    void oneSecondReached();

private:
    SeekToTimeWorker();

    Media    * media_;         // Parent instance that controls this thread
    bool     & threadLoopState_;

    timeval    clock_;
    qsizetype  clkBreakPoint_;
};

#endif // SEEKTOTIMEWORKER_HPP
