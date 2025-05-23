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
{
    // //testing to see what got bundled into the app (ios)
    // QString currDir_ = ".";
    // qDebug() << "checking base:" << currDir_;
    // QDir dir(currDir_);
    // QStringList all = dir.entryList(QStringList()/*, QDir::Dirs*/);
    // for (QString &one: all) {
    //     qDebug() << "checking:" << one;
    // }
}

Directory::~Directory() {}

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

/**
  * When we add an mp3 to Dinkplay using the file browser,
  * the mp3's filepath gets added as an element in audioPaths_
  * array and also gets added as a key to soundHash_ map with
  * its value being the decoded sound file.
  * This method simply receives a pos of an mp3 filepath stored
  * in audioPaths_ as param, then it deletes the mp3's filepath
  * from audioPaths_ that is associated with that pos, then it
  * removes the deleted file's existence from soundHash_ map,
  * freeing up memory if needed.
  * Be aware that the original file on the device is not deleted.
  * Just the decoded copy that Dinkplay generated for use.
  * @returns void
  */
void Directory::deleteAudioPath(qint16 pathPos) {
    // if there is an active, we stop the ma_sound.
    stopAnyCurrentPlaying();

    // retrieve the mp3 file path or return on failure
    QString posToPath = (pathPos >= 0 && pathPos < audioPaths_.size()) ? audioPaths_.at(pathPos) : "";
    if (posToPath.isEmpty()) {
        return ;
    }

    // remove the posToPath from audioPaths_ and free up its memory
    audioPaths_.removeOne(posToPath);
    audioPaths_.squeeze();

    // free the decoded sound from soundsHash_[posToPath] if not nullptr
    if (soundsHash_[posToPath] != nullptr) {
        ma_sound_uninit(soundsHash_[posToPath]);
        delete soundsHash_[posToPath];
    }
    // remove the posToPath from soundsHash_
    soundsHash_.erase(posToPath);

    // If iOS, also delete the file since it is a copied tmp file
    // in our app's sandbox
    #ifdef Q_OS_IOS
    QFile::remove(posToPath);
    #endif
    // If android, the file will be deleted if temp
    #ifdef Q_OS_ANDROID
        QFile::remove(posToPath);
    #endif


    // After delete, now prepare audio for play once again
    preparePathsForPlay();
}


// void    Directory::addDir(QUrl path) {
//     // // Permission request for android users
//     // #ifdef Q_OS_ANDROID
//     // checkPermission();
//     // #endif

//     // if there is an active, we stop the ma_sound.
//     // THIS MUST BE DONE BEFORE TAMPERING WITH
//     // audIt_ or *audIt_ or audioPaths_
//     stopAnyCurrentPlaying();

//     // Remove the rubbish prefixes that Qt does randomly.
//     // Sometimes it sends file:///C:/Users/
//     // Other Times it sends C:/Users/
//     while (path.isLocalFile()) {
//         path = QUrl(path).toLocalFile();
//     }
//     // Now change path to a schemed path AKA file:///
//     if (! path.isLocalFile()) {
//         path.setScheme(QString());
//         path = QUrl::fromLocalFile(path.toString());
//     }

//     // // Arranging directory path according to android uncertainties and specifications
//     // #ifdef Q_OS_ANDROID
//     // // decode twice from %253A to %3A then to :
//     // path.setUrl(QUrl::fromPercentEncoding(path.toString().toLatin1()));
//     // path.setUrl(QUrl::fromPercentEncoding(path.toString().toLatin1()));
//     // //here
//     // qsizetype pos = QString(path.toString()).indexOf("/tree/"); // /tree/primary:
//     // qsizetype subPathPos = QString(path.toString()).indexOf(":", pos);
//     // QString subPath;
//     // if (subPathPos != -1) {
//     //     subPath = QString(path.toString()).sliced(subPathPos + 1);
//     //     if (subPath.size() > 0 && subPath.at(0) != '/')
//     //         subPath.prepend("/");
//     // }

