#include <QDomNode>
#include <QDir>
#include <QCoreApplication>
#include "mangadownloader.h"
#include <QUrl>
#include <QByteArray>
#include <QFile>
#include <QDebug>

#define WEBSITE "http://www.mangareader.net"

MangaDownloader::MangaDownloader(QObject *parent) :
    QObject(parent)
{
    connect(&manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyReceived(QNetworkReply*)));
    doc = new QDomDocument;
}

void MangaDownloader::download(QString name)
{
    this->name = name.replace(' ', "-");

    QNetworkRequest request(QString(WEBSITE) + "/" + this->name);
    //request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    manager.get(request);
}

void MangaDownloader::replyReceived(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << reply->errorString();
        return;
    }

    QByteArray data = reply->readAll();

    if (isDownloadImg) {
        QFile file(QCoreApplication::applicationDirPath() + "/scans/" + name + QString("/%1/%2.jpg").arg(currentChapter).arg(currentPage));
        file.open(QFile::WriteOnly);
        file.write(data);
        file.close();

        isDownloadImg = false;
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
    qDebug() << nbChapter << " chapters found.";
    currentChapter = 1;

    QNetworkRequest request(QString(WEBSITE) + "/" + name + QString("/%1").arg(currentChapter));
    manager.get(request);

    QString path = QCoreApplication::applicationDirPath() + "/scans";
    if (!QDir(path).exists())
        QDir(QCoreApplication::applicationDirPath()).mkdir(path);
    if (!QDir(path + "/" + name).exists())
        QDir(path).mkdir(path + "/" + name);
}

void MangaDownloader::loadChapter()
{
    QDomNode node = elementById("pageMenu", "select");
    QDomElement elem = node.parentNode().firstChildElement("select");
    QDomNodeList list = elem.elementsByTagName("option");

    nbPage = list.count();

    QString path = QCoreApplication::applicationDirPath() + "/scans/" + name;
    QDir(path).mkdir(path + QString("/%1").arg(currentChapter));
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
    QNetworkRequest request(QString(WEBSITE) + "/" + name + QString("/%1/%2").arg(currentChapter).arg(currentPage));
    manager.get(request);
    qDebug() << QString(name + "/%1/%2").arg(currentChapter).arg(currentPage);
}

void MangaDownloader::nextChapter()
{
    ++currentChapter;
    QNetworkRequest request(QString(WEBSITE) + "/" + name + QString("/%1").arg(currentChapter));
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
