/******************************************************************************
**                                                                           **
**  Tektronix Plugin for LabAnlyser, control&visualize data of Tex Scopes.   **
**  Copyright (C) 2015-2021 Andreas Hoffmann                                 **
**                                                                           **
**  Tektronix Plugin is free software: you can redistribute it and/or modify **
**  it under the terms of the GNU General Public License as published by     **
**  the Free Software Foundation, either version 3 of the License, or        **
**  (at your option) any later version.                                      **
**                                                                           **
**  This program is distributed in the hope that it will be useful,          **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of           **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            **
**  GNU General Public License for more details.                             **
**                                                                           **
**  You should have received a copy of the GNU General Public License        **
**  along with this program.  If not, see http://www.gnu.org/licenses/.      **
**                                                                           **
*******************************************************************************
*******************************************************************************/

#include "createsymbols.h"

CreateSymbols::CreateSymbols(QObject* parent, QString DeviceName_, std::map<QString, DataStorage> &m_data_): QObject(parent), DeviceName(DeviceName_), m_data(m_data_)
{
    connect(this,SIGNAL(MessageSender(const QString, const QString, InterfaceData)),parent,SLOT(MessageReceiver(const QString, const QString, InterfaceData)));
    InterfaceData Data;
    Data.SetData(0);
    emit MessageSender("publish_start", DeviceName, Data);

}

void CreateSymbols::PublishVector(QString ID)
{
    InterfaceData _Data;
    _Data.SetDataType("vector<double>");
    _Data.SetType("Data");
    _Data.SetData(DataPair());
    emit MessageSender("publish", ID,  _Data);
}

void CreateSymbols::PublishState(QString ID)
{
    InterfaceData _Data;
    _Data.SetDataType("boolean");
    _Data.SetType("State");
    _Data.SetData(false);
    emit MessageSender("publish", ID,  _Data);
}

void CreateSymbols::PublishInt(QString ID)
{
    InterfaceData _Data;
    _Data.SetType("Data");
    _Data.SetData((uint32_t) 0);
    emit MessageSender("publish", ID,  _Data);
}


void CreateSymbols::PublishParameters()
{

    PublishState(DeviceName + "::ReadChannels");
    PublishInt(DeviceName + "::Aquisition::Counter");

    PublishVector(DeviceName + "::Buffered::C1");
    PublishVector(DeviceName + "::Buffered::C2");
    PublishVector(DeviceName + "::Buffered::C3");
    PublishVector(DeviceName + "::Buffered::C4");

    InterfaceData Data;
    Data.SetData(0);
    emit MessageSender("publish_finished", DeviceName, Data);
}
