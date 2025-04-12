#ifndef PARAMETERMANAGER_H
#define PARAMETERMANAGER_H

#include <QStringList>
#include "ads_globals.h" 

class parameterManager
{
public:
  enum class Mode {
        None,
        Shell,
        Man,
        Edit,
        View,
        Bookmark,
        Help
    };

    parameterManager(QStringList param);
    bool isHelpRequest(); 
    bool isBookmarkRequest(); 
    void showHelp(); 

   /***************************/  
    Mode mode = Mode::None;
    bool createintab = false;
    QString nameParam; // Name of param for man and bookmark
    QString nameFileToEdit;
    QString nameDock;
    ads::DockWidgetArea location;
};

#endif // PARAMETERMANAGER_H
