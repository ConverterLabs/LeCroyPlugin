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

#include "XmlReader.h"
#include <QDebug>
#include <QFile>
#include <QDir>

XmlReader::XmlReader(QObject *parent,MessengerClass & Messenger_, std::map<QString, DataStorage> &m_data_, QString DeviceName_, QStringList &StateIds_, QStringList &StateRequests_, QMap<QString, QString> &StateSetCommands_):
    QObject(parent),
    DeviceName(DeviceName_),
    Messenger( Messenger_),
    StateIds(StateIds_),
    StateRequests(StateRequests_),
    StateSetCommands(StateSetCommands_),
    m_data(m_data_)
{

}

bool XmlReader::read(QString Path)
{

    bool Error = false;
    QFile file(Path);
    QDir T(QFileInfo(file).absoluteDir());
    if(!file.open(QFile::ReadOnly | QFile::Text)){
        Messenger.Error("Error reading " + Path + ": " + file.errorString());
        return true;
    }
    reader.setDevice(&file);

    if (reader.readNextStartElement()) {
        if (reader.name() == "LEDevice")
            readLEDevice();
        else
        {
            reader.raiseError(QObject::tr("Not a Parameter file!"));
            Messenger.Error("Error parsing " + Path + ": " + reader.errorString());
            Error = true;
        }
    }
    file.close();

    if(reader.error())
    {
        Messenger.Error("Error parsing " + Path + ": " + reader.errorString());
    }

    if(IP.isEmpty())
    {
        Messenger.Error("Error parsing " + Path + ": " + "IP tag not found");
        Error = 1;
    }


    return Error||reader.error();

}

void XmlReader::readLEDevice()
{
    while(reader.readNextStartElement()){
       if(reader.name() == "Connection")
            readConnection();
       else if(reader.name() == "Visa")
            readVisa();
       else
           reader.skipCurrentElement();
   }
}

void XmlReader::readVisa()
{

    while(reader.readNextStartElement()){
       if(reader.name() == "Connection")
            readVisaConnection();
       else
           reader.skipCurrentElement();
   }
}

void XmlReader::ReadDouble(bool ReadOnly, bool WriteOnly)
{
    InterfaceData Data;
    if(ReadOnly)
        Data.SetType("Data");
    else
        Data.SetType("Parameter");

    Data.SetData((double) 0.0);

    QString read_ID;
    QString read_Command;

    while(reader.readNextStartElement()){
       if(reader.name() == "ID")
            read_ID = reader.readElementText().trimmed();
       else if(reader.name() == "Command")
            read_Command = reader.readElementText().trimmed();
       else
           reader.skipCurrentElement();
   }

    AddState(read_ID, read_Command, Data, ReadOnly, WriteOnly);

}


void XmlReader::ReadInt(bool ReadOnly, bool WriteOnly)
{
    InterfaceData Data;
    if(ReadOnly)
        Data.SetType("Data");
    else
        Data.SetType("Parameter");

    Data.SetData((uint32_t) 0);

    QString read_ID;
    QString read_Command;

    while(reader.readNextStartElement()){
       if(reader.name() == "ID")
            read_ID = reader.readElementText().trimmed();
       else if(reader.name() == "Command")
            read_Command = reader.readElementText().trimmed();
       else
           reader.skipCurrentElement();
   }

    AddState(read_ID, read_Command, Data, ReadOnly, WriteOnly);

}


void XmlReader::ReadString(bool ReadOnly, bool WriteOnly)
{
    InterfaceData Data;
    if(ReadOnly)
        Data.SetType("Data");
    else
        Data.SetType("Parameter");

    Data.SetData(QString(" "));

    QString read_ID;
    QString read_Command;

    while(reader.readNextStartElement()){
       if(reader.name() == "ID")
            read_ID = reader.readElementText().trimmed();
       else if(reader.name() == "Command")
            read_Command = reader.readElementText().trimmed();
       else
           reader.skipCurrentElement();
   }

    AddState(read_ID, read_Command, Data, ReadOnly, WriteOnly);
}


