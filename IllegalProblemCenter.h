#ifndef ILLEGALPROBLEMCENTER_H
#define ILLEGALPROBLEMCENTER_H
#include <QString>
#include <QDebug>
#include <QElapsedTimer>
/*----------------------------------------------------------------------------
 * Name     : illegalParameterValue
 * Function : Catch all illegal error
 *--------------------------------------------------------------------------*/
class illegalParameterValue
{

public:
    illegalParameterValue():
        message("Illegal parameter value") {}
    illegalParameterValue(QString theMessage) {message = theMessage;}        // Avoid warning:  convert string to char        */
//    illegalParameterValue(char*  theMessage)  {message = theMessage;}
    void outputMessage() {                                                   // Output error message
        qDebug() << message;
    }

private:
    QString message;
};

#endif // ILLEGALPROBLEMCENTER_H
