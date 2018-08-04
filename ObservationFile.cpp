#include "RinexDataCenter.h"
#include "RinexFileCenter.h"


/*--------------------------------------------------------------
 * Name     : ObservationData
 * Function : Constructor of class ObservationData
 *-------------------------------------------------------------*/
ObservationData::ObservationData()
{
    PRN           =  0,  sateClock     =  0, extraTime     =  0;
    frquencyNum_R =  0,  Delta0        =  0, Delta1        =  0;
    Delta2        =  0,  C1 = P1 = L1  =  0, C2 = P2 = L2  =  0;
    C3 = P3 = L3  =  0,  phaseWinding  =  0, tropDryDelay  =  0;
    tideCorre     =  0,  anteHeight    =  0, sagnacEffect  =  0;
    flagOfValid   =  0;
    relativityEffect    =  0;
    anteCenterCorre     =  0;
    tropWetDelayFunc    =  0;
}

/*------------------------------------------------------------------------------------
 * Name     : readFile
 * Function : Dynamically judging version and reading observe file (*.o)
 * Input    : const QString &filePath
 * Output   : bool (if read success)
 *-----------------------------------------------------------------------------------*/
bool ObservationFile::readFile(const QString &filePath)
{
    QFile ObserveFile(filePath);
    if(!  ObserveFile.open( QIODevice::ReadOnly ))
    {
          QMessageBox::warning(NULL,                   "warning",
                              "Observation File Open faild!",
                               QMessageBox::Yes, QMessageBox::Yes);
          return false;
    }

    int version = RinexFileCenter::readRinexVersion(filePath);                        // Judge version
    RinexFileCenter *rinexFilePtr = NULL;                                             // Dynamically reading file
//    if (version == 2)
//        rinexFilePtr = new Rinex2_FileCenter;
    if (version == 3)
        rinexFilePtr = new Rinex3_FileCenter;

    rinexFilePtr->readObserveFile(*this, filePath);

    delete rinexFilePtr;
    ObserveFile.close();
    return true;
}

/*                    ****************************************************                                       */
/*******************************    Rinex version 3 format files     *********************************************/
/*                    ****************************************************                                       */
/*------------------------------------------------------------------------------
 * Name     : initialObsTypeLibraty
 * Function : Initialize the satellite observation type libraty
 * Input    : NULL
 * Output   : NULL
 *-----------------------------------------------------------------------------*/
