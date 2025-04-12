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

         mode = Mode::Help;
         return;
       }
     
      if ( params[n] =="-right") { location=ads::RightDockWidgetArea;    mode = Mode::Shell; }
      if ( params[n] =="-left") { location= ads::LeftDockWidgetArea;     mode = Mode::Shell; }
      if ( params[n] =="-top") { location= ads::TopDockWidgetArea;       mode = Mode::Shell; }
      if ( params[n] =="-bottom") { location= ads::BottomDockWidgetArea; mode = Mode::Shell; }
      if ( params[n] =="-tab") { createintab = true; }

      if ( params[n] =="man") {
         mode = Mode::Man;
         if (params.length() >n +1)
         {
            nameParam = params [n+1];
         }
      }
      if ( params[n] =="edit") {
          if (params.length() >n +1)
          {
            nameFileToEdit = params[n+1];
            mode = Mode::Edit;
          }
      }
      if ( params[n] =="view") {
          if (params.length() >n +1)
          {
            nameFileToEdit = params[n+1];
            mode = Mode::View;
          }
     }
     // Name of tab terminal
      if ( params[n] =="name") {
          if (params.length() >n +1)
          {
              nameDock = params[n+1];
              qDebug() << " NAME " << nameDock;
              mode = Mode::Shell;
              //DockerRegistry.self();
        }
      }
     if ( params[n] =="bookmark") {
         mode = Mode::Bookmark;
         if (params.length() >n +1)
         {
            nameParam = params [n+1];
         }
      }

  }
}

bool parameterManager::isHelpRequest()
{
  return mode == Mode::Help;
}

bool parameterManager::isBookmarkRequest()
{
  return mode == Mode::Bookmark;
}

void parameterManager::showHelp()
{
         qInfo() <<  " name ";
         qInfo() <<  " man ";
         qInfo() <<  " edit [file] ";
         qInfo() <<  " view [file] ";
         qInfo() <<  " bookmark [name]  ";
         qInfo() <<  " OPTIONS ";

      qInfo() << "\t\t-tab\tcreate in tab";
      qInfo() << "\t\t-righti\tcreate on right";
      qInfo() << "\t\t-left\t create on left";
      qInfo() << "\t\t-top\t create on top";
      qInfo() << "\t\t-bottom\t create on bottom";
}
