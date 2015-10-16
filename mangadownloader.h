#ifndef MANGADOWNLOADER_H
#define MANGADOWNLOADER_H

#include <QObject>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QDomDocument>

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
    void loadChapters();
    void loadChapter();
    void getImage();
    void nextChapter();
    void nextPage();

    QDomNode elementById(QString id, QString tagName);

    QNetworkAccessManager manager;
    QDomDocument *doc;
    QString name;
    int nbChapter = 0;
    int currentChapter = 1;
    int nbPage = 0;
    int currentPage = 1;
    bool isDownloadImg = false;
};

#endif // MANGADOWNLOADER_H
