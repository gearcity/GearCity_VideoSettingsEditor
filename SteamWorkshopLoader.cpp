#include "SteamWorkshopLoader.h"
#include "QVector"
#include "QString"
#include <QXmlStreamWriter>
#include <QtXml/QDomDocument>
#include <QDomNode>
#include <QDateTime>
#include <QFileDialog>

#if defined(Q_WS_WIN)
    #include "stdlib.h"
#else
   #include <unistd.h>
#endif


#include "qmessagebox.h"

#if defined(Q_WS_MACX)
#include "OSXHelper.h"
#endif

SteamWorkshopLoader::SteamWorkshopLoader(Ui::MainWindow *ui, bool download)
{
    if(SteamAPI_Init())
    {
        uicpy = ui;
        loadAllSubscribedWorkshopItems();
        changes = false;
    }
}

void SteamWorkshopLoader::loadAllSubscribedWorkshopItems()
{

       loadDeployedItemsList();

       int numberOfSubbedItems = SteamUGC()->GetNumSubscribedItems();
        QVector<PublishedFileId_t> itemsVector(numberOfSubbedItems);
        PublishedFileId_t *itemsArray = itemsVector.data();

        SteamUGC()->GetSubscribedItems( itemsArray, numberOfSubbedItems );

        uicpy->ProgressBar->setMaximum(numberOfSubbedItems+installedItems.size());

        // load all subscribed workshop items
        for ( int iSubscribedItem=0; iSubscribedItem<numberOfSubbedItems; iSubscribedItem++ )
        {
            waitingForMetaDataResults = true;
            PublishedFileId_t workshopItemID = itemsArray[iSubscribedItem];
            loadWorkshopItem( workshopItemID );
            uicpy->ProgressBar->setValue(uicpy->ProgressBar->value()+1);
        }

        for(QMap<unsigned long long,playerInstalledItem >::Iterator it = installedItems.begin();
            it != installedItems.end(); ++it)
        {
            if(it.value().parsed == 0)
            {
                possibleDeleteItem(it.key(), it.value());
                uicpy->ProgressBar->setValue(uicpy->ProgressBar->value()+1);
            }
        }

        saveDeployedItemsList();

}

void SteamWorkshopLoader::possibleDeleteItem(PublishedFileId_t itemID, playerInstalledItem pii)
{
    uint32 itemState = SteamUGC()->GetItemState( itemID );

    if(itemState == 0)
    {
       QMessageBox::StandardButton deleteReply = QMessageBox::question(uicpy->ProgressBar,
                                                                       "Should We Delete This Item?",
                                                                       "It appears that you are no longer "
                                                                       "subscribed to the workshop "
                                                                       "Item "+pii.Name+ ".\n\n"
                                                                       "Would you like to delete this item "
                                                                       "from the game?",QMessageBox::Yes|QMessageBox::No);

       if(deleteReply == QMessageBox::Yes)
       {
           if(pii.both == 0)
             removeDir(pii.locationRel);
           else
           {
               foreach(QString path, pii.pathList)
               {
                   removeDir(path);
               }
           }

           installedItems.remove(itemID);

       }

    }

}

//Recursively Remove Directory...
//Because I am too lazy to write it my self, or copy it from my other programs:
//MIT John Schember
//https://nachtimwald.com/2010/06/08/qt-remove-directory-and-its-contents/
bool SteamWorkshopLoader::removeDir(const QString & dirName)
{
    bool result = true;
    QDir dir(dirName);

    if (dir.exists())
    {
        Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System |
                                                    QDir::Hidden  | QDir::AllDirs |
                                                    QDir::Files, QDir::DirsFirst))
        {
            if (info.isDir())
            {
                result = removeDir(info.absoluteFilePath());
            }
            else
            {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result)
            {
                return result;
            }
        }
        result = QDir().rmdir(dirName);
    }

    return result;
}

