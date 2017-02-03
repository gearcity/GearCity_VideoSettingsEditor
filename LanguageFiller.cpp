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


#include "LanguageFiller.h"
#include <dirent.h>

#if defined(Q_WS_MACX)
    #include "OSXHelper.h"
#endif


void fillLanguageCombo(QComboBox* langCombo)
{
  QStringList langList;

  #if defined(Q_WS_WIN)
      QString loadDir = "..\\localization\\";

  #elif defined(Q_WS_X11)
        QString loadDir = "../localization/";
  #elif defined(Q_WS_MACX)
        //Ogre::String loadDir = OgreFramework::getSingletonPtr()->getMacPath() + "/localization/";
        QString loadDir = OSXHelper::getMacPath() + "/localization/";
  #endif

        QString localNames;

        //Load the folder's info
        DIR *dir;
        struct dirent *ent;
        dir = opendir (loadDir.toStdString().c_str());

        if (dir != NULL) //Directory Exists
        {
          //Add all the files and directories within directory to list
          while ((ent = readdir (dir)) != NULL)
          {
            if(strcmp(ent->d_name,".") != 0  &&  strcmp(ent->d_name,"..") != 0)
            {
              localNames = ent->d_name;
              localNames.chop(4);
              langList.push_back(localNames);
            }
          }

           closedir (dir);

         }
         else
         {
           /* Directory Does Not Exist! */
            return;
         }

  langCombo->addItems(langList);

}
