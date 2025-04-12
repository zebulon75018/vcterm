#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QThread>
#include <QWebPage>



#include "DockManager.h"
#include "qtterminalwidget/terminalwidget.h"
#include "QtAwesome/QtAwesome/QtAwesome.h"


class parameterManager;
class QMenu;
class QFileSystemModel;
class QTreeView;
class QClipboard;
class QStatusBar;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QString adir,QWidget *parent = 0);
    ~MainWindow();

public slots:

    void urlActived(const QUrl &u);
    void commandRecieved(QString cwd,QStringList cmd);
    void sendAll();
    void sendAllPassword();
    void sendMany();
    void getFocusTerm();
    void chooseFont();
    void chooseColorStyle();
    void chooseFileAndPaste ();
    void chooseFilesAndPaste ();
    void chooseDirectoryAndPaste();
    void selectionChangeClipboard();
    void onDirContextMenu(const QPoint &p);
    void consoleMessageReceived(QWebPage::MessageSource source,QWebPage::MessageLevel level, const QString & message, int lineNumber, const QString & sourceID);



private:
   void createMenu();
   QAction *createAction(QString ico, QString tooltip="",int rotation = 0 );
   QString readFile(QString filename);
   void saveFile(QString filename,QString& content);
   ads::CDockWidget* createNewTerminal();
   ads::CDockWidget* createFileSystemTreeDockWidget();
   TerminalWidget* createTerminalWidget();
   void editfile(parameterManager *);

   void sendAllPrivate(QLineEdit::EchoMode mode,QString label);
   QString joinDir(QString dir,QString subdir);
   void handleShellMode(parameterManager& p);
   QList<QAction*> createTerminalToolBarActions(TerminalWidget* t, ads::CDockWidget* dw);
   QTreeView* createConfiguredTreeView();
   QFileSystemModel* createAndSetFileSystemModel(QTreeView* view);
   void configureFileSystemToolbar(ads::CDockWidget* dock, QTreeView* view);
   QString renderTemplate(const QStringList& items, const QString& user);







 //   Ui::MainWindow *ui;
    QList<TerminalWidget *> m_listerm;
    TerminalWidget * activeTerm;
    ads::CDockManager* m_DockManager;
    QMenu *m_toggleMenu;
    QFileSystemModel *m_filesystemmodel;
    QTreeView *m_treeview;
    fa::QtAwesome *awesome;
    QClipboard *clipboard;
    QStatusBar *statusbar;
    QString appdir;

};

#endif // MAINWINDOW_H
