#ifndef CLEARCACHE_H
#define CLEARCACHE_H

#include "QDir"
#include "QString"
#include "OSXHelper.h"
#include "ResetToDefault.h"
#include "SavesSettings.h"
#include "QMessageBox"

class ClearCache
{
public:
    ClearCache(Ui::MainWindow *mwUI);
    void clear();
private:
    void deleteCacheFiles(QString folderPath);
    void deleteSaveGames(QString folderPath);
    void deleteReports(QString folderPath);
    void deleteAllFiles(QString folderPath);
    void deleteCustomGUI(QString folderPath);

    Ui::MainWindow *ui;
};

#endif // CLEARCACHE_H
