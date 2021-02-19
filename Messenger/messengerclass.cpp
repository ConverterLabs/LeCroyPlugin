/***************************************************************************
**                                                                        **
**  LeCroy Plugin for LabAnlyser, control&visualize data of LeCroy Scopes.**
**  Copyright (C) 2015-2021 Andreas Hoffmann                              **
**                                                                        **
**  LeCroy Plugin is free software: you can redistribute it and/or modify **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
**                                                                        **
****************************************************************************
****************************************************************************/

#include "messengerclass.h"

MessengerClass::MessengerClass(QObject *parent, QString PluginName_): QObject(parent), PluginName(PluginName_)
{

}

void MessengerClass::Info(QString text)
{
    if(!text.size())
        return;
    InterfaceData Data;
    Data.SetData(text);
    emit MessageSender("info", PluginName, Data);
}

void MessengerClass::Error(QString text)
{
    if(!text.size())
        return;
    InterfaceData Data;
    Data.SetData(text);
    emit MessageSender("error", PluginName, Data);
}
