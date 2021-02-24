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

#include "readoscichannel.h"

ReadOsciChannel::ReadOsciChannel( QString DeviceName_, MessengerClass &messenger_, LeCroy &Osci_ ,  std::map<QString, DataStorage>& data):
    m_data(data),
    DeviceName(DeviceName_),
    Osci(Osci_),
    messenger(messenger_)
{

}

void ReadOsciChannel::ReadChannel(int Channel)
{
    QString Chan = "C" + QString::number(Channel);
    QString IDM = DeviceName + "::Acquisition::MemorySize";
    if(this->m_data[DeviceName + "::Channel::" + Chan + "::State"].GetBool())
    {
        std::vector<unsigned char> buffer = this->Osci.readbin(Chan + ":WaveForm? ALL",(int)(this->m_data[IDM].GetDouble()) + 500);
        if(buffer.size()<500)
        {
            messenger.Info("Osci doesn't reply to " + Chan);
        }
        else
        {

            int fp = *((int*)&buffer[124+21])+21+367;
            int lp = *((int*)&buffer[128+21]);
            float VERTICAL_GAIN = *((float*)&buffer[156+21]);
            float VERTICAL_OFFSET = *((float*)&buffer[160+21]);
            float HORIZ_INTERVAL = *((float*)&buffer[176+21]);
            double HORIZ_OFFSET = *((double*)&buffer[180+21]);

            //Set new buffered data :)
            QString ID = DeviceName + "::Buffered::" + Chan;
            InterfaceData _Data;
            _Data.SetDataType("vector<double>");
            _Data.SetType("Data");

            std::vector<double> T;
            std::vector<double> Y;

            int j = 0;
            for(int i = fp+1 ;i<lp*2 && i < buffer.size();i=i+2)
            {
                int16_t tmp2 = *((int16_t*)&buffer[i]);
                Y.push_back(tmp2*VERTICAL_GAIN-VERTICAL_OFFSET);
                double tmp1 = (HORIZ_INTERVAL*(j++)+HORIZ_OFFSET);
                T.push_back(tmp1);
            }
            if(T.size() && Y.size())
            {
                _Data.SetData(DataPair( boost::shared_ptr<std::vector<double>>(new std::vector<double>(T)), boost::shared_ptr<std::vector<double>>(new std::vector<double>(Y))));
                messenger.MessageSender("set", ID,  _Data);
            }
        }
    }
}
