#include "mainwindow.h"
#include <QWidget>
#include <QGridLayout>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QMap>
#include <QString>
#include <QDebug>
#include <QVector>
#include <QFile>
#include <QTextStream>
#include <QString>
//#include <QText>
struct PPC{
    int id=0;
    QString name;
    int sline=0;
    int eline=1;
    bool unique=true;
    bool same=true;
    bool filter;
    QString content;

};

QVector<PPC> commands1, commands2;


static QWidget* createScrollableButtonList(const QString& labelPrefix) {
    QWidget *listWidget = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(listWidget);

    for (int i = 0; i < commands1.length(); ++i) {
        QPushButton *pb2= new QPushButton(commands1[i].name);

        // if(i==2){
        QPalette palette = pb2->palette();
        if(commands1[i].unique)
            palette.setColor(QPalette::Button, QColor(Qt::cyan));
        else if(commands1[i].same){
            palette.setColor(QPalette::Button, QColor(Qt::gray));
        }
        else {
            palette.setColor(QPalette::Button, QColor(Qt::yellow));
        }


        // if(commands1[i].filter)
        //          palette.setColor(QPalette::Button, QColor(Qt::red));

        pb2->setPalette(palette);
        pb2->setAutoFillBackground(true);
        pb2->repaint();

        layout->addWidget(pb2);
    }

    layout->addStretch();
    return listWidget;
}

static QWidget* createScrollableButtonList2(const QString& labelPrefix) {
    QWidget *listWidget = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(listWidget);

        for (int i = 0; i < commands2.length(); ++i) {
            QPushButton *pb2= new QPushButton(commands2[i].name);
             QPalette palette = pb2->palette();
            if(commands2[i].unique)
                 palette.setColor(QPalette::Button, QColor(Qt::green));
            else if(commands2[i].same){
                 palette.setColor(QPalette::Button, QColor(Qt::gray));
            }
            else {
               palette.setColor(QPalette::Button, QColor(Qt::yellow));
            }

             if(commands2[i].filter)
                      palette.setColor(QPalette::Button, QColor(Qt::red));


         pb2->setPalette(palette);
            pb2->setAutoFillBackground(true);
            pb2->repaint();

            layout->addWidget(pb2);
        }

    layout->addStretch();
    return listWidget;
}

void MainWindow::createMenus()
{
    // Create File menu
    fileMenu = menuBar()->addMenu(tr("&File"));

    QAction *openLeftAct = fileMenu->addAction(tr("Open Left..."), this, &MainWindow::openLeftFile);
    openLeftAct->setShortcut(QKeySequence::Open);

    QAction *openRightAct = fileMenu->addAction(tr("Open Right..."), this, &MainWindow::openRightFile);
    openRightAct->setShortcut(tr("Ctrl+Shift+O"));
    /*
    fileMenu->addSeparator();

    QAction *saveLeftAct = fileMenu->addAction(tr("Save Left..."), this, &MainWindow::saveLeftFile);
    saveLeftAct->setShortcut(QKeySequence::Save);

    QAction *saveRightAct = fileMenu->addAction(tr("Save Right..."), this, &MainWindow::saveRightFile);
    saveRightAct->setShortcut(tr("Ctrl+Shift+S"));

    fileMenu->addSeparator();
    */
    QAction *quitAct = fileMenu->addAction(tr("&Quit"), this, &MainWindow::quit);
    quitAct->setShortcut(QKeySequence::Quit);

    helpMenu = menuBar()->addMenu(tr("&Filter"));

 helpMenu = menuBar()->addMenu(tr("&Let's Merge!!!"));
    // Create Help menu
    helpMenu = menuBar()->addMenu(tr("&Help"));
    QAction *aboutAct = helpMenu->addAction(tr("&About"), this, &MainWindow::about);
}



void MainWindow::loadFile(const QString &filePath, bool isRight)
{

}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("PPC Merger");
    resize(1200, 800);
     createMenus();

    //load of recomps
    QString fileName = "C:/MT2/qt_project/m/recompiler.cpp";
      QString fileName2 = "C:/MT2/qt_project/o/recompiler.cpp";
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // Handle error: file could not be opened
        return;
    }
    QTextStream in(&file);
    QString searchString = "case PPC_INST_";
     QString searchString2 = "  switch (id)";
    QString filterString = "\t{}.u64 = {}.u64 & ~{}.u64;";
    int lineNumber = 0;
    PPC *testppc = new PPC;
    bool content=false;
    bool foundedsw=false;

    while (!in.atEnd()) {
        QString line = in.readLine();
        lineNumber++;
        if(foundedsw){


        if (line.contains(searchString, Qt::CaseInsensitive)) { // Case-insensitive search
            qDebug() << "String found on line:" << lineNumber << " - " << line;
            // Additional actions can be performed here, e.g., storing line numbers
            int underscoreIndex = line.indexOf('_');
         // line.remove(0, 6);
            QString resultString = line.mid(underscoreIndex + 1);
            resultString.remove(0, 5);
            resultString.chop(1);


            testppc->name=resultString;
            testppc->sline=lineNumber;
            content=true;
        }

        else if (line.contains("break;", Qt::CaseInsensitive)) { // Case-insensitive search
            qDebug() << "break found on line:" << lineNumber << " - " << line;
            // Additional actions can be performed here, e.g., storing line numbers

            testppc->eline=lineNumber;

            //filter
           // if(testppc->content.contains(filterString))
           //     testppc->filter=true;

            commands1.append(*testppc);
           testppc->same=true;
            testppc->unique=true;
           testppc->filter=false;
            content=false;
        }
        else{
            if(content){
                testppc->content.append(line);
            }


        }

    }

        else{

            if (line.contains(searchString2, Qt::CaseInsensitive)) foundedsw=true;
        }
    }

    file.close();