void XmlReader::ReadBoolean(bool ReadOnly, bool WriteOnly)
{
    InterfaceData Data;
    if(ReadOnly)
        Data.SetType("Data");
    else
        Data.SetType("Parameter");

    Data.SetData(false);

    QString read_ID;
    QString read_Command;

    while(reader.readNextStartElement()){
       if(reader.name() == "ID")
            read_ID = reader.readElementText().trimmed();
       else if(reader.name() == "Command")
            read_Command = reader.readElementText().trimmed();
       else
           reader.skipCurrentElement();
   }

    AddState(read_ID, read_Command, Data, ReadOnly, WriteOnly);

}

void XmlReader::ReadGuiSelection(bool ReadOnly, bool WriteOnly)
{
    InterfaceData Data;
    QStringList Selection;


    if(ReadOnly)
        Data.SetType("Data");
    else
        Data.SetType("Parameter");


    QString read_ID;
    QString read_Command;

    while(reader.readNextStartElement()){
       if(reader.name() == "ID")
            read_ID = reader.readElementText().trimmed();
       else if(reader.name() == "Command")
            read_Command = reader.readElementText().trimmed();
       else if(reader.name() == "Selection")
            Selection.push_back(reader.readElementText().trimmed());
       else
           reader.skipCurrentElement();
   }

    Data.SetData(GuiSelection(Selection[0],Selection));

    AddState(read_ID, read_Command, Data, ReadOnly, WriteOnly);

}


void XmlReader::readVisaConnection()
{
    QString ID, Command;

    //Read DataType:
    QString DT = reader.attributes().value("DataType").trimmed().toString().toLower();
    bool ReadOnly = false;
    bool WriteOnly = false;

    if(reader.attributes().hasAttribute("Flag"))
    {
        if(reader.attributes().value("Flag").trimmed().toString().toLower() == "r")
            ReadOnly = true;
        if(reader.attributes().value("Flag").trimmed().toString().toLower() == "w")
            WriteOnly = true;
    }

    if(DT == "double")
            ReadDouble(ReadOnly, WriteOnly);
    else if (DT == "string")
            ReadString(ReadOnly, WriteOnly);
    else if (DT == "boolean")
            ReadBoolean(ReadOnly, WriteOnly);
    else if (DT == "guiselection")
            ReadGuiSelection(ReadOnly, WriteOnly);
    else if (DT == "int")
            ReadInt(ReadOnly, WriteOnly);
    else
        reader.raiseError("Data Type: " + DT + " is unknown. Known Types are int, double, string, boolean, guiselection!" );

}


void XmlReader::readConnection()
{
            readIP();
}

void XmlReader::readIP()
{
    IP = reader.attributes().value("IP").toString();
    reader.skipCurrentElement();
}


void XmlReader::AddState(QString read_ID, QString read_Command, InterfaceData Data, bool ReadOnly, bool WriteOnly)
{

    if(!read_ID.isEmpty() )
    {
        auto ID = DeviceName + "::" + read_ID.trimmed();
        if(read_Command.isEmpty())
        {
            if(WriteOnly)
            {
                StateSetCommands[ID] = " ";
            }
        }
        else
        {
            if(WriteOnly)
            {
                StateSetCommands[ID] = "VBS '" + read_Command.trimmed() +"=";
            }
            else if(ReadOnly)
            {
                StateRequests.push_back(QString("VBS? ") + QString("'Return =") + read_Command + QString("'; "));
                StateIds.push_back(ID);
            }
            else
            {
                StateRequests.push_back(QString("VBS? ") + QString("'Return =") + read_Command + QString("'; "));
                StateIds.push_back(ID);
                StateSetCommands[ID] = "VBS '" + read_Command.trimmed() +"=";
            }
        }
        m_data[ID] = Data;
        emit Messenger.MessageSender("publish", ID,  Data);

    }
}
