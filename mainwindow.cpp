#include <QPushButton>
#include <QPixmap>
#include <QFile>
#include <QPair>
#include <QVBoxLayout>
#include <QNetworkProxy>
#include <QLineEdit>
#include <QDialog>
#include <QCheckBox>
#include <QSpinBox>
#include <QFormLayout>
#include <QLabel>
#include <QDialogButtonBox>
#include <QMenuBar>
#include <QHBoxLayout>
#include <QMenu>
#include <QAction>
#include <QStyle>
#include <QTimer>
#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    md = new MangaDownloader;
    md->moveToThread(&thread);
    connect(&thread, &QThread::finished, md, &QObject::deleteLater);
    thread.start();
    md->load();

    QPair<QProgressBar*, QProgressBar*> bars = md->progressBar();
    scan = bars.first;
    page = bars.second;

    QWidget *widget = new QWidget;
    QWidget *widgetChap = new QWidget;
    QWidget *cWidget = new QWidget;
    QHBoxLayout *layoutH = new QHBoxLayout;
    QHBoxLayout *layoutChap = new QHBoxLayout;
    layoutChap->setContentsMargins(2,2,2,2);
    layoutChap->setSpacing(2);
    QVBoxLayout *layoutV = new QVBoxLayout;
    layoutH->setContentsMargins(2,2,2,2);
    widget->setLayout(layoutH);
    cWidget->setLayout(layoutV);
    widgetChap->setLayout(layoutChap);

    QLabel *label = new QLabel("Scan title:");
    input = new QLineEdit;
    layoutH->addWidget(label);
    layoutH->addWidget(input);

    fromChap = new QLineEdit;
    toChap = new QLineEdit;
    layoutChap->addWidget(new QLabel("From:"), 0, Qt::AlignCenter);
    layoutChap->addWidget(fromChap);
    layoutChap->addWidget(new QLabel("To:"), 0, Qt::AlignCenter);
    layoutChap->addWidget(toChap);

    info = new QLabel("");
    info->setAlignment(Qt::AlignCenter | Qt::AlignTop);

    QPushButton *b = new QPushButton("Go!");
    b->setStyleSheet("QPushButton {"
                        "padding:2px;"
                        "padding-left:10px;"
                        "padding-right:10px;"
                        "color:#EEEEEE;"
                        "background-color:#343434;"
                        "border-radius:5px;"
                     "}");

    layoutV->addWidget(widget, 1);
    layoutV->addWidget(widgetChap, 1);
    layoutV->addWidget(b, 1, Qt::AlignCenter);
    layoutV->addWidget(info, 12);
    layoutV->addWidget(scan, 1);
    layoutV->addWidget(page, 1);
    layoutV->setSpacing(5);
    layoutV->setContentsMargins(2,2,2,2);
    setCentralWidget(cWidget);

    connect(b, SIGNAL(pressed()), this, SLOT(launchDownload()));

    QMenu *menu = new QMenu("Options");
    menuBar()->addMenu(menu);
    menuBar()->setStyleSheet("QMenuBar {"
                                "background-color: #212121;"
                             "}"
                             "QMenuBar::item {"
                                "border-radius: 5px;"
                                "background: transparent;"
                             "}"
                             "QMenuBar::item:selected, QMenuBar::item:pressed {"
                                "background-color: #343434;"
                             "}");

    QAction *actionProxy = new QAction("Set proxy", menu);
    menu->addAction(actionProxy);

    connect(actionProxy, SIGNAL(triggered()), this, SLOT(setProxy()));
    connect(md, SIGNAL(sendInfo(QString)), this, SLOT(insertInfo(QString)));
    connect(md, SIGNAL(sendBackgroundPath(QString)), this, SLOT(setBackground(QString)));

    setStyleSheet("QWidget {"
                    "color: white;"
                    "background-color: #212121;"
                  "}"
                  "QProgressBar {"
                      "border: 2px solid grey;"
                      "border-radius: 5px;"
                      "background-color: #343434;"
                      "text-align: center;"
                  "}"
                  "QProgressBar[scan=true]::chunk {"
                      "background-color: #05B8CC;"
                  "}"
                  "QProgressBar::chunk {"
                      "background-color: #BE87FF;"
                  "}"
                  "QLabel {"
                  " color: #EEEEEE;"
                  "}"
                  "QLineEdit {"
                  " background-color:#343434;"
                  " border-radius: 5px;"
                  "}");
}

MainWindow::~MainWindow()
{
    thread.quit();
    thread.wait();
}

void MainWindow::launchDownload()
{
    md->setChapInterval(fromChap->text().toInt(), toChap->text().toInt());
    md->download(input->text());
    setFocus();
}

void MainWindow::insertInfo(QString s)
{
    info->setText(s);
}

void MainWindow::setProxy()
{
    QDialog dialog;
    QFormLayout form(&dialog);
    QLabel *dialogLabel = new QLabel("Proxy");
    form.addRow(dialogLabel);

    QLineEdit *lineEditHostname = new QLineEdit(&dialog);
    lineEditHostname->setText("193.49.200.22");
    form.addRow("Host name:", lineEditHostname);
    QSpinBox *spinBoxPort = new QSpinBox(&dialog);
    spinBoxPort->setMinimum(1);
    spinBoxPort->setMaximum(99999);
    spinBoxPort->setSingleStep(1);
    spinBoxPort->setValue(3128);
    form.addRow("Port:", spinBoxPort);
    QLineEdit *lineEditPseudo = new QLineEdit(&dialog);
    form.addRow("User:", lineEditPseudo);
    QLineEdit *lineEditPassword = new QLineEdit(&dialog);
    lineEditPassword->setEchoMode(QLineEdit::Password);
    form.addRow("Password:", lineEditPassword);

    QDialogButtonBox buttonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Ok, Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(close()));
    QString hostname, user, password;
    int port;

    do {
        if (dialog.exec() == QDialog::Accepted) {
            hostname = lineEditHostname->text();
            user = lineEditPseudo->text();
            password = lineEditPassword->text();
            port = spinBoxPort->value();
            if (hostname.isEmpty()) {
                dialogLabel->setText("Hostname empty.");
            }
        } else {
            return;
        }
    } while(hostname.isEmpty());

    QNetworkProxy proxy;
    proxy.setType(QNetworkProxy::HttpProxy);
    proxy.setHostName(hostname);
    proxy.setPort(port);
    proxy.setUser(user);
    proxy.setPassword(password);
    md->setProxy(proxy);
}

void MainWindow::setBackground(QString path)
{
    QFile f;
    f.setFileName(path);
    f.open(QFile::ReadOnly);
    QByteArray ba = f.readAll();
    QPixmap p;
    p.loadFromData(ba);
    p = p.scaled(info->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    info->setPixmap(p);

//    setStyleSheet(styleSheet() + ""
//                  "MainWindow {"
//                  " background-image: url('"+ path +"');"
//                  "}");
}
