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


#include "ResetToDefault.h"

//Reset the UI to default values for GC
void ResetToDefault(Ui::MainWindow *ui)
{
    ui->Render_ComboBox->setCurrentIndex(0);
    ui->ColorDepth_ComboBox->setCurrentIndex(0);
    ui->FSAA_ComboBox->setCurrentIndex(0);
    ui->FullScreen_CheckBox->setChecked(false);
    ui->VSYNC_CheckBox->setChecked(true);

    ui->VideoResWidth_TextBox->setText("1024");
    ui->VideoResHeight_TextBox->setText("768");

    //Format text for combo box, then change combo text.
    QString resText = "1024 x 768";

    //Hackish work around to allow text in the ComboBox that is not an Item!
    ui->VideoRes_ComboBox->setEditable(true);
    ui->VideoRes_ComboBox->lineEdit()->setText(resText);
    ui->VideoRes_ComboBox->lineEdit()->setReadOnly(true);

     ui->Language_ComboBox->setCurrentIndex(0);
     ui->Shadow_ComboBox->setCurrentIndex(0);
     ui->Shaders_CheckBox->setChecked(true);
     ui->Compositors_CheckBox->setChecked(false);
     ui->HardwareMouse_CheckBox->setChecked(true);
     ui->MapSensitivity_SpinBox->setValue(0.5);
     ui->MouseSensitivity_SpinBox->setValue(0.5);
     ui->Tutorials_Checkbox->setChecked(true);
     ui->FontSize_SpinBox->setValue(0);
     ui->BorderlessWindow_CheckBox->setChecked(false);
     ui->LowDefTexture_CheckBox->setChecked(false);
     ui->AutoSave_ComboBox->setCurrentIndex(3);
     ui->DeleteOldReports_CheckBox->setChecked(true);
     ui->LockSeasons_CheckBox->setChecked(false);
     ui->SaveOnExit_CheckBox->setChecked(true);

     ui->ScaleGUI_horizontalScrollBar->setValue(0);
     ui->dynamicReports_CheckBox->setChecked(false);
     ui->fpsLimiter_spinnerBox->setValue(60);
     ui->MonitorIndex_SpinBox->setValue(0);

     ui->checkBox_ContinuousMusic->setChecked(false);
     ui->checkBox_NoMusicYear->setChecked(false);
     ui->checkBox_RandomMusic->setChecked(false);

     ui->MusicVolume_SpinBox->setValue(0.15);
     ui->SoundEffects_SpinBox->setValue(0.35);

}
