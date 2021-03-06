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

#include "worker.h"

//Needed for compiling:
#define BOOST_SYSTEM_NO_DEPRECATED
#include <boost/system/error_code.hpp>

#include <iostream>
#include <fcntl.h>
#include <stdlib.h>
#include <boost/program_options.hpp>
#include <boost/thread.hpp>

#include <iomanip>        // std::put_time
#include <thread>         // std::this_thread::sleep_until
#include <chrono>         // std::chrono::system_clock
#include <ctime>          // std::time_t, std::tm, std::localtime, std::mktime

#include <signal.h>
#include <vector>
#include <mutex>

#include <QtWidgets>
#include <cmath>

#include <QDebug>
#include <functional>

#include "Symbols/createsymbols.h"
#include "LoadSave/XmlReader.h"
#include "Osci/readoscichannel.h"


WorkClass::WorkClass(QString DeviceName_)
    : QObject(nullptr), SymbolsPublished(false), Messenger(this, DeviceName_)
{

    DeviceName = DeviceName_;
}

WorkClass::~WorkClass()
{

}


bool WorkClass::Abort()
{
    return abort||Error;
}

void WorkClass::process()
{
    this->ThreadRunning.lock();
    Messenger.Info( DeviceName + " loaded.");



    while(!XmlRead && !Abort())
    {
        QCoreApplication::processEvents();
        QThread::msleep(10);
    }

    if(!Abort())
    {
        int status = this->Osci.Connect(this->IP);
        if(status)
        {
           GetMessenger()->Error("Could not connect to Scope. Visa Error Code: " + QString::number(status));
           this->Error = 1;
        }
        else
            GetMessenger()->Info("Scope Connected");
    }

    //Set Data Format:
    if(!Abort()) this->Osci.write("Comm_ForMaT DEF9, WORD, BIN","");
    if(!Abort()) this->Osci.write("Comm_HeaDeR SHORT","");
    if(!Abort()) this->Osci.write("WaveForm_SetUp SP,1,NP,0,FP,0,SN,0","");

    ReadStreams = 0;
    ReadOsciChannel ChannelReader(DeviceName, Messenger, Osci, m_data);

    this->SymbolsPublished = true;

    QStringList ErrorStates = this->Osci.CheckStates(StateRequests);
    if(ErrorStates.size())
    {
        for(auto itt : ErrorStates)
            Messenger.Error("Visa Command: " + itt + " not found!");
        Error = true;
    }


    // First Loop
    while(!Abort())
    {
        QTime CycleTimer;
        CycleTimer.start();

        //Send States to Oszi
        if(SendQueque.size())
        {
            LockSendQueque.lock();
            while(SendQueque.size())
            {
                QString ID = SendQueque.front().first;
                InterfaceData _data = SendQueque.front().second;
                SendQueque.erase(SendQueque.begin());
                if(this->StateSetCommands.find(ID)!=this->StateSetCommands.end())
                {
                    if(_data.GetDataRaw() != this->m_data[ID].GetDataRaw())
                    {
                        if(this->StateSetCommands.find(ID)!=this->StateSetCommands.end())
                        {
                            if(_data.GetDataRaw() != this->m_data[ID].GetDataRaw())
                            {
                                if(_data.GetDataType().compare("GuiSelection")==0)
                                        this->Osci.write(this->StateSetCommands[ID] + "\"" + _data.GetGuiSelection().first + "\"'",ID);
                                else
                                        this->Osci.write(this->StateSetCommands[ID] + _data.GetString() + "'",ID);

                                this->m_data[ID].SetDataTimeOut(_data.GetDataRaw(), ID, GetMessenger());
                            }
                        }
                    }
                }
            }
            LockSendQueque.unlock();
        }
        //Read States from Osci
        QStringList States = this->Osci.ReadState(StateRequests);
        if(States.size() == StateRequests.size())
        {
            for(int i = 0; i < this->StateIds.size();i++)
            {
                {
                    QString TDate = States[i];
                    InterfaceData _data = this->m_data[StateIds[i]];

                    _data.SetDataKeepType(TDate);                 
                    if(_data.GetDataRaw() != this->m_data[StateIds[i]].GetDataRaw())
                    {

                        this->m_data[StateIds[i]].SetDataRaw(_data.GetDataRaw());          
                        this->MessageSender("set", StateIds[i], _data);
                    }
                }
            }
        }
        else
        {
            GetMessenger()->Error("Error in Message Commands, please check the LAdev file!");
        }

        if(LastTriggerMode == "Single" && this->m_data[DeviceName + "::Trigger::Mode"].GetString() == "Stopped")
        {
            ReadStreams = 1;
        }
        LastTriggerMode = this->m_data[DeviceName + "::Trigger::Mode"].GetString();

        if(ReadStreams)
        {
            int Channel = 1;
            while(m_data.find(DeviceName + "::Channel::C"+ QString::number(Channel) +"::State") != m_data.end())
            {
                ChannelReader.ReadChannel(Channel);
                Channel++;
            }
            QString CounterID = DeviceName + "::ChannelRead::Counter";
            uint32_t tmp = m_data[CounterID].GetUInt32_tData();
            tmp++;
            m_data[CounterID].SetDataKeepType(tmp);
            this->MessageSender("set", CounterID,  m_data[CounterID]);
            ReadStreams = 0;

        }


       //always use a sleep in this loop or the cpu load will be massive
       while( CycleTimer.elapsed() < 100)
       {
           //process Events
           QCoreApplication::processEvents();
           QThread::msleep(1);
       }
    }

    this->Osci.CloseConnection();
    GetMessenger()->Info("Scope Connection Closed");

    if(Error)
        emit MessageSender("CloseProject",objectName(), InterfaceData());


    while(!abort)
        QThread::msleep(1);

    Messenger.Info( DeviceName + " closed.");
    this->ThreadRunning.unlock();
    emit ThreadFinished();
    Finished = true;

}

void WorkClass::ParseSetCommand(const QString ID,InterfaceData  _data)
{
    if(_data.GetType().compare("State"))
    {
        LockSendQueque.lock();
            SendQueque.push_back(QPair<QString,InterfaceData>(ID,_data));
        LockSendQueque.unlock();
    }
    else
    {
        if(ID == DeviceName + "::ReadChannels")
            this->ReadStreams = 1;
    }
}


void WorkClass::MessageReceiver(const QString &Command, const QString &ID, InterfaceData Data)
{

    //ID always begins with plugin name
    if(ID.split("::").at(0) != DeviceName)
        return;

    if(Command == "get")
    {
        InterfaceData Dat = (this->m_data[ID]);
        Dat.SetDataRaw(this->m_data[ID].GetData());
        emit MessageSender("set", ID , Dat);
    }
    else if(Command == "load")
    {
        CreateSymbols Symbols(this, DeviceName, m_data);
        XmlReader reader(this,Messenger, m_data, DeviceName,StateIds, StateRequests, StateSetCommands);
        if(reader.read(Data.GetString()))
        {
            abort = true;
            Error = true;
        }
        IP = reader.GetIP();
        XmlRead = true;
        Symbols.PublishParameters();
    }
    else if(Command.compare("LoadCustomData")==0)
    {

    }
    else if(Command.compare("save")==0)
    {

    }
    else if(Command == "publish")
    {
        emit MessageSender(Command,ID,Data);
    }
    else if(Command.compare("set")==0)
    {
        ParseSetCommand(ID,Data);
    }
    else
        emit MessageSender(Command,ID,Data);
    return;
}


