#include "MainWindow.h"
#include "parametermanager.h"
//#include "ui_MainWindow.h"

#include <QUrl>
#include <QMenu>
#include <QFile>
#include <QMenuBar>
#include <QEvent>
#include <QDebug>
#include <QString>
#include <QTextEdit>
#include <QWebView>
#include <QWebPage>
#include <QWebFrame>
#include <QInputDialog>
#include <QFontDialog>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    createMenu();
    //ui->setupUi(this);

    // Create the dock manager. Because the parent parameter is a QMainWindow
    // the dock manager registers itself as the central widget.
	m_DockManager = new ads::CDockManager(this);

	// Create example content label - this can be any application specific
	// widget
        auto t = createNewTerminal();

	// Create a dock widget with the title Label 1 and set the created label
	// as the dock widget content
	ads::CDockWidget* DockWidget = new ads::CDockWidget("Label 1");
	DockWidget->setWidget(t);

	// Add the toggleViewAction of the dock widget to the menu to give
	// the user the possibility to show the dock widget if it has been closed
	//ui->menuView->addAction(DockWidget->toggleViewAction());

	// Add the dock widget to the top dock widget area
	m_DockManager->addDockWidget(ads::TopDockWidgetArea, DockWidget);
}

MainWindow::~MainWindow()
{
    //delete ui;
}

void MainWindow::createMenu()
{
    auto menubar = menuBar();
    auto fileMenu = new QMenu(QStringLiteral("File"));
    auto m_toggleMenu = new QMenu(QStringLiteral("Toggle"));
    auto m_terminalMenu = new QMenu(QStringLiteral("Command"));
    auto prefMenu = new QMenu(QStringLiteral("Preference"));
    menubar->addMenu(fileMenu);
    menubar->addMenu(m_toggleMenu);
    menubar->addMenu(prefMenu);
    menubar->addMenu(m_terminalMenu);

    QAction *newAction = fileMenu->addAction(QStringLiteral("New DockWidget"));

    connect(newAction, &QAction::triggered, this, [this] {
        static int count = 0;
        count++;
        auto t = createNewTerminal();

	ads::CDockWidget* DockWidget = new ads::CDockWidget("Label 1");
	DockWidget->setWidget(t);
	m_DockManager->addDockWidget(ads::RightDockWidgetArea, DockWidget);
    });

    auto saveLayoutAction = fileMenu->addAction(QStringLiteral("Save Layout"));
    connect(saveLayoutAction, &QAction::triggered, this, [] {
        }
    );


    auto restoreLayoutAction = fileMenu->addAction(QStringLiteral("Restore Layout"));
    connect(restoreLayoutAction, &QAction::triggered, this, [this] {
        /*
        KDDockWidgets::RestoreOptions options = KDDockWidgets::RestoreOption_None;
        if (m_restoreIsRelative)
            options |= KDDockWidgets::RestoreOption_RelativeToMainWindow;

        KDDockWidgets::LayoutSaver saver(options);
        saver.restoreFromFile(QStringLiteral("mylayout.json"));
        */
    });

    auto closeAllAction = fileMenu->addAction(QStringLiteral("Close All"));
    connect(closeAllAction, &QAction::triggered, this, [this] {
       /* for (auto dw : qAsConst(m_dockwidgets))
            dw->close();
       */
    });

    //auto layoutEqually = fileMenu->addAction(QStringLiteral("Layout Equally"));
    //connect(layoutEqually, &QAction::triggered, this, &MainWindow::layoutEqually);

    auto quitAction = fileMenu->addAction(QStringLiteral("Quit"));
    //connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);


    auto termFontIncreaseAction = prefMenu->addAction(QStringLiteral("Font increase Current"));
    connect(termFontIncreaseAction, &QAction::triggered, this,[this] { foreach (TerminalWidget* term, m_listerm) { term->zoomIn(); } });

    auto termFontDecreaseAction = prefMenu->addAction(QStringLiteral("Font decrease Current"));
    connect(termFontDecreaseAction, &QAction::triggered, this,[this] { foreach (TerminalWidget* term, m_listerm) { term->zoomOut(); } });

    auto alltermFontIncreaseAction = prefMenu->addAction(QStringLiteral("Font increase for All"));
    connect(alltermFontIncreaseAction, &QAction::triggered, this,[this] { foreach (TerminalWidget* term, m_listerm) { term->zoomIn(); } });

    auto alltermFontDecreaseAction = prefMenu->addAction(QStringLiteral("Font decrease for All"));
    connect(alltermFontDecreaseAction, &QAction::triggered, this,[this] { foreach (TerminalWidget* term, m_listerm) { term->zoomOut(); } });

    auto termFontChoose = prefMenu->addAction(QStringLiteral("Choose Font"));
    connect(termFontChoose, &QAction::triggered, this,[this] {
               bool ok;
               QFont font = QFontDialog::getFont( &ok, QFont("Helvetica", 10), this);
               if (ok) {
                     foreach (TerminalWidget *term, m_listerm) {
                        term->setTerminalFont(font);
                        }
               }
    });

    auto colorStyleAction = prefMenu->addAction(QStringLiteral("Color Style"));
    connect(colorStyleAction, &QAction::triggered, this,[this] {

           QInputDialog qDialog ;

           QStringList items=TerminalWidget::availableColorSchemes();

           qDialog.setOptions(QInputDialog::UseListViewForComboBoxItems);
           qDialog.setComboBoxItems(items);
           qDialog.setWindowTitle("Choose Color Style");
           if (qDialog.exec() )
           {
               foreach (TerminalWidget* term, m_listerm) { term->setColorScheme(qDialog.textValue()); }
           }
       });

    auto sendallAction = m_terminalMenu->addAction(QStringLiteral("Send All"));
    connect(sendallAction, &QAction::triggered, this,&MainWindow::sendAll);

    auto sendmanyAction = m_terminalMenu->addAction(QStringLiteral("Send Many"));
    connect(sendmanyAction, &QAction::triggered, this,&MainWindow::sendMany);


}

