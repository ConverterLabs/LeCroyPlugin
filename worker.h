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

#pragma once

#include <QMutex>
#include <QSize>
#include <QThread>
#include <QWaitCondition>
#include <boost\function.hpp>
#include <boost\bind.hpp>
#include <boost\shared_ptr.hpp>

#include <vector>
#include <atomic>
#include <QTime>
#include <QTimer>

#include "Messenger/messengerclass.h"
#include "DataType/datastorage.h"
#include "Osci/LeCroyClass.h"


class WorkClass: public QObject
{
    Q_OBJECT

public:
    void SetPorts(QString Port_in);

    WorkClass(QString DeviceName_) ;
    ~WorkClass();

    void Stop(){this->abort = true;}

    MessengerClass* GetMessenger(){return &Messenger;};

    bool HasPublishedSymbols(){return SymbolsPublished;};
    InterfaceData* GetContainer(QString ID){ return &(m_data[ID]);};
    bool IsFinished(){return Finished;};
    bool IsError(){return Error;};

signals:
    void MessageSender(const QString &Command, const QString &ID, InterfaceData Data);
    void ThreadFinished();

public slots:
    void MessageReceiver(const QString &Command, const QString &ID, InterfaceData Data);
    void process();


private:

    void ParseSetCommand(const QString ID,InterfaceData  _data);
    bool Finished = false;
    QString DeviceName;

    std::map<QString, DataStorage> m_data;
    std::atomic<bool> SymbolsPublished = { false };

    bool Abort();

    MessengerClass Messenger;
    QMutex mutex;
    QMutex ThreadRunning;

    QMutex SendBufferMutex;
    std::vector<char> SendBuffer;

    QList<QPair<QString, InterfaceData>> SendQueque;
    QMutex LockSendQueque;


    bool abort = 0;
    bool Error = 0;

    bool ReadStreams = false;

    bool XmlRead = false;
    QStringList StateIds;
    QStringList StateRequests;
    QMap<QString, QString> StateSetCommands;

    QString IP;
    LeCroy Osci;


};