//     // if (QString(path.toString()).sliced(pos + 6).startsWith("primary")) // inbuilt memory contains */tree/primary*
//     //     path.setUrl(subPath.prepend("/storage/emulated/0"));
//     // else
//     //     path.setUrl(QString(path.toString()).sliced(pos + 6, subPathPos - (pos + 6)).prepend("/storage/").append(subPath)); // 6 == len("/tree/"), 9 == len("xxxx-xxxx")
//     // path = QUrl::fromLocalFile(path.toString());
//     // #endif

//     // If IOS, it means we received a big string with each selected filepath separated by a comma.
//     // here we break the string and assign them directly.
//     // #ifdef Q_OS_IOS
//     // QStringList allPaths = path.toString().split(',', Qt::SkipEmptyParts);
//     // for (QString &onePath: allPaths) {
//     //     // If path begins with "file:///", we have to change it to "/"
//     //     // before assigning to audioPaths_
//     //     if (QUrl(onePath).isLocalFile()) {
//     //         // we will use the path to generate a temp file here

//     //         audioPaths_.push_back(QUrl(onePath).toLocalFile());
//     //         // Add the new path to our soundHash_ with its sound ptr = nullptr
//     //         soundsHash_[QUrl(onePath).toLocalFile()] = nullptr;
//     //         qDebug() << "the received path in if: " << path.toString();
//     //     } else {
//     //         audioPaths_.push_back(onePath);
//     //         // Add the new path to our soundHash_ with its sound ptr = nullptr
//     //         soundsHash_[onePath] = nullptr;
//     //         qDebug() << "the received path in else: " << onePath;
//     //     }
//     // }
//     // #endif


//     //////
//     // Extracting the unique App_ID of this app
//     // QString thisAppID = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
//     // qsizetype pos = thisAppID.lastIndexOf("/");
//     // currDir_ = thisAppID.sliced(0, pos + 1);
//     // thisAppID = thisAppID.sliced(pos + 1);
//     // currDir_.append(thisAppID + "/");
//     // now you can use dir.entryList to print out sandox content of *this app

//     currDir_ = path.toString();
//     if (! currDir_.endsWith('/')) {
//         currDir_.append('/');
//     }

//     doAddDir();
// }

QStringList Directory::getAudioPaths() {
    return audioPaths_;
}

// // INVALID COMMENT
// // On iOS, this method will be called from objective-c
// // after the user-selected audios have been stored to
// // sandbox/tmp directory
// void Directory::doAddDir() {
//     // INVALID COMMENT
//     // Since IOS audios will only be picked from sandbox/tmp,
//     // no need to enter this block for iOS users.
//     // #ifndef Q_OS_IOS
//     // // we can clean this below code to if we can test with android to know there is no problem
//     if (QUrl(currDir_).isLocalFile()) {
//         currDir_ = QUrl(currDir_).toLocalFile();
//     }
//     QDir dir(currDir_);

//     // Fetch all filepaths that ends with .mp3 from the directory
//     QStringList mp3 = dir.entryList(QStringList() << "*.mp3", QDir::Files);

//     for (QString &aMp3: mp3) {
//         if (! audioPaths_.contains(currDir_ + aMp3)) {  // no repeat
//             // Add the filepath to our sound stringlist
//             audioPaths_.push_back(currDir_ + aMp3);
//             // use the filepath as key in our soundHash_
//             // dict, with its decodedSound ptr = nullptr
//             soundsHash_[currDir_ + aMp3] = nullptr;
//         }
//     }

//     // #ifndef Q_OS_IOS
//     // // preparing for backup to localStorage onExit.
//     // // This was also placed inside this ifdef coz iOS
//     // // wont backup onExit coz the logic we used is to
//     // // load only the audio files from its sandbox/tmp
//     // // directory during each launch.
//     // backups_.setValue("soundPaths", QVariant::fromValue(audioPaths_));
//     // #endif

//     // // IOS audios will simply be picked from sandbox/tmp.
//     // #ifdef Q_OS_IOS
//     // // We check whether sounds exists in our
//     // // sandbox/tmp directory, so we add them too.
//     // pickIosAudiosFromSandboxTmpDir();
//     // #endif

//     preparePathsForPlay();
// }


