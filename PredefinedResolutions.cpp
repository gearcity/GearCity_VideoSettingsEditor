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

#include "PredefinedResolutions.h"

void populateResolutions(int maxWidth, int maxHeight, QComboBox* resCombo)
{
  if(maxWidth >= 7680 && maxHeight >= 4320)
     resCombo->addItem("7680 x 4320");
  if(maxWidth >= 3840 && maxHeight >= 2160)
     resCombo->addItem("3840 x 2160");
  if(maxWidth >= 2560)
  {
      if(maxHeight >= 1440)
         resCombo->addItem("2560 x 1440");
      if(maxHeight >= 1600)
         resCombo->addItem("2560 x 1600");
  }
  if(maxWidth >= 1920)
  {
      if(maxHeight >= 1080)
         resCombo->addItem("1920 x 1080");
      if(maxHeight >= 1200)
         resCombo->addItem("1920 x 1200");
  }
  if(maxWidth >= 1680 && maxHeight >= 1050)
     resCombo->addItem("1680 x 1050");
  if(maxWidth >= 1600)
  {
      if(maxHeight >= 900)
         resCombo->addItem("1600 x 900");
      if(maxHeight >= 1200)
         resCombo->addItem("1600 x 1200");
  }
  if(maxWidth >= 1440 && maxHeight >= 900)
     resCombo->addItem("1440 x 900");
  if(maxWidth >= 1366 && maxHeight >= 768)
     resCombo->addItem("1366 x 768");
  if(maxWidth >= 1360 && maxHeight >= 768)
     resCombo->addItem("1360 x 768");
  if(maxWidth >= 1280)
  {
      if(maxHeight >= 720)
         resCombo->addItem("1280 x 720");
      if(maxHeight >= 768)
         resCombo->addItem("1280 x 768");
      if(maxHeight >= 800)
         resCombo->addItem("1280 x 800");
      if(maxHeight >= 1024)
         resCombo->addItem("1280 x 1024");
  }
  if(maxWidth >= 1152 && maxHeight >= 864)
     resCombo->addItem("1152 x 864");


  //Default Bare Minimum.
  resCombo->addItem("1024 x 768");


}
