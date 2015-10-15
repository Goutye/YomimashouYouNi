#include "mangadownloader.h"
#include <QUrl>
#include <QByteArray>
#include <QFile>

#define WEBSITE "http://www.mangareader.net"

MangaDownloader::MangaDownloader(QObject *parent) :
    QObject(parent)
{
    manager = new QNetworkAccessManager;
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyReceived(QNetworkReply*)));
}

void MangaDownloader::download(QString name)
{
    name.replace(' ', "-");
    QUrl url(WEBSITE + "/");

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    manager->get(request);
}

void MangaDownloader::replyReceived(QNetworkReply *reply)
{
    QByteArray data = *reply->readAll();

    QFile file(QCoreApplication::applicationDirPath() + "/file.html");
    file.open(QFile::WriteOnly);
    file.write(data);
    file.close();

}
