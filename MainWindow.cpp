#include "MainWindow.h"
#include "parametermanager.h"

#include "DockAreaWidget.h"


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
#include <QClipboard>
#include <QGuiApplication>
#include <QStatusBar>
#include <QMimeData>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{


    this->awesome = new QtAwesome();
    awesome->initFontAwesome();
    this->activeTerm = NULL;
    this->clipboard = QGuiApplication::clipboard();
    this->statusbar = this->statusBar();

    connect( this->clipboard, &QClipboard::selectionChanged,this,&MainWindow::selectionChangeClipboard );
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
    connect(saveLayoutAction, &QAction::triggered, this, [this] {
           QMap<QString, ads::CDockWidget*> dMap =   this->m_DockManager->dockWidgetsMap();
           foreach ( const auto& outerKey, dMap.keys() )
              {
                qDebug() << " Name " << outerKey;
                qDebug() << " DockWidget " << dMap[outerKey];
             }
             QList<ads::CDockContainerWidget*> dc = this->m_DockManager->dockContainers();
             foreach( ads::CDockContainerWidget* c , dc )
             {
                qDebug() << " Dc " << c;
             }

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

    auto sendallActionPass = m_terminalMenu->addAction(QStringLiteral("Send All Password"));
    connect(sendallActionPass,&QAction::triggered, this,&MainWindow::sendAllPassword);

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
        m_treeview = new QTreeView();
        m_treeview ->setFrameShape(QFrame::NoFrame);
        m_treeview ->setSelectionMode(QAbstractItemView::ExtendedSelection);
        m_treeview ->setDragEnabled(true);

        m_treeview->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(m_treeview, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onDirContextMenu(const QPoint &)));
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
        auto hideAction = createAction(fa::eyeslash,"Hide");
        auto colorAction = createAction(fa::blacktie,"Change Dark/White");
        colorAction->setCheckable(true);
        ToolBar->addAction(hideAction );
        ToolBar->addAction(colorAction );
        connect(hideAction, &QAction::triggered, this,[tva] { tva->triggered();  });
        connect(colorAction, &QAction::triggered, this,[this] {
                                     QAction *s =  qobject_cast<QAction*>(sender());
                                     if (s->isChecked())  this->m_treeview->setStyleSheet("background: #050505;color: white");  
                                     else this->m_treeview->setStyleSheet("background: white;color: black");  
                                 });
        // Hide file tree widget
        m_treeview->expandAll();
        return DockWidget;
}