void SteamWorkshopLoader::loadWorkshopItem(PublishedFileId_t itemID)
{

    uint32 itemState = SteamUGC()->GetItemState( itemID );

   /* QMessageBox::critical(uicpy->ProgressBar,"Error","ItemID: "+QString::number(itemID)+
                          "\nplayer itemState: "+QString::number(itemState));
*/

    if(itemState != 5 && itemState != 4)
    {
        return;
    }



    uint32 downloadTime = 0;
    uint64 itemSize = 0;
    char itemSteamFilePath[1024] = { 0 };


    if ( !SteamUGC()->GetItemInstallInfo( itemID, &itemSize, itemSteamFilePath, sizeof(itemSteamFilePath), &downloadTime ) )
    {
        return;
    }
     QMap<unsigned long long, playerInstalledItem>::ConstIterator it = installedItems.find(itemID);

    /* QMessageBox::critical(uicpy->ProgressBar,"Error","Installed: "+QString::number(downloadTime)+
                           "\nplayer Time: "+QString::number(it.value().playerInstalledTime));*/

    if(it == installedItems.end() || it.value().playerInstalledTime < downloadTime)
    {

        if(!changes)
        {

            QMessageBox::critical(uicpy->ProgressBar,"Steamworkshop Items Update","We've noticed that you have new or updated "
                                  "Steam Workshop Items. We will now process these and add them to the game. Don't forget to "
                                  "set the mod you wish to play in the Modifications tab!");

            changes = true;
        }

        itemLocation = itemSteamFilePath;

        // get Workshop item details
        itemInfoHandle = SteamUGC()->CreateQueryUGCDetailsRequest( &itemID, 1 );
        SteamUGC()->SetReturnMetadata(itemInfoHandle, true);
        SteamAPICall_t hSteamAPICall = SteamUGC()->SendQueryUGCRequest(itemInfoHandle);

        cb_swItemMetaData.Set(hSteamAPICall,this,&SteamWorkshopLoader::fetchMetadataAndContinue);

        int count = 0;
        while(waitingForMetaDataResults && count < 120)
        {

            QApplication::processEvents();
            SteamAPI_RunCallbacks();

#if defined(Q_WS_WIN)
            {
                _sleep(1000);
            }
#else
            usleep(100000);
#endif
            count++;
        }

        if(count >= 120)
        {
            QMessageBox::critical(uicpy->ProgressBar,"Error",
                                          "Connection to Steam Timed Out");
        }
    }
    else
    {
        itemLocation = "";
    }



}

void SteamWorkshopLoader::loadDeployedItemsList()
{
#if defined(Q_WS_WIN)
 QString openFileName = "../GearCity/Settings/WorkshopItems.xml" ;
#elif defined(Q_WS_X11)
 QString openFileName = "../GearCity/Settings/WorkshopItems.xml";
#elif defined(Q_WS_MACX)
 QString openFileName = OSXHelper::getMacPath(0) + "/GearCity/Settings/WorkshopItems.xml" ;
#endif

 installedItems.clear();

 if (openFileName != "")
 {

     //Open File
     QFile file(openFileName);
     if(!file.open(QFile::ReadOnly | QFile::Text))
     {

        /*QMessageBox::critical(uicpy->ProgressBar,"File Error",
                              QString("Whopse, opening %1 failed.").arg(openFileName));*/
        return;
     }

     //Process XML File.
     QString errorMsg;
     int errorLine, errorColumn;
     QDomDocument doc;
     QDomElement Element;
     QDomElement childElement;


     if(!doc.setContent(&file,&errorMsg,&errorLine,&errorColumn))
     {
        QMessageBox::critical(uicpy->ProgressBar,"Error",
                             QString("Error Message: %1\nLine: %2\nColumn: %3").arg(
                             errorMsg,errorLine,errorColumn));
         return;
     }

     //Begin reading the data and fill the map.
     QDomElement rootNode = doc.firstChildElement("WorkshopItems");
     if(rootNode.isNull())
     {
         QMessageBox::critical(uicpy->ProgressBar,"Error","Workshop Item List is malformed.");
         return;
     }

     Element = rootNode.firstChildElement("Item");
     if(!Element.isNull())
     {

         do
         {
           playerInstalledItem pii;
           pii.playerInstalledTime = Element.attributeNode("time").value().toUInt();
           pii.locationRel = Element.attributeNode("path").value();
           pii.both = 0;
           pii.parsed = 0;
           pii.Name = Element.attributeNode("name").value();
           installedItems.insert(Element.text().toULongLong(),pii);

           Element = Element.nextSiblingElement("Item");
         }
         while(!Element.isNull());
     }


     Element = rootNode.firstChildElement("Both");
    if(!Element.isNull())
    {
         do
         {
           playerInstalledItem pii;
           pii.playerInstalledTime = Element.attributeNode("time").value().toUInt();
           pii.locationRel = Element.attributeNode("path").value();
           pii.both = 1;
           pii.parsed = 0;
           pii.Name = Element.attributeNode("name").value();
           childElement = Element.firstChildElement("Child");

           if(!childElement.isNull())
           {
               do
               {
                   pii.pathList.push_back(childElement.text());

                   childElement = childElement.nextSiblingElement("Child");
               }
               while(!childElement.isNull());
           }


           installedItems.insert(Element.attributeNode("item").value().toULongLong(),pii);

           Element = Element.nextSiblingElement("Both");
         }
         while(!Element.isNull());
    }

     file.close();
 }


}



