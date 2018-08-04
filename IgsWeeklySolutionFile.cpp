#include "ProductDataCenter.h"

/*------------------------------------------------------------------------------
 * Name     : readFile
 * Function : read igs weekly solution file (*.snx)
 * Input    : const QString &filePath
 * Output   : bool (if read success)
 *-----------------------------------------------------------------------------*/
bool IgsWeeklySolutionFile::readFile(const QString &filePath)
{
    QFile weeklyFile(filePath);
    if(!  weeklyFile.open( QIODevice::ReadOnly ))
    {
          QMessageBox::warning(NULL,                       "warning",
                              "Igs Weekly Solution File Open faild!",
                               QMessageBox::Yes, QMessageBox::Yes);
          return false;
    }

    QString lineQStr = "";
    while ((lineQStr = weeklyFile.readLine()).indexOf("-SITE/ECCENTRICITY") < 0);
    lineQStr         = weeklyFile.readLine();
    lineQStr         = weeklyFile.readLine();
    lineQStr         = weeklyFile.readLine();

    int  sum = 0;
    while((lineQStr  = weeklyFile.readLine()) != "")
    {
        if (lineQStr.indexOf("-SOLUTION/EPOCHS") >= 0)
            break;
        else
            sum++;
    }
    weeklySolutionData = new StationCoordData[sum];
    while ((lineQStr = weeklyFile.readLine()).indexOf("+SOLUTION/ESTIMATE") < 0);

    int iter = 0;
    while ((lineQStr = weeklyFile.readLine()) !=  "")
    {
         if (lineQStr.indexOf("STAX") >= 0)
         {
             weeklySolutionData[iter].MARKER_NAME = lineQStr.mid(14,4);
             weeklySolutionData[iter].obsPos[0]   = lineQStr.mid(47,21).toDouble();
             lineQStr    = weeklyFile.readLine();
             weeklySolutionData[iter].obsPos[1]   = lineQStr.mid(47,21).toDouble();
             lineQStr    = weeklyFile.readLine();
             weeklySolutionData[iter].obsPos[2]   = lineQStr.mid(47,21).toDouble();
             iter++;
         }
         if(lineQStr.indexOf("-SOLUTION/ESTIMATE") >= 0)
             break;

    }
    weeklyFile.close();
}
