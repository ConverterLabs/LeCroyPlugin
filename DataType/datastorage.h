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

#pragma once
#include "../LabAnalyser/plugins/platforminterface.h"
#include "../LabAnalyser/plugins/InterfaceDataType.h"
#include <QTimer>
#include "../Messenger/messengerclass.h"


class DataStorage: public QObject, public InterfaceData
{
    Q_OBJECT

public:
    DataStorage();
    void SetDataTimeOut(boost::variant<DataTypes> DataIn, QString IDin, MessengerClass* Messenger_);
    operator InterfaceData() const;
    DataStorage& operator =( InterfaceData&);
    boost::variant<DataTypes> GetData();
       ~DataStorage();

signals:
    void TimeOut(QString ID);

private slots:
    void TimeOut(void);

private:
        bool UpdateRequested;
        boost::variant<DataTypes> DataTarget;
        QTimer *Timer = NULL;
        QString _ID;
        MessengerClass* Messenger;
};
