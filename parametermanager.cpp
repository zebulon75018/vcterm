#include <QDebug>
#include "parametermanager.h"

parameterManager::parameterManager(QStringList params)
{
  location=ads::RightDockWidgetArea;
    // remove the first element ( program .. )
  params.removeAt(0);
  for (int n = 0; n< params.length();n++) {
      if ( params[n].toLower() =="-h" || params[n].toLower() == "--help") 
      { 

         helpRequest = true;
         return;
       }
     
      shell = true;
      if ( params[n] =="-right") { location=ads::RightDockWidgetArea; }
      if ( params[n] =="-left") { location= ads::LeftDockWidgetArea;}
      if ( params[n] =="-top") { location= ads::TopDockWidgetArea; }
      if ( params[n] =="-bottom") { location= ads::BottomDockWidgetArea; }
      if ( params[n] =="-tab") { createintab = true; }

      if ( params[n] =="man") {
         if (params.length() >n +1)
         {
             nameMan = params [n+1];
            man  = true;
            shell = false;
            edit  = false;
         }
      }

      if ( params[n] =="edit") {
          if (params.length() >n +1)
          {
            nameFileToEdit = params[n+1];
            edit  = true;
            man  = false;
            shell = false;
          }
      }
     // Name of tab terminal
      if ( params[n] =="name") {
          if (params.length() >n +1)
          {
              nameDock = params[n+1];
              qDebug() << " NAME " << nameDock;
              shell = true;
              man  = false;
              //DockerRegistry.self();

        }
      }
  }
}
bool parameterManager::isHelpRequest()
{
  return helpRequest;
}

void parameterManager::showHelp()
{
         qInfo() <<  " name ";
         qInfo() <<  " man ";
         qInfo() <<  " edit : ";
         qInfo() <<  " OPTIONS ";

      qInfo() << "\t\t-tab\tcreate in tab";
      qInfo() << "\t\t-righti\tcreate on right";
      qInfo() << "\t\t-left\t create on left";
      qInfo() << "\t\t-top\t create on top";
      qInfo() << "\t\t-bottom\t create on bottom";
}
