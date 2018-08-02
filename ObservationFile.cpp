#include "RinexDataCenter.h"
#include "RinexFileCenter.h"

/*------------------------------------------------------------------------------------
 * Name     : readFile
 * Function : Dynamically judging version and reading observe file (*.o)
 * Input    : const QString &filePath
 * Output   : bool (if read success)
 *-----------------------------------------------------------------------------------*/
bool ObservationFile::readFile(const QString &filePath)
{
    QFile AntennaFile(filePath);
    if(!  AntennaFile.open( QIODevice::ReadOnly ))
    {
          QMessageBox::warning(NULL,                   "warning",
                              "Observation File Open faild!",
                               QMessageBox::Yes, QMessageBox::Yes);
          return false;
    }

    int version = RinexFileCenter::rinex_version(filePath);                           // Judge version
    RinexFileCenter *rinexFilePtr = NULL;                                             // Dynamically reading file
    if (version == 2)
        rinexFilePtr = new Rinex2_FileCenter;
    if (version == 3)
        rinexFilePtr = new Rinex3_FileCenter;

    rinexFilePtr->readObserveFile(*this, filePath);

    delete rinexFilePtr;
    return true;
}


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
    GPS_ObsTypes.numC1 = 2, GPS_ObsTypes.numL1 = 2, GPS_ObsTypes.numP2 = 2;
    GPS_ObsTypes.numL2 = 2;

    Galileo_ObsTypes.C1_type = new QString[5]{"C1Z","C1X","C1C","C1B","C1A"};
    Galileo_ObsTypes.L1_type = new QString[5]{"L1Z","L1X","L1C","L1B","L1A"};
    Galileo_ObsTypes.P2_type = new QString[3]{"C5X","C5Q","C5I"};
    Galileo_ObsTypes.L2_type = new QString[3]{"L5X","L5Q","L5I"};
    Galileo_ObsTypes.P3_type = new QString[3]{"C7X","C7Q","C7I"};
    Galileo_ObsTypes.L3_type = new QString[3]{"L7X","L7Q","L7I"};
    GPS_ObsTypes.numC1 = 5, GPS_ObsTypes.numL1 = 5, GPS_ObsTypes.numP2 = 3;
    GPS_ObsTypes.numL2 = 3, GPS_ObsTypes.numP3 = 3, GPS_ObsTypes.numL3 = 3;

    QZSS_ObsTypes.C1_type    = new QString[5]{"C1Z","C1X","C1L","C1S","C1C"};
    QZSS_ObsTypes.L1_type    = new QString[5]{"L1Z","L1X","L1L","L1S","L1C"};
    QZSS_ObsTypes.P2_type    = new QString[3]{"C2X","C2L","C2S"};
    QZSS_ObsTypes.L2_type    = new QString[3]{"L2X","L2L","L2S"};
    GPS_ObsTypes.numC1 = 5, GPS_ObsTypes.numL1 = 5, GPS_ObsTypes.numP2 = 3;
    GPS_ObsTypes.numL2 = 3;

    SBAS_ObsTypes.C1_type    = new QString[2]{"C5I","C1C"};
    SBAS_ObsTypes.L1_type    = new QString[2]{"L5I","L1C"};
    SBAS_ObsTypes.P2_type    = new QString[2]{"C5X","C5Q"};
    SBAS_ObsTypes.L2_type    = new QString[2]{"L5X","L5Q"};
    GPS_ObsTypes.numC1 = 2, GPS_ObsTypes.numL1 = 2, GPS_ObsTypes.numP2 = 2;
    GPS_ObsTypes.numL2 = 2;

}

/*------------------------------------------------------------------------------
 * Name     : getTypeIndex
 * Function : Get the position of observation in a line
 * Input    : NULL
 * Output   : NULL
 *-----------------------------------------------------------------------------*/
