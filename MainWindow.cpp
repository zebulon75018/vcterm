#include "MainWindow.h"
#include <memory> // à mettre tout en haut pour std::unique_ptr
#include "parametermanager.h"
#include "typeFile.h"
#include "bookmarks.h"

#include "DockAreaWidget.h"

//#include  "templateengine/template.h"
//#include "minja/minja.hpp"

//#include "inja/inja.hpp"
//#include "nlohmann/json.hpp"
#include "templateEngineQt.h"

#include <QUrl>
#include <QMenu>
#include <QFile>
#include <QMenuBar>
#include <QEvent>
#include <QDebug>
#include <QString>
#include <QTextEdit>
#include <QWebView>
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

#include <QTextCodec>

#include <iostream>

//using json = nlohmann::ordered_json;
//using namespace stefanfrings;




MainWindow::MainWindow(QString adir,QWidget *parent) :
    QMainWindow(parent)
{


    this->appdir = adir;
    this->awesome = new fa::QtAwesome();
    this->awesome->initFontAwesome();
    this->activeTerm = nullptr;
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
    FileSystemWidget->toggleView(false);

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

    BookmarkManager bookmarks;
    bookmarks.loadFromFile(BookmarkManager::pathBookmark());

    auto menubar = menuBar();
    auto fileMenu = new QMenu(QStringLiteral("File"));
    m_toggleMenu = new QMenu(QStringLiteral("Toggle"));
    auto m_terminalMenu = new QMenu(QStringLiteral("Command"));
    auto m_bookmark = new QMenu(QStringLiteral("Bookmark"));
    auto prefMenu = new QMenu(QStringLiteral("Visual"));
    menubar->addMenu(fileMenu);
    menubar->addMenu(m_toggleMenu);
    menubar->addMenu(prefMenu);
    menubar->addMenu(m_terminalMenu);
    menubar->addMenu(m_bookmark);


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
      if ( this->activeTerm != nullptr ) 
      {
         QString filename = QFileDialog::getOpenFileName(this, tr("Choose File"), this->activeTerm->workingDirectory(), tr("Files (*)"));
         if (filename!="")
         {
            this->activeTerm->pasteText(filename);
         }
       }
    });

  
    connect(filesChooseAction, &QAction::triggered, this, [this] {
      if ( this->activeTerm != nullptr ) 
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
      if ( this->activeTerm != nullptr ) 
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


    foreach ( QString bname , bookmarks.getBookmarkNames())
    {
      m_bookmark->addAction(bname);
    }

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
      if ( this->activeTerm != nullptr ) 
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
      if ( this->activeTerm != nullptr ) 
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
      if ( this->activeTerm != nullptr ) 
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

    m_treeview = createConfiguredTreeView();
    m_filesystemmodel = createAndSetFileSystemModel(m_treeview);

    auto dock = new ads::CDockWidget(QString("Filesystem %1").arg(FileSystemCount++));
    dock->setWidget(m_treeview);
    dock->setFeature(ads::CDockWidget::DockWidgetClosable, false);
    dock->setFeature(ads::CDockWidget::DockWidgetMovable, false);
    dock->setFeature(ads::CDockWidget::DockWidgetFloatable, false);

    configureFileSystemToolbar(dock, m_treeview);

    return dock;
}


ads::CDockWidget* MainWindow::createNewTerminal()
{
    TerminalWidget* t = createTerminalWidget();
    auto dw = new ads::CDockWidget(QString("Term") + QString::number(m_listerm.length()));

    dw->setFeature(ads::CDockWidget::DockWidgetDeleteOnClose, true);
    connect(dw, &ads::CDockWidget::closed, this, [t]() {
        delete t;
        qDebug() << "Dock Closed";
    });
    connect(t, &TerminalWidget::finished, this, [dw]() {
        dw->close();
        qDebug() << "Terminal Finished Closed";
    });

    dw->setWidget(t);
    auto toolbar = dw->createDefaultToolBar();
    toolbar->setStyleSheet("background-color:black; color:lightgrey");

    QAction* toggleViewAction = dw->toggleViewAction();
    m_toggleMenu->addAction(toggleViewAction);

    QList<QAction*> actions = createTerminalToolBarActions(t, dw);
    for (QAction* a : actions) {
        toolbar->addAction(a);
    }

    t->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(t, &QWidget::customContextMenuRequested, t, [actions, t](const QPoint& p) {
        QMenu menu;
        for (QAction* ac : actions) {
            ac->setText(ac->toolTip());
            menu.addAction(ac);
        }
        menu.exec(t->mapToGlobal(p));
        for (QAction* ac : actions) ac->setText("");
    });

    return dw;
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
QAction *MainWindow::createAction(QString ico, QString tooltip,int rotation)
{
    QVariantMap options;
    options.insert("color", QColor(Qt::lightGray));
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
    wp->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    connect(wp,   &QWebPage::consoleMessageReceived, this,&MainWindow::consoleMessageReceived);

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

void MainWindow::consoleMessageReceived(QWebPage::MessageSource source, QWebPage::MessageLevel level, const QString & message, int lineNumber, const QString & sourceID)
{
  qDebug() << "consoleMessageReceived";
  qDebug() << message;
  qDebug() << sourceID;
} 



QString MainWindow::renderTemplate(const QStringList& items, const QString& user)
{
}

void MainWindow::editfile(parameterManager *p)
{
       QString strScript="";
       strScript = readFile(joinDir(appdir,"/monaco/index.tpl"));

       // https://stackoverflow.com/questions/5090969/read-a-text-file-to-qstringlist
       //QFile TextFile(p->nameFileToEdit);
       QFile TextFile(p->nameFileToEdit.trimmed());

       if (!TextFile.open(QIODevice::ReadWrite))
        {
           qDebug() << "Unable to create empty file " << p->nameFileToEdit ;
           return;
       }

       //Open file for reading
       
       QStringList SL;

       while(!TextFile.atEnd()) {
            SL.append(QString(TextFile.readLine()).remove("\n").replace("'","\\'"));
       }
       QString str =  SL.join("',\n'"); 
       QVariantMap data;
       data["items"] = SL;
       data["language"] = MonacoLanguage::fromExtension(p->nameFileToEdit);
;
       strScript = TemplateEngineQt::renderFile(joinDir(appdir,"/monaco/index.tpl"), data);
      
       saveFile(joinDir(appdir,"/monaco/index.html"),strScript);
       /*
       QFile filew;
       filew.setFileName(joinDir(appdir,"/monaco/index.html"));
       QTextStream datastream(&filew);

       if(filew.open(QIODevice::WriteOnly)){
              datastream << strScript;
              filew.close();
        } else {
                qDebug() << " Can write file ";
        }
      */
      
      urlActived(QUrl("file:///"+joinDir(appdir,"/monaco/index.html")));
}
    


/*********************************************
  * Command recieve form QSingletonApplication
***********************************************/

void MainWindow::handleShellMode(parameterManager& p)
{
    if (!p.nameDock.isEmpty()) {
        auto dMap = this->m_DockManager->dockWidgetsMap();
        for (const auto& key : dMap.keys()) {
            if (key == p.nameDock) {
                auto dw = createNewTerminal();
                if (p.createintab) {
                    m_DockManager->addDockWidgetTab(static_cast<ads::DockWidgetArea>(p.location), dw);
                } else {
                    m_DockManager->addDockWidget(static_cast<ads::DockWidgetArea>(p.location), dw, dMap[key]->dockAreaWidget());
                }
                return;
            }
        }
    }

    // Default: create normally
    auto dw = createNewTerminal();
    m_DockManager->addDockWidget(static_cast<ads::DockWidgetArea>(p.location), dw);
}



void MainWindow::commandRecieved(QString cwd, QStringList cmd)
{
    qDebug() << cmd;

    std::unique_ptr<parameterManager> p = std::make_unique<parameterManager>(cmd);
    QFileInfo fi(p->nameFileToEdit);

    switch (p->mode)
    {
    case parameterManager::Mode::Shell:
        handleShellMode(*p);
        break;

    case parameterManager::Mode::Man:
        urlActived(QUrl("https://man7.org/linux/man-pages/man1/" + p->nameParam + ".1.html"));
        break;

    case parameterManager::Mode::View:
        if ( fi.isDir()) {
            QDir dir(p->nameFileToEdit);
            QStringList filter;
            QVariantMap data;
            filter <<"*.jpg" << "*.png" << "*.jpeg";
            QFileInfoList files = dir.entryInfoList(filter);
            QStringList listFiles ;
            for ( auto f : files)
                 listFiles.append(QUrl::fromLocalFile(f.absoluteFilePath()).toString().replace("é","&eacute;"));
            data["files"] = listFiles;
            QString strScript = TemplateEngineQt::renderFile(joinDir(appdir,"/monaco/explorer.tpl"), data);
            saveFile(joinDir(appdir,"/monaco/explorer.html"),strScript);
            urlActived(QUrl::fromLocalFile(joinDir(appdir,"/monaco/explorer.html")));
            return;
        }

        if (MonacoLanguage::isMarkdown(p->nameFileToEdit) ) 
        {
          QVariantMap data;
          data["content"] = readFile(fi.absoluteFilePath());
          QString strScript = TemplateEngineQt::renderFile(joinDir(appdir,"/monaco/markdown.tpl"), data);
          saveFile(joinDir(appdir,"/monaco/markdown.html"),strScript);
          urlActived(QUrl::fromLocalFile(joinDir(appdir,"/monaco/markdown.html")));
        
        } else {
           if (MonacoLanguage::isVideo(p->nameFileToEdit) ) 
          {
          QVariantMap data;
          data["file"] = QUrl::fromLocalFile(fi.absoluteFilePath());
          QString strScript = TemplateEngineQt::renderFile(joinDir(appdir,"/monaco/video.tpl"), data);
          saveFile(joinDir(appdir,"/monaco/video.html"),strScript);
          urlActived(QUrl::fromLocalFile(joinDir(appdir,"/monaco/video.html")));
           } 
            else 
           {
              urlActived(QUrl::fromLocalFile(fi.absoluteFilePath()));
           }
        }
        break;

    case parameterManager::Mode::Edit:
        editfile(p.get());
        break;

    default:
        break;
    }
}

QString MainWindow::joinDir(QString dir,QString subdir)
{
   return QDir::cleanPath(dir + subdir);
} 
   
void MainWindow::saveFile(QString filename,QString& content)
{
       QFile filew;
       filew.setFileName(filename);
       QTextStream datastream(&filew);

       if(filew.open(QIODevice::WriteOnly)){
              datastream << content;
              filew.close();
        } else {
                qDebug() << " Can write file ";
        }
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
       } else {
                qDebug() << " can't read file  " << filename;
       }
       return result;
}

QList<QAction*> MainWindow::createTerminalToolBarActions(TerminalWidget* t, ads::CDockWidget* dw)
{
    QList<QAction*> actions;

    QAction* zoomInAction     = createAction("fa_solid expand", "Font size increase");
    QAction* zoomOutAction    = createAction("fa_solid compress", "Font size decrease");
    QAction* fontAction       = createAction("fa_solid font", "Choose Font");
    QAction* searchAction     = createAction("fa_solid search", "Search in console");
    QAction* clearAction      = createAction("fa_solid bath", "Clear");
    QAction* newTermAction    = createAction("fa_solid clone", "New Term in tab");
    QAction* newTermDown      = createAction("fa_solid columns", "New Term Down", 90);
    QAction* newTermTab       = createAction("fa_solid plus", "New Tab on the right");
    QAction* insertFolder     = createAction("fa_solid folder", "Choose a folder and paste");
    QAction* insertFile       = createAction("fa_solid file", "Choose files and paste");
    QAction* hideAction       = createAction("fa_solid eyeslash", "Hide Console");
    QAction* colorStyle       = createAction("fa_solid blacktie", "Choose Color Style");

    // connections
    connect(zoomInAction, &QAction::triggered, t, &TerminalWidget::zoomIn);
    connect(zoomOutAction, &QAction::triggered, t, &TerminalWidget::zoomOut);
    connect(searchAction, &QAction::triggered, t, &TerminalWidget::toggleShowSearchBar);
    connect(clearAction, &QAction::triggered, t, &TerminalWidget::clear);
    connect(fontAction, &QAction::triggered, this, &MainWindow::chooseFont);
    connect(colorStyle, &QAction::triggered, this, &MainWindow::chooseColorStyle);
    connect(hideAction, &QAction::triggered, this, [dw]() {
        dw->toggleViewAction()->trigger();
    });

    connect(insertFile, &QAction::triggered, this, &MainWindow::chooseFilesAndPaste);
    connect(insertFolder, &QAction::triggered, this, &MainWindow::chooseDirectoryAndPaste);

    connect(newTermTab, &QAction::triggered, this, [this]() {
        auto docknew = createNewTerminal();
        auto tn = qobject_cast<TerminalWidget*>(docknew->widget());
        if (activeTerm) tn->changeDir(activeTerm->workingDirectory());
        m_DockManager->addDockWidgetTab(ads::RightDockWidgetArea, docknew);
    });

    connect(newTermAction, &QAction::triggered, this, [this]() {
        auto docknew = createNewTerminal();
        auto tn = qobject_cast<TerminalWidget*>(docknew->widget());
        if (activeTerm) tn->changeDir(activeTerm->workingDirectory());
        m_DockManager->addDockWidget(ads::RightDockWidgetArea, docknew);
    });

    connect(newTermDown, &QAction::triggered, this, [this]() {
        auto docknew = createNewTerminal();
        auto tn = qobject_cast<TerminalWidget*>(docknew->widget());
        if (activeTerm) tn->changeDir(activeTerm->workingDirectory());
        m_DockManager->addDockWidget(ads::BottomDockWidgetArea, docknew);
    });

    // Regroup all
    actions << zoomInAction << zoomOutAction << fontAction << colorStyle << clearAction
            << searchAction << insertFile << insertFolder << newTermAction << newTermDown << newTermTab << hideAction;

    return actions;
}

QTreeView* MainWindow::createConfiguredTreeView()
{
    auto view = new QTreeView();
    view->setFrameShape(QFrame::NoFrame);
    view->setSelectionMode(QAbstractItemView::ExtendedSelection);
    view->setDragEnabled(true);
    view->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(view, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onDirContextMenu(const QPoint &)));
    return view;
}

QFileSystemModel* MainWindow::createAndSetFileSystemModel(QTreeView* view)
{
    auto model = new QFileSystemModel(view);
    model->setRootPath(QDir::currentPath());
    view->setModel(model);
    for (int i = 1; i < model->columnCount(); ++i)
        view->hideColumn(i);
    view->expandAll();
    return model;
}

void MainWindow::configureFileSystemToolbar(ads::CDockWidget* dock, QTreeView* view)
{
    auto toolbar = dock->createDefaultToolBar();
    QAction* toggleAction = dock->toggleViewAction();
    m_toggleMenu->addAction(toggleAction);

    auto hideAction = createAction("fa_solid eyeslash", "Hide");
    auto colorToggle = createAction("fa_solid blacktie", "Change Dark/White");
    colorToggle->setCheckable(true);

    toolbar->addAction(hideAction);
    toolbar->addAction(colorToggle);

    connect(hideAction, &QAction::triggered, this, [toggleAction]() {
        toggleAction->trigger();
    });


    connect(colorToggle, &QAction::triggered, this, [view, colorToggle]() {
      if (colorToggle->isChecked()) {
        view->setStyleSheet("background: #050505; color: white");
      } else {
        view->setStyleSheet("background: white; color: black");
      }
    });

    toolbar->setStyleSheet("background-color:black; color:lightgrey");
}

