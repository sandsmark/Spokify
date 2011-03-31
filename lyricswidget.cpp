
#include <QDebug>
#include <QDomDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QRegExp>


#include "lyricswidget.h"

LyricsWidget::LyricsWidget(QWidget* parent): QTextEdit(parent),
    m_networkAccessManager(new QNetworkAccessManager)
{
    setReadOnly(true);
}
LyricsWidget::~LyricsWidget()
{
    delete m_networkAccessManager;
}

void LyricsWidget::setTrack(const QString& artist, const QString& title)
{
    setHtml("<i>Loading...</i>");
    
    QUrl listUrl("http://lyrics.wikia.com/api.php");
    listUrl.addQueryItem("action", "lyrics");
    listUrl.addQueryItem("func", "getSong");
    listUrl.addQueryItem("fmt", "xml");
    listUrl.addQueryItem("artist", artist);
    listUrl.addQueryItem("song", title);
    connect(m_networkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(receiveListReply(QNetworkReply*)));
    m_networkAccessManager->get(QNetworkRequest(listUrl));
}

void LyricsWidget::receiveListReply(QNetworkReply* reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Error while fetching lyrics: " << reply->errorString();
        setHtml("<span style='color:red'>Error while retrieving lyrics!</span>");
        return;
    }

    QDomDocument document;
    document.setContent(reply);
    QString artist = document.elementsByTagName("artist").at(0).toElement().text();
    QString title = document.elementsByTagName("song").at(0).toElement().text();
    
    
    QUrl url("http://lyrics.wikia.com/api.php");
    url.addQueryItem("action", "query");
    url.addQueryItem("prop", "revisions");
    url.addQueryItem("rvprop", "content");
    url.addQueryItem("format", "xml");
    url.addQueryItem("titles", artist + ":" + title);
    disconnect(m_networkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(receiveListReply(QNetworkReply*)));
    connect(m_networkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(receiveLyricsReply(QNetworkReply*)));
    m_networkAccessManager->get(QNetworkRequest(url));
}
void LyricsWidget::receiveLyricsReply(QNetworkReply* reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Error while fetching lyrics: " << reply->errorString();
        setHtml("<span style='color:red'>Error while retrieving lyrics!</span>");
        return;
    }
    
    QString content = QString::fromUtf8(reply->readAll());
    QRegExp regexp("&lt;lyrics&gt;(.*)&lt;/lyrics&gt;");
    if (regexp.indexIn(content) == -1) {
        setText("Invalid lyrics returned, try this:\n" + content);
        return;
    }
    content = regexp.cap(0);
    setText("<pre>" + content + "</pre>");
}
