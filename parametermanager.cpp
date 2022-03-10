#include <QDebug>
#include "parametermanager.h"

parameterManager::parameterManager(QStringList params)
{
//   location=KDDockWidgets::Location_OnRight;
    // remove the first element ( program .. )
   shell = true;
  params.removeAt(0);
  for (int n = 0; n< params.length();n++) {
     
/*      if ( params[n] =="-right") { location=KDDockWidgets::Location_OnRight; }
      if ( params[n] =="-left") { location= KDDockWidgets::Location_OnLeft;}
      if ( params[n] =="-top") { location= KDDockWidgets::Location_OnTop; }
      if ( params[n] =="-bottom") { location= KDDockWidgets::Location_OnBottom; }
*/
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
