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
#include <QObject>
#include <QFile>
#include <QXmlStreamReader>
#include "../Messenger/messengerclass.h"

class XmlReader: public QObject
{
   Q_OBJECT
public:
    XmlReader(QObject *parent ,MessengerClass & Messenger_, QString DeviceName_,    QStringList &StateIds,    QStringList &StateRequests,    QMap<QString, QString> &StateSetCommands);
    bool read(QString path);
    QString errorString() { return QString();};

    QString GetIP(){return IP;};



private:
    QXmlStreamReader reader;
    void readLEDevice();
    void readConnection();
    void readIP();

    void readVisa();
    void readVisaConnection();

    void ReadDouble(bool ReadOnly);
    void ReadBoolean(bool ReadOnly);
    void ReadGuiSelection(bool ReadOnly);
    void ReadString(bool ReadOnly);
    void ReadInt(bool ReadOnly);

    QString DeviceName;

    QString IP;
    MessengerClass &Messenger;

    QStringList& StateIds;
    QStringList& StateRequests;
    QMap<QString, QString> &StateSetCommands;


};

