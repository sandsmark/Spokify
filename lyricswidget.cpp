
#include <QDebug>
#include <QDomDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>


#include "lyricswidget.h"

LyricsWidget::LyricsWidget(QWidget* parent): QTextEdit(parent),
    m_networkAccessManager(new QNetworkAccessManager)
{
    setReadOnly(true);
    setWordWrapMode(QTextOption::WordWrap);
    
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
    disconnect(m_networkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(receiveListReply(QNetworkReply*)));
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
    connect(m_networkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(receiveLyricsReply(QNetworkReply*)));
    m_networkAccessManager->get(QNetworkRequest(url));
}
void LyricsWidget::receiveLyricsReply(QNetworkReply* reply)
{
    disconnect(m_networkAccessManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(receiveLyricsReply(QNetworkReply*)));
    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << "Error while fetching lyrics: " << reply->errorString();
        setHtml("<span style='color:red'>Error while retrieving lyrics!</span>");
        return;
    }
    
    QString content = QString::fromUtf8(reply->readAll());
    int lIndex = content.indexOf("&lt;lyrics&gt;");
    int rIndex = content.indexOf("&lt;/lyrics&gt;");
    if (lIndex == -1 || rIndex == -1) {
        qWarning() << Q_FUNC_INFO << "Unable to find lyrics in text";
        setText(content);
        return;
    }
    lIndex += 15; // We skip the tag
    content = content.mid(lIndex, rIndex - lIndex);
    setText(content);
}