int *Rinex3_FileCenter::getTypeIndex(QVector<QString> obsTypeOrder,
                                     Rinex3ObsTypeLibrary system,
                                     QString *obsTypeOfFile)
{
    int  resTypeIndex[6] = {-1, -1, -1, -1, -1, -1};                            // position array to return
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



bool Rinex3_FileCenter::readObserveFile(ObservationFile &obFile, const QString &filePath)
{
    QFile obsInFile(filePath);
    if(!  obsInFile.open( QIODevice::ReadOnly ))
    {
          QMessageBox::warning(NULL,                   "warning",
                              "Observation File Open faild!",
                               QMessageBox::Yes, QMessageBox::Yes);
          return false;
    }


    initialObsTypeLibraty();
    QString lineQStr  = "";
    do
    {
        if (lineQStr.indexOf("MARKER NAME")  >= 0)
            obFile.MARKER_NAME        = lineQStr.mid(0,  4).trimmed();

        if (lineQStr.indexOf("ANT # / TYPE") >= 0)
        {
            obFile.ANT                = lineQStr.mid(0, 20).trimmed();
            obFile.ANT_TYPE           = lineQStr.mid(20,20).trimmed();
        }
        if(lineQStr.indexOf("REC # / TYPE / VERS")   >= 0)
        {
            obFile.REC                = lineQStr.mid(0, 20).trimmed();
            obFile.REC_TYPE           = lineQStr.mid(20,20).trimmed();
        }
        if (lineQStr.indexOf("APPROX POSITION XYZ")  >= 0)
        {
            obFile.APPROX_POSITION[0] = lineQStr.mid(0, 14).toDouble();
            obFile.APPROX_POSITION[1] = lineQStr.mid(14,14).toDouble();
            obFile.APPROX_POSITION[2] = lineQStr.mid(28,14).toDouble();
        }
        if (lineQStr.indexOf("ANTENNA: DELTA H/E/N") >= 0)
        {
            obFile.ANTENNA_DELTA_H    = lineQStr.mid(0, 14).toDouble();
            obFile.ANTENNA_DELTA_E    = lineQStr.mid(14,14).toDouble();
            obFile.ANTENNA_DELTA_N    = lineQStr.mid(28,14).toDouble();
        }


        if (lineQStr.indexOf("SYS / # / OBS TYPES") >= 0)
        {
            QString system            = lineQStr.mid(0,1);
            int     obsTypeNum        = lineQStr.mid(4,2).toInt();
            QVector <QString> obsTypeOrder;

            for(int n = 0; n < obsTypeNum; n++)
                obsTypeOrder.push_back(lineQStr.mid(7+4*n, 3));

            if (obsTypeNum > 13)
            {
                lineQStr  = obsInFile.readLine();
                for(int n = 0; n < obsTypeNum - 13; n++)
                    obsTypeOrder.push_back(lineQStr.mid(7+4*n, 3));

            }
            /*----------------------------------*/
            if(sys=="G")
            {
                observe[i].GPS_TYPES_NUMBER=typenumber;
//                RINEX_3_type type;
//                get_types("GPS",type);
                QVector<double> tp=gettypepos(obstype,type,observe[i].GPS_TYPES);
                observe[i].GPS_TYPES_pos[0]=tp[0];
                observe[i].GPS_TYPES_pos[2]=tp[1];
                observe[i].GPS_TYPES_pos[4]=tp[2];
                observe[i].GPS_TYPES_pos[5]=tp[3];
                observe[i].GPS_TYPES_pos[7]=tp[4];
                observe[i].GPS_TYPES_pos[8]=tp[5];
            }
            if(sys=="R")
            {
                observe[i].GLONASS_TYPES_NUMBER=typenumber;
                RINEX_3_type type;
                get_types("GLONASS",type);
                QVector<double> tp=gettypepos(obstype,type,observe[i].GLONASS_TYPES);
                observe[i].GLONASS_TYPES_pos[0]=tp[0];
                observe[i].GLONASS_TYPES_pos[2]=tp[1];
                observe[i].GLONASS_TYPES_pos[4]=tp[2];
                observe[i].GLONASS_TYPES_pos[5]=tp[3];
            }
            if(sys=="C")
            {
                observe[i].BDS_TYPES_NUMBER=typenumber;
                RINEX_3_type type;
                get_types("BDS",type);
                QVector<double> tp=gettypepos(obstype,type,observe[i].BDS_TYPES);
                observe[i].BDS_TYPES_pos[0]=tp[0];
                observe[i].BDS_TYPES_pos[2]=tp[1];
                observe[i].BDS_TYPES_pos[4]=tp[2];
                observe[i].BDS_TYPES_pos[5]=tp[3];
                observe[i].BDS_TYPES_pos[7]=tp[4];
                observe[i].BDS_TYPES_pos[8]=tp[5];
            }
            if(sys=="E")
            {
                observe[i].Galileo_TYPES_NUMBER=typenumber;
                RINEX_3_type type;
                get_types("GALILEO",type);
                QVector<double> tp=gettypepos(obstype,type,observe[i].Galileo_TYPES);
                observe[i].Galileo_TYPES_pos[0]=tp[0];
                observe[i].Galileo_TYPES_pos[2]=tp[1];
                observe[i].Galileo_TYPES_pos[4]=tp[2];
                observe[i].Galileo_TYPES_pos[5]=tp[3];
                observe[i].Galileo_TYPES_pos[7]=tp[4];
                observe[i].Galileo_TYPES_pos[8]=tp[5];
            }
            if(sys=="J")
            {
                observe[i].QZSS_TYPES_NUMBER=typenumber;
                RINEX_3_type type;
                get_types("QZSS",type);
                QVector<double> tp=gettypepos(obstype,type,observe[i].QZSS_TYPES);
                observe[i].QZSS_TYPES_pos[0]=tp[0];
                observe[i].QZSS_TYPES_pos[2]=tp[1];
                observe[i].QZSS_TYPES_pos[4]=tp[2];
                observe[i].QZSS_TYPES_pos[5]=tp[3];
            }
            if(sys=="S")
            {
                observe[i].SBAS_TYPES_NUMBER=typenumber;
                RINEX_3_type type;
                get_types("SBAS",type);
                QVector<double> tp=gettypepos(obstype,type,observe[i].SBAS_TYPES);
                observe[i].SBAS_TYPES_pos[0]=tp[0];
                observe[i].SBAS_TYPES_pos[2]=tp[1];
                observe[i].SBAS_TYPES_pos[4]=tp[2];
                observe[i].SBAS_TYPES_pos[5]=tp[3];

            }

        }
        if (line.indexOf("INTERVAL")>=0)
        {
            observe[i].INTERVAL=line.mid(3,8).toDouble();
        }
        line=str.readLine();
    }
    while (lineQStr.indexOf("END OF HEADER") < 0);   //Headers read complete

    while ((line=str.readLine())!="")
    {
        if(line.mid(31,1).toInt()==0)
        {
            satellite_o temp1;
            temp1.epoch_time[0]=line.mid(2,4).toDouble();
            temp1.epoch_time[1]=line.mid(7,2).toDouble();
            temp1.epoch_time[2]=line.mid(10,2).toDouble();
            temp1.epoch_time[3]=line.mid(13,2).toDouble();
            temp1.epoch_time[4]=line.mid(16,2).toDouble();
            temp1.epoch_time[5]=line.mid(19,10).toDouble();

            temp1.julday=julday(temp1.epoch_time[0],temp1.epoch_time[1],temp1.epoch_time[2],temp1.epoch_time[3]+temp1.epoch_time[4]/60+temp1.epoch_time[5]/3600);
            gps_time(temp1.julday,temp1.week, temp1.gpst);
            m_time t;
            t.julday=temp1.julday;
            check_time(t);
            temp1.mjd=t.mjulday;
             temp1.doy=doy(temp1.epoch_time[0],temp1.epoch_time[1],temp1.epoch_time[2]);
            temp1.satellite_num = line.mid(33,2).toInt();
            temp1.satellite_observe.resize(temp1.satellite_num);
            for(int j=0;j<temp1.satellite_num;j++)
            {
                line=str.readLine();
                temp1.satellite_observe[j].PRN=line.mid(1,2).toInt();
                temp1.satellite_observe[j].Sat_Type=line.mid(0,1);
                if( temp1.satellite_observe[j].Sat_Type==" ")
                {
                    temp1.satellite_observe[j].Sat_Type=="G";
                }
                if(temp1.satellite_observe[j].Sat_Type=="G")
                {
                    if(observe[i].GPS_TYPES_pos[0]!=-1)
                    {
                        if(line.length()>=(observe[i].GPS_TYPES_pos[0]*16+17))
                        {
                            temp1.satellite_observe[j].C1=line.mid(observe[i].GPS_TYPES_pos[0]*16+3,14).toDouble();
                        }
                    }
                    if(observe[i].GPS_TYPES_pos[1]!=-1)
                    {
                        if(line.length()>=(observe[i].GPS_TYPES_pos[1]*16+17))
                        {
                            temp1.satellite_observe[j].P1=line.mid(observe[i].GPS_TYPES_pos[1]*16+3,14).toDouble();
                        }
                    }
                    if(observe[i].GPS_TYPES_pos[2]!=-1)
                    {
                        if(line.length()>=(observe[i].GPS_TYPES_pos[2]*16+17))
                        {
                            temp1.satellite_observe[j].L1=line.mid(observe[i].GPS_TYPES_pos[2]*16+3,14).toDouble();
                        }
                    }
                    if(observe[i].GPS_TYPES_pos[3]!=-1)
                    {
                        if(line.length()>=(observe[i].GPS_TYPES_pos[3]*16+17))
                        {
                            temp1.satellite_observe[j].C2=line.mid(observe[i].GPS_TYPES_pos[3]*16+3,14).toDouble();
                        }
                    }
                    if(observe[i].GPS_TYPES_pos[4]!=-1)
                    {
                        if(line.length()>=(observe[i].GPS_TYPES_pos[4]*16+17))
                        {
                            temp1.satellite_observe[j].P2=line.mid(observe[i].GPS_TYPES_pos[4]*16+3,14).toDouble();
                        }
                    }
                    if(observe[i].GPS_TYPES_pos[5]!=-1)
                    {
                        if(line.length()>=(observe[i].GPS_TYPES_pos[5]*16+17))
                        {
                            temp1.satellite_observe[j].L2=line.mid(observe[i].GPS_TYPES_pos[5]*16+3,14).toDouble();
                        }
                    }
                    if(observe[i].GPS_TYPES_pos[6]!=-1)
                    {
                        if(line.length()>=(observe[i].GPS_TYPES_pos[6]*16+17))
                        {
                            temp1.satellite_observe[j].C3=line.mid(observe[i].GPS_TYPES_pos[6]*16+3,14).toDouble();
                        }
                    }
                    if(observe[i].GPS_TYPES_pos[7]!=-1)
                    {
                        if(line.length()>=(observe[i].GPS_TYPES_pos[7]*16+17))
                        {
                            temp1.satellite_observe[j].P3=line.mid(observe[i].GPS_TYPES_pos[7]*16+3,14).toDouble();
                        }
                    }
                    if(observe[i].GPS_TYPES_pos[8]!=-1)
                    {
                        if(line.length()>=(observe[i].GPS_TYPES_pos[8]*16+17))
                        {
                            temp1.satellite_observe[j].L3=line.mid(observe[i].GPS_TYPES_pos[8]*16+3,14).toDouble();
                        }
                    }

                }
                if(temp1.satellite_observe[j].Sat_Type=="R")
                {
                    if(observe[i].GLONASS_TYPES_pos[0]!=-1)
                    {
                        if(line.length()>=(observe[i].GLONASS_TYPES_pos[0]*16+17))
                        {
                            temp1.satellite_observe[j].C1=line.mid(observe[i].GLONASS_TYPES_pos[0]*16+3,14).toDouble();
                        }
                    }
                    if(observe[i].GLONASS_TYPES_pos[1]!=-1)
                    {
                        if(line.length()>=(observe[i].GLONASS_TYPES_pos[1]*16+17))
                        {
                            temp1.satellite_observe[j].P1=line.mid(observe[i].GLONASS_TYPES_pos[1]*16+3,14).toDouble();
                        }
                    }
                    if(observe[i].GLONASS_TYPES_pos[2]!=-1)
                    {
                        if(line.length()>=(observe[i].GLONASS_TYPES_pos[2]*16+17))
                        {
                            temp1.satellite_observe[j].L1=line.mid(observe[i].GLONASS_TYPES_pos[2]*16+3,14).toDouble();
                        }
                    }
                    if(observe[i].GLONASS_TYPES_pos[3]!=-1)
                    {
                        if(line.length()>=(observe[i].GLONASS_TYPES_pos[3]*16+17))
                        {
                            temp1.satellite_observe[j].C2=line.mid(observe[i].GLONASS_TYPES_pos[3]*16+3,14).toDouble();
                        }
                    }
                    if(observe[i].GLONASS_TYPES_pos[4]!=-1)
                    {
                        if(line.length()>=(observe[i].GLONASS_TYPES_pos[4]*16+17))
                        {
                            temp1.satellite_observe[j].P2=line.mid(observe[i].GLONASS_TYPES_pos[4]*16+3,14).toDouble();
                        }
                    }
                    if(observe[i].GLONASS_TYPES_pos[5]!=-1)
                    {
                        if(line.length()>=(observe[i].GLONASS_TYPES_pos[5]*16+17))
                        {
                            temp1.satellite_observe[j].L2=line.mid(observe[i].GLONASS_TYPES_pos[5]*16+3,14).toDouble();
                        }
                    }
                    if(observe[i].GLONASS_TYPES_pos[6]!=-1)
                    {
                        if(line.length()>=(observe[i].GLONASS_TYPES_pos[6]*16+17))
                        {
                            temp1.satellite_observe[j].C3=line.mid(observe[i].GLONASS_TYPES_pos[6]*16+3,14).toDouble();
                        }
                    }
                    if(observe[i].GLONASS_TYPES_pos[7]!=-1)
                    {
                        if(line.length()>=(observe[i].GLONASS_TYPES_pos[7]*16+17))
                        {
                            temp1.satellite_observe[j].P3=line.mid(observe[i].GLONASS_TYPES_pos[7]*16+3,14).toDouble();
                        }
                    }
                    if(observe[i].GLONASS_TYPES_pos[8]!=-1)
                    {
                        if(line.length()>=(observe[i].GLONASS_TYPES_pos[8]*16+17))
                        {
                            temp1.satellite_observe[j].L3=line.mid(observe[i].GLONASS_TYPES_pos[8]*16+3,14).toDouble();
                        }
                    }
                }
                if(temp1.satellite_observe[j].Sat_Type=="C")
                {
                    if(observe[i].BDS_TYPES_pos[0]!=-1)
                    {
                        if(line.length()>=(observe[i].BDS_TYPES_pos[0]*16+17))
                        {
                            temp1.satellite_observe[j].C1=line.mid(observe[i].BDS_TYPES_pos[0]*16+3,14).toDouble();
                        }
                    }
                    if(observe[i].BDS_TYPES_pos[1]!=-1)
                    {
                        if(line.length()>=(observe[i].BDS_TYPES_pos[1]*16+17))
                        {
                            temp1.satellite_observe[j].P1=line.mid(observe[i].BDS_TYPES_pos[1]*16+3,14).toDouble();
                        }
                    }
                    if(observe[i].BDS_TYPES_pos[2]!=-1)
                    {
                        if(line.length()>=(observe[i].BDS_TYPES_pos[2]*16+17))
                        {
                            temp1.satellite_observe[j].L1=line.mid(observe[i].BDS_TYPES_pos[2]*16+3,14).toDouble();
                        }
                    }
                    if(observe[i].BDS_TYPES_pos[3]!=-1)
                    {
                        if(line.length()>=(observe[i].BDS_TYPES_pos[3]*16+17))
                        {
                            temp1.satellite_observe[j].C2=line.mid(observe[i].BDS_TYPES_pos[3]*16+3,14).toDouble();
                        }
                    }
                    if(observe[i].BDS_TYPES_pos[4]!=-1)
                    {
                        if(line.length()>=(observe[i].BDS_TYPES_pos[4]*16+17))
                        {
                            temp1.satellite_observe[j].P2=line.mid(observe[i].BDS_TYPES_pos[4]*16+3,14).toDouble();
                        }
                    }
                    if(observe[i].BDS_TYPES_pos[5]!=-1)
                    {
                        if(line.length()>=(observe[i].BDS_TYPES_pos[5]*16+17))
                        {
                            temp1.satellite_observe[j].L2=line.mid(observe[i].BDS_TYPES_pos[5]*16+3,14).toDouble();
                        }
                    }
                    if(observe[i].BDS_TYPES_pos[6]!=-1)
                    {
                        if(line.length()>=(observe[i].BDS_TYPES_pos[6]*16+17))
                        {
                            temp1.satellite_observe[j].C3=line.mid(observe[i].BDS_TYPES_pos[6]*16+3,14).toDouble();
                        }
                    }
                    if(observe[i].BDS_TYPES_pos[7]!=-1)
                    {
                        if(line.length()>=(observe[i].BDS_TYPES_pos[7]*16+17))
                        {
                            temp1.satellite_observe[j].P3=line.mid(observe[i].BDS_TYPES_pos[7]*16+3,14).toDouble();
                        }
                    }
                    if(observe[i].BDS_TYPES_pos[8]!=-1)
                    {
                        if(line.length()>=(observe[i].BDS_TYPES_pos[8]*16+17))
                        {
                            temp1.satellite_observe[j].L3=line.mid(observe[i].BDS_TYPES_pos[8]*16+3,14).toDouble();
                        }
                    }
                }

                if(temp1.satellite_observe[j].Sat_Type=="E")
                {
                    if(observe[i].Galileo_TYPES_pos[0]!=-1)
                    {
                        if(line.length()>=(observe[i].Galileo_TYPES_pos[0]*16+17))
                        {
                            temp1.satellite_observe[j].C1=line.mid(observe[i].Galileo_TYPES_pos[0]*16+3,14).toDouble();
                        }
                    }
                    if(observe[i].Galileo_TYPES_pos[1]!=-1)
                    {
                        if(line.length()>=(observe[i].Galileo_TYPES_pos[1]*16+17))
                        {
                            temp1.satellite_observe[j].P1=line.mid(observe[i].Galileo_TYPES_pos[1]*16+3,14).toDouble();
                        }
                    }
                    if(observe[i].Galileo_TYPES_pos[2]!=-1)
                    {
                        if(line.length()>=(observe[i].Galileo_TYPES_pos[2]*16+17))
                        {
                            temp1.satellite_observe[j].L1=line.mid(observe[i].Galileo_TYPES_pos[2]*16+3,14).toDouble();
                        }
                    }
                    if(observe[i].Galileo_TYPES_pos[3]!=-1)
                    {
                        if(line.length()>=(observe[i].Galileo_TYPES_pos[3]*16+17))
                        {
                            temp1.satellite_observe[j].C2=line.mid(observe[i].Galileo_TYPES_pos[3]*16+3,14).toDouble();
                        }
                    }
                    if(observe[i].Galileo_TYPES_pos[4]!=-1)
                    {
                        if(line.length()>=(observe[i].Galileo_TYPES_pos[4]*16+17))
                        {
                            temp1.satellite_observe[j].P2=line.mid(observe[i].Galileo_TYPES_pos[4]*16+3,14).toDouble();
                        }
                    }
                    if(observe[i].Galileo_TYPES_pos[5]!=-1)
                    {
                        if(line.length()>=(observe[i].Galileo_TYPES_pos[5]*16+17))
                        {
                            temp1.satellite_observe[j].L2=line.mid(observe[i].Galileo_TYPES_pos[5]*16+3,14).toDouble();
                        }
                    }
                    if(observe[i].Galileo_TYPES_pos[6]!=-1)
                    {
                        if(line.length()>=(observe[i].Galileo_TYPES_pos[6]*16+17))
                        {
                            temp1.satellite_observe[j].C3=line.mid(observe[i].Galileo_TYPES_pos[6]*16+3,14).toDouble();
                        }
                    }
                    if(observe[i].Galileo_TYPES_pos[7]!=-1)
                    {
                        if(line.length()>=(observe[i].Galileo_TYPES_pos[7]*16+17))
                        {
                            temp1.satellite_observe[j].P3=line.mid(observe[i].Galileo_TYPES_pos[7]*16+3,14).toDouble();
                        }
                    }
                    if(observe[i].Galileo_TYPES_pos[8]!=-1)
                    {
                        if(line.length()>=(observe[i].Galileo_TYPES_pos[8]*16+17))
                        {
                            temp1.satellite_observe[j].L3=line.mid(observe[i].Galileo_TYPES_pos[8]*16+3,14).toDouble();
                        }
                    }
                }
                if(temp1.satellite_observe[j].Sat_Type=="J")
                {
                    if(observe[i].QZSS_TYPES_pos[0]!=-1)
                    {
                        if(line.length()>=(observe[i].QZSS_TYPES_pos[0]*16+17))
                        {
                            temp1.satellite_observe[j].C1=line.mid(observe[i].QZSS_TYPES_pos[0]*16+3,14).toDouble();
                        }
                    }
                    if(observe[i].QZSS_TYPES_pos[1]!=-1)
                    {
                        if(line.length()>=(observe[i].QZSS_TYPES_pos[1]*16+17))
                        {
                            temp1.satellite_observe[j].P1=line.mid(observe[i].QZSS_TYPES_pos[1]*16+3,14).toDouble();
                        }
                    }
                    if(observe[i].QZSS_TYPES_pos[2]!=-1)
                    {
                        if(line.length()>=(observe[i].QZSS_TYPES_pos[2]*16+17))
                        {
                            temp1.satellite_observe[j].L1=line.mid(observe[i].QZSS_TYPES_pos[2]*16+3,14).toDouble();
                        }
                    }
                    if(observe[i].QZSS_TYPES_pos[3]!=-1)
                    {
                        if(line.length()>=(observe[i].QZSS_TYPES_pos[3]*16+17))
                        {
                            temp1.satellite_observe[j].C2=line.mid(observe[i].QZSS_TYPES_pos[3]*16+3,14).toDouble();
                        }
                    }
                    if(observe[i].QZSS_TYPES_pos[4]!=-1)
                    {
                        if(line.length()>=(observe[i].QZSS_TYPES_pos[4]*16+17))
                        {
                            temp1.satellite_observe[j].P2=line.mid(observe[i].QZSS_TYPES_pos[4]*16+3,14).toDouble();
                        }
                    }
                    if(observe[i].QZSS_TYPES_pos[5]!=-1)
                    {
                        if(line.length()>=(observe[i].QZSS_TYPES_pos[5]*16+17))
                        {
                            temp1.satellite_observe[j].L2=line.mid(observe[i].QZSS_TYPES_pos[5]*16+3,14).toDouble();
                        }
                    }
                    if(observe[i].QZSS_TYPES_pos[6]!=-1)
                    {
                        if(line.length()>=(observe[i].QZSS_TYPES_pos[6]*16+17))
                        {
                            temp1.satellite_observe[j].C3=line.mid(observe[i].QZSS_TYPES_pos[6]*16+3,14).toDouble();
                        }
                    }
                    if(observe[i].QZSS_TYPES_pos[7]!=-1)
                    {
                        if(line.length()>=(observe[i].QZSS_TYPES_pos[7]*16+17))
                        {
                            temp1.satellite_observe[j].P3=line.mid(observe[i].QZSS_TYPES_pos[7]*16+3,14).toDouble();
                        }
                    }
                    if(observe[i].QZSS_TYPES_pos[8]!=-1)
                    {
                        if(line.length()>=(observe[i].QZSS_TYPES_pos[8]*16+17))
                        {
                            temp1.satellite_observe[j].L3=line.mid(observe[i].QZSS_TYPES_pos[8]*16+3,14).toDouble();
                        }
                    }
                }
                if(temp1.satellite_observe[j].Sat_Type=="S")
                {
                    if(observe[i].SBAS_TYPES_pos[0]!=-1)
                    {
                        if(line.length()>=(observe[i].SBAS_TYPES_pos[0]*16+17))
                        {
                            temp1.satellite_observe[j].C1=line.mid(observe[i].SBAS_TYPES_pos[0]*16+3,14).toDouble();
                        }
                    }
                    if(observe[i].SBAS_TYPES_pos[1]!=-1)
                    {
                        if(line.length()>=(observe[i].SBAS_TYPES_pos[1]*16+17))
                        {
                            temp1.satellite_observe[j].P1=line.mid(observe[i].SBAS_TYPES_pos[1]*16+3,14).toDouble();
                        }
                    }
                    if(observe[i].SBAS_TYPES_pos[2]!=-1)
                    {
                        if(line.length()>=(observe[i].SBAS_TYPES_pos[2]*16+17))
                        {
                            temp1.satellite_observe[j].L1=line.mid(observe[i].SBAS_TYPES_pos[2]*16+3,14).toDouble();
                        }
                    }
                    if(observe[i].SBAS_TYPES_pos[3]!=-1)
                    {
                        if(line.length()>=(observe[i].SBAS_TYPES_pos[3]*16+17))
                        {
                            temp1.satellite_observe[j].C2=line.mid(observe[i].SBAS_TYPES_pos[3]*16+3,14).toDouble();
                        }
                    }
                    if(observe[i].SBAS_TYPES_pos[4]!=-1)
                    {
                        if(line.length()>=(observe[i].SBAS_TYPES_pos[4]*16+17))
                        {
                            temp1.satellite_observe[j].P2=line.mid(observe[i].SBAS_TYPES_pos[4]*16+3,14).toDouble();
                        }
                    }
                    if(observe[i].SBAS_TYPES_pos[5]!=-1)
                    {
                        if(line.length()>=(observe[i].SBAS_TYPES_pos[5]*16+17))
                        {
                            temp1.satellite_observe[j].L2=line.mid(observe[i].SBAS_TYPES_pos[5]*16+3,14).toDouble();
                        }
                    }
                    if(observe[i].SBAS_TYPES_pos[6]!=-1)
                    {
                        if(line.length()>=(observe[i].SBAS_TYPES_pos[6]*16+17))
                        {
                            temp1.satellite_observe[j].C3=line.mid(observe[i].SBAS_TYPES_pos[6]*16+3,14).toDouble();
                        }
                    }
                    if(observe[i].SBAS_TYPES_pos[7]!=-1)
                    {
                        if(line.length()>=(observe[i].SBAS_TYPES_pos[7]*16+17))
                        {
                            temp1.satellite_observe[j].P3=line.mid(observe[i].SBAS_TYPES_pos[7]*16+3,14).toDouble();
                        }
                    }
                    if(observe[i].SBAS_TYPES_pos[8]!=-1)
                    {
                        if(line.length()>=(observe[i].SBAS_TYPES_pos[8]*16+17))
                        {
                            temp1.satellite_observe[j].L3=line.mid(observe[i].SBAS_TYPES_pos[8]*16+3,14).toDouble();
                        }
                    }
                }
            }
            observe[i].satellite.push_back(temp1);
        }
        else
        {
            int row=line.mid(33,2).toInt();
            for(int i=0;i<row;i++)
            {
                line=str.readLine();
            }
        }

    }
    readfile.close();
}





























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
