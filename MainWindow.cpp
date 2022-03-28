#include "MainWindow.h"
#include "parametermanager.h"


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
#include <QFileDialog>
#include <QFileSystemModel>
#include <QTreeView>
#include <QToolBar>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{

    this->awesome = new QtAwesome();
    awesome->initFontAwesome();
    this->activeTerm = NULL;
    createMenu();

    // Create the dock manager. Because the parent parameter is a QMainWindow
    // the dock manager registers itself as the central widget.
    m_DockManager = new ads::CDockManager(this);


    auto FileSystemWidget = createFileSystemTreeDockWidget();
    auto ToolBar = FileSystemWidget->createDefaultToolBar();
    m_DockManager->addDockWidget(ads::LeftDockWidgetArea, FileSystemWidget);
    FileSystemWidget->viewToggled(false);

    // Create example content label - this can be any application specific
    // widget
    auto dw = createNewTerminal();
    // Add the dock widget to the top dock widget area
    m_DockManager->addDockWidget(ads::RightDockWidgetArea, dw);
}

MainWindow::~MainWindow()
{
    //delete ui;
}

void MainWindow::createMenu()
{
    auto menubar = menuBar();
    auto fileMenu = new QMenu(QStringLiteral("File"));
    m_toggleMenu = new QMenu(QStringLiteral("Toggle"));
    auto m_terminalMenu = new QMenu(QStringLiteral("Command"));
    auto prefMenu = new QMenu(QStringLiteral("Visual"));
    menubar->addMenu(fileMenu);
    menubar->addMenu(m_toggleMenu);
    menubar->addMenu(prefMenu);
    menubar->addMenu(m_terminalMenu);


    // Add the toggleViewAction of the dock widget to the menu to give
    // the user the possibility to show the dock widget if it has been closed

    QAction *newAction = fileMenu->addAction(QStringLiteral("New DockWidget"));
    QAction *fileChooseAction = fileMenu->addAction(QStringLiteral("Choose File and paste path"));
    QAction *filesChooseAction = fileMenu->addAction(QStringLiteral("Choose multiple Files and paste path"));
    QAction *dirChooseAction = fileMenu->addAction(QStringLiteral("Choose Dir and paste path"));

    connect(newAction, &QAction::triggered, this, [this] {
        static int count = 0;
        count++;
        auto dw = createNewTerminal();
	m_DockManager->addDockWidget(ads::RightDockWidgetArea, dw);
    });

    connect(fileChooseAction, &QAction::triggered, this, [this] {
      if ( this->activeTerm != NULL ) 
      {
         QString filename = QFileDialog::getOpenFileName(this, tr("Choose File"), this->activeTerm->workingDirectory(), tr("Files (*)"));
         if (filename!="")
         {
            this->activeTerm->pasteText(filename);
         }
       }
    });

  
    connect(filesChooseAction, &QAction::triggered, this, [this] {
      if ( this->activeTerm != NULL ) 
      {
         QStringList fileNames;

         fileNames = QFileDialog::getOpenFileNames(this, tr("Choose files"), this->activeTerm->workingDirectory(),("Files (*)"));
         for ( auto f : fileNames)
         {  
            this->activeTerm->pasteText(QString("\"") + f + QString("\" "));
         }
       }
    });


    connect(dirChooseAction, &QAction::triggered, this, [this] {
      if ( this->activeTerm != NULL ) 
      {
        QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                             this->activeTerm->workingDirectory(),
                                             QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
         if (dir!="")
         {
            this->activeTerm->pasteText(dir);
         }
     }
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
    connect(quitAction, &QAction::triggered, this, [this] { exit(0);  });


    auto termFontIncreaseAction = prefMenu->addAction(QStringLiteral("Font increase Current"));
    connect(termFontIncreaseAction, &QAction::triggered, this,[this] { foreach (TerminalWidget* term, m_listerm) { term->zoomIn(); } });

    auto termFontDecreaseAction = prefMenu->addAction(QStringLiteral("Font decrease Current"));
    connect(termFontDecreaseAction, &QAction::triggered, this,[this] { foreach (TerminalWidget* term, m_listerm) { term->zoomOut(); } });

    auto alltermFontIncreaseAction = prefMenu->addAction(QStringLiteral("Font increase for All"));
    connect(alltermFontIncreaseAction, &QAction::triggered, this,[this] { foreach (TerminalWidget* term, m_listerm) { term->zoomIn(); } });

    auto alltermFontDecreaseAction = prefMenu->addAction(QStringLiteral("Font decrease for All"));
    connect(alltermFontDecreaseAction, &QAction::triggered, this,[this] { foreach (TerminalWidget* term, m_listerm) { term->zoomOut(); } });

    auto termFontChoose = prefMenu->addAction(QStringLiteral("Choose Font"));
    connect(termFontChoose, &QAction::triggered, this,&MainWindow::chooseFont);
    auto colorStyleAction = prefMenu->addAction(QStringLiteral("Color Style"));
    connect(colorStyleAction, &QAction::triggered, this,&MainWindow::chooseColorStyle);

    auto sendallAction = m_terminalMenu->addAction(QStringLiteral("Send All"));
    connect(sendallAction, &QAction::triggered, this,&MainWindow::sendAll);

    auto sendmanyAction = m_terminalMenu->addAction(QStringLiteral("Send Many"));
    connect(sendmanyAction, &QAction::triggered, this,&MainWindow::sendMany);


}

void MainWindow::chooseFont() 
{
               bool ok;
               QFont font = QFontDialog::getFont( &ok, QFont("Helvetica", 10), this);
               if (ok) {
                     foreach (TerminalWidget *term, m_listerm) {
                        term->setTerminalFont(font);
                        }
               }
}


void MainWindow::chooseColorStyle()
{
           QInputDialog qDialog ;

           QStringList items=TerminalWidget::availableColorSchemes();

           qDialog.setOptions(QInputDialog::UseListViewForComboBoxItems);
           qDialog.setComboBoxItems(items);
           qDialog.setWindowTitle("Choose Color Style");
           if (qDialog.exec() )
           {
               foreach (TerminalWidget* term, m_listerm) { term->setColorScheme(qDialog.textValue()); }
           }
}

void MainWindow::chooseFileAndPaste () 
{
      if ( this->activeTerm != NULL ) 
      {
         QString filename = QFileDialog::getOpenFileName(this, tr("Choose File"), this->activeTerm->workingDirectory(), tr("Files (*)"));
         if (filename!="")
         {
            this->activeTerm->pasteText(filename);
         }
       }
}

  
void MainWindow::chooseFilesAndPaste () 
{
      if ( this->activeTerm != NULL ) 
      {
         QStringList fileNames;

         fileNames = QFileDialog::getOpenFileNames(this, tr("Choose files"), this->activeTerm->workingDirectory(),("Files (*)"));
         for ( auto f : fileNames)
         {  
            this->activeTerm->pasteText(QString("\"") + f + QString("\" "));
         }
       }
}


void MainWindow::chooseDirectoryAndPaste () 
{
      if ( this->activeTerm != NULL ) 
      {
        QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                             this->activeTerm->workingDirectory(),
                                             QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
         if (dir!="")
         {
            this->activeTerm->pasteText(dir);
         }
     }
}





//============================================================================
ads::CDockWidget* MainWindow::createFileSystemTreeDockWidget()
{
        static int FileSystemCount = 0;
        QTreeView* m_treeview = new QTreeView();
        m_treeview ->setFrameShape(QFrame::NoFrame);
        m_treeview ->setSelectionMode(QAbstractItemView::ExtendedSelection);
        m_treeview ->setDragEnabled(true);
        //w->setAcceptDrops(true);
        //w->setDropIndicatorShown(true);

        m_filesystemmodel = new QFileSystemModel(m_treeview);
        m_filesystemmodel->setRootPath(QDir::currentPath());
        m_treeview->setModel(m_filesystemmodel);
        for (int i = 1; i < m_filesystemmodel->columnCount(); ++i)
                m_treeview->hideColumn(i);
        ads::CDockWidget* DockWidget = new ads::CDockWidget(QString("Filesystem %1")
                .arg(FileSystemCount++));
        DockWidget->setWidget(m_treeview);
        DockWidget->setFeature(ads::CDockWidget::DockWidgetClosable, false);
        DockWidget->setFeature(ads::CDockWidget::DockWidgetMovable, false);
        DockWidget->setFeature(ads::CDockWidget::DockWidgetFloatable, false);

        auto ToolBar = DockWidget->createDefaultToolBar();
        //ViewMenu->addAction(DockWidget->toggleViewAction());
        QAction * tva = DockWidget->toggleViewAction();
        m_toggleMenu->addAction(tva);
        auto hideAction = createAction(fa::eyeslash);
        ToolBar->addAction(hideAction );
        connect(hideAction, &QAction::triggered, this,[tva] { tva->triggered();  });
        //tva->setChecked(false);
        m_treeview->expandAll();
        return DockWidget;
}

ads::CDockWidget* MainWindow::createNewTerminal()
{

    TerminalWidget *t= createTerminalWidget();
/*
    connect(t, &TerminalWidget::termGetFocus, this,[this] {  this->activeTerm =  qobject_cast<TerminalWidget*>(sender());
      qDebug() << this->activeTerm->workingDirectory();
      //m_filesystemmodel->setRootPath(this->activeTerm->workingDirectory());
      auto index =  m_filesystemmodel->index(this->activeTerm->workingDirectory());
      qDebug() << m_filesystemmodel->index(this->activeTerm->workingDirectory()).isValid();
      m_treeview->expand(index);
      m_treeview->scrollTo(index);
      //m_treeview->setCurrentIndex(m_filesystemmodel->index(this->activeTerm->workingDirectory()));

     });*/
    //connect(t, &TerminalWidget::termLostFocus,[this] { this->activeTerm =  NULL; });

    ads::CDockWidget* DockWidget = new ads::CDockWidget(QString("Term") + QString::number(m_listerm.length()));
    connect(DockWidget, & ads::CDockWidget::closed, this,[t] { qDebug() <<"Doc Cloed ";    });
    connect(t, &TerminalWidget::finished, this,[DockWidget,t] { qDebug() <<"Terminal Finished Cloed ";/* Todo erase from list */    });
    DockWidget->setWidget(t);
    auto ToolBar = DockWidget->createDefaultToolBar();
    QAction * tva = DockWidget->toggleViewAction();
    m_toggleMenu->addAction(tva);

    QVariantMap options;
    auto zoomInAction = createAction(fa::plus);
    auto zoomOutAction = createAction(fa::minus);
    auto fontAction = createAction(fa::font);
    auto searchAction = createAction(fa::search);
    auto clearAction = createAction(fa::bath);
    auto newTermAction = createAction(fa::columns);
    auto newTermTabAction = createAction(fa::clone);
    auto insertFolderAction = createAction(fa::folder);
    auto insertFileAction = createAction(fa::file);
    auto hideAction = createAction(fa::eyeslash);
    auto colorStyleAction = createAction(fa::blacktie);
    ToolBar->addAction(hideAction) ;
    ToolBar->addAction(zoomInAction );
    ToolBar->addAction(zoomOutAction );
    ToolBar->addAction(fontAction );
    ToolBar->addAction(colorStyleAction );
    ToolBar->addAction(searchAction );
    ToolBar->addAction( insertFileAction);
    ToolBar->addAction( insertFolderAction);
    ToolBar->addAction(newTermTabAction );
    ToolBar->addAction(newTermAction );
    ToolBar->addAction(clearAction );

    connect(insertFileAction, &QAction::triggered, this,&MainWindow::chooseFilesAndPaste);
    connect(insertFolderAction, &QAction::triggered, this,&MainWindow::chooseDirectoryAndPaste);
    connect(zoomInAction, &QAction::triggered, this,[t] { t->zoomIn();  });
    connect(zoomOutAction, &QAction::triggered, this,[t] { t->zoomOut();  });
    connect(searchAction, &QAction::triggered, this,[t] { t->toggleShowSearchBar();  });
    connect(clearAction, &QAction::triggered, this,[t] { t->clear();  });
    connect(fontAction, &QAction::triggered, this,[t] { t->clear();  });
    connect(hideAction, &QAction::triggered, this,[tva] { tva->triggered();  });
    connect(fontAction, &QAction::triggered, this,&MainWindow::chooseFont);
    connect(colorStyleAction, &QAction::triggered, this,&MainWindow::chooseColorStyle);

    connect(newTermTabAction, &QAction::triggered, this,[this] {
      ads::CDockWidget* docknew = createNewTerminal();
       TerminalWidget* tn = qobject_cast<TerminalWidget*>(docknew->widget());
       if (this->activeTerm != NULL)
       {
            tn->changeDir(this->activeTerm->workingDirectory());
       }
       m_DockManager->addDockWidgetTab(ads::RightDockWidgetArea, docknew);

    });

    connect(newTermAction, &QAction::triggered, this,[this] {
       ads::CDockWidget* docknew = createNewTerminal();
       TerminalWidget* tn = qobject_cast<TerminalWidget*>(docknew->widget());
       if (this->activeTerm != NULL)
       {
            tn->changeDir(this->activeTerm->workingDirectory());
       }
       m_DockManager->addDockWidget(ads::RightDockWidgetArea, docknew);
    });

    //ToolBar->addAction(ui.actionRestoreState);


    return DockWidget;
}

void MainWindow::getFocusTerm()
{
      this->activeTerm =  qobject_cast<TerminalWidget*>(sender());
      qDebug() << this->activeTerm->workingDirectory();
      //m_filesystemmodel->setRootPath(this->activeTerm->workingDirectory());
      //auto index =  m_filesystemmodel->index(this->activeTerm->workingDirectory());
      //qDebug() << m_filesystemmodel->index(this->activeTerm->workingDirectory()).isValid();
      //m_treeview->setCurrentIndex(index);
      //m_treeview->scrollTo(index);
}

/** 
  * Create a QAction with a awesome font 
***/
QAction *MainWindow::createAction(fa::icon ico )
{
    QVariantMap options;
    auto result = new QAction("");
    result->setIcon( awesome->icon( ico,options  ) );
    return result;
}

TerminalWidget* MainWindow::createTerminalWidget()
{
    TerminalWidget *t= new TerminalWidget();
    m_listerm.append(t);
    connect(t, &TerminalWidget::termGetFocus, this, &MainWindow::getFocusTerm);
    connect(t, &TerminalWidget::urlActivated, this,&MainWindow::urlActived);
    connect(t, &TerminalWidget::copyAvailable, this,[this] { qDebug() << "copyAvailable " << this->activeTerm->selectedText();  });
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

             auto dw = createNewTerminal();
             TerminalWidget* t = qobject_cast<TerminalWidget*>(dw->widget());
             t->pasteText(cmd + "\n");
	     m_DockManager->addDockWidget(ads::RightDockWidgetArea, dw);
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

