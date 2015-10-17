#include <QPair>
#include <QVBoxLayout>
#include <QLabel>
#include <QHBoxLayout>
#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    md = new MangaDownloader;

    QPair<QProgressBar*, QProgressBar*> bars = md->progressBar();
    scan = bars.first;
    page = bars.second;

    QWidget *widget = new QWidget;
    QWidget *widgetChap = new QWidget;
    QWidget *cWidget = new QWidget;
    QHBoxLayout *layoutH = new QHBoxLayout;
    QHBoxLayout *layoutChap = new QHBoxLayout;
    QVBoxLayout *layoutV = new QVBoxLayout;
    widget->setLayout(layoutH);
    cWidget->setLayout(layoutV);
    widgetChap->setLayout(layoutChap);

    QLabel *label = new QLabel("Scan title:");
    input = new QLineEdit;
    layoutH->addWidget(label);
    layoutH->addWidget(input);

    fromChap = new QLineEdit;
    toChap = new QLineEdit;
    layoutChap->addWidget(new QLabel("From:"));
    layoutChap->addWidget(fromChap);
    layoutChap->addWidget(new QLabel("To:"));
    layoutChap->addWidget(toChap);

    layoutV->addWidget(widget);
    layoutV->addWidget(widgetChap);
    layoutV->addWidget(scan);
    layoutV->addWidget(page);
    setCentralWidget(cWidget);

    connect(input, SIGNAL(returnPressed()), this, SLOT(launchDownload()));
    connect(fromChap, SIGNAL(returnPressed()), this, SLOT(launchDownload()));
    connect(toChap, SIGNAL(returnPressed()), this, SLOT(launchDownload()));
}

MainWindow::~MainWindow()
{

}

void MainWindow::launchDownload()
{
    md->setChapInterval(fromChap->text().toInt(), toChap->text().toInt());
    md->download(input->text());
}