void Rinex3_FileCenter::initialObsTypeLibraty()
{
    GPS_ObsTypes.C1_type     = new QString[7]{"C1M","C1Y","C1W","C1P","C1L","C1S","C1C"};
    GPS_ObsTypes.L1_type     = new QString[7]{"L1M","L1Y","L1W","L1P","L1X","L1L","L1C"};
    GPS_ObsTypes.P2_type     = new QString[7]{"C2W","C2P","C2X","C2L","C2S","C2D","C2C"};
    GPS_ObsTypes.L2_type     = new QString[7]{"L2W","L2P","L2X","L2L","L2S","L2D","L2C"};
    GPS_ObsTypes.P3_type     = new QString[3]{"C5X","C5Q","C5I"};
    GPS_ObsTypes.L3_type     = new QString[3]{"L5X","L5Q","L5I"};
    GPS_ObsTypes.numC1 = 7, GPS_ObsTypes.numL1 = 7, GPS_ObsTypes.numP2 = 7;
    GPS_ObsTypes.numL2 = 7, GPS_ObsTypes.numP3 = 3, GPS_ObsTypes.numL3 = 3;

    BDS_ObsTypes.C1_type     = new QString[6]{"C2X","C2Q","C2I","C1X","C1Q","C1I"};
    BDS_ObsTypes.L1_type     = new QString[6]{"L2X","L2Q","L2I","L1X","L1Q","L1I"};
    BDS_ObsTypes.P2_type     = new QString[3]{"C7X","C7Q","C7I"};
    BDS_ObsTypes.L2_type     = new QString[3]{"L7X","L7Q","L7I"};
    BDS_ObsTypes.P3_type     = new QString[3]{"C6X","C6Q","C6I"};
    BDS_ObsTypes.L3_type     = new QString[3]{"L6X","L6Q","L6I"};
    BDS_ObsTypes.numC1 = 6, BDS_ObsTypes.numL1 = 6, BDS_ObsTypes.numP2 = 3;
    BDS_ObsTypes.numL2 = 3, BDS_ObsTypes.numP3 = 3, BDS_ObsTypes.numL3 = 3;

    GLONASS_ObsTypes.C1_type = new QString[2]{"C1P","C1C"};
    GLONASS_ObsTypes.L1_type = new QString[2]{"L1P","L1C"};
    GLONASS_ObsTypes.L2_type = new QString[2]{"L2P","L2C"};
    GLONASS_ObsTypes.P2_type = new QString[2]{"C2P","C2C"};
    GLONASS_ObsTypes.numC1 = 2, GLONASS_ObsTypes.numL1 = 2,
    GLONASS_ObsTypes.numP2 = 2, GLONASS_ObsTypes.numL2 = 2;

    Galileo_ObsTypes.C1_type = new QString[5]{"C1Z","C1X","C1C","C1B","C1A"};
    Galileo_ObsTypes.L1_type = new QString[5]{"L1Z","L1X","L1C","L1B","L1A"};
    Galileo_ObsTypes.P2_type = new QString[3]{"C5X","C5Q","C5I"};
    Galileo_ObsTypes.L2_type = new QString[3]{"L5X","L5Q","L5I"};
    Galileo_ObsTypes.P3_type = new QString[3]{"C7X","C7Q","C7I"};
    Galileo_ObsTypes.L3_type = new QString[3]{"L7X","L7Q","L7I"};
    Galileo_ObsTypes.numC1 = 5, Galileo_ObsTypes.numL1 = 5, Galileo_ObsTypes.numP2 = 3;
    Galileo_ObsTypes.numL2 = 3, Galileo_ObsTypes.numP3 = 3, Galileo_ObsTypes.numL3 = 3;

    QZSS_ObsTypes.C1_type    = new QString[5]{"C1Z","C1X","C1L","C1S","C1C"};
    QZSS_ObsTypes.L1_type    = new QString[5]{"L1Z","L1X","L1L","L1S","L1C"};
    QZSS_ObsTypes.P2_type    = new QString[3]{"C2X","C2L","C2S"};
    QZSS_ObsTypes.L2_type    = new QString[3]{"L2X","L2L","L2S"};
    QZSS_ObsTypes.numC1 = 5, QZSS_ObsTypes.numL1 = 5, QZSS_ObsTypes.numP2 = 3;
    QZSS_ObsTypes.numL2 = 3;

    SBAS_ObsTypes.C1_type    = new QString[2]{"C5I","C1C"};
    SBAS_ObsTypes.L1_type    = new QString[2]{"L5I","L1C"};
    SBAS_ObsTypes.P2_type    = new QString[2]{"C5X","C5Q"};
    SBAS_ObsTypes.L2_type    = new QString[2]{"L5X","L5Q"};
    SBAS_ObsTypes.numC1 = 2, SBAS_ObsTypes.numL1 = 2, SBAS_ObsTypes.numP2 = 2;
    SBAS_ObsTypes.numL2 = 2;

}

/*------------------------------------------------------------------------------
 * Name     : getTypeIndex
 * Function : Get the position of observation in a line
 * Input    : QVector<QString> obsTypeOrder .(Array saves type order read from file)
              Rinex3ObsTypeLibrary system .(Saves all observe type in Rinex 3 format)
              QString *obsTypeOfFile .(Saves observe type read in this file)
 * Output   : NULL
 *-----------------------------------------------------------------------------*/
