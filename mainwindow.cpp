#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "FileCenter.h"
#include "ProductDataCenter.h"
#include "RinexDataCenter.h"
#include "ResultDataCenter.h"
#include "DataProcessCenter.h"
#include "FunctionalModelCenter.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void getAllFilePath(QString sourcePath, QFileInfoList *List)
{
    // QString sourcePath = "D:\\PPPData\\cut0_product";
    ////// QString path = SourcePath + F_info.year + "\\" + F_info.year + F_info.station; // assign a source path

    // create a object of QDir to screen all the file
    // and list files ( except the symbolic links files ) by the order of Name
    QDir dir(sourcePath);
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks); // set a general conditions of filter to ignore some files
    dir.setSorting(QDir::Name ); // sort the files by the order of Name
    QStringList strings; // set the filter type
    strings << "*clk" <<"*15P"<< "*erp"<< "*sp3" << "*atx" <<"*bsx" <<"*blq" << "*.coord"<<"*15o"; // choose all the files which suffix is " .16zpd "

    // screen all the files has "strings" and returns a path list of all the files and directories in the directory
    *List = dir.entryInfoList( strings, QDir::AllEntries, QDir::DirsFirst );
    // screen all the folder and returns a path list of all the folders in the directory
   // *List = dir.entryInfoList( QDir::NoDotAndDotDot |QDir::Dirs | QDir::Hidden | QDir::NoSymLinks, QDir::DirsFirst );
}




void MainWindow::on_pushButton_ReadFile_clicked()
{
//    //1 ......................................................................
//    QString filename = QFileDialog::getOpenFileName(this,
//                                                    "Open Document",
//                                                    QDir::currentPath(),
//                                                    "Document files (*.txt );;All files(*.*)");
//    if (!filename.isNull()) { //用户选择了文件  //
//       // 处理文件

//       PrecisionEphemerisFile PF;
//       PF.setFilePath(filename);
//       PF.readFile();
//       QMessageBox::information(this, "Document", "成功选择文件", QMessageBox::Ok | QMessageBox::Cancel);
//    } else // 用户取消选择
//       QMessageBox::information(this, "Document", "未选择文件", QMessageBox::Ok | QMessageBox::Cancel);

}

void MainWindow::on_pushButton_OPENFILE_clicked()
{
    ModeFlag::hasGPS   = true;
    ModeFlag::P1_P2    = true;
    ModeFlag::Model_UD = true;
    ModeFlag::TF       = false;
    ModeFlag::getSystemNum();

    ObsJudgeCenter::eleMaskAngle = 5;   // ele set 5 degree

    QFileInfoList   filePathList ;
    getAllFilePath("D:\\PPPData\\cut0_product", &filePathList);

    PrecisionEphemerisFile ephFile;
    PrecisionClockFile clkFile;
    OceanTideFile oceFile;
    AntennaInfoFile antFile;

    DifferentCodeBiasFile dcbFile;
    IgsStationCoordinateFile coordFile;
    EarthRotationParameterFile erpFile;

    // "*clk" <<"*15P"<< "*erp"<< "*sp3" << "*atx" <<"*bsx" <<"*blq" << "*.coord";
    // 看看是不可以用指针动态指向这些文件
    for( int i = 0; i < filePathList.size(); ++i )
    {
        if (filePathList.at( i ).filePath().indexOf("atx") >= 0){
            antFile.setFilePath(filePathList.at( i ).filePath());
            antFile.readFile();
        } else if (filePathList.at( i ).filePath().indexOf("clk") >= 0){
            clkFile.setFilePath(filePathList.at( i ).filePath());
            clkFile.readFile();
        } else if (filePathList.at( i ).filePath().indexOf("erp") >= 0){
            erpFile.setFilePath(filePathList.at( i ).filePath());
            erpFile.readFile();
        } else if (filePathList.at( i ).filePath().indexOf("sp3") >= 0){
            ephFile.setFilePath(filePathList.at( i ).filePath());
            ephFile.readFile();
        } else if (filePathList.at( i ).filePath().indexOf("blq") >= 0){
            oceFile.setFilePath(filePathList.at( i ).filePath());
            oceFile.readFile();
        } else if (filePathList.at( i ).filePath().indexOf("coord") >= 0){
            coordFile.setFilePath(filePathList.at( i ).filePath());
            coordFile.readFile();
        }
    } // end for

    ObservationFile obsFile;
    NavigationFile  navFile;

    getAllFilePath("D:\\PPPData\\cut0_rinex", &filePathList);
    for( int i = 0; i < filePathList.size(); ++i )
    {
        if (filePathList.at( i ).filePath().indexOf("15p") >= 0){
            navFile.setFilePath(filePathList.at( i ).filePath());
            navFile.readFile();
        } else if (filePathList.at( i ).filePath().indexOf("15o") >= 0){
            obsFile.setFilePath(filePathList.at( i ).filePath());
            obsFile.readFile();
        }
    } // end for


//    // 下面这一块  是否放入PPPoutFile里面
//    int coordDate  =   obsFile.AllObservationData.front().myTime.GPT.week * 10 +
//                   int(obsFile.AllObservationData.front().myTime.GPT.sec  * SEC_DAY);
//    int posOfCoord =   DataMatchingCenter::seekApproxCoordPos(coordFile, obsFile.MARKER_NAME, coordDate);
//    if (posOfCoord == -1)
//        throw illegalParameterValue("Precision coordinate can't find !");
//    else
//    {
//        obsFile.APPROX_POSITION[0] = coordFile.stationCoordinateData[posOfCoord].obsPos[0];
//        obsFile.APPROX_POSITION[1] = coordFile.stationCoordinateData[posOfCoord].obsPos[1];
//        obsFile.APPROX_POSITION[2] = coordFile.stationCoordinateData[posOfCoord].obsPos[2];
//    }


    ResultFile_PPP resFile;
    resFile.getFilesPtr(&ephFile, &clkFile, &oceFile,   &antFile,
                        &erpFile, &obsFile, &coordFile, &sunMoonPos,  navFile.K);
    resFile.setFilePath("D:\\PPPData\\");
    resFile.outputFile();


    QFile inFile("D:\\PPPData\\neu.neu");
    if (! inFile.open( QIODevice::WriteOnly))
    {
          QMessageBox::warning(NULL,  "warning", "errorMessag",
                               QMessageBox::Yes, QMessageBox::Yes);
    }
    QTextStream neuText(&inFile);
    FilterProcessingCenter pppFilter;
    pppFilter.filterProcessing(resFile.finalDataFile, neuText);


    QMessageBox::information(this, "Document", "成功选择文件", QMessageBox::Ok | QMessageBox::Cancel);
}

void MainWindow::on_pushButton_PPP_clicked()
{
    ResultFile_PPP resFile;

}
