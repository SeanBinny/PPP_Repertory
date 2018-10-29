#include "ProductDataCenter.h"
#include "MyFunctionCenter.h"
/*--------------------------------------------------------------------------------------------
 * Name     : ~AntennaInfoFile
 * Function :  To disconstruct pointer member
 * Input    :  None
 * Output   :  None
 *------------------------------------------------------------------------------------------*/
AntennaInfoFile::~AntennaInfoFile()
{
    for (int i = 0; i < 32; i++)
    {
         delete [] antennaData_GPS[i];
         delete [] antennaData_BDS[i];
         delete [] antennaData_GLONASS[i];
    }
    for (int i = 0; i < 60; i++)
         delete [] antennaData_Galileo[i];
    delete antennaData_GPS;
    delete antennaData_BDS;
    delete antennaData_GLONASS;
    delete antennaData_Galileo;
}

/*------------------------------------------------------------------------------------------
 * Name     : readFile
 * Function : read antenna information file (*.atx)
 * Input    : NULL
 * Output   : bool (if read success)
 *-----------------------------------------------------------------------------------------*/
bool  AntennaInfoFile::readFile()                                                          // Inherit function
{
    if (!fileCommonDeal("Antenna information File Open faild!"))
        return false;

    QTextStream inText(&inFile);
    QString     lineQStr  = "";
    QString     Month[12];                                                                 // Define a months library
    Month[0] = "JAN", Month[1] = "FEB", Month[2 ] = "MAR", Month[3 ] = "APR",
    Month[4] = "MAY", Month[5] = "JUN", Month[6 ] = "Jul", Month[7 ] = "AUG",
    Month[8] = "SEP", Month[9] = "OCT", Month[10] = "NOV", Month[11] = "DEC";

    /*------------------------- Read Header ------------------------------------------------*/
    lineQStr      = inText.readLine();                                                     // ANTEX VERSION / SYST
    ANTEX_VERSION = lineQStr.mid(0, 11).toDouble();
    SYST          = lineQStr.mid(12,38).trimmed();
    lineQStr      = inText.readLine();                                                     // PCV TYPE / REFANT
    PCV_TYPE      = lineQStr.mid(0, 19).trimmed();
    REFANT        = lineQStr.mid(20,19).trimmed();
    if (REFANT   == "")
        REFANT    = "AOAD/M_T";

    while ((lineQStr = inText.readLine()) != "")
    {
        if (lineQStr.indexOf("END OF HEADER")  >= 0)
            break;
    }
    /*------------------------- Read Data -------------------------------------------------*/
    while ((lineQStr = inText.readLine()) != "")
    {
        if (lineQStr.indexOf("START OF ANTENNA") >= 0)                                      // START OF ANTENNA
        {
            AntennaData antennaData;
            lineQStr                = inText.readLine();                                    // TYPE / SERIAL NO
            antennaData.antennaType = lineQStr.mid(0, 20).trimmed();
            antennaData.sNNType     = lineQStr.mid(20, 1);
            antennaData.sNN         = lineQStr.mid(21,18).toInt();
            antennaData.sNNNType    = lineQStr.mid(40, 1);
            antennaData.sNNN        = lineQStr.mid(41, 9).toInt();
            antennaData.COSPAR_ID   = lineQStr.mid(50, 9).trimmed();

            lineQStr                      = inText.readLine();                              // METH / BY / # / DATE
            antennaData.calibrationMethod = lineQStr.mid(0, 19).trimmed();
            antennaData.agencyName        = lineQStr.mid(20,19).trimmed();
            antennaData.calibrationMethod = lineQStr.mid(0, 19).trimmed();
            antennaData.myTime.EPT.year   = lineQStr.mid(57, 2).toInt() + 2000;
            for (int i = 0; i < 12; i++)
            {
                if (lineQStr.mid(53,3) == Month[i]){
                    antennaData.myTime.EPT.month = i + 1;
                    break;
                }
            }

            lineQStr               = inText.readLine();                                    // DAZI
            antennaData.DAZI       = lineQStr.mid(0 ,59).toDouble();

            lineQStr               = inText.readLine();                                    // ZEN1 / ZEN2 / DZEN
            antennaData.ZEN1       = lineQStr.mid(4,  5).toDouble();
            antennaData.ZEN2       = lineQStr.mid(9,  7).toDouble();
            antennaData.DZEN       = lineQStr.mid(16,12).toDouble();

            lineQStr               = inText.readLine();                                    // # OF FREQUENCIES
            antennaData.OF_FREQUENCIES  = lineQStr.mid(0,59).toDouble();

            while((lineQStr = inText.readLine()) != "")
            {
                if (lineQStr.indexOf("START OF FREQUENCY")  >= 0)
                    break;
                double Y       = lineQStr.mid(2,  8).toInt();
                double M       = lineQStr.mid(10, 6).toInt();
                double D       = lineQStr.mid(16, 6).toInt();
                double h       = lineQStr.mid(22, 6).toInt();
                double m       = lineQStr.mid(28, 5).toInt();
                double s       = lineQStr.mid(33,16).toDouble();
                EpochTime tempTime(Y, M, D, h, m, s);

                if (lineQStr.indexOf("VALID FROM")  >= 0)                                   // VALID FROM
                    antennaData.VALID_FROM  = MyFunctionCenter::Gre_to_JD(tempTime);
                if (lineQStr.indexOf("VALID UNTIL") >= 0)                                   // VALID UNTIL
                    antennaData.VALID_UNTIL = MyFunctionCenter::Gre_to_JD(tempTime);
            }

            antennaData.FREQUENCY_CODE  = lineQStr.mid(4,3).toInt();
            antennaData.FREQUENCY_Type  = lineQStr.mid(3,1);

            while ((lineQStr = inText.readLine()) != "")                                    // Skip Comment line
            {
                if ((lineQStr.indexOf("NORTH / EAST / UP") >= 0))
                    break;
            }

            antennaData.F1_NEU[0]       = lineQStr.mid(0, 14).toDouble();
            antennaData.F1_NEU[1]       = lineQStr.mid(14, 8).toDouble();
            antennaData.F1_NEU[2]       = lineQStr.mid(22,25).toDouble();

            int row =  1;                                                                   // Get row and col for data
            if (antennaData.DAZI       != 0.0)
                row = (int)  360 / antennaData.DAZI + 2;
            int col = (int)((antennaData.ZEN2 - antennaData.ZEN1) / antennaData.DZEN + 1);

            antennaData.F1_NOAZI.resize(row, col);
            antennaData.F2_NOAZI.resize(row, col);
            for (int i = 0; i < row; i++)                                                   // Save F1_NOAZI
            {
                lineQStr = inText.readLine();
                for (int j = 0; j < col; j++)
                    antennaData.F1_NOAZI(i, j) = lineQStr.mid(8+j*8, 8).toDouble();
            }

            lineQStr              = inText.readLine();                                      // Skip unuseful line
            lineQStr              = inText.readLine();                                      // Skip unuseful line
            lineQStr              = inText.readLine();                                      // Skip unuseful line
            antennaData.F2_NEU[0] = lineQStr.mid(0, 14).toDouble();
            antennaData.F2_NEU[1] = lineQStr.mid(14, 8).toDouble();
            antennaData.F2_NEU[2] = lineQStr.mid(22,25).toDouble();
            for (int i = 0; i < row; i++)                                                   // Save F1_NOAZI
            {
                lineQStr = inText.readLine();
                for (int j = 0; j < col; j++)
                    antennaData.F2_NOAZI(i, j) = lineQStr.mid(8+j*8, 8).toDouble();
            }

            while ((lineQStr = inText.readLine()) != "")                                    // End a data block of antenna data
            {
                if (lineQStr.indexOf("END OF ANTENNA") >= 0)
                    break;
            }

            /*----------------------------- Choose tyoe and save --------------------------*/
            if (antennaData.sNNType == "G")
            {
                if (antennaData_GPS == NULL)                                                // Set 1D and set NULL
                {
                    antennaData_GPS = new vector <AntennaData> *[32];
                    for (int i = 0; i < 32; i++)                                            // Set 2D
                        antennaData_GPS[i] = NULL;
                }
                if (antennaData_GPS[antennaData.sNN - 1] == NULL)
                    antennaData_GPS[antennaData.sNN - 1] = new vector <AntennaData>;
                antennaData_GPS[antennaData.sNN - 1]->push_back(antennaData);
          }
            else if (antennaData.sNNType == "C")
            {
                if (antennaData_BDS == NULL)
                {
                    antennaData_BDS = new vector <AntennaData> *[32];
                    for (int i = 0; i < 32; i++)
                        antennaData_BDS[i] = NULL;
                }
                if (antennaData_BDS[antennaData.sNN - 1] == NULL)
                    antennaData_BDS[antennaData.sNN - 1] = new vector <AntennaData>;
                antennaData_BDS[antennaData.sNN - 1]->push_back(antennaData);
            }
            else if (antennaData.sNNType == "R")
            {
                if (antennaData_GLONASS  == NULL)
                {
                    antennaData_GLONASS  = new vector <AntennaData> *[32];
                    for (int i = 0; i < 32; i++)
                        antennaData_GLONASS[i] = NULL;
                }
                if (antennaData_GLONASS[antennaData.sNN - 1] == NULL)
                    antennaData_GLONASS[antennaData.sNN - 1] = new vector <AntennaData>;
                antennaData_GLONASS[antennaData.sNN - 1]->push_back(antennaData);
            }
            else if (antennaData.sNNType == "E")
            {
                if (antennaData_Galileo  == NULL)
                {
                    antennaData_Galileo  = new vector <AntennaData> *[60];
                    for (int i = 0; i < 60; i++)
                        antennaData_Galileo[i] = NULL;
                }
                if (antennaData_Galileo[antennaData.sNN - 1] == NULL)
                    antennaData_Galileo[antennaData.sNN - 1] = new vector <AntennaData>;
                antennaData_Galileo[antennaData.sNN - 1]->push_back(antennaData);
            }
            else
                antennaData_Other.push_back(antennaData);                                   // If can not find proper type, save here
        }
   }

   closeFile();
   return true;
}
