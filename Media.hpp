#ifndef MEDIA_HPP
#define MEDIA_HPP

#include <QObject>
#include <QStringList>
#include <QString>
#include <QUrl>
#include <Qt>
#include <unordered_map>
#include <QFileInfo>
#include <QStandardPaths>
// #include <QDebug> //

#ifndef MINIAUDIO_IMPLEMENTATION
#define MINIAUDIO_IMPLEMENTATION
extern "C"
{
#include "ma_lib/miniaudio.h"
}
#endif // MINIAUDIO_IMPLEMENTATION


// /**
//  * @enum Media::RepeatMode
//  * @brief Defines the repeat behavior for media playback
//  *
//  * Used in repeat functionality and QML property binding.
//  * Q_ENUM registered for Qt meta-system integration.
//  */
// enum class RepeatMode : quint8
// {
//     REPEAT_NONE = 0,      // Play through list once without repeating
//     REPEAT_SINGLE = 1,    // Continuously repeat current track
//     REPEAT_ALL = 2        // Repeat entire playlist indefinitely
// };
// CONTINUE: USE THE REPEAT STATE ENUM TO ELIMINATE REPEAT MAGIC NUMBER



class Media : public QObject
{
    Q_OBJECT

    /********* For Optimizing Battery Begins *************/
    Q_PROPERTY(bool appMinimizedStatus READ getAppMinimizedStatus NOTIFY appMinimizedStatusChanged)
    /****************************************************/

    /********* For Interval Controls Begins *************/
    Q_PROPERTY(bool intervalStatus READ getIntervalStatus WRITE setIntervalStatus NOTIFY intervalStatusChanged)
    Q_PROPERTY(qint16 lapCount WRITE setLapCount)
    Q_PROPERTY(qint16 lapDuration WRITE setLapDuration)
    Q_PROPERTY(qint16 restDuration WRITE setRestDuration)
    /****************************************************/

    /* ******* For The Media Player Begins ***************/
    Q_PROPERTY(PlaybackState player READ playbackState WRITE setPlaybackState NOTIFY playbackStateChanged)
    Q_PROPERTY(QString playSource WRITE playSource NOTIFY playbackStateChanged) // For when a link is clicked in audio listings
    Q_PROPERTY(bool change WRITE changePlay NOTIFY playbackStateChanged)        // When prevOrNext clicked by user
    Q_PROPERTY(QString title READ getTitle NOTIFY playbackStateChanged)         // Get the filename when new sound starts playing
    Q_PROPERTY(quint8 repeat READ getRepeat WRITE setRepeat NOTIFY repeatChanged)
    /******************************************************/

    /************ For Directory Management Begins *********/
    // Q_PROPERTY(QString currDir /*READ getDir*/ WRITE addDir/* NOTIFY dirChanged*/)          // Returns the current directory
    Q_PROPERTY(QStringList audiopaths READ getAudioPaths NOTIFY audioPathsChanged)  // Returns the new list of files that should be indexed.
    Q_PROPERTY(qint16 deleteAudioPath WRITE deleteAudioPath NOTIFY audioPathsChanged)
    Q_PROPERTY(bool openDialog READ manageDocumentPickModal)
    /******************************************************/

    // try adding the and sign
    /************ For Seek To Time Management Begins *********/
    Q_PROPERTY(quint64 focusedAudioLengthInt READ getLengthOfFocusedAudio NOTIFY lengthofFocusedAudioChanged)
    Q_PROPERTY(quint64 focusedAudioCursorTimeInt READ getCursorTimeOfFocusedAudio WRITE setCursorTimeOfFocusedAudio NOTIFY currTimeOfFocusedAudioChanged)
    /******************************************************/

public:
    /*
     * @enum Media::PlaybackState
     * @brief Represents the current state of media playback
     *
     * Used in Q_PROPERTY and state management throughout the media system.
     */
    enum PlaybackState : quint8
    {
        PLAYBACK_STOPPED = 0,   // No audio is playing
        PLAYBACK_PLAYING = 1,   // Audio is actively playing
        PLAYBACK_PAUSED = 2     // Audio is paused (retains playback position)
    };
    Q_ENUM(PlaybackState)       // Q_ENUM registered to enable PlaybackState be recognized as a typedef from QML / Qt meta-object system integration.



    /******** Constructor && Destructor ********/
    explicit Media(QObject *parent = nullptr);
    virtual  ~Media() = 0;
    /*******************************************/

    /******** Methods Implemented here in Media.cpp ********/
    void        setRepeat(quint8 val);
    quint8      getRepeat();
    void        changeAppLifecycleState(Qt::ApplicationState state);
    bool        getAppMinimizedStatus();
    /*******************************************************/

    /*****  Implemented in Interval.hpp Begins  *****/
    virtual void killIntervalThread() = 0;
    virtual void setIntervalStatus(bool flag) = 0;
    virtual bool getIntervalStatus() = 0;
    virtual void setLapCount(qint16 newValue) = 0;
    virtual void setLapDuration(qint16 newValue) = 0;
    virtual void setRestDuration(qint16 newValue) = 0;
    /***********************************************/

    /********** implemented in Player.cpp ***********/
    virtual PlaybackState   playbackState() = 0;
    virtual void            setPlaybackState(PlaybackState) = 0;
    virtual void            setSource(const char * path) = 0;
    virtual void            playSource(QString path) = 0;
    virtual void            stopAnyCurrentPlaying() = 0;
    virtual void            play() = 0;
    virtual void            pause() = 0;
    virtual void            suspendAudio() = 0;
    virtual void            unsuspendAudio() = 0;
    virtual void            playOrPause() = 0;
    virtual QString         getTitle() = 0;
    virtual void            readyAudioForNewPlay() = 0;
    /*************************************************/

