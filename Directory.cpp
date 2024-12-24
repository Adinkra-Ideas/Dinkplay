#include "Directory.hpp"

#ifdef Q_OS_ANDROID
bool checkPermission() {
    QList<bool> permissions;

    /* PERMISSION REQUIRED TO ACCESS AUDIO FILE TYPES IN DEVICE FROM OUR APP */
    // Allow read of audio files (Granular Media Permissions) (for API 33 and above)
    // https://developer.android.com/about/versions/13/behavior-changes-13#granular-media-permissions
    auto r = QtAndroidPrivate::checkPermission("android.permission.READ_MEDIA_AUDIO").result();
    if (r != QtAndroidPrivate::Authorized)
    {
        r = QtAndroidPrivate::requestPermission("android.permission.READ_MEDIA_AUDIO").result();
        if (r == QtAndroidPrivate::Denied)
            permissions.append(false);
    }
    // Allow read of all file types (for API 32 and below)
    r = QtAndroidPrivate::checkPermission("android.permission.READ_EXTERNAL_STORAGE").result();
    if (r != QtAndroidPrivate::Authorized)
    {
        r = QtAndroidPrivate::requestPermission("android.permission.READ_EXTERNAL_STORAGE").result();
        if (r == QtAndroidPrivate::Denied)
            permissions.append(false);
    }

    // Allow display of notifications / foreground service notifications (for API 33 and above)
    r = QtAndroidPrivate::checkPermission("android.permission.POST_NOTIFICATIONS").result();
    if (r != QtAndroidPrivate::Authorized)
    {
        r = QtAndroidPrivate::requestPermission("android.permission.POST_NOTIFICATIONS").result();
        if (r == QtAndroidPrivate::Denied)
            permissions.append(false);
    }

    // r = QtAndroidPrivate::checkPermission("android.permission.WRITE_EXTERNAL_STORAGE").result();
    // if (r != QtAndroidPrivate::Authorized)
    // {
    //     r = QtAndroidPrivate::requestPermission("android.permission.WRITE_EXTERNAL_STORAGE").result();
    //     if (r == QtAndroidPrivate::Denied)
    //         permissions.append(false);
    // }

    // r = QtAndroidPrivate::checkPermission("android.permission.MANAGE_EXTERNAL_STORAGE").result();
    // if (r != QtAndroidPrivate::Authorized)
    // {
    //     r = QtAndroidPrivate::requestPermission("android.permission.MANAGE_EXTERNAL_STORAGE").result();
    //     if (r == QtAndroidPrivate::Denied)
    //         permissions.append(false);
    // }

    // r = QtAndroidPrivate::checkPermission("android.permission.READ_MEDIA_IMAGES").result();
    // if (r != QtAndroidPrivate::Authorized)
    // {
    //     r = QtAndroidPrivate::requestPermission("android.permission.READ_MEDIA_IMAGES").result();
    //     if (r == QtAndroidPrivate::Denied)
    //         permissions.append(false);
    // }

    // r = QtAndroidPrivate::checkPermission("android.permission.WRITE_MEDIA_IMAGES").result();
    // if (r != QtAndroidPrivate::Authorized)
    // {
    //     r = QtAndroidPrivate::requestPermission("android.permission.WRITE_MEDIA_IMAGES").result();
    //     if (r == QtAndroidPrivate::Denied)
    //         permissions.append(false);
    // }

    return (permissions.count() != 5);
}
#endif


Directory::Directory(QObject *parent) :
    Media{parent},
    backups_{"Adinkra-Ideas", "Dinkplay"}
{}

Directory::~Directory(){}

/**
  * This method loads the QSettings persisted
  * paths stored with key 'soundPaths' into
  * audioPaths_ and then calls preparePathsForPlay();
  * to prepare the app accordingly.
  * NOTE that before loading the persisted data,
  * the App must be at a point where startup is
  * completed. That is why I had to call it from the
  * bottom of main.cpp right before the return;
  * @returns void
  */
void Directory::loadSavedPaths() {
    audioPaths_ = backups_.value("soundPaths").value<QList<QString>>();

    for (QString &aPath: audioPaths_) {
        soundsHash_[aPath] = nullptr;
    }

    preparePathsForPlay();
}


