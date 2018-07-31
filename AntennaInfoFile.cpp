#include "DataCenter.h"
#include "MyFunctionCenter.h"


/*------------------ Define static member----------------------------------------------------*/
double      AntennaInfoFile::ANTEX_VERSION;
QString     AntennaInfoFile::SYST;
QString     AntennaInfoFile::REFANT;
QString     AntennaInfoFile::PCV_TYPE;
QString     AntennaInfoFile::productType;
AntennaData AntennaInfoFile::antennaData_GPS[32][3];
AntennaData AntennaInfoFile::antennaData_BDS[32][3];
AntennaData AntennaInfoFile::antennaData_GLONASS[32][3];
AntennaData AntennaInfoFile::antennaData_Galileo[60][3];
vector <AntennaData> AntennaInfoFile::antennaData_Other;
/*------------------------------------------------------------------------------------------*/


bool  AntennaInfoFile::readFile(const QString &filePath)                                     // Inherit function
{
//    QFileInfo fi;
//    fi = QFileInfo(filepath);
//    QString file_name = fi.fileName();
//    QString file_type=file_name.right(3);
//    QString product_type=file_name.left(3);

    QFile AntennaFile(filePath);
    if(!  AntennaFile.open( QIODevice::ReadOnly ))
    {
          QMessageBox::warning(NULL,                   "warning",
                              "Antenna information File Open faild!",
                               QMessageBox::Yes, QMessageBox::Yes);
          return false;
    }

    QString lineQStr  = "";
    QString Month[12];                                                                       // Define a months library
    Month[0] = "JAN", Month[1] = "FEB", Month[2 ] = "MAR", Month[3 ] = "APR",
    Month[4] = "MAY", Month[5] = "JUN", Month[6 ] = "Jul", Month[7 ] = "AUG",
    Month[8] = "SEP", Month[9] = "OCT", Month[10] = "NOV", Month[11] = "DEC";

    /*------------------------- Read Header ------------------------------------------------*/
    lineQStr      = AntennaFile.readLine();                                                  // ANTEX VERSION / SYST
    ANTEX_VERSION = lineQStr.mid(0, 11).toDouble();
    SYST          = lineQStr.mid(12,38);
    lineQStr      = AntennaFile.readLine();                                                  // PCV TYPE / REFANT
    PCV_TYPE      = lineQStr.mid(0, 19);
    REFANT        = lineQStr.mid(20,19);
    if (REFANT   == "")
        REFANT    = "AOAD/M_T";

    while ((lineQStr = AntennaFile.readLine()) != "")
    {
        if (lineQStr.indexOf("END OF HEADER") >= 0)
            break;
    }
    /*------------------------- Read Data -------------------------------------------------*/
    while ((lineQStr = AntennaFile.readLine()) != "")
    {
        if (lineQStr.indexOf("START OF ANTENNA") >= 0)                                      // START OF ANTENNA
        {
            AntennaData antennaData;
            lineQStr                = AntennaFile.readLine();                               // TYPE / SERIAL NO
            antennaData.antennaType = lineQStr.mid(0, 20);
            antennaData.sateOneType = lineQStr.mid(20, 1);
            antennaData.sateOneNum  = lineQStr.mid(21,18).toInt();
            antennaData.sateTwoType = lineQStr.mid(40, 1);
            antennaData.sateTwoNum  = lineQStr.mid(41, 9).toInt();
            antennaData.COSPAR_ID   = lineQStr.mid(50, 9);

            lineQStr                      = AntennaFile.readLine();                         // METH / BY / # / DATE
            antennaData.calibrationMethod = lineQStr.mid(0, 19);
            antennaData.agencyName        = lineQStr.mid(20,19);
            antennaData.calibrationMethod = lineQStr.mid(0, 19);
            antennaData.myTime.EPT.year   = lineQStr.mid(57, 2).toInt() + 2000;
            for (int i = 0; i < 12; i++)
            {
                if (lineQStr.mid(53,3) == Month[i]){
                    antennaData.myTime.EPT.month = i + 1;
                    break;
                }
            }

            lineQStr               = AntennaFile.readLine();                                // DAZI
            antennaData.DAZI       = lineQStr.mid(0 ,59).toDouble();

            lineQStr               = AntennaFile.readLine();                                // ZEN1 / ZEN2 / DZEN
            antennaData.ZEN1       = lineQStr.mid(4,  5).toDouble();
            antennaData.ZEN2       = lineQStr.mid(9,  7).toDouble();
            antennaData.DZEN       = lineQStr.mid(16,12).toDouble();

            lineQStr               = AntennaFile.readLine();                                // # OF FREQUENCIES
            antennaData.OF_FREQUENCIES  = lineQStr.mid(0,59).toDouble();

            while((lineQStr = AntennaFile.readLine()) != "")
            {
                double Y       = lineQStr.mid(2,  8).toInt();
                double M       = lineQStr.mid(10, 6).toInt();
                double D       = lineQStr.mid(16, 6).toInt();
                double h       = lineQStr.mid(22, 6).toInt();
                double m       = lineQStr.mid(28, 5).toInt();
                double s       = lineQStr.mid(33,16).toDouble();
                EpochTime tempTime(Y, M, D, h, m, s);

                if (lineQStr.indexOf("VALID FROM")  >= 0)                                   // VALID FROM
                    antennaData.VALID_FROM = MyFuncionCenter::Gre_to_JD(tempTime);
                if (lineQStr.indexOf("VALID UNTIL") >= 0)                                   // VALID UNTIL
                    antennaData.VALID_UNTIL = MyFuncionCenter::Gre_to_JD(tempTime);
                if (lineQStr.indexOf("SINEX CODE")  >= 0)                                   // SINEX CODE
                    break;
            }

            lineQStr                    = AntennaFile.readLine();   // 自己改的              // START OF FREQUENCY
            antennaData.SINEX_CODE      = lineQStr.mid(4,3).toInt();
            antennaData.SINEX_CODE_Type = lineQStr.mid(3,1);

            while ((lineQStr = AntennaFile.readLine()) != "")                              // Skip Comment line
            {
                if (!(lineQStr.indexOf("COMMENT") >= 0))
                    break;
            }

            //lineQStr                    = AntennaFile.readLine();   // 是不是多余了
            antennaData.F1_NEU[0]       = lineQStr.mid(0, 14).toDouble();
            antennaData.F1_NEU[1]       = lineQStr.mid(14, 8).toDouble();
            antennaData.F1_NEU[2]       = lineQStr.mid(22,25).toDouble();

            int row =  1;                                                                  // Get row and col for data
            if (antennaData.DAZI      != 0.0)
                row = (int)360 / antennaData.DAZI + 2;
            int col = (int)((antennaData.ZEN2 - antennaData.ZEN1) / antennaData.DZEN + 1);

            antennaData.F1_NOAZI.resize(row, col);
            antennaData.F2_NOAZI.resize(row, col);
            for (int i = 0; i < row; i++)                                                  // Save F1_NOAZI
            {
                lineQStr = AntennaFile.readLine();
                for (int j = 0; j < col; j++)
                    antennaData.F1_NOAZI(i, j) = lineQStr.mid(8+j*8, 8).toDouble();
            }

            lineQStr              = AntennaFile.readLine();                                // Skip unuseful line
            lineQStr              = AntennaFile.readLine();                                // Skip unuseful line
            lineQStr              = AntennaFile.readLine();                                // Skip unuseful line
            antennaData.F2_NEU[0] = lineQStr.mid(0, 14).toDouble();
            antennaData.F2_NEU[1] = lineQStr.mid(14, 8).toDouble();
            antennaData.F2_NEU[2] = lineQStr.mid(22,25).toDouble();
            for (int i = 0; i < row; i++)                                                  // Save F1_NOAZI
            {
                lineQStr = AntennaFile.readLine();
                for (int j = 0; j < col; j++)
                    antennaData.F2_NOAZI(i, j) = lineQStr.mid(8+j*8, 8).toDouble();
            }

            while ((lineQStr = AntennaFile.readLine()) != "")                              // End a data block of antenna data
            {
                if (lineQStr.indexOf("END OF ANTENNA") >= 0)
                    break;
            }

            /*----------------------------- Choose tyoe and save -------------------------*/
            if (antennaData.sateOneType == "G")
            {
                if (antennaData_GPS[antennaData.sateOneNum - 1][0].sateOneNum == 0)
                    antennaData_GPS[antennaData.sateOneNum - 1][0] = antennaData;
                else if (antennaData_GPS[antennaData.sateOneNum - 1][1].sateOneNum == 0)
                    antennaData_GPS[antennaData.sateOneNum - 1][1] = antennaData;
                else
                    antennaData_GPS[antennaData.sateOneNum - 1][2] = antennaData;          // If more than three will be overlap
            }
            else if (antennaData.sateOneType == "C")
            {
                if (antennaData_BDS[antennaData.sateOneNum - 1][0].sateOneNum == 0)
                    antennaData_BDS[antennaData.sateOneNum - 1][0] = antennaData;
                else if (antennaData_BDS[antennaData.sateOneNum - 1][1].sateOneNum == 0)
                    antennaData_BDS[antennaData.sateOneNum - 1][1] = antennaData;
                else
                    antennaData_BDS[antennaData.sateOneNum - 1][2] = antennaData;          // If more than three will be overlap
            }
            else if (antennaData.sateOneType == "R")
            {
                if (antennaData_GLONASS[antennaData.sateOneNum - 1][0].sateOneNum == 0)
                    antennaData_GLONASS[antennaData.sateOneNum - 1][0] = antennaData;
                else if (antennaData_GLONASS[antennaData.sateOneNum - 1][1].sateOneNum == 0)
                    antennaData_GLONASS[antennaData.sateOneNum - 1][1] = antennaData;
                else
                    antennaData_GLONASS[antennaData.sateOneNum - 1][2] = antennaData;     // If more than three will be overlap
            }
            else if (antennaData.sateOneType == "E")
            {
                if (antennaData_Galileo[antennaData.sateOneNum - 1][0].sateOneNum == 0)
                    antennaData_Galileo[antennaData.sateOneNum - 1][0] = antennaData;
                else if (antennaData_Galileo[antennaData.sateOneNum - 1][1].sateOneNum == 0)
                    antennaData_Galileo[antennaData.sateOneNum - 1][1] = antennaData;
                else
                    antennaData_Galileo[antennaData.sateOneNum - 1][2] = antennaData;     // If more than three will be overlap
            }
            else
                antennaData_Other.push_back(antennaData);                                 // If can not find proper type, save here
        }
   }
   AntennaFile.close();
   return true;

   // 数组里面村指针
}