void Directory::addStartupAudiosOnEmptyStartupAudioListings() {
    // if no audio on startup, add the factory audio file.
    // We had to copy it into a temp directory created
    // by qt coz c cannot open(""assets:/Default.mp3")
    // for miniaudio to play the file.
    QString tempFile = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/Default.mp3";
    if (! QFile::copy(":/ui/audios/Default.mp3", tempFile)) {
        tempFile.clear();
    }
    if (! tempFile.isEmpty()) {
        indexToAudioList(tempFile);
    }

    // For iOS, we now check whether sounds exists in our
    // sandbox/tmp directory. if yes, we add them too.
    #ifdef Q_OS_IOS
    // Now we check whether more sounds exists in our
    // iOS sandbox/tmp directory, so we add them too.
    pickIosAudiosFromSandboxTmpDir();
    #endif
}

// THIS METHOD CAN BE ADAPTED TO REPLACE THE for() LOOP IN doAddDir()
/**
  * This method is for iOS only.
  * It simply scans the app's sandbox/tmp directory
  * and load all .mp3 files into audioPaths_ and
  * soundsHash_, making them ready for play.
  * @returns void
  */
void Directory::pickIosAudiosFromSandboxTmpDir() {
    QString sandboxTmpDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/tmp/";
    QDir tmpDir(sandboxTmpDir);
    QStringList all = tmpDir.entryList(QStringList() << "*.mp3" << "*.flac" << "*.wav", QDir::Files);
    for (QString &one: all) {
        indexToAudioList(sandboxTmpDir + one);
    }
}

/**
 * will rename to backupAudioPathsAndPreparePathsForPlay
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
    if (audioPaths_.isEmpty()) {
        addStartupAudiosOnEmptyStartupAudioListings();
    }

    #ifndef Q_OS_IOS
    // preparing for backup to localStorage onExit.
    // This was also placed inside this ifdef coz iOS
    // wont backup onExit coz the logic we used is to
    // load only the audio files from its sandbox/tmp
    // directory during each launch.
    backups_.setValue("soundPaths", QVariant::fromValue(audioPaths_));
    #endif

    // set iterator back to beginning
    audIt_ = audioPaths_.begin();

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


/**
  * This method is used to add one audio
  * file to Dinkplay's Audio List.
  * @param oneFile path to the audio file
  * @returns void
  */
void Directory::addFileToDinkplay(QString oneFile) {
    // this might be necessary for playing audio in non ios and non android
    // environment. e.g., windows
    // oneFile = QUrl::fromLocalFile(oneFile).toString();

    // Arranging directory path according to android uncertainties and specifications
    #ifdef Q_OS_ANDROID
    // decode twice from %253A to %3A then to :
    oneFile = QUrl::fromPercentEncoding(oneFile.toLatin1());
    oneFile = QUrl::fromPercentEncoding(oneFile.toLatin1());

    // trimming the useless beginning contents
    // for both sd card and internal storage files.
    qsizetype pos = oneFile.indexOf("/document/"); // /tree/primary: for dir, /document/primary: for file selection
    qsizetype subPathPos = oneFile.indexOf(":", pos);
    QString subPath;
    if (subPathPos != -1) {
        subPath = oneFile.sliced(subPathPos + 1);
        if (subPath.size() > 0 && subPath.at(0) != '/')
            subPath.prepend("/");
    }

    // prepend file located in internal storage with "/storage/emulated/0".
    // else if located in sd card, prepend with "/storage/"
    if (oneFile.sliced(pos + 10).startsWith("primary")) // inbuilt memory contains */document/primary*
        oneFile = subPath.prepend("/storage/emulated/0");
    else
        oneFile = oneFile.sliced(pos + 10, subPathPos - (pos + 10)).prepend("/storage/").append(subPath); // 6 == len("/tree/"), 9 == len("xxxx-xxxx")
    #endif

    // add the oneFile
    indexToAudioList(oneFile);
}


/**
  * This method is called from non iOS devices.
  * It opens a native filedialog and then adds
  * the user-selected audio files to dinkplay.
  * @returns void
  */
