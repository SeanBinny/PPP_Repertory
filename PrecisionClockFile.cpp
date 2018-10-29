#include "ProductDataCenter.h"
#include "MyFunctionCenter.h"

/*------------------------------------------------------------------------------
 * Name     : readFile
 * Function : read precision clock file (*.clk)
 * Input    : const QString &filePath
 * Output   : bool (if read success)
 *-----------------------------------------------------------------------------*/
bool PrecisionClockFile::readFile()
{
    if (!fileCommonDeal("Precision Clock File Open faild!"))
        return false;

    QString lineQStr = "";
    QTextStream inText(&inFile);
    lineQStr  = inText.readLine();
    /*------------------ Read Head --------------------------------------------*/
    while((lineQStr  = inText.readLine())!="")
    {
        if (lineQStr.indexOf("END OF HEADER") >= 0)
            break;
        if (lineQStr.indexOf("LEAP SECONDS") >= 0)
            leapSecond     = lineQStr.mid(4,2).toDouble();
        if (lineQStr.indexOf("ANALYSIS CENTER") >= 0)
            navigationType = lineQStr.mid(0,3);
    }
    /*------------------ Read Data --------------------------------------------*/
    while((lineQStr  = inText.readLine())!="")
     {
         if (lineQStr.mid(0,2) == "AR")                                         // Circle to ignore block of AR
            continue;

         AS_clkData AsData;
         AsData.myTime.EPT.year   = lineQStr.mid(8, 4).toDouble();
         AsData.myTime.EPT.month  = lineQStr.mid(13,2).toDouble();
         AsData.myTime.EPT.day    = lineQStr.mid(16,2).toDouble();
         AsData.myTime.EPT.hour   = lineQStr.mid(19,2).toDouble();
         AsData.myTime.EPT.minute = lineQStr.mid(22,2).toDouble();
         AsData.myTime.EPT.second = lineQStr.mid(25,2).toDouble();


         AsData.myTime = MyFunctionCenter::timeIntegrator(AsData.myTime.EPT);
         //AsData.accumulateGpsSec += PrecisionEphemerisFile::timeTrans;        // Convert accumulate GPS Time to BDS Time

         int weekDiffer           = 0;                                          // Difference of week
         if (!satelliteClkData.empty())
             weekDiffer           = AsData.myTime.GPT.week -
                                    satelliteClkData.front().myTime.GPT.week;

         AsData.accumulateGpsSec  = AsData.myTime.GPT.sec +                     // Get accumulate GPS seconds
                                    weekDiffer * 604800   +
                                    PrecisionEphemerisFile::timeTrans;          // Convert accumulate GPS Time to BDS Time
         do                                     /* 需不需要加14秒*/               // Circle to read block of AS
         {
             QString satType   = lineQStr.mid(3,  1);
             double  clockData = lineQStr.mid(40,19).toDouble();
             int     satLabel  = lineQStr.mid(4,  2).toInt();                   // Number of satellite

             /*----------- choose satellite type -----------------------------*/
             if      (satType == "G")
                 AsData.clockData_GPS[satLabel-1]     = clockData;
             else if (satType == "C")
                 AsData.clockData_BDS[satLabel-1]     = clockData;
             else if (satType == "R")
                 AsData.clockData_GLONASS[satLabel-1] = clockData;
             else if (satType == "E")
                 AsData.clockData_Galileo[satLabel-1] = clockData;
             else if (satType == "J")
                 AsData.clockData_QZSS[satLabel-1]    = clockData;
             else if (satType == "S")
                 AsData.clockData_SBAS[satLabel-1]    = clockData;
         }
         while ((lineQStr = inText.readLine()) != "" &&
                 lineQStr.mid(0,2) == "AS");
         satelliteClkData.push_back(AsData);
     }

     if (satelliteClkData.size() < 2)                                           // Judge if data are enough
     {
         QMessageBox::warning(NULL,                   "warning",
                             "Precision clock data are not enough!",
                              QMessageBox::Yes, QMessageBox::Yes);
         return false;
     }
     else
        INTERVAL = abs(int(satelliteClkData[1].myTime.GPT.sec -                 // Save interval
                           satelliteClkData[0].myTime.GPT.sec)) + 1;

     closeFile();
     return true;
}

