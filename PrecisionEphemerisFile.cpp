#include "ProductDataCenter.h"
#include "MyFunctionCenter.h"

/*------------------ Define static member--------------------------------------------*/
vector <EpochPreciseOrbitData> PrecisionEphemerisFile::allPrecionData;
int     PrecisionEphemerisFile::epochNum    = 0;
int     PrecisionEphemerisFile::satelliteNum= 0;
int     PrecisionEphemerisFile::timeTrans   = 0;
double  PrecisionEphemerisFile::julianDay   = 0;
QString PrecisionEphemerisFile::systemType  = "";

/*-----------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------------
 * Name     : readFile
 * Function : read precision Ephemeris file (*.sp3)
 * Input    : const QString &filePath
 * Output   : bool (if read success)
 *-----------------------------------------------------------------------------------*/
bool PrecisionEphemerisFile::readFile(const QString &filePath)
{
    QFile EphemerisFile(filePath);
    if(!  EphemerisFile.open( QIODevice::ReadOnly ))
    {
          QMessageBox::warning(NULL,                   "warning",
                              "Precision precision ephemeris File "
                              "Open faild!",
                               QMessageBox::Yes, QMessageBox::Yes);
          return false;
    }

    const regex epochPattern    ("\\* {2}(\\d{4}) +(\\d+) +(\\d+) +(\\d+)"             // Regex Pattern
                                        " +(\\d+) +(\\d+\\.\\d{8}) *\\n");

    match_results <string::const_iterator> result;

    int     lineNum  =  0;
    QString lineQStr = "",  codFlag   = "";
    string  lineStr  = "",  helpStr   = "";

    lineQStr = EphemerisFile.readLine(),lineNum++;
    lineQStr.simplified();
    lineStr  = lineQStr.toStdString();                                                 // QString transport to string
    while (lineQStr.mid(0,3) != "EOF")
    {
        bool isEpochValid = regex_match(lineStr, result, epochPattern);
        /*------------------------------- read Head ----------------------------------*/
        if (!isEpochValid)
        {
            if (lineNum == 1 )
                epochNum     = lineQStr.mid(32,7).toInt();                             // Save number of epochs
            if (lineNum == 2 )
                julianDay    = lineQStr.mid(39,5).toDouble();                          // Save julian day
            if (lineNum == 3 )
                satelliteNum = lineQStr.mid(4,2) .toInt();                             // Save number of satellites
            if (lineNum == 13)
                systemType   = lineQStr.mid(3,1);                                      // Save system type
            if (lineNum == 19)
                codFlag      = lineQStr.mid(3,4);
            if (lineQStr.mid(9,3) == "BDT")                                            // Judge if it is BDS Time
                timeTrans=14;

            lineQStr = EphemerisFile.readLine(),lineNum++;
            lineQStr.trimmed();
            lineStr  = lineQStr.toStdString();                                         // QString transport to string
        }
        /*------------------------------- read Data ----------------------------------*/
        if ( isEpochValid)
        {
            EpochPreciseOrbitData epochData;

           /*---------- save epoch time ------------*/
            helpStr                     =     result[1];
            epochData.myTime.EPT.year   = stoi(helpStr);
            helpStr                     =     result[2];
            epochData.myTime.EPT.month  = stoi(helpStr);
            helpStr                     =     result[3];
            epochData.myTime.EPT.day    = stoi(helpStr);
            helpStr                     =     result[4];
            epochData.myTime.EPT.hour   = stoi(helpStr);
            helpStr                     =     result[5];
            epochData.myTime.EPT.minute = stoi(helpStr);
            helpStr                     =     result[6];
            epochData.myTime.EPT.second = stod(helpStr);

            epochData.myTime = MyFuncionCenter::timeIntegrator(epochData.myTime.EPT);
            epochData.accumulateGpsSec   += timeTrans;                                 // Convert accumulate GPS Time to BDS Time

            /* moon and sun correction
            m_time t1=GPS2UTC(t);
            m_time t2=UTC2TT(t1);
            hp.sunposition=sunPostion(t2);
            hp.moonposition=moonPostion(t2);
            P.push_back(hp);
            */
            int weekDiffer          = 0;                                               // Difference of week
            if (!allPrecionData.empty())
                weekDiffer          = epochData.myTime.GPT.week -
                                      allPrecionData.front().myTime.GPT.week;

            epochData.accumulateGpsSec = epochData.myTime.GPT.sec +                    // Get accumulate GPS seconds
                                         weekDiffer * 604800;

            /*---------- save orbit data ------------*/
            do
            {
                lineQStr     = EphemerisFile.readLine(),lineNum++;
                lineStr      = lineQStr.toStdString();
                isEpochValid = regex_match(lineStr, result, epochPattern);             // Judge if enter the next epoch

                if (!isEpochValid)
                {
                    PreciseOrbitData orbitData;
                    QString satType          = lineQStr.mid(1,  1);
                    orbitData.SatelliteName  = lineQStr.mid(1,  3);

                    orbitData.x              = lineQStr.mid(4 ,14).toDouble()*1000;
                    orbitData.y              = lineQStr.mid(18,14).toDouble()*1000;
                    orbitData.z              = lineQStr.mid(32,14).toDouble()*1000;

                    if (satType == "G")                                                // Choose type and save orbitData
                        epochData.OrbitData_GPS.    push_back(orbitData);
                    if (satType == "C")
                        epochData.OrbitData_BDS.    push_back(orbitData);
                    if (satType == "R")
                        epochData.OrbitData_GLONASS.push_back(orbitData);
                    if (satType == "E")
                        epochData.OrbitData_Galileo.push_back(orbitData);
                }
            }
            while (!isEpochValid && lineQStr.mid(0,3) != "EOF");                       // only loop with valid data
            allPrecionData.push_back(epochData);
        }
    }

    EphemerisFile.close();
    return true;
}








//            bool isDataValid = false;
//            do
//            {
//                lineQStr     = OrbitFile.readLine(),lineNum++;
//                lineStr      = lineQStr.toStdString();
//                isEpochValid = regex_match(lineStr, result, epochPattern);            // Judge if enter the next epoch
//                isDataValid  = regex_match(lineStr, result, orbitDataPattern);
//                if (isDataValid)
//                {
//                    PreciseOrbitData orbitData;
//                    string satType           = result[1];
//                    helpStr                  = result[2];
//                    orbitData.SatelliteName  = QString::fromStdString(satType + helpStr);

//                    helpStr                  = result[3];
//                    orbitData.x              = stod(helpStr) * 1000;
//                    helpStr                  = result[4];
//                    orbitData.y              = stod(helpStr) * 1000;
//                    helpStr                  = result[5];
//                    orbitData.z              = stod(helpStr) * 1000;

//                    if (satType == "G")                                               // Choose type and save orbitData
//                        epochData.OrbitData_GPS.    push_back(orbitData);
//                    if (satType == "C")
//                        epochData.OrbitData_BDS.    push_back(orbitData);
//                    if (satType == "R")
//                        epochData.OrbitData_GLONASS.push_back(orbitData);
//                    if (satType == "E")
//                        epochData.OrbitData_Galileo.push_back(orbitData);
//                }
//            }
//            while (!isEpochValid && isDataValid);                                     // only loop with valid data