    /******** implemented in Directory.cpp ***********/
    // virtual void    addDir(QUrl path) = 0;
    // virtual QString getDir() const = 0;
    virtual QStringList getAudioPaths() = 0;
    virtual void loadSavedPaths() = 0;
    virtual void deleteAudioPath(qint16 pathPos) = 0;
    virtual void preparePathsForPlay() = 0;
    virtual void addFileToDinkplay(QString oneFile) = 0;
    virtual void indexToAudioList(QString audioFilePath) = 0;
    /*************************************************/

    /******** Implemented in ChangePlay.cpp **********/
    virtual void    changePlay(bool move) = 0;
    /*************************************************/

    /******** Implemented in SeekToTime.cpp **********/
    virtual void            startSeekToTimeThreadLoop() = 0;
    virtual void            stopSeekToTimeThreadLoop() = 0;
    virtual const quint64 & getLengthOfFocusedAudio() const = 0;
    virtual void            updateCursorTimeOfFocusedAudio() = 0;
    virtual const quint64 & getCursorTimeOfFocusedAudio() const = 0;
    virtual void            setCursorTimeOfFocusedAudio(const quint64 & newTime) = 0;
    /*************************************************/

    /******* Implemented in Top.cpp ***************/
    virtual void killAllThreads() = 0;
    virtual void notifyJavaSeviceAboutPlaying(bool isplaying) = 0;
    virtual void updateAllAudioDetailsDisplayers() = 0;
    virtual bool seizeControlOfAudioSession() = 0;
    virtual bool manageDocumentPickModal() = 0;
    /**********************************************/

public slots:
    virtual void checkForBackPress() = 0;
    virtual const QString secondsToDigitalClock(quint64 total) const = 0;   /* SeekToTime.cpp */
    virtual void generateReversedAudioAtByteLevel(qint16 pathPos) = 0;      /* ModifyAudioFrames.cpp */
    // virtual void generateReversedAudioAtBitLevel(qint16 pathPos) = 0;    /* ModifyAudioFrames.cpp */

signals:
    /******* Mostly Used in Interval.hpp Begins  ********/
    // For starting the Blocking method in its sub thread
    void startTheIntervalLooping();
    // For Refreshing the UI onIntervalStatusChanged
    void intervalStatusChanged();
    /***************************************************/

    /********* Mostly used in Directory.hpp ************/
    // For getting the indexed filelists to frontend
    void audioPathsChanged();
    /***************************************************/

    /********* Mostly Used from Player.cpp *************/
    // Whenever a playing sound changes
    void playbackStateChanged(QString newPath);
    // Notify mediacontrols about length of new audio
    void lengthofFocusedAudioChanged();
    /***************************************************/

    /******** Mostly used from here in media.cpp *******/
    // For notifying frontend that repeat-X changed
    void repeatChanged();
    void appMinimizedStatusChanged();
    /***************************************************/

    /******** Mostly used from SeekToTime.cpp **********/
    // Notify mediacontrols about current time of audio
    void currTimeOfFocusedAudioChanged();
    // For starting the Blocking method in its sub thread
    void startTheSeekToTimeThreadLoop();
    // for receiving app in front or in back state
    void applicationStateChanged(Qt::ApplicationState state);
    /***************************************************/

protected:
    bool                   appMinimizedStatus_;
    QString                  currDir_;  // Dir selected by the user, from where media files was last added
    quint8                    repeat_;  // 0 == repeat none, 1 == repeat 1, 2 == repeat all
    PlaybackState              state_;  // Holds the current media playback state at any given time.
    bool         taskRunningDontPlay_;  // mostly needed for preventing play while reverse audio is being generated. Coy if miniaudio plays, the app will crash
    QStringList           audioPaths_;  // holds path to each mp3 files found in directory selected by the user for media search
    QStringList::iterator      audIt_;  // iterator to audioPaths_
    QString       currentPlayingPath_;  // stores the filepath of currently active audio
    QString      currentPlayingTitle_;  // stores the title of currently active audio
    QString     currentPlayingArtist_;  // stores the Artist of currently active audio
    std::unordered_map<QString, ma_sound *> soundsHash_;    // used as a storage for Holding one path from audioPaths_ as keys and their associated values == their decoded ma_sound.
    bool                    suspended_;                     // if true, it means the current nowPlaying audio is suspended. In this case, calling unsuspendAudio() will play it. If false, calling unsuspendAudio will do nothing.

    ma_engine   engine_;
    ma_device*  device_;                    // After the ma_engine_init() ints our engine, we backup the device it created for us here so we can use it when necessary
    ma_uint32   sampleRate_;                // sampleRate is the number of PCM frames that are processed per second. Useful for converting PCM frames to seconds or milliseconds since ma_sound_seek_to_pcm_frame() has no seek to seconds alt. To jump to 2 secs just do (sampleRate * 2)
    ma_uint64   totalPcmFrames_;            // totalPcmFrames_ is the total number of PCM frames in focused audio file. Useful for getting the total length of focused audio in seconds with (totalPcmFrames_ / sampleRate_)
    quint64     totalAudioSecs_;            // totalAudioSecs_ == total length of focused audio in seconds
    quint64     currentFrameNumberToSec_;   // currentFrameNumberInt_ == Current frame number of now playing cursor in focused audio
    quint8*     combinedAudioFrames_;       // when we're modifying an audio file's raw frame, we first retrieve the raw frames and store them here
};

#endif // MEDIA_HPP
