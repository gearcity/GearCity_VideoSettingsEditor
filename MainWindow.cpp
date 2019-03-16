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

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QMessageBox>
#include "PredefinedResolutions.h"
#include "LanguageFiller.h"
#include "SettingsImporter.h"
#include "ResetToDefault.h"
#include "SavesSettings.h"
#include "ReadModFile.h"
#include "ClearCache.h"
#include "SteamWorkshopLoader.h"


#if defined(Q_WS_MACX)
#include "OSXHelper.h"
#endif

/*Constructor for Main Window*/
MainWindow::MainWindow(QWidget *parent) :
 QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);



  //Top Menus
  QObject::connect(ui->actionAbout, SIGNAL(triggered()),this,SLOT(aboutProgram()));
  QObject::connect(ui->actionExit, SIGNAL(triggered()),this,SLOT(exitProgram()));
  QObject::connect(ui->actionReset, SIGNAL(triggered()),this,SLOT(resetToDefault()));
  QObject::connect(ui->actionSave, SIGNAL(triggered()),this,SLOT(saveVideoSettings()));

  //Bottom Buttons
  QObject::connect(ui->MainSave_Button,SIGNAL(clicked()),this,SLOT(saveVideoSettings()));
  QObject::connect(ui->MainCancel_Button,SIGNAL(clicked()),this,SLOT(exitProgram()));
  QObject::connect(ui->MainResetToDefault_Button,SIGNAL(clicked()),this,SLOT(resetToDefault()));

  //First Tab
  #if !defined(Q_WS_WIN)
    ui->Render_ComboBox->removeItem(0);
  #endif

  QObject::connect(ui->Render_ComboBox,SIGNAL(currentIndexChanged(int)),
                   this,SLOT(rendererChanged()));

  ui->VideoResHeight_TextBox->setValidator(new QIntValidator());
  ui->VideoResWidth_TextBox->setValidator(new QIntValidator());

  //Get current desktop resolution then populate the combobox with
  //predefined resolution values.
  deskWidget = new QDesktopWidget();
  QRect currentRes = deskWidget->availableGeometry(deskWidget->primaryScreen());

  maxResWidth = currentRes.width();
  maxResHeight = currentRes.height();

  //predefined resolutions
  populateResolutions(maxResWidth, maxResHeight, ui->VideoRes_ComboBox);

  QObject::connect(ui->VideoRes_ComboBox,SIGNAL(currentIndexChanged(QString)),
                   this,SLOT(resolutionChanged()));
  QObject::connect(ui->VideoResWidth_TextBox,SIGNAL(textEdited(QString)),
                   this,SLOT(resolutionTextBoxesChanged()));
  QObject::connect(ui->VideoResHeight_TextBox,SIGNAL(textEdited(QString)),
                   this,SLOT(resolutionTextBoxesChanged()));


  //Fill Language Dropdown
  fillLanguageCombo(ui->Language_ComboBox);

  //Set up default just in case.
  ResetToDefault(ui);


   ui->label_FullScreenWarning->hide();
   ui->ProgressBar->hide();
   ui->progressBarReports->hide();



   connect(&timer,SIGNAL(timeout()),this,SLOT(checkSteamAndSetSettings()));



    timer.start(100);


}

/*Deconstructor*/
MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::checkSteamAndSetSettings()
{

    timer.stop();

#if STEAMSUPPORT
  ui->ProgressBar->show();
  SteamWorkshopLoader(ui, false);
  ui->ProgressBar->hide();
#endif

  //Import Current Game Settings and Mod Folders
  #if defined(Q_WS_WIN)
   SettingsImporter(ui,"../GearCity/Settings/","../media/Mods/" );
  #elif defined(Q_WS_X11)
   SettingsImporter(ui,"../GearCity/Settings/","../media/Mods/");
  #elif defined(Q_WS_MACX)
   SettingsImporter(ui,OSXHelper::getMacPath(0) + "/GearCity/Settings/",
                    OSXHelper::getMacPath(0) +"/media/Mods/");
  #endif


}

/*About the program*/
void MainWindow::aboutProgram()
{
  QMessageBox::about(this,tr("About GearCity Video Settings Editor"),
                     tr("GearCity Video Settings Editor\n"
                        "Copyright (c) 2016, Visual Entertainment And Technologies, LLC\n"
                        "Released Under Revised BSD License\n "
                        "See licenses.txt for details.\n"));
}



/*Save the settings to the GearCity Settings file*/
void MainWindow::saveVideoSettings()
{

    #if defined(Q_WS_WIN)
     SaveSettings(ui,"../GearCity/Settings/");
    #elif defined(Q_WS_X11)
     SaveSettings(ui,"../GearCity/Settings/");
    #elif defined(Q_WS_MACX)
     SaveSettings(ui,OSXHelper::getMacPath(0) + "/GearCity/Settings/");
    #endif

}

