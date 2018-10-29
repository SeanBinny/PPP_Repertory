#include "ProductDataCenter.h"

/*------------------------------------------------------------------------------
 * Name     : readFile
 * Function : read igs weekly solution file (*.snx)
 * Input    : const QString &filePath
 * Output   : bool (if read success)
 *-----------------------------------------------------------------------------*/
bool IgsWeeklySolutionFile::readFile()
{
    if (!fileCommonDeal("Igs Weekly Solution File Open faild!"))
        return false;

    QTextStream inText(&inFile);
    QString lineQStr = "";
    while ((lineQStr = inText.readLine()).indexOf("-SITE/ECCENTRICITY") < 0);     // Ignore the unuseful line
    lineQStr         = inText.readLine();
    lineQStr         = inText.readLine();
    lineQStr         = inText.readLine();

    int  sum = 0;
    while((lineQStr  = inText.readLine()) != "")                                 // Get station number
    {
        if (lineQStr.indexOf("-SOLUTION/EPOCHS") >= 0)
            break;
        else
            sum++;
    }
    weeklySolutionData = new StationCoordData[sum];
    while ((lineQStr = inText.readLine()).indexOf("+SOLUTION/ESTIMATE") < 0);    // Ignore the unuseful line

    int iter = 0;
    while ((lineQStr = inText.readLine()) !=  "")
    {
         if (lineQStr.indexOf("STAX") >= 0)
         {
             weeklySolutionData[iter].MARKER_NAME = lineQStr.mid(14,4);
             weeklySolutionData[iter].obsPos[0]   = lineQStr.mid(47,21).toDouble();
             lineQStr    = inText.readLine();
             weeklySolutionData[iter].obsPos[1]   = lineQStr.mid(47,21).toDouble();
             lineQStr    = inText.readLine();
             weeklySolutionData[iter].obsPos[2]   = lineQStr.mid(47,21).toDouble();
             iter++;
         }
         if(lineQStr.indexOf("-SOLUTION/ESTIMATE") >= 0)
             break;

    }
    closeFile();
    return true;
}
