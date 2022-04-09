#ifndef PARAMETERMANAGER_H
#define PARAMETERMANAGER_H

#include <QStringList>
#include "ads_globals.h" 

class parameterManager
{
public:
    parameterManager(QStringList param);
    bool isHelpRequest(); 
    void showHelp(); 

   /***************************/  
    bool helpRequest = false;
    bool man = false;
    bool createintab = false;
    QString nameMan;
    bool edit = false;
    QString nameFileToEdit;
    bool shell = true;
    QString nameDock;
    ads::DockWidgetArea location;
};

#endif // PARAMETERMANAGER_H