int *Rinex3_FileCenter::getTypeIndex(QVector<QString> obsTypeOrder,
                                     Rinex3ObsTypeLibrary system,
                                     QString *obsTypeOfFile)
{
    int  *resTypeIndex   = new int[6]{-1,-1,-1,-1,-1,-1};                       // position array to return

    QString *helpType    = NULL;                                                // Type selection  helper
    int      helpNum     = 0;                                                   // Number of loop helper
    for (int type = 0; type <  6; type ++)                                      // Type loop
    {
        helpType  = NULL, helpNum = 0;
        /*----------------------  Get type ------------------------------------*/
        if (type == 0 && system.C1_type != NULL)
            helpType = system.C1_type, helpNum = system.numC1;
        if (type == 1 && system.L1_type != NULL)
            helpType = system.L1_type, helpNum = system.numL1;
        if (type == 2 && system.P2_type != NULL)
            helpType = system.P2_type, helpNum = system.numP2;
        if (type == 3 && system.L2_type != NULL)
            helpType = system.L2_type, helpNum = system.numL2;
        if (type == 4 && system.P3_type != NULL)
            helpType = system.P3_type, helpNum = system.numP3;
        if (type == 5 && system.L3_type != NULL)
            helpType = system.L3_type, helpNum = system.numL3;
        /*------------------ Find position index ------------------------------*/
        for(int k = 0; k < helpNum; k++)                                         // Library loop
        {
            for(int j = 0; j < obsTypeOrder.size(); j++)                         // Observation order loop
            {
                if(obsTypeOrder[j].indexOf(helpType[k]) >= 0)
                {
                   resTypeIndex[type]  = j;
                   obsTypeOfFile[type] = obsTypeOrder[j];
                   break;
                }
            }
            if (resTypeIndex[type] != -1)
                break;
        }
    }
    return resTypeIndex;
}

/*------------------------------------------------------------------------------
 * Name     : setTypePos
 * Function : Set all observation value index
 * Input    : QString system,
 *            int tempPosArray[6] .( Array where save the index of essential type)
 * Output   : int *(return special array)
 *-----------------------------------------------------------------------------*/
int *Rinex3_FileCenter::setTypePos(QString system, int tempPosArray[6])
{
    int *resPosArray = new int[9]{-1,-1,-1,-1,-1,-1,-1,-1,-1};

    if (system == "G"  || system == "C" || system == "E")
    {
        resPosArray[0] = tempPosArray[0];
        resPosArray[2] = tempPosArray[1];
        resPosArray[4] = tempPosArray[2];
        resPosArray[5] = tempPosArray[3];
        resPosArray[7] = tempPosArray[4];
        resPosArray[8] = tempPosArray[5];
    }
    if (system == "R"  || system == "J" || system == "S")
    {
        resPosArray[0] = tempPosArray[0];
        resPosArray[2] = tempPosArray[1];
        resPosArray[4] = tempPosArray[2];
        resPosArray[5] = tempPosArray[3];
    }
    return resPosArray;
}
/*------------------------------------------------------------------------------
 * Name     : getObserveDataInLine
 * Function : Decompose and store observation data in a linw
 * Input    : ObservationData &ObsData .( Observe data need to be modefied )
              int typePosArray[9] .( Array where save index of all types )
              QString lineQStr .( Target line )
 * Output   : None
 *-----------------------------------------------------------------------------*/
