#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>

#include "DockManager.h"
#include "qtterminalwidget/terminalwidget.h"
#include "QtAwesome/QtAwesome/QtAwesome.h"


class QMenu;
class QFileSystemModel;
class QTreeView;
class QClipboard;
class QStatusBar;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
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



private:
   void createMenu();
   QAction *createAction(QString ico, QString tooltip="",int rotation = 0 );
   QString readFile(QString filename);
   ads::CDockWidget* createNewTerminal();
   ads::CDockWidget* createFileSystemTreeDockWidget();
   TerminalWidget* createTerminalWidget();

   void sendAllPrivate(QLineEdit::EchoMode mode,QString label);



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

};

#endif // MAINWINDOW_H