void Directory::openDialogFromCpp() {
    // Permission request for android users
    #ifdef Q_OS_ANDROID
    checkPermission();
    #endif

    QFileDialog dialog(0);
    dialog.setFileMode(QFileDialog::ExistingFiles);
    // dialog.setNameFilter(tr("*.mp3"));
    dialog.setDirectory(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation));

    QStringList fileNames;
    if (dialog.exec()) {
        fileNames = dialog.selectedFiles();
    } else {
        return ; // do nothing if user cancelled file dialog they opened
    }

    // if there is an active, we stop the ma_sound.
    // THIS MUST BE DONE BEFORE TAMPERING WITH
    // audIt_ or *audIt_ or audioPaths_
    stopAnyCurrentPlaying();

    for (QString &oneFile: fileNames) {
        // for paths that were selected from shortcuts
        // AKA Media or Download FileProvider, they carry the link like
        // content://com.android.providers.downloads.documents/document/344
        // Since it is impossible to trace the actual location of the file
        // to /storage/ path, we use QML internal link resolver to copy the
        // file and play it from another location we will create.
        // see https://ekkesapps.wordpress.com/qt-6-cmake/android-scopedstorage-filedialog/
        if (oneFile.indexOf("content://com.android.providers") == 0) {
            // First we retrieve the filename
            QString filename = QFileInfo(oneFile).fileName();
            // Then we save it to temporal playable location
            if (filename.endsWith(".mp3") || filename.endsWith(".flac") || oneFile.endsWith(".wav")) {
                if ( QFile::exists(QQmlFile::urlToLocalFileOrQrc(oneFile)) ) {
                    saveFilesFromMediaandDownloadFileproviderLinks(oneFile, filename);
                }
            }
        }
        // Else it is a ExternalStorage file that the user selected from
        // internal or sd card directly. Or it is simply a file selected
        // from Windows app.
        // In first case, we can trace the /storage/ path and play it directly
        // without first copying to temp playable location
        else if (oneFile.endsWith(".mp3") || oneFile.endsWith(".flac") || oneFile.endsWith(".wav")) {
            addFileToDinkplay(oneFile);
        } 
    }

    // #ifndef Q_OS_IOS
    // // preparing for backup to localStorage onExit.
    // // This was also placed inside this ifdef coz iOS
    // // wont backup onExit coz the logic we used is to
    // // load only the audio files from its sandbox/tmp
    // // directory during each launch.
    // backups_.setValue("soundPaths", QVariant::fromValue(audioPaths_));
    // #endif

    // Now make the audio list usable after tampering
    // with audioPaths_'s data
    preparePathsForPlay();
}

void Directory::saveFilesFromMediaandDownloadFileproviderLinks(QString filePath, QString filename) {
    #ifdef Q_OS_ANDROID
    QString tempFile = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/" + filename;
    if (! QFile::copy(filePath, tempFile)) {
        tempFile.clear();
    }
    if (! tempFile.isEmpty()) {
        indexToAudioList(tempFile);
    }
    #endif
}

void Directory::indexToAudioList(QString audioFilePath) {
    if (! audioPaths_.contains(audioFilePath)) { // no repeat
        // Add the filepath to our sound stringlist
        audioPaths_.push_back(audioFilePath);
        // use the filepath as key in our soundHash_
        // dict, with its decodedSound ptr = nullptr
        soundsHash_[audioFilePath] = nullptr;
    }
}

//qDebug() << "path: " << path.toString();
//qsizetype pos = path.toString().lastIndexOf("/");
//currDir_ = path.toString();
// currDir_.truncate(pos + 1);
// path = path.toString().sliced(pos + 1);
// qDebug() << "currDir: " << currDir_ << "pos: " << pos;
// qDebug() << "path: " << path.toString();
// audioPaths_.push_back(currDir_ + path.toString());
// soundsHash_[currDir_ + path.toString()] = nullptr;


// home dir in simulation == /data/Containers/Data/Application/FC24ED6A-E8AA-44FC-B38C-55B7993BE416/
// file explorere         == /data/Containers/Shared/AppGroup/FC24ED6A-E8AA-44FC-B38C-55B7993BE416/File Provider Storage/