void Rinex3_FileCenter::getObserveDataInLine(ObservationData &ObsData,
                                             int typePosArray[9], QString lineQStr)
{
    ObsData.PRN = lineQStr.mid(1,2).toInt();                                    // Get satellite number
    for (int type = 0; type < 9; type++)                                        // Get satellite type
    {
        if (typePosArray[type] == -1)                                           // If not find
            continue;
        if (lineQStr.length() < typePosArray[type]*16 + 17)                     // If not have enough length
            continue;
        if (0 == type)                                                          // 0, 1, 2, 3, 4, 5, 6, 7, 8
            ObsData.C1 = lineQStr.mid(typePosArray[type]*16+3, 14).toDouble();  // C1 P1 L1 C2 P2 L2 C3 P3 L3
        else if (1 == type)
            ObsData.P1 = lineQStr.mid(typePosArray[type]*16+3, 14).toDouble();
        else if (2 == type)
            ObsData.L1 = lineQStr.mid(typePosArray[type]*16+3, 14).toDouble();
        else if (3 == type)
            ObsData.C2 = lineQStr.mid(typePosArray[type]*16+3, 14).toDouble();
        else if (4 == type)
            ObsData.P2 = lineQStr.mid(typePosArray[type]*16+3, 14).toDouble();
        else if (5 == type)
            ObsData.L2 = lineQStr.mid(typePosArray[type]*16+3, 14).toDouble();
        else if (6 == type)
            ObsData.C3 = lineQStr.mid(typePosArray[type]*16+3, 14).toDouble();
        else if (7 == type)
            ObsData.P3 = lineQStr.mid(typePosArray[type]*16+3, 14).toDouble();
        else if (8 == type)
            ObsData.L3 = lineQStr.mid(typePosArray[type]*16+3, 14).toDouble();
    }
}

/*------------------------------------------------------------------------------
 * Name     : readObserveFile
 * Function : Reading observe file of version 3 format(*.o)
 * Input    : const QString &filePath
 *            ObservationFile &obsFile .(Save observation data)
 * Output   : bool (if read success)
 *-----------------------------------------------------------------------------*/
