#include "ProductDataCenter.h"

/*------------------------------------------------------------------------------
 * Name     : readFile
 * Function : read igs station coordinate file (*.coord)
 * Input    : const QString &filePath
 * Output   : bool (if read success)
 *-----------------------------------------------------------------------------*/
bool IgsStationCoordinateFile::readFile()
{
    if (!fileCommonDeal("IGS station coordinate file open faild!"))
        return false;
    QTextStream inText(&inFile);

    while(!inText.atEnd())
    {
        StationCoordData staCoordData;
        QString     lineQStr     = inText.readLine();
        QStringList strList      = lineQStr.simplified().split(",");
        staCoordData.date        = strList.at(0).toInt();
        staCoordData.MARKER_NAME = strList.at(1);
        staCoordData.obsPos[0]   = strList.at(2).toDouble();
        staCoordData.obsPos[1]   = strList.at(3).toDouble();
        staCoordData.obsPos[2]   = strList.at(4).toDouble();

        stationCoordinateData.push_back(staCoordData);
    }

    closeFile();
    return true;
}
