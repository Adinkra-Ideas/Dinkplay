// CLEANED
#ifndef DIRECTORY_HPP
#define DIRECTORY_HPP

#include <qglobal.h>
#ifdef Q_OS_ANDROID
#include <QCoreApplication>
#include <QtCore/private/qandroidextras_p.h>
#endif

#include <QObject>
#include <QString>
#include <QUrl>
#include <QDir>
#include <QStringList>
#include <QSettings>
#include <QStandardPaths>
#include <QTemporaryDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QQmlFile>

#include "Media.hpp"

class Directory : virtual public Media
{
public:
    explicit Directory(QObject *parent = nullptr);
    virtual ~Directory();

    // void        addDir(QUrl path) override;
    QStringList getAudioPaths() override;
    void        loadSavedPaths() override;
    void        deleteAudioPath(qint16 pathPos) override;
    void        preparePathsForPlay() override;
    void        addFileToDinkplay(QString oneFile) override;
    void        indexToAudioList(QString audioFilePath) override;



    void addStartupAudiosOnEmptyStartupAudioListings();
    void openDialogFromCpp();
    void pickIosAudiosFromSandboxTmpDir();
    void saveFilesFromMediaandDownloadFileproviderLinks(QString filePath, QString filename);
    // void        doAddDir();


private:
    QSettings   backups_;

};

#endif // DIRECTORY_HPP
