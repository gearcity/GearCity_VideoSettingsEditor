/*Copyright 2016 Visual Entertainment And Technologies, LLC. All Rights Reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

- Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

- Neither the name of Visual Entertainment And Technologies, LLC
nor the names of its contributors may be used to endorse or promote products
derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.")*/


/* *
 * CODING GUIDELINES...
 * Allman Indent Style!
 * 100 +/- Column Width
 * Avoid Namespaces
 * Lower Camel Case for variables and functions.
 * Upper Camel Case for everything else. (UI Widgets, Class Names)
 * Don't shoot me if I forget these things as well...
 * */



#include "SettingsImporter.h"
#include <QtXml/QDomDocument>
#include <QFile>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QString>
#include "ResetToDefault.h"
#include <QDir>

SettingsImporter::SettingsImporter(Ui::MainWindow *ui, QString settingsFolderName
                                   , QString modFolderPath )
{
    //Set settings file names, it's different for unix's than windows.
    QString videoFileName = "",volumeFileName=settingsFolderName+"Volume.xml";

    #ifdef Q_WS_WIN
        videoFileName = settingsFolderName + "VideoConfig.xml";
    #else
        videoFileName = settingsFolderName + "LinuxVideoConfig.xml";
    #endif

    //Parse Mod Folder And Put Mod Names into mod combo box...
        QDir dir(modFolderPath);
        dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
        QStringList fileList = dir.entryList();

        //Default mod files that are not in the folder
        fileList.push_front("Default");

        ui->comboBox_Mod_AvaliableMods->clear();
        ui->comboBox_Mod_AvaliableMods->addItems(fileList);



    //Import the Settings Files into the GUI
    import(ui,videoFileName,volumeFileName,true);


}

/*
 * This function imports the settings file from the given video and volume file strings.
 * if "start" boolan is true, the program will reset GUI settings to default if the files
 * are not found or there is an error processing the data.
 */
