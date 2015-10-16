#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    md = new MangaDownloader;
    md->download("gordian knot");
}

MainWindow::~MainWindow()
{

}
