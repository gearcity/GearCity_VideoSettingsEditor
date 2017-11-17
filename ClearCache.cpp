#include "ClearCache.h"

ClearCache::ClearCache(Ui::MainWindow *mwUI)
{
    ui = mwUI;
    ui->ProgressBar->show();
    ui->ProgressBar->setMinimum(0);
    ui->ProgressBar->setMaximum(5);
}

void ClearCache::clear()
{

    ResetToDefault(ui);
    ui->ProgressBar->setValue(ui->ProgressBar->value()+1);
    #if defined(Q_WS_WIN)
     deleteCacheFiles("../GearCity/Cache/");
     ui->ProgressBar->setValue(ui->ProgressBar->value()+1);

     deleteSaveGames("../GearCity/SaveGames/");
     ui->ProgressBar->setValue(ui->ProgressBar->value()+1);

     deleteReports("../GearCity/Sites/");
     ui->ProgressBar->setValue(ui->ProgressBar->value()+1);

     deleteCustomGUI("../media/GuiSheets/");
     ui->ProgressBar->setValue(ui->ProgressBar->value()+1);

     deleteAllFiles("../GearCity/Settings/");
     ui->ProgressBar->setValue(ui->ProgressBar->value()+1);

     SaveSettings(ui,"../GearCity/Settings/");
     ui->ProgressBar->setValue(ui->ProgressBar->value()+1);

    #elif defined(Q_WS_X11)
     deleteCacheFiles("../GearCity/Cache/");
     ui->ProgressBar->setValue(ui->ProgressBar->value()+1);

     deleteSaveGames("../GearCity/SaveGames/");
     ui->ProgressBar->setValue(ui->ProgressBar->value()+1);

     deleteReports("../GearCity/Sites/");
     ui->ProgressBar->setValue(ui->ProgressBar->value()+1);

     deleteCustomGUI("../media/GuiSheets/");
     ui->ProgressBar->setValue(ui->ProgressBar->value()+1);

     deleteAllFiles("../GearCity/Settings/");
     ui->ProgressBar->setValue(ui->ProgressBar->value()+1);

     SaveSettings(ui,"../GearCity/Settings/");
     ui->ProgressBar->setValue(ui->ProgressBar->value()+1);

    #elif defined(Q_WS_MACX)
     deleteCacheFiles(OSXHelper::getMacPath(0) + "/GearCity/Cache/");
     ui->ProgressBar->setValue(ui->ProgressBar->value()+1);

     deleteSaveGames(OSXHelper::getMacPath(0) + "/GearCity/SaveGames/");
     ui->ProgressBar->setValue(ui->ProgressBar->value()+1);

     deleteReports(OSXHelper::getMacPath(0) + "/GearCity/Sites/");
     ui->ProgressBar->setValue(ui->ProgressBar->value()+1);

     deleteCustomGUI(OSXHelper::getMacPath(0) + "/media/GuiSheets/");
     ui->ProgressBar->setValue(ui->ProgressBar->value()+1);

     deleteAllFiles(OSXHelper::getMacPath(0) + "/GearCity/Settings/");
     ui->ProgressBar->setValue(ui->ProgressBar->value()+1);

     SaveSettings(ui,OSXHelper::getMacPath(0) + "/GearCity/Settings/");
     ui->ProgressBar->setValue(ui->ProgressBar->value()+1);

    #endif

     QMessageBox::information(ui->centralWidget,"Done",
                              "All game caches have been successfully cleared.\n\n"
                              "If you are using the Steam version of the game, you may need to "
                              "verify your game cache and/or uninstall and reinstall the game. "
                              "If you are running this to free up space because Steam does not "
                              "remove save files at uninstall. You may uninstall the game now.",
                              QMessageBox::Ok);



}

void ClearCache::deleteCacheFiles(QString folderPath)
{
    QDir dir(folderPath);
    dir.setNameFilters(QStringList() << "*.*");
    dir.setFilter(QDir::Files);
    foreach(QString dirFile, dir.entryList())
    {
        dir.remove(dirFile);
    }


}

void ClearCache::deleteSaveGames(QString folderPath)
{
    QDir dir(folderPath);
    dir.setNameFilters(QStringList() << "*.db");
    dir.setFilter(QDir::Files);
    foreach(QString dirFile, dir.entryList())
    {
        dir.remove(dirFile);
    }
}


void ClearCache::deleteReports(QString folderPath)
{
    QDir dir(folderPath);
    //dir.setNameFilters(QStringList() << "*");
    dir.setFilter(QDir::Dirs);
    QMessageBox::warning(ui->centralWidget,"This May Take A While",
                         "We will be cleaning up any reports. This may take a few minutes to "
                         "complete depending on how many save games you have.");
    ui->progressBarReports->show();
    ui->progressBarReports->setMinimum(0);
    ui->progressBarReports->setMaximum(dir.entryList().size());
    foreach(QString dirFile, dir.entryList())
    {
        if(dirFile != "." && dirFile != ".." &&
                dirFile != "_charts")
        {
            deleteAllFiles(dir.absoluteFilePath(dirFile));
            QDir().rmdir(dir.absoluteFilePath(dirFile));

        }
        ui->progressBarReports->setValue(ui->progressBarReports->value()+1);
    }
    ui->progressBarReports->hide();
}

void ClearCache::deleteAllFiles(QString folderPath)
{
    QDir dir(folderPath);
    dir.setNameFilters(QStringList() << "*.*");
    dir.setFilter(QDir::Files);
    foreach(QString dirFile, dir.entryList())
    {
        dir.remove(dirFile);
    }
}

void ClearCache::deleteCustomGUI(QString folderPath)
{
    QDir dir(folderPath);
    dir.setNameFilters(QStringList() << "Custom_*.*");
    dir.setFilter(QDir::Files);
    foreach(QString dirFile, dir.entryList())
    {
        dir.remove(dirFile);
    }
}
