#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QProgressBar>
#include <QThread>
#include <QLabel>
#include <QSpinBox>
#include <QByteArray>
#include "mangadownloader.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void launchDownload();
    void insertInfo(QString s);
    void setBackground(QString path);

private:
    MangaDownloader *md;
    QLineEdit *input;
    QLineEdit *fromChap;
    QLineEdit *toChap;
    QProgressBar *scan;
    QProgressBar *page;
    QThread thread;
    QLabel *info;
};

#endif // MAINWINDOW_H
