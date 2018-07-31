#include "DataCenter.h"
#include "MyFunctionCenter.h"

/*------------------ Define static member--------------------------------------*/
vector<AS_clkData> PrecisionClockFile::SatelliteClkData;
double  PrecisionClockFile::leapSecond;
double  PrecisionClockFile::INTERVAL;
QString PrecisionClockFile::navigationType;
/*-----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 * Name     : readFile
 * Function : read precision clock file (*.clk)
 * Input    : const QString &filePath
 * Output   : bool (if read success)
 *-----------------------------------------------------------------------------*/
bool PrecisionClockFile::readFile(const QString &filePath)
{
    QFile ClockFile(filePath);
    if(!  ClockFile.open( QIODevice::ReadOnly ))
    {
          QMessageBox::warning(NULL,                   "warning",
                              "Precision Clock File Open faild!",
                               QMessageBox::Yes, QMessageBox::Yes);
          return false;
    }

    QString lineQStr = "";

    /*------------------ Read Head --------------------------------------------*/
    while((lineQStr  = ClockFile.readLine())!="")
    {
        if (lineQStr.indexOf("END OF HEADER") >= 0)
            break;
        if (lineQStr.indexOf("LEAP SECONDS") >= 0)
            leapSecond     = lineQStr.mid(4,2).toDouble();
        if (lineQStr.indexOf("ANALYSIS CENTER") >= 0)
            navigationType = lineQStr.mid(0,3);
    }
    /*------------------ Read Data --------------------------------------------*/
    while((lineQStr  = ClockFile.readLine())!="")
     {
         if (lineQStr.mid(0,2) == "AR")
            continue;

         AS_clkData AsData;
         AsData.myTime.EPT.year   = lineQStr.mid(8, 4).toDouble();
         AsData.myTime.EPT.month  = lineQStr.mid(13,2).toDouble();
         AsData.myTime.EPT.day    = lineQStr.mid(16,2).toDouble();
         AsData.myTime.EPT.hour   = lineQStr.mid(19,2).toDouble();
         AsData.myTime.EPT.minute = lineQStr.mid(22,2).toDouble();
         AsData.myTime.EPT.second = lineQStr.mid(25,2).toDouble();


         AsData.myTime = MyFuncionCenter::timeIntegrator(AsData.myTime.EPT);
         AsData.accumulateGpsSec += PrecisionEphemerisFile::timeTrans;          // Convert accumulate GPS Time to BDS Time

         int weekDiffer           = 0;                                          // Difference of week
         if (!SatelliteClkData.empty())
             weekDiffer           = AsData.myTime.GPT.week -
                                    SatelliteClkData.front().myTime.GPT.week;

         AsData.accumulateGpsSec  = AsData.myTime.GPT.sec +                     // Get accumulate GPS seconds
                                    weekDiffer * 604800;
         do
         {
             QString satType   = lineQStr.mid(3,  1);
             double  clockData = lineQStr.mid(40,19).toDouble();
             int     satLabel  = lineQStr.mid(4,  2).toInt();                   // Number of satellite

             /*----------- choose satellite type -----------------------------*/
             if (satType == "G"){
                 AsData.clockData_GPS[satLabel-1]     = clockData;
                 continue;
             }
             if (satType == "C"){
                 AsData.clockData_BDS[satLabel-1]     = clockData;
                 continue;
             }
             if (satType == "R"){
                 AsData.clockData_GLONASS[satLabel-1] = clockData;
                 continue;
             }
             if (satType == "E"){
                 AsData.clockData_Galileo[satLabel-1] = clockData;
                 continue;
             }
             if (satType == "J"){
                 AsData.clockData_J[satLabel-1]       = clockData;
                 continue;
             }
             if (satType == "S"){
                 AsData.clockData_S[satLabel-1]       = clockData;
                 continue;
             }
         }
         while ((lineQStr = ClockFile.readLine())!= "" &&
                 lineQStr.mid(0,2) == "AS");

         SatelliteClkData.push_back(AsData);
     }

     if (SatelliteClkData.size() < 2)                                           // Judge if data are enough
     {
         QMessageBox::warning(NULL,                   "warning",
                             "Precision clock data are not enough!",
                              QMessageBox::Yes, QMessageBox::Yes);
         return false;
     }
     else
        INTERVAL = abs(int(SatelliteClkData[1].myTime.GPT.sec -                 // Save interval
                           SatelliteClkData[0].myTime.GPT.sec)) + 1;

     ClockFile.close();
     return true;
}
