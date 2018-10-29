#ifndef FILECENTER_H
#define FILECENTER_H
#include <qfile.h>
#include <QTextStream>
#include <QMessageBox>
#include <regex>
#include "IllegalProblemCenter.h"


/*---------------------------------------------------------------------
 * Name     : FileCenter
 * Function : Deal with all document related process
 *--------------------------------------------------------------------*/
class FileCenter
{
public:

    virtual void  setFilePath(const QString &FilePath) = 0;
    virtual bool  readFile(){return false;}
    virtual bool  outputFile(){return false;}

protected:
    virtual bool  fileCommonDeal(const QString &errorMessage)           // Be used to finished all file common processing
    {
        inFile.setFileName(filePath);
        if (! inFile.open( QIODevice::ReadOnly ))
        {
              QMessageBox::warning(NULL,  "warning", errorMessage,
                                   QMessageBox::Yes, QMessageBox::Yes);
              return false;
        }
        return true;
    }
    virtual void closeFile() {inFile.close();}
protected:
    QString       filePath;
    QFile         inFile;
};

#endif // FILECENTER_H