bool Rinex3_FileCenter::readObserveFile(ObservationFile &obsFile,
                                        const QString &filePath)
{
    QFile obsInFile(filePath);
    if(!  obsInFile.open( QIODevice::ReadOnly ))
    {
          QMessageBox::warning(NULL,                   "warning",
                              "Observation File Open faild!",
                               QMessageBox::Yes, QMessageBox::Yes);
          return false;
    }

    /*--------------------------------------------------------------------------*/
    int  *GPS_ObsTypeIndex     = NULL;                                           // Use as 1x9 array to save
    int  *BDS_ObsTypeIndex     = NULL;
    int  *GLONASS_ObsTypeIndex = NULL;                                           // 0, 1, 2, 3, 4, 5, 6, 7, 8
    int  *Galileo_ObsTypeIndex = NULL;                                           // C1 P1 L1 C2 P2 L2 C3 P3 L3
    int  *QZSS_ObsTypeIndex    = NULL;
    int  *SBAS_ObsTypeIndex    = NULL;
    /*-------------------------------------------------------------------------*/
    int  *tempObsTyoeIndex     = NULL;                                           // Use as 1x6 array to save
                                                                                 // 0, 1, 2, 3, 4, 5
    /*-------------------------------------------------------------------------*/// C1 L1 P2 L2 P3 L3


    initialObsTypeLibraty();                                                     // Initialize observation types libraty
    /*-----------------------------   Read  Head -------------------------------*/
    QString lineQStr  = "";
    do
    {
        if (lineQStr.indexOf("MARKER NAME")  >= 0)
            obsFile.MARKER_NAME        = lineQStr.mid(0,  4).trimmed();

        if (lineQStr.indexOf("ANT # / TYPE") >= 0)
        {
            obsFile.ANT                = lineQStr.mid(0, 20).trimmed();
            obsFile.ANT_TYPE           = lineQStr.mid(20,20).trimmed();
        }
        if (lineQStr.indexOf("REC # / TYPE / VERS")  >= 0)
        {
            obsFile.REC                = lineQStr.mid(0, 20).trimmed();
            obsFile.REC_TYPE           = lineQStr.mid(20,20).trimmed();
        }
        if (lineQStr.indexOf("APPROX POSITION XYZ")  >= 0)
        {
            obsFile.APPROX_POSITION[0] = lineQStr.mid(0, 14).toDouble();
            obsFile.APPROX_POSITION[1] = lineQStr.mid(14,14).toDouble();
            obsFile.APPROX_POSITION[2] = lineQStr.mid(28,14).toDouble();
        }
        if (lineQStr.indexOf("ANTENNA: DELTA H/E/N") >= 0)
        {
            obsFile.ANTENNA_DELTA_H    = lineQStr.mid(0, 14).toDouble();
            obsFile.ANTENNA_DELTA_E    = lineQStr.mid(14,14).toDouble();
            obsFile.ANTENNA_DELTA_N    = lineQStr.mid(28,14).toDouble();
        }

        if (lineQStr.indexOf("SYS / # / OBS TYPES")  >= 0)                       // Judge and save observation types order
        {
            QString system             = lineQStr.mid(0,1);
            int     obsTypeNum         = lineQStr.mid(4,2).toInt();
            QVector <QString> obsTypeOrder;                                      // Save observation types order

            for(int n = 0; n < obsTypeNum; n++)
                obsTypeOrder.push_back(lineQStr.mid(7+4*n, 3));

            if (obsTypeNum > 13)
            {
                lineQStr  = obsInFile.readLine();
                for(int n = 0; n < obsTypeNum - 13; n++)
                    obsTypeOrder.push_back(lineQStr.mid(7+4*n, 3));
            }
            /*--------------- Classification search index vector  ----------*/
            if(system == "G")
            {
                obsFile.GPS_ObserveNum  = obsTypeNum;
                tempObsTyoeIndex       = getTypeIndex(obsTypeOrder, GPS_ObsTypes,
                                                      GPS_TYPES);
                GPS_ObsTypeIndex       = setTypePos(system, tempObsTyoeIndex);
            }
            else if(system == "C")
            {
                obsFile.BDS_ObserveNum  = obsTypeNum;
                tempObsTyoeIndex        = getTypeIndex(obsTypeOrder, BDS_ObsTypes,
                                                       BDS_TYPES);
                BDS_ObsTypeIndex        = setTypePos(system, tempObsTyoeIndex);
            }
            else if(system == "R")
            {
                obsFile.GLONASS_ObserveNum = obsTypeNum;
                tempObsTyoeIndex        = getTypeIndex(obsTypeOrder, GLONASS_ObsTypes,
                                                       GLONASS_TYPES);
                GLONASS_ObsTypeIndex    = setTypePos(system, tempObsTyoeIndex);
            }
            else if(system == "E")
            {
                obsFile.Galileo_ObserveNum = obsTypeNum;
                tempObsTyoeIndex        = getTypeIndex(obsTypeOrder, Galileo_ObsTypes,
                                                       Galileo_TYPES);
                Galileo_ObsTypeIndex    = setTypePos(system, tempObsTyoeIndex);
            }
            else if(system == "J")
            {
                obsFile.QZSS_ObserveNum = obsTypeNum;
                tempObsTyoeIndex        = getTypeIndex(obsTypeOrder, QZSS_ObsTypes,
                                                       QZSS_TYPES);
                QZSS_ObsTypeIndex       = setTypePos(system, tempObsTyoeIndex);
            }
            else if(system == "S")
            {
                obsFile.SBAS_ObserveNum = obsTypeNum;
                tempObsTyoeIndex        = getTypeIndex(obsTypeOrder, SBAS_ObsTypes,
                                                       SBAS_TYPES);
                SBAS_ObsTypeIndex       = setTypePos(system, tempObsTyoeIndex);
            }
        }
        if (lineQStr.indexOf("INTERVAL") >= 0)
            obsFile.INTERVAL            = lineQStr.mid(3,8).toDouble();

        lineQStr = obsInFile.readLine();
    }
    while (lineQStr.indexOf("END OF HEADER") < 0);

    /*-----------------------------   Read  Data -------------------------------*/
    while ((lineQStr = obsInFile.readLine()) != "")
    {
        if (lineQStr.mid(31,1).toInt() == 0)
        {
            EpochObservationData epochData;
            epochData.myTime.EPT.year   = lineQStr.mid(2,  4).toInt();
            epochData.myTime.EPT.month  = lineQStr.mid(7,  2).toInt();
            epochData.myTime.EPT.day    = lineQStr.mid(10, 2).toInt();
            epochData.myTime.EPT.hour   = lineQStr.mid(13, 2).toInt();
            epochData.myTime.EPT.minute = lineQStr.mid(16, 2).toInt();
            epochData.myTime.EPT.second = lineQStr.mid(19,10).toDouble();

            epochData.sateNum           = lineQStr.mid(33, 2).toInt();

            /*---------------  Choose system and save data ---------------------*/
            for (int n = 0; n < epochData.sateNum; n++)
            {
                lineQStr         = obsInFile.readLine();
                ObservationData obsData;
                obsData.sateType = lineQStr.mid(0,1);
                if (obsData.sateType == " ")
                    obsData.sateType =  "G";

                if (obsData.sateType == "G")
                    getObserveDataInLine(obsData, GPS_ObsTypeIndex, lineQStr);
                else if (obsData.sateType == "C")
                    getObserveDataInLine(obsData, BDS_ObsTypeIndex, lineQStr);
                else if (obsData.sateType == "R")
                    getObserveDataInLine(obsData, GLONASS_ObsTypeIndex, lineQStr);
                else if (obsData.sateType == "E")
                    getObserveDataInLine(obsData, Galileo_ObsTypeIndex, lineQStr);
                else if (obsData.sateType == "J")
                    getObserveDataInLine(obsData, QZSS_ObsTypeIndex, lineQStr);
                else if (obsData.sateType == "S")
                    getObserveDataInLine(obsData, SBAS_ObsTypeIndex, lineQStr);

                epochData.epochObserveData.push_back(obsData);
            }
            obsFile.AllObservationData.push_back(epochData);
        }
        else                                                                                // ignore the invalid data
        {
            int  ignoreRow = lineQStr.mid(33,2).toInt();
            for (int r = 0; r < ignoreRow; r++)
                 lineQStr  = obsInFile.readLine();
        }
    }
    delete tempObsTyoeIndex;
    delete GPS_ObsTypeIndex;
    delete BDS_ObsTypeIndex;
    delete GLONASS_ObsTypeIndex;
    delete Galileo_ObsTypeIndex;
    delete QZSS_ObsTypeIndex;
    delete SBAS_ObsTypeIndex;
    delete tempObsTyoeIndex;

    obsInFile.close();
    return true;
}











