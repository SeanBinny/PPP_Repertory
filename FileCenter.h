#ifndef FILECENTER_H
#define FILECENTER_H
#include <qfile.h>
#include <QTextStream>
#include <QMessageBox>
#include <regex>

/*--------------------------------------------------------------
 * Name     : FileCenter
 * Function : Deal with all document related process
 *-------------------------------------------------------------*/
class FileCenter
{
public:

    virtual  bool readFile(const QString &filePath) = 0;

             void outputFile();

};

#endif // FILECENTER_H
