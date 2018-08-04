#include "ProductDataCenter.h"

bool IgsStationCoordinateFile::readFile(const QString &filePath)
{
    QFile staCoordfile(filePath);
    if(!staCoordfile.open(QIODevice::ReadOnly))
    {
          QMessageBox::warning(NULL,                          "warning",
                              "IGS station coordinate file open faild!",
                               QMessageBox::Yes, QMessageBox::Yes);
          return false;
    }

    QTextStream txtInput(&staCoordfile);
    while(!txtInput.atEnd())
    {
        StationCoordData staCoordData;
        QString     lineQStr     = txtInput.readLine();
        QStringList strList      = lineQStr.simplified().split(",");
        staCoordData.date        = strList.at(0).toInt();
        staCoordData.MARKER_NAME = strList.at(1);
        staCoordData.obsPos[0]   = strList.at(2).toDouble();
        staCoordData.obsPos[1]   = strList.at(3).toDouble();
        staCoordData.obsPos[2]   = strList.at(4).toDouble();

        stationCoordinateData.push_back(staCoordData);
    }


    staCoordfile.close();
    return true;
}
