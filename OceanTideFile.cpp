#include "DataCenter.h"
#include "MyFunctionCenter.h"
/*------------------ Define static member-------------------------------------------*/
vector <OceanData> OceanTideFile::allOceanData;
bool    OceanTideFile::isModeEasy = false;                                           // The default is set to a complex mode
/*----------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 * Name     : readFile
 * Function : read precision clock file (*.blq)
 * Input    : const QString &filePath
 * Output   : bool (if read success)
 *-----------------------------------------------------------------------------*/
bool OceanTideFile::readFile(const QString &filePath)
{
    QFile OceanFile(filePath);
    if (! OceanFile.open( QIODevice::ReadOnly))
    {
          QMessageBox::warning(NULL,              "warning",
                              "Ocean tide File Open faild!",
                               QMessageBox::Yes, QMessageBox::Yes);
          return false;
    }


    QString lineQStr   = "";
    int observeTypeNum =  0;
    vector <ObserveData> obseveDataPattern;                                          // A pattern to save type by an order
    /*---------------- Read header --------------------------------------------------*/
    while ((lineQStr = OceanFile.readLine()) != "")
    {
        if (lineQStr.indexOf("COLUMN ORDER") >= 0)
        {            
            lineQStr       = lineQStr.trimmed();
            observeTypeNum =(lineQStr.length() - 16) / 4;
            for (int i = 0; i < observeTypeNum; i++)
            {
                ObserveData obdata;
                obdata.observeType = lineQStr.mid(16 + 4*i, 4).trimmed();            // Get type of observe value
                obseveDataPattern.push_back(obdata);
            }
        }
        if (lineQStr.indexOf("END HEADER")>= 0)
            break;
    }
    /*---------------- Read data ----------------------------------------------------*/
    while ((lineQStr = OceanFile.readLine()) != "")
    {
        OceanData oceanData;
        lineQStr                       = OceanFile.readLine();
        oceanData.stationName          = lineQStr.mid(2,4);                          // Get station name
        lineQStr                       = OceanFile.readLine();
        if ( isModeEasy)
        {
            oceanData.Longitude        = lineQStr.mid(58,10).toDouble();
            oceanData.Latitude         = lineQStr.mid(68,10).toDouble();
        }

        if (!isModeEasy)
        {
            QString Month[12];                                                       // Define a months library
            Month[0] = "JAN", Month[1] = "FEB", Month[2 ] = "MAR", Month[3 ] = "APR",
            Month[4] = "MAY", Month[5] = "JUN", Month[6 ] = "Jul", Month[7 ] = "AUG",
            Month[8] = "SEP", Month[9] = "OCT", Month[10] = "NOV", Month[11] = "DEC";

            lineQStr                   = OceanFile.readLine();
            /*------------- Get time ----------------------------------*/
            oceanData.myTime.EPT.year  = lineQStr.mid(63,4).toInt();
            for (int i = 0; i < 12; i++)
            {
                if (lineQStr.mid(68,3) == Month[i])
                {
                    oceanData.myTime.EPT.month = i + 1;
                    break;
                }
            }
            oceanData.myTime.EPT.day   = lineQStr.mid(72,2).toInt();
            oceanData.myTime.EPT.hour  = 0;
            oceanData.myTime.EPT.minute= 0;
            oceanData.myTime.EPT.second= 0;
            oceanData.myTime           = MyFuncionCenter::
                                         timeIntegrator(oceanData.myTime.EPT);
            /*----------- Get position --------------------------------*/
            lineQStr                   = OceanFile.readLine();
            oceanData.Longitude        = lineQStr.mid(49,10).toDouble();
            oceanData.Latitude         = lineQStr.mid(59,10).toDouble();
            lineQStr                   = lineQStr.trimmed();
            if (lineQStr.length() > 70)
                oceanData.Height       = lineQStr.mid(69,9).toDouble();
            else
                oceanData.Height       = 0;
        }

        oceanData.tideData             =  obseveDataPattern;                        // Save array follow the order in header
        for (int i = 0; i < 6; i++)
        {
            lineQStr                   = OceanFile.readLine();
            for (int j = 0; j < observeTypeNum; j++)
            {
                 oceanData.tideData[j].
                       observeValue[i] = lineQStr.mid(1+7*j, 7).toDouble();         // Read the data every seven spaces
            }
        }
        allOceanData.push_back(oceanData);                                          // Push this data block into array
     }

    OceanFile.close();
    return true;
}