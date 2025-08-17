#include "mainwindow.h"
#include <QGridLayout>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMap>
#include <QString>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QCoreApplication>
#include <QSettings>

//#include <QText>


QVector<PPC> commands0, commands2;

struct AppConfig {
    QString path1;
    QString path2;
    QString path3;
    QString text;
    int n1 = 0;
    int n2 = 0;
};

inline AppConfig loadConfig(const QString& iniFile) {
    QSettings s(iniFile, QSettings::IniFormat);
    AppConfig cfg;

    s.beginGroup("Paths");
    cfg.path1 = s.value("path1").toString();
    cfg.path2 = s.value("path2").toString();
    cfg.path3 = s.value("path3").toString();
    s.endGroup();

    s.beginGroup("Params");
    cfg.text = s.value("filter").toString();
    cfg.n1 = s.value("p1").toInt();
    cfg.n2 = s.value("p2").toInt();
    s.endGroup();

    return cfg;
}


static QWidget* createScrollableButtonList(const QString& labelPrefix, QVector<IdButton*> &buttons2,
                                           QTextEdit *textEdit, QVector<PPC> &commands1, bool second) {
    QWidget *listWidget = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(listWidget);

    for (int i = 0; i < commands1.length(); ++i) {

      auto* pb2= new IdButton( commands1[i].content,commands1[i].name, *textEdit);//,  m_content2, textEdit);
        buttons2.append(pb2);
        // if(i==2){
        QPalette palette = pb2->palette();
        if(commands1[i].unique){
            if(second) palette.setColor(QPalette::Button, QColor(Qt::green)); else
            palette.setColor(QPalette::Button, QColor(Qt::cyan));
        }
        else if(commands1[i].same){
            palette.setColor(QPalette::Button, QColor(Qt::gray));
        }
        else {
            palette.setColor(QPalette::Button, QColor(Qt::yellow));
        }

       if(commands1[i].filter)
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

    QAction *mergeAct = fileMenu->addAction(tr("&Let's Merge!!!"), this, &MainWindow::mergeFile);
    mergeAct->setShortcut(tr("Ctrl+M"));
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

// helpMenu = menuBar()->addMenu(tr("&Let's Merge!!!"));
    // Create Help menu
    helpMenu = menuBar()->addMenu(tr("&Help"));
    QAction *aboutAct = helpMenu->addAction(tr("&About"), this, &MainWindow::about);
}

void MainWindow::mergeFile()
{

    QString wfileName = "D:/MT2/qt_project/recompiler.cpp"; // Or provide a full path
    QFile wfile(wfileName);
    if (!wfile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        // Handle error: file could not be opened
        qDebug() << "Could not open file for writing.";
        return;
    }
    QTextStream out(&wfile);
//     for (int i=0; i<commands2.length(); i++)
//     {
//     out << commands2[i].content;
//     out <<Qt::endl;
//      out <<Qt::endl;
// }
//     for (int i=0; i<commands0.length(); i++)
//          if(commands0[i].unique){
//         out << commands0[i].content;
//     out <<Qt::endl;
//     out <<Qt::endl;
//          }

    for (int i=0; i<commands0.length(); i++)
    {
        out << commands0[i].content;
        out <<Qt::endl;
        out <<Qt::endl;
    }
    for (int i=0; i<commands2.length(); i++)
        if(commands2[i].unique&&!commands2[i].filter){
            out << commands2[i].content;
            out <<Qt::endl;
            out <<Qt::endl;
        }

    wfile.close();
}

void MainWindow::loadFile(const QString &filePath, bool isRight)
{

}

void MainWindow::loadCommands(const QString &fileName, QVector<PPC> &commands1, bool second){
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // Handle error: file could not be opened
        return;
    }
    QTextStream in(&file);
    QString searchString = "case PPC_INST_";
    QString searchString2 = "  switch (id)";
      QString filterString = "tsimd";

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
                testppc->content.append(line);
                testppc->content.append("\n");
            }

            else if (line.contains("break;", Qt::CaseInsensitive)) { // Case-insensitive search
                qDebug() << "break found on line:" << lineNumber << " - " << line;
                // Additional actions can be performed here, e.g., storing line numbers

                testppc->eline=lineNumber;

                if(second){
                //  foreach(auto ppc2, commands1){
                for(int j=0;j<commands0.length();j++){
                    auto &ppc2 =commands0[j];
                    if(ppc2.name==testppc->name){
                        ppc2.unique=false;
                        testppc->unique=false;
                        if( ppc2.content!=testppc->content){
                            ppc2.same=false;
                            testppc->same=false;
                        }
                    }
                }
                }

                testppc->content.append(line); //- break;
                    //  testppc->content.append("\n");
                commands1.append(*testppc);
                //set default
                content=false;
                testppc->same=true;
                testppc->unique=true;
                testppc->filter=false;
                testppc->content="";
            }
            else if (line.contains("return false;", Qt::CaseInsensitive)) {
                break;
            }
            else{
                if(content){
                    testppc->content.append(line);
                    testppc->content.append("\n");

                    if(testppc->content.contains(filterString))//(QStringLiteral("\t"),Qt::CaseInsensitive))
                        testppc->filter=true;

                }
            }

        }

        else{

            if (line.contains(searchString2, Qt::CaseInsensitive)) foundedsw=true;
        }
    }

    file.close();

}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("PPC Merger");
    resize(1200, 800);
     createMenus();

     const auto cfg = loadConfig(QCoreApplication::applicationDirPath() + "/default.ini");

    //load of recomps
    QString fileName = "D:/MT2/qt_project/c/recompiler.cpp";
      QString fileName2 = "D:/MT2/qt_project/o/recompiler.cpp";

    loadCommands(fileName,commands0,0);
      loadCommands(fileName2,commands2,1);

    qDebug() << "PPC from 1:" <<  commands0.length();
    qDebug() << "PPC from 2:" <<  commands2.length();

    QWidget *central = new QWidget(this);
QGridLayout *grid = new QGridLayout(central);
    // Set proportional resizing
    grid->setColumnStretch(0, 4);
    grid->setColumnStretch(1, 5);
    grid->setRowStretch(0, 70);
    grid->setRowStretch(1, 30);
  leftTextEdit1 = new QTextEdit(this);
    rightTextEdit1 = new QTextEdit(this);
    // Top-left
    QScrollArea *scrollAreaTL = new QScrollArea;
    scrollAreaTL->setWidgetResizable(true);
     scrollAreaTL->setWidget(createScrollableButtonList("Top Left Button", buttons, leftTextEdit1, commands0,0));
  grid->addWidget(scrollAreaTL, 0, 0);
    // Top-right
QScrollArea *scrollAreaTR = new QScrollArea;
    scrollAreaTR->setWidgetResizable(true);
    scrollAreaTR->setWidget(createScrollableButtonList("Top Right Button", buttons, rightTextEdit1, commands2,1));
    grid->addWidget(scrollAreaTR, 0, 1);
    leftTextEdit1->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    grid->addWidget(leftTextEdit1, 1, 0);
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