//            temp1.julday=julday(temp1.epoch_time[0],temp1.epoch_time[1],temp1.epoch_time[2],
            //temp1.epoch_time[3]+temp1.epoch_time[4]/60+temp1.epoch_time[5]/3600);
//            gps_time(temp1.julday,temp1.week, temp1.gpst);
//            m_time t;
//            t.julday=temp1.julday;
//            check_time(t);
//            temp1.mjd=t.mjulday;
//             temp1.doy=doy(temp1.epoch_time[0],temp1.epoch_time[1],temp1.epoch_time[2]);





















/*     QFileInfo fi;
    fi = QFileInfo(path_file);
    QFile readfile(path_file);
    if(!readfile.open(QIODevice::ReadOnly))
    {
        qDebug()<<"can't open "<<endl;
        exit(EXIT_FAILURE);
    }
    observe[i].pathname=fi.absolutePath();

    QString pt=observe[i].pathname+"/result";
    observe[i].pathname=observe[i].pathname+"/result/";
    //Create output folders
    QDir *mp = new QDir;
    bool exist = mp->exists(pt);
    if(!exist)
    {
        bool ok=mp->mkdir(pt);
        if(!ok)
        {
            QMessageBox mes;
            mes.setText("Create a file failed!");
            mes.exec();
        }
    }
    QString station=fi.fileName();
    int pos=station.indexOf('.');
    observe[i].station=station.mid(0,pos);
    QTextStream str(&readfile);
    QString line;
    QString rec="";
    line=str.readLine();
*/
