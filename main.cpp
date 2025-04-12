#include "MainWindow.h"
#include "parametermanager.h"
#include "QSingleInstance/qsingleinstance.h"
#include "threadhttpd.h"
#include "bookmarks.h"

#include <QDir>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QStringList cmd;
    for( int n = 0; n< argc; n++)
    {
      cmd << QString(argv[n]);
    }
    auto p = parameterManager(cmd);
    if ( p.isHelpRequest() ) 
    {
        p.showHelp();
        exit(0);
    }
    if ( p.isBookmarkRequest() ) 
    {
        BookmarkManager b ; 
        b.loadFromFile(BookmarkManager::pathBookmark());
        b.addBookmark (p.nameParam);
        b.saveToFile(BookmarkManager::pathBookmark());
    }

    QSingleInstance instance;

    instance.setAutoRecovery(true);

    MainWindow *w= nullptr;

    instance.setStartupFunction([&]() -> int {
                w = new MainWindow( a.applicationDirPath() );
                ThreadHttpd *t = new ThreadHttpd(w);
                instance.setNotifyWindow(w);
                t->start();
                w->show();
                return 0;
    });

    QObject::connect(&instance, &QSingleInstance::instanceMessage, [&](QStringList args){
                if (w)  {
                        qDebug() << args;
                        w->commandRecieved(QDir::currentPath(),args);
                       }
    });

    return instance.singleExec();
}