void SettingsImporter::import(Ui::MainWindow *ui, QString videoFile, QString volumeFile, bool start)
{
    QStringList fileData;

    //Check Base Video File and Reset if not found and start is flagged.
    QFile baseVideoFile(videoFile);
    if(!baseVideoFile.open(QFile::ReadOnly | QFile::Text))
    {
       QMessageBox::critical(ui->centralWidget,"File Not Found",
                             QString("File: %1 not found!").arg(videoFile));

       if(start)
           //resetToDefault
           ResetToDefault(ui);
       return;
    }

     //Check Volume File and Reset if not found and start is flagged.
    QFile baseVolumeFile(volumeFile);
    if(!baseVolumeFile.open(QFile::ReadOnly | QFile::Text))
    {
       QMessageBox::critical(ui->centralWidget,"File Not Found",
                             QString("File: %1 not found!").arg(volumeFile));

       if(start)
           //resetToDefault
           ResetToDefault(ui);
       return;
    }



    //Open up baseVideoFile and start parsing, if failed to parse, reset if start is flagged.
    QString errorMsg;
    int errorLine, errorColumn;
    QDomDocument doc;

    if(!doc.setContent(&baseVideoFile,&errorMsg,&errorLine,&errorColumn))
    {

       QMessageBox::critical(ui->centralWidget,"Error Base Video",
                            QString("Error Message: %1\nLine: %2\nColumn: %3").arg(
                            errorMsg,errorLine,errorColumn));

       if(start)
           //resetToDefault
           ResetToDefault(ui);
       return;
    }

    //Begin Reading and Setting UI values.
    QDomElement rootNode = doc.firstChildElement("Video");
    if(rootNode.isNull())
    {
        QMessageBox::critical(ui->centralWidget,"Error","Settings file is malformed");
        return;
    }

    //Change UI Values in order of the XML document.
    //Render System
    QDomElement tmpElement = rootNode.firstChildElement("RenderSystem");
    if(!tmpElement.isNull())
    {
        if(tmpElement.text() == "D3D9 RenderSystem")
        {
          ui->Render_ComboBox->setCurrentIndex(0);
        }
        else
        {
            #if defined(Q_WS_WIN)
             ui->Render_ComboBox->setCurrentIndex(1);
            #else
             ui->Render_ComboBox->setCurrentIndex(0);
            #endif
        }
    }
    else
    {
     ui->Render_ComboBox->setCurrentIndex(0);
    }

    //Color Depth
    tmpElement = rootNode.firstChildElement("ColorDepth");
    if(!tmpElement.isNull())
    {
      if(tmpElement.text() == "32")
        ui->ColorDepth_ComboBox->setCurrentIndex(0);
      else
        ui->ColorDepth_ComboBox->setCurrentIndex(1);
    }
    else
    {
      ui->ColorDepth_ComboBox->setCurrentIndex(0);
    }

    //FSAA
    tmpElement = rootNode.firstChildElement("FSAA");
    if(!tmpElement.isNull())
    {
      if(tmpElement.text()=="0")
        ui->FSAA_ComboBox->setCurrentIndex(0);
      else if(tmpElement.text()=="2")
        ui->FSAA_ComboBox->setCurrentIndex(1);
      else if(tmpElement.text()=="4")
        ui->FSAA_ComboBox->setCurrentIndex(2);
      else if(tmpElement.text()=="6")
        ui->FSAA_ComboBox->setCurrentIndex(3);
      else if(tmpElement.text()=="8")
        ui->FSAA_ComboBox->setCurrentIndex(4);
    }
    else
    {
       ui->FSAA_ComboBox->setCurrentIndex(0);
    }

    //FPM? Forget it!

    //Full Screen
    tmpElement = rootNode.firstChildElement("Fullscreen");
    if(!tmpElement.isNull())
    {
      if(tmpElement.text()=="Yes")
        ui->FullScreen_CheckBox->setChecked(true);
      else
        ui->FullScreen_CheckBox->setChecked(false);
    }
    else
    {
      ui->FullScreen_CheckBox->setChecked(false);
    }


    //VSYNC
    tmpElement = rootNode.firstChildElement("VSYNC");
    if(!tmpElement.isNull())
    {
      if(tmpElement.text()=="Yes")
        ui->VSYNC_CheckBox->setChecked(true);
      else
        ui->VSYNC_CheckBox->setChecked(false);
    }
    else
    {
        ui->VSYNC_CheckBox->setChecked(true);
    }

    //Video Resolution
    tmpElement = rootNode.firstChildElement("VideoRes");
    if(!tmpElement.isNull())
    {
      QString tmpStr = tmpElement.text();
      QString vidRes = (*tmpStr.split(" @ ").begin());
      QStringList resValsList = vidRes.split(" x ");

      int width=0,height=0;

      if(resValsList.size() == 2) //Should have only Width and Height after split...
      {
        //Loop through list, first should be width, second height...
        for(QStringList::iterator it = resValsList.begin(); it != resValsList.end(); ++it)
        {
          if(width == 0)
          {
            width = (*it).toInt();
          }
          else
          {
            height = (*it).toInt();
          }
        }
       }
      else
      {
          width = 1024, height=768;
      }

      //Set Textbox sizes
      ui->VideoResWidth_TextBox->setText(QString::number(width));
      ui->VideoResHeight_TextBox->setText(QString::number(height));

      //Format text for combo box, then change combo text.
      QString resText = ui->VideoResWidth_TextBox->text() + " x " + ui->VideoResHeight_TextBox->text();

      //Hackish work around to allow text in the ComboBox that is not an Item!
      ui->VideoRes_ComboBox->setEditable(true);
      ui->VideoRes_ComboBox->lineEdit()->setText(resText);
      ui->VideoRes_ComboBox->lineEdit()->setReadOnly(true);

    }
    else
    {
        ui->VideoResWidth_TextBox->setText("1024");
        ui->VideoResHeight_TextBox->setText("768");

        //Format text for combo box, then change combo text.
        QString resText = "1024 x 768";

        //Hackish work around to allow text in the ComboBox that is not an Item!
        ui->VideoRes_ComboBox->setEditable(true);
        ui->VideoRes_ComboBox->lineEdit()->setText(resText);
        ui->VideoRes_ComboBox->lineEdit()->setReadOnly(true);
    }


    //Language
    tmpElement = rootNode.firstChildElement("Language");
    if(!tmpElement.isNull())
    {
        int index = ui->Language_ComboBox->findData(tmpElement.text());
        if(index != -1)
          ui->Language_ComboBox->setCurrentIndex(index);
        else
          ui->Language_ComboBox->setCurrentIndex(0);
    }
    else
    {
        ui->Language_ComboBox->setCurrentIndex(0);
    }




    //Shadows
    tmpElement = rootNode.firstChildElement("Shadows");
    if(!tmpElement.isNull())
    {
        if(tmpElement.text()=="0")
          ui->Shadow_ComboBox->setCurrentIndex(0);
        else if(tmpElement.text()=="1")
          ui->Shadow_ComboBox->setCurrentIndex(1);
        else if(tmpElement.text()=="2")
          ui->Shadow_ComboBox->setCurrentIndex(2);
        else if(tmpElement.text()=="3")
          ui->Shadow_ComboBox->setCurrentIndex(3);
        else if(tmpElement.text()=="4")
          ui->Shadow_ComboBox->setCurrentIndex(4);
    }
    else
    {
       ui->Shadow_ComboBox->setCurrentIndex(0);
    }



    //Shaders
    tmpElement = rootNode.firstChildElement("Shaders");
    if(!tmpElement.isNull())
    {
      if(tmpElement.text()=="true")
        ui->Shaders_CheckBox->setChecked(true);
      else
        ui->Shaders_CheckBox->setChecked(false);

    }
    else
    {
       ui->Shaders_CheckBox->setChecked(true);
    }


    //Compositors
    tmpElement = rootNode.firstChildElement("Compositors");
    if(!tmpElement.isNull())
    {
      if(tmpElement.text() =="true")
        ui->Compositors_CheckBox->setChecked(true);
      else
        ui->Compositors_CheckBox->setChecked(false);

    }
    else
    {
      ui->Compositors_CheckBox->setChecked(false);
    }


    //HardMouse
    tmpElement = rootNode.firstChildElement("HardMouse");
    if(!tmpElement.isNull())
    {
      if(tmpElement.text() == "true")
        ui->HardwareMouse_CheckBox->setChecked(true);
      else
        ui->HardwareMouse_CheckBox->setChecked(false);
    }
    else
    {
        ui->HardwareMouse_CheckBox->setChecked(true);
    }



    //Map Sensitivity
    tmpElement = rootNode.firstChildElement("Map");
    if(!tmpElement.isNull())
    {
      ui->MapSensitivity_SpinBox->setValue(tmpElement.text().toDouble());
    }
    else
    {
       ui->MapSensitivity_SpinBox->setValue(0.5);
    }



    //Mouse Sensitivity
    tmpElement = rootNode.firstChildElement("Mouse");
    if(!tmpElement.isNull())
    {
      ui->MouseSensitivity_SpinBox->setValue(tmpElement.text().toDouble());
    }
    else
    {
      ui->MouseSensitivity_SpinBox->setValue(0.5);
    }


    //Tutorials
    tmpElement = rootNode.firstChildElement("Tutorials");
    if(!tmpElement.isNull())
    {
      if(tmpElement.text() == "true")
        ui->Tutorials_Checkbox->setChecked(true);
      else
        ui->Tutorials_Checkbox->setChecked(false);
    }
    else
    {
        ui->Tutorials_Checkbox->setChecked(true);
    }


    //FontSize
    tmpElement = rootNode.firstChildElement("FontSize");
    if(!tmpElement.isNull())
    {
      ui->FontSize_SpinBox->setValue(tmpElement.text().toInt()/2);
    }
    else
    {
      ui->FontSize_SpinBox->setValue(0);
    }


    //Borderless Windows
    tmpElement = rootNode.firstChildElement("Borderless");
    if(!tmpElement.isNull())
    {
      if(tmpElement.text() == "true")
        ui->BorderlessWindow_CheckBox->setChecked(true);
      else
        ui->BorderlessWindow_CheckBox->setChecked(false);
    }
    else
    {
        ui->BorderlessWindow_CheckBox->setChecked(false);
    }


    //Low Definition Textures
    tmpElement = rootNode.firstChildElement("ld");
    if(!tmpElement.isNull())
    {
      if(tmpElement.text() == "true")
        ui->LowDefTexture_CheckBox->setChecked(true);
      else
        ui->LowDefTexture_CheckBox->setChecked(false);
    }
    else
    {
        ui->LowDefTexture_CheckBox->setChecked(false);
    }



    //AutoSave
    tmpElement = rootNode.firstChildElement("AutoSave");
    if(!tmpElement.isNull())
    {
        if(tmpElement.text()=="0")
          ui->AutoSave_ComboBox->setCurrentIndex(0);
        else if(tmpElement.text()=="1")
          ui->AutoSave_ComboBox->setCurrentIndex(1);
        else if(tmpElement.text()=="2")
          ui->AutoSave_ComboBox->setCurrentIndex(2);
        else if(tmpElement.text()=="3")
          ui->AutoSave_ComboBox->setCurrentIndex(3);

    }
    else
    {
       ui->AutoSave_ComboBox->setCurrentIndex(3);
    }

    //ScaleGUI
    tmpElement = rootNode.firstChildElement("ScaleGUI");
    if(!tmpElement.isNull())
    {
        ui->ScaleGUI_horizontalScrollBar->setValue((tmpElement.text().toDouble()-1)*100);
    }
    else
    {
       ui->ScaleGUI_horizontalScrollBar->setValue(0);
    }


    //PowerOfTwo
    tmpElement = rootNode.firstChildElement("PowerOfTwo");
    if(!tmpElement.isNull())
    {
        if(tmpElement.text()=="true")
         ui->dynamicReports_CheckBox->setChecked(true);
        else
         ui->dynamicReports_CheckBox->setChecked(false);


    }
    else
    {
       ui->dynamicReports_CheckBox->setChecked(false);
    }

    //FrameRateLimit
    tmpElement = rootNode.firstChildElement("FrameRateLimit");
    if(!tmpElement.isNull())
    {
        ui->fpsLimiter_spinnerBox->setValue(tmpElement.text().toInt());
    }
    else
    {
       ui->fpsLimiter_spinnerBox->setValue(60);
    }

    //MonitorIndex
    tmpElement = rootNode.firstChildElement("MonitorIndex");
    if(!tmpElement.isNull())
    {
        ui->MonitorIndex_SpinBox->setValue(tmpElement.text().toInt());
    }
    else
    {
       ui->MonitorIndex_SpinBox->setValue(0);
    }

    //Delete Old Save Reports every few years...
    tmpElement = rootNode.firstChildElement("DeleteOldReports");
    if(!tmpElement.isNull())
    {
      if(tmpElement.text() == "true")
        ui->DeleteOldReports_CheckBox->setChecked(true);
      else
        ui->DeleteOldReports_CheckBox->setChecked(false);
    }
    else
    {
        ui->DeleteOldReports_CheckBox->setChecked(true);
    }


    //Lock Office Seasons Artwork.
    tmpElement = rootNode.firstChildElement("LockSeasons");
    if(!tmpElement.isNull())
    {
      if(tmpElement.text() == "true")
        ui->LockSeasons_CheckBox->setChecked(true);
      else
        ui->LockSeasons_CheckBox->setChecked(false);
    }
    else
    {
        ui->LockSeasons_CheckBox->setChecked(false);
    }



    //Save on Exit
    tmpElement = rootNode.firstChildElement("SaveOnExit");
    if(!tmpElement.isNull())
    {
      if(tmpElement.text() == "true")
        ui->SaveOnExit_CheckBox->setChecked(true);
      else
        ui->SaveOnExit_CheckBox->setChecked(false);
    }
    else
    {
        ui->SaveOnExit_CheckBox->setChecked(true);
    }

    //Disable Hotkeys
    tmpElement = rootNode.firstChildElement("DisableHotkeys");
    if(!tmpElement.isNull())
    {
      if(tmpElement.text() == "true")
        ui->DisableHotkeys_Checkbox->setChecked(true);
      else
        ui->DisableHotkeys_Checkbox->setChecked(false);
    }
    else
    {
        ui->DisableHotkeys_Checkbox->setChecked(false);
    }


    //AutoSaveGUI
    tmpElement = rootNode.firstChildElement("AutoSaveGUI");
    if(!tmpElement.isNull())
    {
      if(tmpElement.text() == "true")
        ui->AutoGUI_Checkbox->setChecked(true);
      else
        ui->AutoGUI_Checkbox->setChecked(false);
    }
    else
    {
        ui->AutoGUI_Checkbox->setChecked(false);
    }


    //Which Mod Is Selected
    tmpElement = rootNode.firstChildElement("Mod");
    if(!tmpElement.isNull())
    {
        ui->comboBox_Mod_AvaliableMods->setCurrentIndex(
                ui->comboBox_Mod_AvaliableMods->findText(tmpElement.text()));

    }
    else
    {
         ui->comboBox_Mod_AvaliableMods->setCurrentIndex(0);
    }

    //Units!
    tmpElement = rootNode.firstChildElement("Units_Fuel");
    if(!tmpElement.isNull())
    {
        if(tmpElement.text() == "mpg")
            ui->rb_units_fuel_mpg->setChecked(true);
        else if(tmpElement.text() == "km/l")
            ui->rb_units_fuel_kml->setChecked(true);
        else if(tmpElement.text() == "l/100")
            ui->rb_units_fuel_lkm->setChecked(true);
    }
    else
    {
        ui->rb_units_fuel_mpg->setChecked(true);
    }

    tmpElement = rootNode.firstChildElement("Units_HP");
    if(!tmpElement.isNull())
    {
        if(tmpElement.text() == "hp")
            ui->rb_units_power_hp->setChecked(true);
        else if(tmpElement.text() == "ps")
            ui->rb_units_power_PS->setChecked(true);
        else if(tmpElement.text() == "kw")
            ui->rb_units_power_kw->setChecked(true);
    }
    else
    {
        ui->rb_units_power_hp->setChecked(true);
    }

    tmpElement = rootNode.firstChildElement("Units_Torque");
    if(!tmpElement.isNull())
    {
        if(tmpElement.text() == "ft-lbs")
            ui->rb_units_force_ftlbs->setChecked(true);
        else if(tmpElement.text() == "Nm")
            ui->rb_units_force_nm->setChecked(true);

    }
    else
    {
        ui->rb_units_force_ftlbs->setChecked(true);
    }

    tmpElement = rootNode.firstChildElement("Units_Weight");
    if(!tmpElement.isNull())
    {
        if(tmpElement.text() == "lbs")
            ui->rb_units_weight_lbs->setChecked(true);
        else if(tmpElement.text() == "kg")
            ui->rb_units_weight_kg->setChecked(true);

    }
    else
    {
        ui->rb_units_weight_lbs->setChecked(true);
    }

    tmpElement = rootNode.firstChildElement("Units_Distance");
    if(!tmpElement.isNull())
    {
        if(tmpElement.text() == "in")
            ui->rb_units_length_inches->setChecked(true);
        else if(tmpElement.text() == "mm")
            ui->rb_units_length_mm->setChecked(true);

    }
    else
    {
        ui->rb_units_length_inches->setChecked(true);
    }

    tmpElement = rootNode.firstChildElement("Units_BigVol");
    if(!tmpElement.isNull())
    {
        if(tmpElement.text() == "cu-ft")
            ui->rb_units_vol_cuft->setChecked(true);
        else if(tmpElement.text() == "L")
            ui->rb_units_vol_L->setChecked(true);

    }
    else
    {
        ui->rb_units_vol_cuft->setChecked(true);
    }


    tmpElement = rootNode.firstChildElement("Units_SmallVol");
    if(!tmpElement.isNull())
    {
        if(tmpElement.text() == "ci")
            ui->rb_units_displacement_ci->setChecked(true);
        else if(tmpElement.text() == "cc")
            ui->rb_units_displacement_cc->setChecked(true);

    }
    else
    {
        ui->rb_units_displacement_cc->setChecked(true);
    }

    tmpElement = rootNode.firstChildElement("Units_Speed");
    if(!tmpElement.isNull())
    {
        if(tmpElement.text() == "mph")
            ui->rb_units_speed_mph->setChecked(true);
        else if(tmpElement.text() == "km/h")
            ui->rb_units_speed_kmh->setChecked(true);

    }
    else
    {
        ui->rb_units_speed_mph->setChecked(true);
    }


    /*
     * Now we handle the volume settings files stuff.
     */

    QDomDocument voldoc;
    if(!voldoc.setContent(&baseVolumeFile,&errorMsg,&errorLine,&errorColumn))
    {

       QMessageBox::critical(ui->centralWidget,"Error Volume File",
                            QString("Error Message: %1\nLine: %2\nColumn: %3").arg(
                            errorMsg,errorLine,errorColumn));

       return;
    }

    //Begin Reading and Setting UI values.
    rootNode = voldoc.firstChildElement("Sound");
    tmpElement = rootNode.firstChildElement("Volume");

    if(!tmpElement.isNull())
    {
      ui->MusicVolume_SpinBox->setValue(tmpElement.text().toDouble());
    }
    else
    {
      ui->MusicVolume_SpinBox->setValue(0.15);
    }

   tmpElement = rootNode.firstChildElement("SoundFX");
   if(!tmpElement.isNull())
   {
     ui->SoundEffects_SpinBox->setValue(tmpElement.text().toDouble());
   }
   else
   {
     ui->SoundEffects_SpinBox->setValue(0.35);
   }


   baseVideoFile.close();
   baseVolumeFile.close();


}
