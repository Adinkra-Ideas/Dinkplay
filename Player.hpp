// CLEANED
#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <QObject>

#include "Media.hpp"

#ifndef MINIAUDIO_IMPLEMENTATION
#define MINIAUDIO_IMPLEMENTATION
extern "C"
{
#include "ma_lib/miniaudio.h"
}
#endif // MINIAUDIO_IMPLEMENTATION

class Player : virtual public Media
{
public:
    explicit Player(QObject *parent = nullptr);
    ~Player();

    quint8  playbackState() override;
    void    setSource(const char * path) override;
    void    playSource(QString) override;
    void    setPlaybackState(quint8 state) override;
    void    stopAnyCurrentPlaying() override;
    QString getTitle() override;

    void    play() override;
    void    pause() override;
    // void    playOrPause() override;
    void    stop();

protected:
    ma_sound    sound_;
    ma_result   result_;
    ma_engine   engine_;

    bool        engineInit_;    // True if mini audio engine was successfully init-ed, else false
    quint8      state_;         // Holds the current media playback state at any given time. 0 == stopped, 1 == playing, 2 == paused
};

#endif // PLAYER_HPP
