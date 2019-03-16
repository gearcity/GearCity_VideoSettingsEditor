#ifndef STEAMWORKSHOPLOADER_H
#define STEAMWORKSHOPLOADER_H

#include "steam/steam_api.h"
#include "steam/isteamugc.h"
#include "QMap"
#include "ui_MainWindow.h"


class SteamWorkshopLoader
{
public:
    SteamWorkshopLoader(Ui::MainWindow *ui, bool download);


private:
    struct playerInstalledItem
    {
        quint32 playerInstalledTime;
        QString locationRel;
        int both;
        QStringList pathList;
        int parsed;
        QString Name;
    };

    QMap<unsigned long long,playerInstalledItem > installedItems;
    Ui::MainWindow *uicpy;
    UGCQueryHandle_t itemInfoHandle;
    CCallResult<SteamWorkshopLoader, SteamUGCQueryCompleted_t> cb_swItemMetaData;
    bool waitingForMetaDataResults;
    QString itemLocation;
    PublishedFileId_t currentItemID;
    bool changes;


    void loadAllSubscribedWorkshopItems();
    void loadWorkshopItem(PublishedFileId_t itemID);
    void loadDeployedItemsList();
    void saveDeployedItemsList();
    void fetchMetadataAndContinue(SteamUGCQueryCompleted_t* callBack, bool failure);
    void moveBothAndRename(SteamUGCDetails_t data);
    void moveMapAndRename(SteamUGCDetails_t data, bool both);
    void moveModAndRename(SteamUGCDetails_t data, bool both);

    QString readMapFileForName(QString fileName);
    QString readModFileForName(QString fileName);
    void recursiveFileCopier(QString sourcePath, QString targetPath);
    bool removeDir(const QString & dirName);


    void possibleDeleteItem(PublishedFileId_t itemID, playerInstalledItem pii);

};

#endif // STEAMWORKSHOPLOADER_H