/*Quit the Program*/
void MainWindow::exitProgram()
{
  qApp->quit();
}

/*Reset the settings to defaults the game ships with.*/
void MainWindow::resetToDefault()
{
  ResetToDefault(ui);
}

/*Hide/Show options depending on the selected renderer.*/
void MainWindow::rendererChanged()
{
  if(ui->Render_ComboBox->currentIndex() == 0) //DX
  {
    ui->BorderWindow_Label->show();
    ui->BorderlessWindow_CheckBox->show();
  }
  else //OpenGL
  {
    ui->BorderWindow_Label->hide();
    ui->BorderlessWindow_CheckBox->hide();
  }

}

/*
 * If Resolution in ComboBox Changed, split the text and stick it in the Textboxes'
 */
void MainWindow::resolutionChanged()
{
 int width = 0, height = 0;

 //Split Combobox Text to fill Textboxes.
 QStringList resValsList = ui->VideoRes_ComboBox->currentText().split(" x ");

 if(resValsList.size() == 2) //Combo should have only Width and Height after split...
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
     //Oh Noes! Split produced more than just width and a height...
    QMessageBox::critical(this,tr("Error"),tr("Split failed to produce 2 resolution sizes!"));
    return;
  }

 //Assign Values to TextBoxes.
 ui->VideoResWidth_TextBox->setText(QString::number(width));
 ui->VideoResHeight_TextBox->setText(QString::number(height));
}

/* Resolution Textbox has  been changed, check against max
 * then update the update the combobox text...*/
void MainWindow::resolutionTextBoxesChanged()
{
  //Check against maxRes, if larger, retext the text boxes.
  if(ui->VideoResWidth_TextBox->text().toInt() > maxResWidth)
  {
    ui->VideoResWidth_TextBox->setText(QString::number(maxResWidth));
  }
  if(ui->VideoResHeight_TextBox->text().toInt() > maxResHeight)
  {
    ui->VideoResHeight_TextBox->setText(QString::number(maxResHeight));
  }

  //Format text for combo box, then change combo text.
  QString resText = ui->VideoResWidth_TextBox->text() + " x " + ui->VideoResHeight_TextBox->text();

  //Hackish work around to allow text in the ComboBox that is not an Item!
  ui->VideoRes_ComboBox->setEditable(true);
  ui->VideoRes_ComboBox->lineEdit()->setText(resText);
  ui->VideoRes_ComboBox->lineEdit()->setReadOnly(true);
}


void MainWindow::on_comboBox_Mod_AvaliableMods_currentIndexChanged(const QString &arg1)
{
    if(ui->comboBox_Mod_AvaliableMods->currentIndex() == 0)
    {
        ui->label_Mod_Author->setText("Eric B Jones");
        ui->textBrowser_Mod_Description->setText("Default Game Files for GearCity");
        ui->listWidget_Mod_Maps->clear();
         ui->listWidget_Mod_Maps->addItem("All");
         ui->tableWidget_Mod_Features->clearContents();
         ui->tableWidget_Mod_Features->setRowCount(0);
    }
    else
    {
        #if defined(Q_WS_WIN)
         ReadModFile(ui,"../media/Mods/"+ui->comboBox_Mod_AvaliableMods->currentText() );
        #elif defined(Q_WS_X11)
         ReadModFile(ui,"../media/Mods/"+ui->comboBox_Mod_AvaliableMods->currentText());
        #elif defined(Q_WS_MACX)
         ReadModFile(ui, OSXHelper::getMacPath(0) +"/media/Mods/"+
                     ui->comboBox_Mod_AvaliableMods->currentText());
        #endif
    }

}

void MainWindow::on_FullScreen_CheckBox_clicked()
{
    if(ui->FullScreen_CheckBox->isChecked())
    {
        ui->VideoRes_ComboBox->setCurrentIndex(0);
        ui->VideoRes_ComboBox->setEnabled(false);

        ui->VideoResHeight_TextBox->setEnabled(false);
        ui->VideoResWidth_TextBox->setEnabled(false);
        ui->label_FullScreenWarning->show();
        resolutionChanged();
    }
    else
    {
        ui->VideoRes_ComboBox->setEnabled(true);
        ui->VideoResHeight_TextBox->setEnabled(true);
        ui->VideoResWidth_TextBox->setEnabled(true);
        ui->label_FullScreenWarning->hide();
    }
}

void MainWindow::on_Main_ClearCache_clicked()
{

    QMessageBox::StandardButton answer;
     answer = QMessageBox::question(this, "Are you sure?", "This will delete all save games, "
                                   " clear all cached files, and reset all settings files."
                                   " You should do this if you are having problems or have"
                                   " reinstalled the game.\n\n Are you sure you want to do this?",
                                   QMessageBox::Yes|QMessageBox::No);
     if (answer == QMessageBox::Yes)
     {
       ClearCache cc(ui);
       cc.clear();
       ui->ProgressBar->hide();
     }

}
