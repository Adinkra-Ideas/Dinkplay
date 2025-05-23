#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <QObject>
#include <cstdlib> // malloc

#include "Media.hpp"

class Player : virtual public Media
{
public:
    explicit Player(QObject *parent = nullptr);
    ~Player();

    void            setSource(const char * path) override;
    void            playSource(QString) override;
    PlaybackState   playbackState() override;
    void            setPlaybackState(PlaybackState state) override;
    void            stopAnyCurrentPlaying() override;
    QString         getTitle() override;
    void            readyAudioForNewPlay() override;

    void    play() override;
    void    pause() override;
    void    suspendAudio() override;
    void    unsuspendAudio() override;
    void    playOrPause() override;
    void    endOfCurrentAudio(bool shouldStopCompletely = false); // 3 GREAT CHANGE CHNAGED THIS TO having a param

protected:
    ma_sound            sound_;
    ma_result           result_;
    ma_device_config    config_;

    bool        engineInit_;    // True if mini audio engine was successfully init-ed, else false
};

#endif // PLAYER_HPP