void    Directory::addDir(QUrl path) {
    #ifdef Q_OS_ANDROID
    checkPermission();
    #endif

    // if there is an active, we stop the ma_sound.
    // THIS MUST BE DONE BEFORE TAMPERING WITH
    // audIt_ or *audIt_ or audioPaths_
    stopAnyCurrentPlaying();

    // Remove the rubbish prefixes that Qt does randomly.
    // Sometimes it sends file:///C:/Users/
    // Other Times it sends C:/Users/
    while (path.isLocalFile()) {
        path = QUrl(path).toLocalFile();
    }
    // Now change path to a schemed path in times
    if (! path.isLocalFile()) {
        path.setScheme(QString());
        path = QUrl::fromLocalFile(path.toString());
    }

    #ifdef Q_OS_ANDROID
    // decode twice from %253A to %3A then to :
    path.setUrl(QUrl::fromPercentEncoding(path.toString().toLatin1()));
    path.setUrl(QUrl::fromPercentEncoding(path.toString().toLatin1()));

    qsizetype pos = QString(path.toString()).indexOf("/tree/"); // /tree/primary:
    qsizetype subPathPos = QString(path.toString()).indexOf(":", pos);
    QString subPath;
    if (subPathPos != -1) {
        subPath = QString(path.toString()).sliced(subPathPos + 1);
        if (subPath.size() > 0 && subPath.at(0) != '/')
            subPath.prepend("/");
    }

    if (QString(path.toString()).sliced(pos + 6).startsWith("primary")) // inbuilt memory contains */tree/primary*
        path.setUrl(subPath.prepend("/storage/emulated/0"));
    else
        path.setUrl(QString(path.toString()).sliced(pos + 6, subPathPos - (pos + 6)).prepend("/storage/").append(subPath)); // 6 == len("/tree/"), 9 == len("xxxx-xxxx")
    path = QUrl::fromLocalFile(path.toString());
    #endif

    // how do we know a path that requires permission outside our sandbox?
    // I believe this homelocation will change output if we have permission.


    //////
    /// // /private/var/mobile/Containers/Data/Application/C2103CFC-BECD-4184-8BA5-55D41E7E8C99/dab.mp3
    //currDir_ = "file:///private/var/mobile/Containers/Data/Application/";
    //qDebug() << "recvd path " << path;
    //QString homeDir2 = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/";
    //qDebug() << "the home" << homeDir2;
    //currDir_ = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/";
    //currDir_.prepend("file://");
    currDir_ = path.toString();
    currDir_.chop(22);
    qDebug() << "the curr" << currDir_;
    qDebug() << "test " << QDir(QUrl(currDir_).toLocalFile()).exists();
    //////
    QDir dir(QUrl(currDir_).toLocalFile());
    //QDir dir(currDir_);
    qDebug() << "the dir" << dir;
    QStringList all = dir.entryList();
    for (QString &one: all) {
        qDebug() << "checking:" << one;
    }

    // currDir_ = path.toString();
    if (! currDir_.endsWith('/')) {
        currDir_.append('/');
    }

    doAddDir();
    // emit dirChanged();
}

QStringList Directory::getAudioPaths() {
    return audioPaths_;
}

void Directory::doAddDir() {
    //////
    audioPaths_.push_back(currDir_ + "dab.mp3");
    soundsHash_[currDir_ + "dab.mp3"] = nullptr;

    QDir dir(QUrl(currDir_).toLocalFile());

    // remove the file:/// from the beginning of file name
    // This problem exist coz we were initially building
    // for qmediaplayer
    if (QUrl(currDir_).isLocalFile()) {
        currDir_ = QUrl(currDir_).toLocalFile();
    }

    // Fetch all filepaths that ends with .mp3 from the directory
    QStringList mp3 = dir.entryList(QStringList() << "*.mp3", QDir::Files);

    for (QString &aMp3: mp3) {
        qDebug() << "here means access to files were granted" << aMp3 ;
        if (! audioPaths_.contains(currDir_ + aMp3)) {  // no repeat
            // Add the new path to our sound stringlist
            audioPaths_.push_back(currDir_ + aMp3);
            // Add the new path to our soundHash_ with its sound ptr = nullptr
            soundsHash_[currDir_ + aMp3] = nullptr;
        }
    }
    // QStringList mp4 = dir.entryList(QStringList() << "*.mp4", QDir::Files);
    // for (QString &aMp4: mp4) {
    //     videoPaths_.push_back(currDir_ + aMp4);
    // }

    // preparing for store to localStorage onExit
    backups_.setValue("soundPaths", QVariant::fromValue(audioPaths_));

    preparePathsForPlay();

    //////
    play();
}

/**
  * By the time this method is called,
  * audioPaths_ should already have its
  * list of filepaths that we need to
  * use to populate the audio page directory.
  * This method simply sets the path at the
  * index of audiopaths_ as the 'play now' AKA
  * When the user clicks on play, it starts
  * Playing.
  * And then this method emits the audioPathsChanged()
  * signal which tells frontend to reindex the
  * audio lists.
  * @returns void
  */
void Directory::preparePathsForPlay() {
    // set iterator back to beginning
    audIt_ = audioPaths_.begin();
    // vpIt_ = videoPaths_.begin();

    // so that after the user selects a directory
    // If they now sets interval timer, the interval
    // timer can trigger the play() from the mp3 list
    if (audioPaths_.size()) {
        // Set the path at *audit_ as active, ready for play
        setSource((*audIt_).toLocal8Bit().constData());
    }

    // After update, now refresh the qml view displaying audioPaths_ as list
    emit audioPathsChanged();
}
