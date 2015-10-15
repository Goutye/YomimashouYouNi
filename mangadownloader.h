#ifndef MANGADOWNLOADER_H
#define MANGADOWNLOADER_H

#include <QObject>
#include <QNetworkReply>
#include <QNetworkAccessManager>

class MangaDownloader : public QObject
{
    Q_OBJECT
public:
    explicit MangaDownloader(QObject *parent = 0);
    void download(QString name);
signals:

public slots:

private slots:
    void replyReceived(QNetworkReply *reply);

private:
    QNetworkAccessManager *manager;
};

#endif // MANGADOWNLOADER_H
