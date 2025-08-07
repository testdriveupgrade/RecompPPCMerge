#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include <QMenuBar>
#include <QLineEdit>
#include <QListWidget>
#include <QSplitter>
#include <QMap>
#include <QTextEdit>
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    QTextEdit  *leftTextEdit1, *rightTextEdit1;

private slots:
    void openLeftFile();
    void openRightFile();
  //  void saveLeftFile();
 //   void saveRightFile();
    void about();
 //   void searchIds();
    void quit();
  //  void handleSearchResultClicked(QListWidgetItem* item);

private:
    void createMenus();
    void createLayout();
    void loadFile(const QString &filePath, bool isLeft);
//    void saveFile(const QString &filePath, bool isLeft, int quadrant = 0);
    void updateSearchResults();

    // Menus
    QMenu *fileMenu;
    QMenu *helpMenu;

    // File paths
    QString leftFilePath, leftFilePath2;
    QString rightFilePath, rightFilePath2;
};

#endif // MAINWINDOW_H 