TerminalWidget *MainWindow::createNewTerminal()
{
    TerminalWidget *t= new TerminalWidget();
    m_listerm.append(t);
    connect(t, &TerminalWidget::urlActivated, this,&MainWindow::urlActived);
    return t;
}

void MainWindow::sendAll() {
 bool ok;
 QString text = QInputDialog::getText(0, "Input dialog", "command", QLineEdit::Normal, "", &ok);
 text = text + "\n";
 if (ok && !text.isEmpty()) {
        TerminalWidget* term;
        // Be carefull , when terminal is killed , it's not remove from list
        foreach (term, m_listerm) {
             term->pasteText(text);
        }
 }
}
void MainWindow::sendMany() {
 bool ok;
 QString text = QInputDialog::getMultiLineText(this, tr("Open Terminal with commands"),
                                                  tr("Commands:"), "", &ok);
 if (ok && !text.isEmpty()) {
        QStringList lt = text.split("\n");
        foreach (QString cmd, lt) {
             if (cmd.isEmpty()) {
                continue;
             }
             TerminalWidget* term;
             // Be carefull , when terminal is killed , it's not remove from list

             //connect(newdock,&DockWidgetBase::aboutToDeleteOnClose    , this, [this] {
             //      qDebug() << " Dock Close ";
             // }
             //);

	     ads::CDockWidget* DockWidget = new ads::CDockWidget("Label 1");
             auto t = createNewTerminal();
             t->pasteText(cmd + "\n");
	     DockWidget->setWidget(t);
	     m_DockManager->addDockWidget(ads::RightDockWidgetArea, DockWidget);
        }
 }
}


void MainWindow::urlActived(const QUrl &u) {
    qDebug() << u;
    auto wv = new QWebView();
    auto wp = new QWebPage();
    wv->setPage(wp);
    auto wf = wp->currentFrame();
    wf->setUrl(u);

    ads::CDockWidget* DockWidget = new ads::CDockWidget(u.toString());
    DockWidget->setWidget(wv);
    m_DockManager->addDockWidget(ads::RightDockWidgetArea, DockWidget);
}

void MainWindow::commandRecieved(QString cwd,QStringList cmd)
{
 qDebug() << cmd;
 auto p = new parameterManager(cmd);
/*
 if (p->shell)
 {
     if (p->nameDock.length()> 0)
       {
         auto *dock = KDDockWidgets::DockRegistry::self()->dockByName(p->nameDock);
         qDebug() << dock;
         auto newdock= newDockWidget();
         if (dock != 0x0)
         {
            dock->addDockWidgetAsTab(newdock);
         }
         else
         {
           qDebug() << " dock namle not found " << p->nameDock;
         }
     }
     else
     {
         auto dock= newDockWidget();
         addDockWidget(dock, p->location);
     }
   }
*/
   if ( p->man)
   {
      urlActived(QUrl("https://man7.org/linux/man-pages/man1/"+p->nameMan+".1.html"));
    }
   if ( p->edit)
   {
       QString strScript="";
       strScript = readFile("/home/minix/STUFF/QT/KDDockWidgets/examples/minimal/monaco/index.tpl");
       strScript.replace(QString("@CONTENT@"),QString(readFile(p->nameFileToEdit)));
       QFile filew;
       filew.setFileName("/home/minix/STUFF/QT/KDDockWidgets/examples/minimal/monaco/index.html");
       QTextStream data(&filew);

       if(filew.open(QIODevice::WriteOnly)){
              data << strScript;
              filew.close();
        } else {
                qDebug() << " Can write file ";
        }
      urlActived(QUrl("file:////home/minix/STUFF/QT/KDDockWidgets/examples/minimal/monaco/index.html"));
  }
  delete(p);


}

QString MainWindow::readFile(QString filename)
{
       QString result="";
       QFile file;
       file.setFileName(filename);
       QTextStream data(&file);

       if(file.open(QIODevice::ReadOnly)){
             result= file.readAll();
             file.close();
       }
       return result;
}