foundedsw=false;
    //load recomp 2
    QFile file2(fileName2);

    if (!file2.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // Handle error: file could not be opened
        return;
    }
    QTextStream in2(&file2);

     lineNumber = 0;

    while (!in2.atEnd()) {
        QString line = in2.readLine();
        lineNumber++;


  if(foundedsw){
        if (line.contains(searchString, Qt::CaseInsensitive)) { // Case-insensitive search
            qDebug() << "String found on line:" << lineNumber << " - " << line;
            // Additional actions can be performed here, e.g., storing line numbers
            int underscoreIndex = line.indexOf('_');
            QString resultString = line.mid(underscoreIndex + 1);
            resultString.remove(0, 5);
            resultString.chop(1);


            testppc->name=resultString;
            testppc->sline=lineNumber;
            content=true;
        }



        else if (line.contains("break;", Qt::CaseInsensitive)) { // Case-insensitive search
            qDebug() << "break found on line:" << lineNumber << " - " << line;
            // Additional actions can be performed here, e.g., storing line numbers

            testppc->eline=lineNumber;

            //filter
            // if(testppc->content.contains(filterString))
            //     testppc->filter=true;



         //  foreach(auto ppc2, commands1){
           for(int j=0;j<commands1.length();j++){
          auto &ppc2 =commands1[j];
               if(ppc2.name==testppc->name){
                   ppc2.unique=false;
                   testppc->unique=false;
                   if( ppc2.content!=testppc->content){
                        ppc2.same=false;
                       testppc->same=false;
                   }
               }

        }
            commands2.append(*testppc);
            content=false;
            testppc->same=true;
            testppc->unique=true;
              testppc->filter=false;

        }
        else{
            if(content){
                testppc->content.append(line);
            }


        }

  }

  else{

      if (line.contains(searchString2, Qt::CaseInsensitive)) foundedsw=true;
  }

    }

    file2.close();

    qDebug() << "PPC from 1:" <<  commands1.length();
     qDebug() << "PPC from 2:" <<  commands2.length();

    QString wfileName = "D:/MT2/qt_project/r1/out.cpp"; // Or provide a full path
    QFile wfile(wfileName);
    if (!wfile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        // Handle error: file could not be opened
        qDebug() << "Could not open file for writing.";
        return;
    }
    QTextStream out(&wfile);

    out << commands1[0].content;

        wfile.close();

    QWidget *central = new QWidget(this);
QGridLayout *grid = new QGridLayout(central);
    // Set proportional resizing
    grid->setColumnStretch(0, 4);
    grid->setColumnStretch(1, 5);
    grid->setRowStretch(0, 70);
    grid->setRowStretch(1, 30);

    // Top-left
    QScrollArea *scrollAreaTL = new QScrollArea;

    scrollAreaTL->setWidgetResizable(true);
     scrollAreaTL->setWidget(createScrollableButtonList("Top Left Button"));
  grid->addWidget(scrollAreaTL, 0, 0);

    // Top-right
QScrollArea *scrollAreaTR = new QScrollArea;
    scrollAreaTR->setWidgetResizable(true);
    scrollAreaTR->setWidget(createScrollableButtonList2("Top Right Button"));
    grid->addWidget(scrollAreaTR, 0, 1);


   QTextEdit  *leftTextEdit1 = new QTextEdit(this);
    leftTextEdit1->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    grid->addWidget(leftTextEdit1, 1, 0);


 QTextEdit  *rightTextEdit1 = new QTextEdit(this);
    rightTextEdit1->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);



    grid->addWidget(rightTextEdit1, 1, 1);

    setCentralWidget(central);
}

void MainWindow::openLeftFile()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Open Left File");
    if (!filePath.isEmpty()) {
        leftFilePath = filePath;
        loadFile(filePath, true);
    }
}

void MainWindow::openRightFile()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Open Right File");
    if (!filePath.isEmpty()) {
        rightFilePath = filePath;
        loadFile(filePath, false);
    }
}

void MainWindow::about()
{
    QMessageBox::about(this, "About PPC Merger",
                       "PPC Merger v1.0\n\n"
                       "A simple tool to compare and search through two files side by side.\n\n"
                       "Features:\n"
                       "- Open and compare two files\n"
                       "- Search functionality\n"
                       "- Save files\n"
                       "- Results listing");
}
/*
void MainWindow::searchText()
{
    updateSearchResults();
}
*/

void MainWindow::quit()
{
    QApplication::quit();
}

