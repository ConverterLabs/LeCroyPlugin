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

//#define LECROYLIBRARY_EXPORT

#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <iostream>

#include <map>
#include <boost\any.hpp>
#include <boost\thread.hpp>
#include <boost\shared_ptr.hpp>
#include <boost\variant.hpp>

#include "visa.h"

#include <QString>
#include <QStringList>

#include <QBuffer>


class LeCroy
{
public:
    LeCroy();
	~LeCroy();	  
    int Connect(QString IP);
    int Calibrate();
    bool CloseConnection();

    QStringList ReadState(QStringList);
    void write(QString command, QString logMsg);
    QStringList read(QString command, QString logMsg);
    std::vector<unsigned char> readbin(QString command, int size);

    int GetStatus() { return status;}


private:


	    // for VISA-communication
	ViSession defaultRMS;       // connection to main VISA driver
    ViSession *defaultRM;       // connection to main VISA driver
    ViSession session;          // connection to device
    ViStatus status;            // communication status
    ViUInt32 retCount;          // retur count from string I/O


};