void SteamWorkshopLoader::saveDeployedItemsList()
{
#if defined(Q_WS_WIN)
 QString saveFileName = "../GearCity/Settings/WorkshopItems.xml" ;
#elif defined(Q_WS_X11)
 QString saveFileName = "../GearCity/Settings/WorkshopItems.xml" ;
#elif defined(Q_WS_MACX)
 QString saveFileName = OSXHelper::getMacPath(0) + "/GearCity/Settings/WorkshopItems.xml" ;
#endif


 if (saveFileName != "")
 {
     if (!saveFileName.endsWith(".xml"))
         saveFileName += ".xml";

         //Create/overwrite workshop manifest
         QFile saveFile(saveFileName);

         if(!saveFile.open(QFile::WriteOnly | QFile::Text))
         {
            QMessageBox::critical(uicpy->ProgressBar,"Error",
                                  QString("Can not write to %1\n%2").arg(saveFileName,
                                  saveFile.errorString()));
            return;
         }

         //Lets Start Writing!
         QXmlStreamWriter xmlWriter(&saveFile);
         xmlWriter.setAutoFormatting(true);
         xmlWriter.writeStartDocument();

         xmlWriter.writeStartElement("WorkshopItems");

         for(QMap<unsigned long long, playerInstalledItem>::iterator it = installedItems.begin();
             it != installedItems.end(); ++it)
         {
            if(it.value().both == 0)
            {
                 xmlWriter.writeStartElement("Item");
                     xmlWriter.writeAttribute("time",QString::number(it.value().playerInstalledTime) );
                     xmlWriter.writeAttribute("path",it.value().locationRel);
                     xmlWriter.writeAttribute("name",it.value().Name);
                     xmlWriter.writeCharacters(QString::number(it.key()));
                 xmlWriter.writeEndElement(); //Item
            }
            else if(it.value().both == 1)
            {
                xmlWriter.writeStartElement("Both");
                    xmlWriter.writeAttribute("time",QString::number(it.value().playerInstalledTime) );
                    xmlWriter.writeAttribute("path",it.value().locationRel);
                    xmlWriter.writeAttribute("name",it.value().Name);
                    xmlWriter.writeAttribute("item",QString::number(it.key()));
                    foreach(QString childPath, it.value().pathList)
                    {
                      xmlWriter.writeStartElement("Child");
                        xmlWriter.writeCharacters(childPath);
                      xmlWriter.writeEndElement(); //Child
                    }

                xmlWriter.writeEndElement(); //Both
            }
        }



           xmlWriter.writeEndElement(); //WorkshopItems



         //close the final tag and close the file.
          xmlWriter.writeEndDocument();

          saveFile.close();

          //We Did it! Yay!
          if(changes)
          {
            QMessageBox::information(uicpy->ProgressBar,"Item Installation Complete",
                                   "Item Installation Complete! Be sure to select the mod"
                                     " you want to play in the Mods tab.", QMessageBox::Ok);
          }
 }

}


void SteamWorkshopLoader::fetchMetadataAndContinue(SteamUGCQueryCompleted_t* callBack, bool failure)
{

    char metadata[k_cchDeveloperMetadataMax];
    quint32 metadataSize;


    SteamUGCDetails_t data;
    if(SteamUGC()->GetQueryUGCResult(itemInfoHandle,0, &data))
    {

        if(SteamUGC()->GetQueryUGCMetadata(itemInfoHandle,0,metadata,k_cchDeveloperMetadataMax))
            {

                QString mString(metadata);

                if(mString == "both")
                    moveBothAndRename(data);
                else if(mString == "map")
                    moveMapAndRename(data,false);
                else if(mString == "mod")
                 {
                    moveModAndRename(data,false);
                }

            }
            else
            {
                  QMessageBox::critical(uicpy->ProgressBar,"Error",QString(data.m_rgchTitle) + " is missing Metadata!");
            }
    }

    waitingForMetaDataResults = false;
    SteamUGC()->ReleaseQueryUGCRequest(itemInfoHandle);


}

