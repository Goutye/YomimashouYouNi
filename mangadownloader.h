#ifndef MANGADOWNLOADER_H
#define MANGADOWNLOADER_H

#include <QList>
#include <QObject>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QDomDocument>
#include <QProgressBar>

class MangaDownloader : public QObject
{
    Q_OBJECT
public:
    explicit MangaDownloader(QObject *parent = 0);
    void download(QString name);
    QPair<QProgressBar*, QProgressBar*> progressBar();
    void setChapInterval(int from, int to);
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
    int endChapter;
    int startChapter;
    int currentChapter = 1;
    int nbPage = 0;
    int currentPage = 1;
    bool isDownloadImg = false;
    QProgressBar *scanBar;
    QProgressBar *pageBar;

    int currentWebsite = 0;
    QList<QString> websites;
};

#endif // MANGADOWNLOADER_H
