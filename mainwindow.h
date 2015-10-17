#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QProgressBar>
#include "mangadownloader.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void launchDownload();

private:
    MangaDownloader *md;
    QLineEdit *input;
    QLineEdit *fromChap;
    QLineEdit *toChap;
    QProgressBar *scan;
    QProgressBar *page;
};

#endif // MAINWINDOW_H