void SteamWorkshopLoader::moveBothAndRename(SteamUGCDetails_t data)
{
    QString sourcePath = itemLocation;

    playerInstalledItem pii;
    pii.playerInstalledTime = QDateTime::currentDateTime().toTime_t();
    pii.locationRel = "";
    pii.both = 1;
    pii.parsed = 1;
    pii.Name = data.m_rgchTitle;
    installedItems[data.m_nPublishedFileId] = pii;

    //Get our folder
    QDir sourceFolder(sourcePath);

    //Get Folders in our main folder
    QStringList folderNames = sourceFolder.entryList(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    foreach(QString folderName, folderNames)
    {
        QFileInfo fi(sourcePath + "/" + folderName+"/ModFile.xml");

        if(fi.exists())
        {
            itemLocation = sourcePath + "/" + folderName;
            moveModAndRename(data,true);
        }
        else
        {
            QFileInfo fiMap(sourcePath + "/" + folderName+"/MapFileForGame.xml");
            if(fiMap.exists())
            {
                itemLocation = sourcePath + "/" + folderName;
                moveMapAndRename(data,true);
            }
        }

    }

    itemLocation = sourcePath;


    waitingForMetaDataResults = false;

}


void SteamWorkshopLoader::moveMapAndRename(SteamUGCDetails_t data, bool both)
{

    #if defined(Q_WS_WIN)
     QString targetPath = "../media/Maps/" ;
    #elif defined(Q_WS_X11)
     QString targetPath = "../media/Maps/";
    #elif defined(Q_WS_MACX)
     QString targetPath = OSXHelper::getMacPath(0) + "/media/Maps/" ;
    #endif

     QString sourcePath = itemLocation;

     QString mapName = readMapFileForName(sourcePath+"/MapFileForGame.xml");
     if(mapName != "")
         targetPath += mapName +"/";
     else
         return;



     QDir().mkdir(targetPath);


    //Get our folder
        QDir sourceFolder(sourcePath);

    //Get Folders in our main folder
        QStringList folderNames = sourceFolder.entryList(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
        foreach(QString folderName, folderNames)
        {
            QDir().mkdir(targetPath+folderName);
            recursiveFileCopier(sourcePath+"/"+folderName, targetPath+folderName);
        }


    //Get list of files in folder
        QStringList fileNames = sourceFolder.entryList(QDir::Files | QDir::Dirs |
                                                    QDir::NoDotAndDotDot | QDir::Hidden |
                                                    QDir::System);
       //Go through the files in the folder
        foreach(QString fileName, fileNames)
        {
            //Make the paths
            QString sourceFile = sourcePath + "/" + fileName;
            QString targetFile = targetPath + "/" + fileName;

            //copy the files.
            QFile::copy(sourceFile, targetFile);
        }


    waitingForMetaDataResults = false;
    if(!both)
    {
        playerInstalledItem pii;
        pii.playerInstalledTime = QDateTime::currentDateTime().toTime_t();
        pii.locationRel = targetPath;
        pii.both = 0;
        pii.parsed = 1;
        pii.Name = data.m_rgchTitle;
        installedItems[data.m_nPublishedFileId] = pii;
    }
    else
    {
        installedItems[data.m_nPublishedFileId].pathList.push_back(targetPath);
    }
}


void SteamWorkshopLoader::moveModAndRename(SteamUGCDetails_t data, bool both)
{

#if defined(Q_WS_WIN)
 QString targetPath = "../media/Mods/" ;
#elif defined(Q_WS_X11)
 QString targetPath = "../media/Mods/";
#elif defined(Q_WS_MACX)
 QString targetPath = OSXHelper::getMacPath(0) + "/media/Mods/" ;
#endif

 QString sourcePath = itemLocation;
/*
#if defined(Q_WS_MACX)
 sourcePath += "/"+ QString(data.m_rgchTitle);
#endif*/


 QString modName = readModFileForName(sourcePath+"/ModFile.xml");
 if(modName != "")
     targetPath += modName+"/";
 else
     return;


 QDir().mkdir(targetPath);



//Get our folder
    QDir sourceFolder(sourcePath);

//Get Folders in our main folder
    QStringList folderNames = sourceFolder.entryList(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    foreach(QString folderName, folderNames)
    {
        QDir().mkdir(targetPath+folderName);
        recursiveFileCopier(sourcePath+"/"+folderName, targetPath+folderName);
    }


//Get list of files in folder
    QStringList fileNames = sourceFolder.entryList(QDir::Files | QDir::Dirs |
                                                QDir::NoDotAndDotDot | QDir::Hidden |
                                                QDir::System);
   //Go through the files in the folder
    foreach(QString fileName, fileNames)
    {
        //Make the paths
        QString sourceFile = sourcePath + "/" + fileName;
        QString targetFile = targetPath + "/" + fileName;


        //copy the files.
        QFile::copy(sourceFile, targetFile);
    }


    waitingForMetaDataResults = false;

    if(!both)
    {
        playerInstalledItem pii;
        pii.playerInstalledTime = QDateTime::currentDateTime().toTime_t();
        pii.locationRel = targetPath;
        pii.both = 0;
        pii.parsed = 1;
        pii.Name = data.m_rgchTitle;
        installedItems[data.m_nPublishedFileId] = pii;
    }
    else
    {
        installedItems[data.m_nPublishedFileId].pathList.push_back(targetPath);
    }
}

void SteamWorkshopLoader::recursiveFileCopier(QString sourcePath, QString targetPath)
{
    //Get our folder
        QDir sourceFolder(sourcePath);

    //Get Folders in our main folder
        QStringList folderNames = sourceFolder.entryList(QDir::AllDirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);
        foreach(QString folderName, folderNames)
        {
            QDir().mkdir(targetPath+"/"+folderName);
            recursiveFileCopier(sourcePath+"/"+folderName, targetPath+folderName);
        }


    //Get list of files in folder
        QStringList fileNames = sourceFolder.entryList(QDir::Files | QDir::Dirs |
                                                    QDir::NoDotAndDotDot | QDir::Hidden |
                                                    QDir::System);
       //Go through the files in the folder
        foreach(QString fileName, fileNames)
        {
            //Make the paths
            QString sourceFile = sourcePath + "/" + fileName;
            QString targetFile = targetPath + "/" + fileName;


            //copy the files.
            QFile::copy(sourceFile, targetFile);
        }

}


QString SteamWorkshopLoader::readMapFileForName(QString fileName)
{
    QString mapName = "";

    //Open File
    QFile file(fileName);
    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
       return "";
    }

    //Process XML File.
    QString errorMsg;
    int errorLine, errorColumn;
    QDomDocument doc;
    QDomElement Element;


    if(!doc.setContent(&file,&errorMsg,&errorLine,&errorColumn))
    {
       QMessageBox::critical(uicpy->ProgressBar,"Error",
                            QString("Error Message: %1\nLine: %2\nColumn: %3").arg(
                            errorMsg,errorLine,errorColumn));
       return "";
    }

    //Begin reading the data and fill the map.
    QDomElement rootNode = doc.firstChildElement("MapInfo");
    if(rootNode.isNull())
    {
        QMessageBox::critical(uicpy->ProgressBar,"Error","MapFile is malformed.");
        return "";
    }

    Element = rootNode.firstChildElement("MapName");
    if(!Element.isNull())
     mapName = Element.text();

    file.close();

    return mapName;

}



QString SteamWorkshopLoader::readModFileForName(QString fileName)
{
    QString modName = "";

    //Open File
    QFile file(fileName);
    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        //QMessageBox::critical(uicpy->ProgressBar,"Error","Can't open");
       return "";
    }

    //Process XML File.
    QString errorMsg;
    int errorLine, errorColumn;
    QDomDocument doc;
    QDomElement Element;


    if(!doc.setContent(&file,&errorMsg,&errorLine,&errorColumn))
    {
       QMessageBox::critical(uicpy->ProgressBar,"Error",
                            QString("Error Message: %1\nLine: %2\nColumn: %3").arg(
                            errorMsg,errorLine,errorColumn));
       return "";
    }

    //Begin reading the data and fill the map.
    QDomElement rootNode = doc.firstChildElement("ModFile");
    if(rootNode.isNull())
    {
        QMessageBox::critical(uicpy->ProgressBar,"Error","ModFile is malformed.");
        return "";
    }

    Element = rootNode.firstChildElement("ModName");
    if(!Element.isNull())
     modName = Element.text();

    file.close();

    return modName;

}
