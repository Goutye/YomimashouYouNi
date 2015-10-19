#include <QDomNode>
#include <QDir>
#include <QCoreApplication>
#include "mangadownloader.h"
#include <QUrl>
#include <QByteArray>
#include <QFile>
#include <QDebug>
#include <sstream>
#include <iomanip>
#include <QNetworkProxy>

MangaDownloader::MangaDownloader(QObject *parent) :
    QObject(parent)
{
    /*QNetworkProxy proxy;
    proxy.setType(QNetworkProxy::HttpProxy);
    proxy.setHostName("193.49.200.22");
    proxy.setPort(3128);
    proxy.setUser("");
    proxy.setPassword("");
    manager.setProxy(proxy);*/

    connect(&manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyReceived(QNetworkReply*)));
    doc = new QDomDocument;
    scanBar = new QProgressBar;
    scanBar->setFormat("%v/%m");
    pageBar = new QProgressBar;
    pageBar->setFormat("%v/%m");

    websites.append("http://www.mangareader.net");
    websites.append("http://www.mangapanda.com");
}

void MangaDownloader::setChapInterval(int from, int to)
{
    startChapter = from > 0 ? from : 1;
    endChapter = to;
}

void MangaDownloader::download(QString name)
{
    this->name = name.replace(' ', "-");

    QNetworkRequest request(websites.at(currentWebsite) + "/" + this->name);
    //request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    manager.get(request);
}

QPair<QProgressBar*, QProgressBar*> MangaDownloader::progressBar()
{
    return qMakePair(scanBar, pageBar);
}

void MangaDownloader::replyReceived(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << reply->errorString();

        if (reply->error() == QNetworkReply::ContentNotFoundError) {
            if (nbChapter == 0) {
                ++currentWebsite;
                if (currentWebsite < websites.count())
                    download(name);
                else {
                    qDebug() << "Manga not found.";
                    return;
                }
            } else {
                ++currentWebsite;
                if (currentWebsite < websites.count()) {
                    QNetworkRequest request(websites.at(currentWebsite) + "/" + name + QString("/%1/%2").arg(currentChapter).arg(currentPage));
                    manager.get(request);
                    qDebug() << QString(name + "/%1/%2").arg(currentChapter).arg(currentPage) << websites.at(currentWebsite);
                }
                else {
                    currentWebsite = 0;
                    qDebug() << "Echec download of " << currentChapter << "/" << currentPage;
                    nextPage();
                }
                return;
            }
        }
    }

    QByteArray data = reply->readAll();

    if (isDownloadImg) {
        std::stringstream dirName, fileName;
        dirName << std::setfill('0') << std::setw(3) << currentChapter;
        fileName << std::setfill('0') << std::setw(3) << currentPage;
        QString dirStr = dirName.str().c_str(), fileStr = fileName.str().c_str();
        QFile file(QCoreApplication::applicationDirPath() + "/scans/" + name + "/" + name + "-" + dirStr + "/" + fileStr);
        file.open(QFile::WriteOnly);
        file.write(data);
        file.close();

        isDownloadImg = false;
        currentWebsite = 0;
        nextPage();
        return;
    }

    if (!doc->setContent(data)) {
        qDebug() << "Error on loading dom tree";
        return;
    }

    if (nbChapter == 0) {
        loadChapters();
    } else {
        if (nbPage == 0) {
            loadChapter();
        }

        isDownloadImg = true;
        getImage();
    }
}

void MangaDownloader::loadChapters()
{
    QDomElement elem = elementById("listing", "table").toElement();
    QDomNodeList list = elem.elementsByTagName("tr");

    nbChapter = list.count() - 1;

    if (endChapter == 0)
        nbChapter = nbChapter;
    else if (endChapter < nbChapter)
        nbChapter = endChapter;

    qDebug() << list.count() - 1 << " chapters found.";
    currentChapter = startChapter;
    if (currentChapter > nbChapter) {
        qDebug() << currentChapter << "/" << nbChapter;
        return;
    }

    QNetworkRequest request(websites.at(currentWebsite) + "/" + name + QString("/%1").arg(currentChapter));
    manager.get(request);

    QString path = QCoreApplication::applicationDirPath() + "/scans";
    if (!QDir(path).exists())
        QDir(QCoreApplication::applicationDirPath()).mkdir(path);
    if (!QDir(path + "/" + name).exists())
        QDir(path).mkdir(path + "/" + name);

    scanBar->setMaximum(nbChapter);
    scanBar->setValue(currentChapter);
}

void MangaDownloader::loadChapter()
{
    QDomNode node = elementById("pageMenu", "select");
    QDomElement elem = node.parentNode().firstChildElement("select");
    QDomNodeList list = elem.elementsByTagName("option");

    nbPage = list.count();

    QString path = QCoreApplication::applicationDirPath() + "/scans/" + name + "/" + name + "-";
    std::stringstream dirName;
    dirName << std::setfill('0') << std::setw(3) << currentChapter;
    QDir(path).mkdir(path + dirName.str().c_str());

    scanBar->setValue(currentChapter);
    pageBar->setValue(1);
    pageBar->setMaximum(nbPage);
}

void MangaDownloader::getImage() {
    QDomNode node = elementById("img", "img");
    QDomElement elem = node.parentNode().firstChildElement("img");
    QString url = elem.attribute("src");

    QNetworkRequest request(url);
    manager.get(request);
}

void MangaDownloader::nextPage()
{
    ++currentPage;
    if (currentPage > nbPage) {
        nbPage = 0;
        currentPage = 1;
        ++currentChapter;

        if (currentChapter > nbChapter) {
            qDebug() << "End of downloading";
            return;
        }
    }
    QNetworkRequest request(websites.at(currentWebsite) + "/" + name + QString("/%1/%2").arg(currentChapter).arg(currentPage));
    manager.get(request);
    qDebug() << QString(name + "/%1/%2").arg(currentChapter).arg(currentPage);

    pageBar->setValue(currentPage);
}

void MangaDownloader::nextChapter()
{
    ++currentChapter;
    QNetworkRequest request(websites.at(currentWebsite) + "/" + name + QString("/%1").arg(currentChapter));
    manager.get(request);
}

QDomNode MangaDownloader::elementById(QString id, QString tagName)
{
    QDomNodeList list = doc->elementsByTagName(tagName);
    for (int i = 0; i < list.count(); ++i) {
        if (list.at(i).attributes().namedItem("id").nodeValue() == id) {
            return list.at(i);
        }
    }

    qDebug() << "No id " + id + " found.";
    return QDomNode();
}
