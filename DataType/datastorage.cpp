/***************************************************************************
**                                                                        **
**  LeCroy Plugin for LabAnlyser, control&visualize data of LeCroy Oscis. **
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

#include "datastorage.h"

DataStorage::DataStorage():UpdateRequested(false)
{

}

DataStorage::~DataStorage()
{

}

DataStorage& DataStorage::operator =( InterfaceData& Input)
{
    this->SetDataRaw(Input.GetData());
    this->SetDataType(Input.GetDataType());
    this->SetType(Input.GetType());
    this->SetStateDependency(Input.GetStateDependency());

    return *this;
}

boost::variant<DataTypes> DataStorage::GetData()
{
    if(UpdateRequested)
        return DataTarget;
    else
        return this->GetDataRaw();
}

void DataStorage::SetDataTimeOut(boost::variant<DataTypes> DataIn,QString IDin, MessengerClass* Messenger_)
{
    Messenger = Messenger_;
    this->_ID = IDin;
    UpdateRequested = true;
    this->DataTarget = DataIn;


    if(Timer)
    {
        Timer->stop();
        delete Timer;
    }
    Timer = new QTimer;
    connect(Timer,SIGNAL(timeout()),this,SLOT(TimeOut()));
    Timer->setInterval(300);
    Timer->setSingleShot(true);
    Timer->start();

}

void DataStorage::TimeOut()
{
    UpdateRequested = false;
    if(this->DataTarget != this->GetDataRaw())
    {
        this->DataTarget = this->GetDataRaw();
        InterfaceData InterData = *this;
        Messenger->MessageSender("set", this->_ID, InterData);
    }
}
