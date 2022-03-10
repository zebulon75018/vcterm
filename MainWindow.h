#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "DockManager.h"
#include "qtterminalwidget/terminalwidget.h"

/*namespace Ui {
class MainWindow;
}
*/
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
    void sendMany();

private:
   void createMenu();
   QString readFile(QString filename);
   TerminalWidget *createNewTerminal();

 //   Ui::MainWindow *ui;
    QList<TerminalWidget *> m_listerm;
    ads::CDockManager* m_DockManager;
};

#endif // MAINWINDOW_H
