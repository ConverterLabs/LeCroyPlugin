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

#include <QObject>
#include <QtPlugin>
#include "worker.h"
#include "DataType/datastorage.h"

#undef GetObject// undefine this windows macro

class LeCroyPlugin_Class : public QObject, public Platform_Interface
{
    Q_OBJECT

public:
    QObject* GetObject() Q_DECL_OVERRIDE;
    InterfaceData* GetSymbol(const QString &ID) Q_DECL_OVERRIDE;
    LeCroyPlugin_Class(QObject* messenger_);
    ~LeCroyPlugin_Class();

public slots:
    void MessageReceiver(const QString &Command, const QString &ID, InterfaceData Data) Q_DECL_OVERRIDE;
    void ThreadFinished();
private slots:
    void Initialize();

signals:
    void MessageSender(const QString &Command, const QString &ID, InterfaceData Data) Q_DECL_OVERRIDE;

private:
    void Info(QString text);
    void Error(QString text);
    void StatusMessage(QString text);

    WorkClass* Work;
    QObject* messenger;
    QThread* Worker;

    bool ThreadFinishedSignal = false;

  //

};

class LeCroyPlugin_Class_Fabric : public QObject, Platform_Fabric
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "TUD/ETI/LE/LAB_GUI_V1")
    Q_INTERFACES(Platform_Fabric)
public:
    Platform_Interface* GetInterface(QObject* messenger) Q_DECL_OVERRIDE {
        return dynamic_cast<Platform_Interface*>(new LeCroyPlugin_Class(messenger))
                ;}
};


