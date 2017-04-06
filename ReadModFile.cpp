#include "ReadModFile.h"
#include <QtXml/QDomDocument>
#include <QFile>
#include <QMessageBox>
#include <QString>


ReadModFile::ReadModFile(Ui::MainWindow *ui, QString modFileName)
{
    modFileName = modFileName + "/ModFile.xml";

    QFile file(modFileName);

    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
       QMessageBox::critical(ui->centralWidget,"File Not Found",
                             QString("File: %1 not found!").arg(modFileName));
       return;
    }

    QString errorMsg;
    int errorLine, errorColumn;
    QDomDocument doc;

    if(!doc.setContent(&file,&errorMsg,&errorLine,&errorColumn))
    {

        QMessageBox::critical(ui->centralWidget,"Error Mod File",
                             QString("Error Message: %1\nLine: %2\nColumn: %3").arg(
                             errorMsg,errorLine,errorColumn));

    }

    //Begin Reading and Setting UI values.
    QDomElement rootNode = doc.firstChildElement("ModFile");
    if(rootNode.isNull())
    {
        QMessageBox::critical(ui->centralWidget,"Error","Mod file is malformed");
        return;
    }

    QDomElement tmpElement = rootNode.firstChildElement("Author");
    if(!tmpElement.isNull())
    {
       ui->label_Mod_Author->setText(tmpElement.text());

    }
    else
    {
      ui->label_Mod_Author->setText("");
    }

    //Color Depth
    tmpElement = rootNode.firstChildElement("Comments");
    if(!tmpElement.isNull())
    {
       ui->textBrowser_Mod_Description->setText(tmpElement.text());

    }
    else
    {
      ui->textBrowser_Mod_Description->setText("");
    }

    //Supported Maps
    QStringList listOfMaps;

    tmpElement = rootNode.firstChildElement("SupportedMaps");
    if(!tmpElement.isNull())
    {
        QDomElement subElement = tmpElement.firstChildElement("Map");

        while(!subElement.isNull())
        {
            listOfMaps.push_back(subElement.text());
            subElement = subElement.nextSiblingElement("Map");
        }
    }

    ui->listWidget_Mod_Maps->clear();
    ui->listWidget_Mod_Maps->addItems(listOfMaps);



    //List of features
    QVector<QPair<QString,QString> > features;
    tmpElement = rootNode.firstChildElement("NameList");
    if(!tmpElement.isNull())
    {
        if(tmpElement.attributeNode("Override").value() == "1")
        {
            features.push_back(QPair<QString,QString>("Names", "All"));
        }
    }

    tmpElement = rootNode.firstChildElement("Newspaper");
    if(!tmpElement.isNull())
    {
        if(tmpElement.attributeNode("Override").value() == "1")
        {
            features.push_back(QPair<QString,QString>("Newspaper Articles", "All"));
        }
    }

    tmpElement = rootNode.firstChildElement("Reviews");
    if(!tmpElement.isNull())
    {
        if(tmpElement.attributeNode("Override").value() == "1")
        {
            features.push_back(QPair<QString,QString>("Reviews", "All"));
        }
    }

    tmpElement = rootNode.firstChildElement("Components");
    if(!tmpElement.isNull())
    {
        if(tmpElement.attributeNode("Override").value() == "1")
        {
            features.push_back(QPair<QString,QString>("Components", "All"));
        }
    }


    tmpElement = rootNode.firstChildElement("ComponentsPop");
    if(!tmpElement.isNull())
    {
        if(tmpElement.attributeNode("Override").value() == "1")
        {
            features.push_back(QPair<QString,QString>("Components Starting Popularities", "All"));
        }
    }

    tmpElement = rootNode.firstChildElement("RacingSeries");
    if(!tmpElement.isNull())
    {
        if(tmpElement.attributeNode("Override").value() == "1")
        {
            features.push_back(QPair<QString,QString>("Racing Series", "All"));
        }
    }

    tmpElement = rootNode.firstChildElement("ContractInfo");
    if(!tmpElement.isNull())
    {
        if(tmpElement.attributeNode("Override").value() == "1")
        {
            features.push_back(QPair<QString,QString>("Contract Info", "All"));
        }
    }

    tmpElement = rootNode.firstChildElement("AIModFiles");
    if(!tmpElement.isNull())
    {
        QDomElement subElement = tmpElement.firstChildElement("AI");

        while(!subElement.isNull())
        {
             features.push_back(QPair<QString,QString>("AI",
                 subElement.attributeNode("Map").value()));

            subElement = subElement.nextSiblingElement("AI");
        }
    }

    tmpElement = rootNode.firstChildElement("CityModFiles");
    if(!tmpElement.isNull())
    {
        QDomElement subElement = tmpElement.firstChildElement("CityFile");

        while(!subElement.isNull())
        {
             features.push_back(QPair<QString,QString>("CityFile",
                 subElement.attributeNode("Map").value()));

            subElement = subElement.nextSiblingElement("CityFile");
        }
    }

    tmpElement = rootNode.firstChildElement("TurnEventsModFiles");
    if(!tmpElement.isNull())
    {
        QDomElement subElement = tmpElement.firstChildElement("TurnFile");

        while(!subElement.isNull())
        {
             features.push_back(QPair<QString,QString>("TurnFile",
                 subElement.attributeNode("Map").value()));

            subElement = subElement.nextSiblingElement("TurnFile");
        }
    }

    ui->tableWidget_Mod_Features->clearContents();
    ui->tableWidget_Mod_Features->setRowCount(features.size());
    int i = 0;
    for(QVector<QPair<QString,QString> >::Iterator it = features.begin(); it != features.end(); ++it)
    {
        ui->tableWidget_Mod_Features->setItem(i,0,new QTableWidgetItem((*it).first));
        ui->tableWidget_Mod_Features->setItem(i,1,new QTableWidgetItem((*it).second));
        i++;
    }

    file.close();

}
