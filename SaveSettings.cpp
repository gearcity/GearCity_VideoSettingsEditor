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


#include "SavesSettings.h"
#include <QFile>
#include <QXmlStreamWriter>
#include <QMessageBox>

SaveSettings::SaveSettings(Ui::MainWindow *ui, QString folderName)
{
    QString videoFileName = "",volumeFileName=folderName+"Volume.xml";

    #if defined(Q_WS_WIN)
        videoFileName = folderName + "VideoConfig.xml";
    #else
        videoFileName = folderName + "LinuxVideoConfig.xml";
    #endif

    save(ui,videoFileName,volumeFileName);
}

/*
 * Saves UI values to the defined files.
 */
void SaveSettings::save(Ui::MainWindow *ui, QString videoFile, QString volumeFile)
{
    /*
     * We could use DOM since we already read everything in, but I didn't think about it
     * to begin with, and it's not really that big of a deal to write with XML streamer.
     * so we'll go that way.
     */

    //Check if we can open and write to the needed files!
    QFile baseVideoFile(videoFile);
    if(!baseVideoFile.open(QFile::WriteOnly | QFile::Text))
    {
       QMessageBox::critical(ui->centralWidget,"Error",
                             QString("Can not write to %1\n%2").arg(videoFile,
                             baseVideoFile.errorString()));

       return;
    }

    QFile baseVolumeFile(volumeFile);
    if(!baseVolumeFile.open(QFile::WriteOnly | QFile::Text))
    {
       QMessageBox::critical(ui->centralWidget,"Error",
                             QString("Can not write to %1\n%2").arg(volumeFile,
                             baseVolumeFile.errorString()));

       return;
    }

    bool isOpenGL = false;

    //Lets Start Writing!
    QXmlStreamWriter xmlWriter(&baseVideoFile);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument();

    xmlWriter.writeStartElement("Video");

    #if defined(Q_OS_WIN)
     if(ui->Render_ComboBox->currentIndex() == 0)
     {
         xmlWriter.writeTextElement("RenderSystem","D3D9 RenderSystem");
     }
     else
     {
         isOpenGL = true;
         xmlWriter.writeTextElement("RenderSystem","OpenGL Rendering Subsystem");
     }
    #else
        isOpenGL = true;
        xmlWriter.writeTextElement("RenderSystem","OpenGL Rendering Subsystem");
    #endif

     xmlWriter.writeTextElement("ColorDepth",ui->ColorDepth_ComboBox->currentText());
     xmlWriter.writeTextElement("FSAA",ui->FSAA_ComboBox->currentText());
     xmlWriter.writeTextElement("FPM","");
     if(ui->FullScreen_CheckBox->isChecked())
        xmlWriter.writeTextElement("Fullscreen","Yes");
     else
        xmlWriter.writeTextElement("Fullscreen","No");

     if(ui->VSYNC_CheckBox->isChecked())
        xmlWriter.writeTextElement("VSYNC","Yes");
     else
        xmlWriter.writeTextElement("VSYNC","No");


       xmlWriter.writeTextElement("VideoRes",ui->VideoResWidth_TextBox->text()+" x "+
                                  ui->VideoResHeight_TextBox->text()+" @ "+
                                  ui->ColorDepth_ComboBox->currentText()+"-bit colour");


     xmlWriter.writeTextElement("Language",ui->Language_ComboBox->currentText());
     xmlWriter.writeTextElement("Shadows",QString::number(ui->Shadow_ComboBox->currentIndex()));

     if(ui->Shaders_CheckBox->isChecked())
        xmlWriter.writeTextElement("Shaders","true");
     else
        xmlWriter.writeTextElement("Shaders","false");

     if(ui->Compositors_CheckBox->isChecked())
        xmlWriter.writeTextElement("Compositor","true");
     else
        xmlWriter.writeTextElement("Compositor","false");

     if(ui->HardwareMouse_CheckBox->isChecked())
        xmlWriter.writeTextElement("HardMouse","true");
     else
        xmlWriter.writeTextElement("HardMouse","false");

     xmlWriter.writeTextElement("Map",QString::number(ui->MapSensitivity_SpinBox->value()));
     xmlWriter.writeTextElement("Mouse",QString::number(ui->MouseSensitivity_SpinBox->value()));

     if(ui->Tutorials_Checkbox->isChecked())
        xmlWriter.writeTextElement("Tutorials","true");
     else
        xmlWriter.writeTextElement("Tutorials","false");

     xmlWriter.writeTextElement("FontSize",QString::number(ui->FontSize_SpinBox->value()*2));

     if(ui->BorderlessWindow_CheckBox->isChecked())
        xmlWriter.writeTextElement("Borderless","true");
     else
        xmlWriter.writeTextElement("Borderless","false");

     if(ui->LowDefTexture_CheckBox->isChecked())
        xmlWriter.writeTextElement("ld","true");
     else
        xmlWriter.writeTextElement("ld","false");

     xmlWriter.writeTextElement("AutoSave",ui->AutoSave_ComboBox->currentText());

     if(ui->DeleteOldReports_CheckBox->isChecked())
        xmlWriter.writeTextElement("DeleteOldReports","true");
     else
        xmlWriter.writeTextElement("DeleteOldReports","false");

     if(ui->LockSeasons_CheckBox->isChecked())
        xmlWriter.writeTextElement("LockSeasons","true");
     else
        xmlWriter.writeTextElement("LockSeasons","false");

     if(ui->SaveOnExit_CheckBox->isChecked())
        xmlWriter.writeTextElement("SaveOnExit","true");
     else
        xmlWriter.writeTextElement("SaveOnExit","false");

     if(ui->DisableHotkeys_Checkbox->isChecked())
        xmlWriter.writeTextElement("DisableHotkeys","true");
     else
        xmlWriter.writeTextElement("DisableHotkeys","false");

     if(ui->DisableHotkeys_Checkbox->isChecked())
        xmlWriter.writeTextElement("AutoSaveGUI","true");
     else
        xmlWriter.writeTextElement("AutoSaveGUI","false");

     xmlWriter.writeTextElement("Mod",ui->comboBox_Mod_AvaliableMods->currentText());

     if(ui->rb_units_fuel_mpg->isChecked())
         xmlWriter.writeTextElement("Units_Fuel","mpg");
     else if(ui->rb_units_fuel_kml->isChecked())
         xmlWriter.writeTextElement("Units_Fuel","km/l");
     if(ui->rb_units_fuel_lkm->isChecked())
         xmlWriter.writeTextElement("Units_Fuel","l/100");


     if(ui->rb_units_power_hp->isChecked())
         xmlWriter.writeTextElement("Units_HP","hp");
     else if(ui->rb_units_power_PS->isChecked())
         xmlWriter.writeTextElement("Units_HP","ps");
     else if(ui->rb_units_power_kw->isChecked())
         xmlWriter.writeTextElement("Units_HP","kw");

     if(ui->rb_units_force_ftlbs->isChecked())
         xmlWriter.writeTextElement("Units_Torque","ft-lbs");
     else if(ui->rb_units_force_nm->isChecked())
         xmlWriter.writeTextElement("Units_Torque","Nm");

     if(ui->rb_units_weight_lbs->isChecked())
         xmlWriter.writeTextElement("Units_Weight","lbs");
     else if(ui->rb_units_weight_kg->isChecked())
         xmlWriter.writeTextElement("Units_Weight","kg");

     if(ui->rb_units_length_inches->isChecked())
         xmlWriter.writeTextElement("Units_Distance","in");
     else if(ui->rb_units_length_mm->isChecked())
         xmlWriter.writeTextElement("Units_Distance","mm");

     if(ui->rb_units_vol_cuft->isChecked())
         xmlWriter.writeTextElement("Units_BigVol","cu-ft");
     else if(ui->rb_units_vol_L->isChecked())
         xmlWriter.writeTextElement("Units_BigVol","L");

     if(ui->rb_units_displacement_ci->isChecked())
         xmlWriter.writeTextElement("Units_SmallVol","ci");
     else if(ui->rb_units_displacement_cc->isChecked())
         xmlWriter.writeTextElement("Units_SmallVol","cc");

     if(ui->rb_units_speed_mph->isChecked())
         xmlWriter.writeTextElement("Units_Speed","mph");
     else if(ui->rb_units_speed_kmh->isChecked())
         xmlWriter.writeTextElement("Units_SmallVol","km/h");

     xmlWriter.writeEndDocument();


     //Now lets write the volume out!
     QXmlStreamWriter xmlVolumeWriter(&baseVolumeFile);
     xmlVolumeWriter.setAutoFormatting(true);
     xmlVolumeWriter.writeStartDocument();

     xmlVolumeWriter.writeStartElement("Sound");
     xmlVolumeWriter.writeTextElement("Volume",QString::number(ui->MusicVolume_SpinBox->value()));
     xmlVolumeWriter.writeTextElement("SoundFX",QString::number(ui->SoundEffects_SpinBox->value()));

     xmlVolumeWriter.writeEndDocument();

     baseVolumeFile.close();
     baseVideoFile.close();

     //We Did it! Yay!
     QMessageBox::information(ui->centralWidget,"Save Complete",
                              "Your Settings Have Been Saved!", QMessageBox::Ok);



}