ads::CDockWidget* MainWindow::createNewTerminal()
{

    TerminalWidget *t= createTerminalWidget();
    QList<QAction *> lstAction;

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
    DockWidget->setFeature(ads::CDockWidget::DockWidgetDeleteOnClose, true); 
    connect(DockWidget, & ads::CDockWidget::closed, this,[t] { delete(t) ; qDebug() <<"Doc Closed ";    });
    connect(t, &TerminalWidget::finished, this,[DockWidget,t] {
           DockWidget->close(); qDebug() <<"Terminal Finished Closed ";
           /* Todo erase from list */   
          });
    DockWidget->setWidget(t);
    auto ToolBar = DockWidget->createDefaultToolBar();
    QAction * tva = DockWidget->toggleViewAction();
    m_toggleMenu->addAction(tva);

    QVariantMap options;
    auto zoomInAction = createAction(fa::expand,"Font size increase");
    auto zoomOutAction = createAction(fa::compress,"Font size Decrease");
    auto fontAction = createAction(fa::font,"Choose Font");
    auto searchAction = createAction(fa::search,"Search in console");
    auto clearAction = createAction(fa::bath,"Clear");
    auto newTermAction = createAction(fa::clone,"New Term in tab");
    auto newTermActionDown = createAction(fa::columns,"New Term Down",90);
    auto newTermTabAction = createAction(fa::plus,"New Tab on the right");
    auto insertFolderAction = createAction(fa::folder,"Choose a folder and add path in console ");
    auto insertFileAction = createAction(fa::file,"Choose files and add in console ");
    auto hideAction = createAction(fa::eyeslash,"Hide Console");
    auto colorStyleAction = createAction(fa::blacktie,"Choose Color Style");
    ToolBar->addAction(hideAction) ;
    ToolBar->addAction(zoomInAction );
    ToolBar->addAction(zoomOutAction );
    ToolBar->addAction(fontAction );
    ToolBar->addAction(colorStyleAction );
    ToolBar->addSeparator();
    ToolBar->addAction(clearAction );
    ToolBar->addSeparator();
    ToolBar->addAction(searchAction );
    ToolBar->addAction( insertFileAction);
    ToolBar->addAction( insertFolderAction);
    ToolBar->addSeparator();
    ToolBar->addAction(newTermAction );
    ToolBar->addAction(newTermActionDown );
    ToolBar->addAction(newTermTabAction );
    lstAction.append(zoomInAction );
    lstAction.append(zoomOutAction );
    lstAction.append(fontAction );
    lstAction.append(clearAction );

    connect(insertFileAction,   &QAction::triggered, this,&MainWindow::chooseFilesAndPaste);
    connect(insertFolderAction, &QAction::triggered, this,&MainWindow::chooseDirectoryAndPaste);
    connect(zoomInAction,       &QAction::triggered, this,[t] { t->zoomIn();  });
    connect(zoomOutAction,      &QAction::triggered, this,[t] { t->zoomOut();  });
    connect(searchAction,       &QAction::triggered, this,[t] { t->toggleShowSearchBar();  });
    connect(clearAction,        &QAction::triggered, this,[t] { t->clear();  });
    connect(fontAction,         &QAction::triggered, this,[t] { t->clear();  });
    connect(hideAction,         &QAction::triggered, this,[tva] { tva->triggered();  });
    connect(fontAction,         &QAction::triggered, this,&MainWindow::chooseFont);
    connect(colorStyleAction,   &QAction::triggered, this,&MainWindow::chooseColorStyle);

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

    connect(newTermActionDown, &QAction::triggered, this,[this] {
       ads::CDockWidget* docknew = createNewTerminal();
       TerminalWidget* tn = qobject_cast<TerminalWidget*>(docknew->widget());
       if (this->activeTerm != NULL)
       {
            tn->changeDir(this->activeTerm->workingDirectory());
       }
       m_DockManager->addDockWidget(ads::BottomDockWidgetArea, docknew);
    });

    t->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(t,&QWidget::customContextMenuRequested, 
        t,[this,t,lstAction] (const QPoint &p) {
        QMenu menu;
        foreach (QAction *ac, lstAction) {
            menu.addAction(ac);
            ac->setText(ac->toolTip());
        }
        menu.exec(t->mapToGlobal(p));
        foreach (QAction *ac, lstAction) {
           ac->setText("");
        }
       }
    );

    //ToolBar->addAction(ui.actionRestoreState);

    ToolBar->setStyleSheet("background-color:black; color:lightgrey");
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
QAction *MainWindow::createAction(fa::icon ico, QString tooltip,int rotation)
{
    QVariantMap options;
    auto result = new QAction("");
    if ( rotation != 0) {
        QTransform tr;
        tr.rotate(90);
        auto icon= awesome->icon( ico,options  ) ;
        QPixmap p = icon.pixmap(QSize(16,16));
        p = p.transformed(tr);
        result->setIcon( QIcon(p)  );
    } 
    else 
    {
      result->setIcon( awesome->icon( ico,options  ) );
    }
    result->setToolTip( tooltip );
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
   sendAllPrivate(QLineEdit::Normal,"command");
}

void MainWindow::sendAllPassword() {
   sendAllPrivate(QLineEdit::Password,"password be carefull the text will be pass in clear to terminal");
}

void MainWindow::sendAllPrivate(QLineEdit::EchoMode mode,QString label) {
 bool ok;
 QString text = QInputDialog::getText(0, "Input dialog", label, mode, "", &ok);
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

void MainWindow::selectionChangeClipboard() {
     const QMimeData *mimeData = clipboard->mimeData();

     if (mimeData->hasImage()) {
        //setPixmap(qvariant_cast<QPixmap>(mimeData->imageData()));
        this->statusbar->showMessage("clipboard changed: Image  " ,1000);
    } else if (mimeData->hasText()) {
        this->statusbar->showMessage("clipboard changed: " + mimeData->text() ,1000);
    } else if (mimeData->hasHtml()) {
        //this->statusbar->showMessage("clipboard changed: Html  " ,1000);
        this->statusbar->showMessage("clipboard changed: " + mimeData->text() ,1000);
        //setText(mimeData->html());
        //setTextFormat(Qt::RichText);
    } else {
        this->statusbar->showMessage("clipboard changed can't display " ,1000);
    }
}

void MainWindow::onDirContextMenu(const QPoint &p) 
{
    QModelIndex index = m_treeview->indexAt(p);
    qDebug() << " index.isValid()  ";
    QMenu *menu = new QMenu;
 
     //QString fileName = this->model()->data(this->model()->index(index.row(), 0),0).toString();
     menu->addAction(QString("Open"), this, [this] {});
     menu->addAction(QString("Rename"), this, [this] {});
     menu->exec(m_treeview->viewport()->mapToGlobal(p));
     menu->deleteLater();
   /* if (index.isValid() && index.row() % 2 == 0) {
        contextMenu->exec(m_treeview->viewport()->mapToGlobal(p));
    }    
*/
}

/*********************************************
  * Command recieve form QSingletonApplication
***********************************************/
void MainWindow::commandRecieved(QString cwd,QStringList cmd)
{
 qDebug() << cmd;
 auto p = new parameterManager(cmd);

 if (p->shell)
 {
     if (p->nameDock.length()> 0)
       {
         QMap<QString, ads::CDockWidget*> dMap =   this->m_DockManager->dockWidgetsMap();
         foreach ( const auto& outerKey, dMap.keys() ) {
              if (outerKey  == p->nameDock) {
                     auto dw = createNewTerminal();  
                     if (p->createintab)
                     {
	                m_DockManager->addDockWidgetTab( p->location, dw);
                     } else {
	                m_DockManager->addDockWidget(p->location, dw, dMap[outerKey]->dockAreaWidget());
                     }
              }
         }
     }
     else
     {
           auto dw = createNewTerminal();  
	   m_DockManager->addDockWidget(p->location, dw);
     }
   }

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

