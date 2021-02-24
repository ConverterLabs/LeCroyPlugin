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

#include "LeCroyClass.h"
#include "visa.h"
#include "math.h"
#include <string>
#include <QDebug>


//#include <QVector>

LeCroy::LeCroy()  // std::string name, std::string ip)
{

    this->defaultRM = &defaultRMS;
    int state = viOpenDefaultRM(this->defaultRM);
    this->session = NULL;

}


int LeCroy::Connect(QString IP)
{
    CloseConnection();
    QString geraet = "VICP::" + IP;

    status = viOpen(*defaultRM, (ViRsrc)geraet.toStdString().c_str(), VI_NULL,2000, &session);
    if (status < VI_SUCCESS) {
        session = NULL;
        return status;
    }

    status = viSetAttribute(session, VI_ATTR_TMO_VALUE, 2000);
    if (status < VI_SUCCESS) {
        session = NULL;
        return status;
    }
    else {
        return status;
    }
}

int LeCroy::Calibrate()
{

    status = viSetAttribute(session, VI_ATTR_TMO_VALUE, 15000);
    if (status < VI_SUCCESS) {
        std::cout << "Setting time out failed";
    }

    QString command = "*CAL?";
    QString logMsg = "Calibrating oscilloscope";
    read(command, logMsg);

    std::cout << "Setting time out to 0.5 seconds";
    status = viSetAttribute(session, VI_ATTR_TMO_VALUE, 500);
    if (status < VI_SUCCESS) {
        std::cout << "Setting time out failed";
    }

    return 0;
}

bool LeCroy::CloseConnection()
{
    if (session!=NULL){
        status = viClose(session);
        if (status < VI_SUCCESS){
            return false;
        }
        else{
            return true;
        }
        session = NULL;
    }
    else{
        return  true;
    }
}


LeCroy::~LeCroy()
{

}



QStringList LeCroy::read(QString command, QString logMsg)
{
    // creating log message for debugging



    status = viWrite(session, (ViBuf)command.toStdString().c_str(), command.length(), &retCount);
    if (status < VI_SUCCESS)
    {
        qDebug() << "Sending command failed";
        qDebug()  << "VISA error code:" << status;
    }


    unsigned char buffer[1024];
    for (int i =0; i < sizeof(buffer); i++) buffer[i] = ' ';

    status = viRead(session, (ViPBuf)buffer, (ViUInt32)sizeof(buffer), &retCount);
    if (status < VI_SUCCESS) {
        qDebug()  << logMsg << "failed";
        qDebug()  << "VISA error code:" << status;
    }
    QString Answer(QString::fromLocal8Bit((const char*) buffer,retCount));
    auto AnswerParts = Answer.simplified().split(";");



    return AnswerParts;
}

QStringList LeCroy::CheckStates(QStringList CommandList)
{
    QString  logMsg = "";
    QStringList ErrorCommands;

    for(auto itt : CommandList)
    {
        QStringList Answers = read(itt,logMsg);
        if(Answers[0].contains("VBS Object doesn't support this property or method"))
        {
            ErrorCommands.push_back(itt);
        }
    }

    return ErrorCommands;
}

QStringList LeCroy::ReadState(QStringList CommandList)
{
    QString  command;
    for(int i = 0; i < CommandList.size();i++)
    {
        command.append(CommandList[i]);
    }

    QString  logMsg = "";
    QStringList Answers = read(command,logMsg);
    try
    {
        if(Answers.size()>2)
            for(int i = 0; i < Answers.size(); i++)
            {
                if(Answers[i].split(" ").size()>1)
                    Answers[i] = Answers[i].split(" ").at(1).simplified();
            }
    }
    catch(...)
    {
        qDebug() << Answers;
    }
    return Answers;
}

void LeCroy::write(QString command, QString logMsg)
{
    status = viWrite(session, (ViBuf)command.toStdString().c_str(), command.length(), &retCount);
    if (status < VI_SUCCESS) {
        qDebug()  << logMsg << "failed";
        qDebug()  << "VISA error code:" << status;
    }
}

std::vector<unsigned char> LeCroy::readbin(QString command, int size)
{
    status = viWrite(session, (ViBuf)command.toStdString().c_str(), command.length(), &retCount);
    if (status < VI_SUCCESS)
    {
        qDebug() << "Sending command failed";
        qDebug()  << "VISA error code:" << status;
    }

    std::vector<unsigned char> buffer;
    buffer.resize(220e6);
    for (int i =0; i< buffer.size(); i++) buffer[i] = ' ';

    status = viRead(session, (ViPBuf)&(buffer[0]), (ViUInt32)buffer.size(), &retCount);
    if (status < VI_SUCCESS) {

        qDebug()  << "VISA error code:" << status;
    }

    buffer.erase(buffer.begin()+retCount,buffer.end());
    return buffer;
}
