#include "ProductDataCenter.h"
#include "MyFunctionCenter.h"
#include "DataProcessCenter.h"

/*------------------ Define static member--------------------------------------------*/
int     PrecisionEphemerisFile::timeTrans   = 0;
/*-----------------------------------------------------------------------------------*/

vector<SunMoonPosition> sunMoonPos; /////////////////////////////////



int  PrecisionEphemerisFile::getSatIndex(const vector <PreciseOrbitData> &aimArray,
                                         const int     &aimPrn)
{
    if (aimPrn < aimArray.size()){                                                     // If the index is not out of range of array
        if (aimArray[aimPrn - 1].satTN.PRN == aimPrn)                                  // If you just find the PRN you want
            return   aimPrn - 1;
        else{
            for (int n = aimPrn - 2; n >= 0; n--){                                     // Fast forward looking for elements
                if (aimArray[n].satTN.PRN == aimPrn)
                    return   n;
            }
        }
    }
    for(int n = 0; n < aimArray.size(); n++)                                           // If the index is out of range of array,
    {                                                                                  // find target form all element
        if (aimArray[n].satTN.PRN == aimPrn)
            return n;
    }
    return -1;                                                                         // Can't find target, this case can not happen
}


/*------------------------------------------------------------------------------------
 * Name     : readFile
 * Function : read precision Ephemeris file (*.sp3)
 * Input    : const QString &filePath
 * Output   : bool (if read success)
 *-----------------------------------------------------------------------------------*/
bool PrecisionEphemerisFile::readFile()
{
    if (!fileCommonDeal("Precision precision ephemeris File Open faild!"))
        return false;

    QTextStream inText(&inFile);
    const regex epochPattern  ("\\* {2}(\\d{4}) +(\\d+) +(\\d+) +(\\d+)"               // Regex Pattern
                                      " +(\\d+) +(\\d+\\.\\d{8}) *");
    match_results <string::const_iterator> result;

    int     lineNum  =  0;
    QString lineQStr = "",  codFlag   = "";
    string  lineStr  = "",  helpStr   = "";

    lineQStr = inText.readLine(),lineNum++;
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

            lineQStr = inText.readLine(),lineNum++;
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

            epochData.myTime = MyFunctionCenter::timeIntegrator(epochData.myTime.EPT);
            int weekDiffer          = 0;                                               // Difference of week
            if (!allPrecionData.empty())
                weekDiffer          = epochData.myTime.GPT.week -
                                      allPrecionData.front().myTime.GPT.week;

            epochData.accumulateGpsSec = epochData.myTime.GPT.sec +                    // Get accumulate GPS seconds
                                         weekDiffer * 604800 + timeTrans;              // Convert accumulate GPS Time to BDS Time
                                                               /* 需不需要加14秒*/
            SunMoonPosition smPos;
            MyTime utc = MyFunctionCenter::GPS_to_UTC(epochData.myTime);
            MyTime tdt = MyFunctionCenter::UTC_to_TDT(utc);

            //  tdt.EPT.second = floor(tdt.EPT.second);
            smPos.sunPosition      = DataMatchingCenter::seekSunPosition(tdt);  // 这一块是不是可以放到输出ppp那
            smPos.moonPosition     = DataMatchingCenter::seekMoonPosition(tdt);
            smPos.accumulateGpsSec = epochData.accumulateGpsSec;
            sunMoonPos.push_back(smPos);

            /*---------- save orbit data ------------*/
            do
            {
                lineQStr     = inText.readLine(),lineNum++;
                lineStr      = lineQStr.toStdString();
                isEpochValid = regex_match(lineStr, result, epochPattern);             // Judge if enter the next epoch

                if (!isEpochValid)
                {
                    PreciseOrbitData orbitData;
                    orbitData.satTN.Type     = lineQStr.mid(1,  1);
                    orbitData.satTN.PRN      = lineQStr.mid(2,  2).toInt();

                    orbitData.x              = lineQStr.mid(4 ,14).toDouble()*1000;
                    orbitData.y              = lineQStr.mid(18,14).toDouble()*1000;
                    orbitData.z              = lineQStr.mid(32,14).toDouble()*1000;

                    if (orbitData.satTN.Type == "G")                                   // Choose type and save orbitData
                        epochData.OrbitData_GPS.    push_back(orbitData);
                    if (orbitData.satTN.Type == "C")
                        epochData.OrbitData_BDS.    push_back(orbitData);
                    if (orbitData.satTN.Type == "R")
                        epochData.OrbitData_GLONASS.push_back(orbitData);
                    if (orbitData.satTN.Type == "E")
                        epochData.OrbitData_Galileo.push_back(orbitData);
                }
            }
            while (!isEpochValid && lineQStr.mid(0,3) != "EOF");                       // only loop with valid data
            allPrecionData.push_back(epochData);
        }
    }

    closeFile();
    return true;
}

