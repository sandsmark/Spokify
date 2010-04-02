/*
 * This file is part of Spokify.
 * Copyright (C) 2010 Rafael Fernández López <ereslibre@kde.org>
 *
 * Spokify is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Spokify is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Spokify.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mainwindow.h"
#include "login.h"
#include "trackmodel.h"
#include "mainwidget.h"
#include "soundfeeder.h"
#include "playlistmodel.h"

#include <QtCore/QTimer>
#include <QtCore/QBuffer>

#include <QtGui/QLabel>
#include <QtGui/QListView>
#include <QtGui/QBoxLayout>
#include <QtGui/QDockWidget>
#include <QtGui/QProgressBar>

#include <KDebug>
#include <KAction>
#include <KLocale>
#include <KLineEdit>
#include <KComboBox>
#include <KAboutData>
#include <KStatusBar>
#include <KPushButton>
#include <KMessageBox>
#include <KApplication>
#include <KStandardDirs>
#include <KStandardAction>
#include <KActionCollection>
#include <KStatusNotifierItem>

MainWindow *MainWindow::s_self = 0;

//BEGIN: SpotifySession - application bridge
namespace SpotifySession {

    static void loggedIn(sp_session *session, sp_error error)
    {
        Q_UNUSED(session);

        if (error == SP_ERROR_OK) {
            MainWindow::self()->spotifyLoggedIn();
            return;
        }
        MainWindow::self()->restoreStatusBarSlot();
        MainWindow::self()->actionCollection()->action("login")->setEnabled(true);
        switch (error) {
            case SP_ERROR_BAD_API_VERSION:
            case SP_ERROR_API_INITIALIZATION_FAILED:
            case SP_ERROR_RESOURCE_NOT_LOADED:
            case SP_ERROR_BAD_APPLICATION_KEY:
            case SP_ERROR_CLIENT_TOO_OLD:
            case SP_ERROR_BAD_USER_AGENT:
            case SP_ERROR_MISSING_CALLBACK:
            case SP_ERROR_INVALID_INDATA:
            case SP_ERROR_INDEX_OUT_OF_RANGE:
            case SP_ERROR_OTHER_TRANSIENT:
            case SP_ERROR_IS_LOADING:
                KMessageBox::sorry(MainWindow::self(), i18n("An internal error happened with error code (%1).\n\nPlease, report this bug.").arg(error),
                                   i18n("A critical error happened"));
                break;
            case SP_ERROR_BAD_USERNAME_OR_PASSWORD:
                KMessageBox::sorry(MainWindow::self(), i18n("Invalid username or password"),
                                   i18n("Invalid username or password"));
                break;
            case SP_ERROR_USER_BANNED:
                KMessageBox::sorry(MainWindow::self(), i18n("This user has been banned"),
                                   i18n("User banned"));
                break;
            case SP_ERROR_UNABLE_TO_CONTACT_SERVER:
                KMessageBox::sorry(MainWindow::self(), i18n("Cannot connect to server"),
                                   i18n("Cannot connect to server"));
                break;
            case SP_ERROR_OTHER_PERMANENT:
                KMessageBox::sorry(MainWindow::self(), i18n("Something wrong happened.\n\nWhatever it is, it is permanent."),
                                   i18n("Something wrong happened"));
                break;
            case SP_ERROR_USER_NEEDS_PREMIUM:
                KMessageBox::sorry(MainWindow::self(), i18n("You need to be a Premium User in order to login"),
                                   i18n("Premium User access required"));
                break;
            default:
                break;
        }
    }

    static void loggedOut(sp_session *session)
    {
        Q_UNUSED(session);

        MainWindow::self()->spotifyLoggedOut();
    }

    static void metadataUpdated(sp_session *session)
    {
        Q_UNUSED(session);
    }

    static void connectionError(sp_session *session, sp_error error)
    {
        Q_UNUSED(session);
        Q_UNUSED(error);
    }

    static void messageToUser(sp_session *session, const char *message)
    {
        Q_UNUSED(session);
        Q_UNUSED(message);
    }

    static void notifyMainThread(sp_session *session)
    {
        Q_UNUSED(session);
        MainWindow::self()->setCheckSpotifyEvents(true);
    }

    static int musicDelivery(sp_session *session, const sp_audioformat *format, const void *frames, int numFrames)
    {
        Q_UNUSED(session);

        if (!numFrames) {
            return 0;
        }

        QMutex &m = MainWindow::self()->dataMutex();
        m.lock();
        Chunk c;
        c.m_data = malloc(numFrames * sizeof(int16_t) * format->channels);
        memcpy(c.m_data, frames, numFrames * sizeof(int16_t) * format->channels);
        c.m_dataFrames = numFrames;
        MainWindow::self()->newChunk(c);
        m.unlock();
        MainWindow::self()->pcmWaitCondition().wakeAll();

        return numFrames;
    }

    static void playTokenLost(sp_session *session)
    {
        Q_UNUSED(session);
    }

    static void logMessage(sp_session *session, const char *data)
    {
        Q_UNUSED(session);
        Q_UNUSED(data);
    }

    static void endOfTrack(sp_session *session)
    {
        Q_UNUSED(session);
    }

    static sp_session_callbacks spotifyCallbacks = {
        &SpotifySession::loggedIn,
        &SpotifySession::loggedOut,
        &SpotifySession::metadataUpdated,
        &SpotifySession::connectionError,
        &SpotifySession::messageToUser,
        &SpotifySession::notifyMainThread,
        &SpotifySession::musicDelivery,
        &SpotifySession::playTokenLost,
        &SpotifySession::logMessage,
        &SpotifySession::endOfTrack
    };

}
//END: SpotifySession - application bridge

//BEGIN: SpotifyPlaylists - application bridge
namespace SpotifyPlaylists {

    static void tracksAdded(sp_playlist *pl, sp_track *const *tracks, int numTracks, int position, void *userdata)
    {
        Q_UNUSED(pl);
        Q_UNUSED(tracks);
        Q_UNUSED(numTracks);
        Q_UNUSED(position);
        Q_UNUSED(userdata);
    }

    static void tracksRemoved(sp_playlist *pl, const int *tracks, int numTracks, void *userdata)
    {
        Q_UNUSED(pl);
        Q_UNUSED(tracks);
        Q_UNUSED(numTracks);
        Q_UNUSED(userdata);
    }

    static void tracksMoved(sp_playlist *pl, const int *tracks, int numTracks, int newPosition, void *userdata)
    {
        Q_UNUSED(pl);
        Q_UNUSED(tracks);
        Q_UNUSED(numTracks);
        Q_UNUSED(newPosition);
        Q_UNUSED(userdata);
    }

    static void playlistRenamed(sp_playlist *pl, void *userdata)
    {
        Q_UNUSED(pl);
        Q_UNUSED(userdata);
    }

    static void playlistStateChanged(sp_playlist *pl, void *userdata)
    {
        Q_UNUSED(pl);
        Q_UNUSED(userdata);
    }

    static void playlistUpdateInProgress(sp_playlist *pl, bool done, void *userdata)
    {
        Q_UNUSED(pl);
        Q_UNUSED(done);
        Q_UNUSED(userdata);
    }

    static void playlistMetadataUpdated(sp_playlist *pl, void *userdata)
    {
        Q_UNUSED(pl);
        Q_UNUSED(userdata);
    }

    static sp_playlist_callbacks spotifyCallbacks = {
        &SpotifyPlaylists::tracksAdded,
        &SpotifyPlaylists::tracksRemoved,
        &SpotifyPlaylists::tracksMoved,
        &SpotifyPlaylists::playlistRenamed,
        &SpotifyPlaylists::playlistStateChanged,
        &SpotifyPlaylists::playlistUpdateInProgress,
        &SpotifyPlaylists::playlistMetadataUpdated
    };

}
//END: SpotifyPlaylists - application bridge

//BEGIN: SpotifyPlaylistContainer - application bridge
namespace SpotifyPlaylistContainer {

    static void playlistAdded(sp_playlistcontainer *pc, sp_playlist *playlist, int position, void *userdata)
    {
        Q_UNUSED(pc);
        Q_UNUSED(playlist);
        Q_UNUSED(position);
        Q_UNUSED(userdata);
    }

    static void playlistRemoved(sp_playlistcontainer *pc, sp_playlist *playlist, int position, void *userdata)
    {
        Q_UNUSED(pc);
        Q_UNUSED(playlist);
        Q_UNUSED(position);
        Q_UNUSED(userdata);
    }

    static void playlistMoved(sp_playlistcontainer *pc, sp_playlist *playlist, int position, int newPosition, void *userdata)
    {
        Q_UNUSED(pc);
        Q_UNUSED(playlist);
        Q_UNUSED(position);
        Q_UNUSED(newPosition);
        Q_UNUSED(userdata);
    }

    static void containerLoaded(sp_playlistcontainer *pc, void *userdata)
    {
        Q_UNUSED(pc);
        Q_UNUSED(userdata);
    }

    static sp_playlistcontainer_callbacks spotifyCallbacks = {
        &SpotifyPlaylistContainer::playlistAdded,
        &SpotifyPlaylistContainer::playlistRemoved,
        &SpotifyPlaylistContainer::playlistMoved,
        &SpotifyPlaylistContainer::containerLoaded
    };

}
//END: SpotifyPlaylistContainer - application bridge

//BEGIN: SpotifySearch - application bridge
namespace SpotifySearch {

    static void searchComplete(sp_search *result, void *userdata)
    {
        Q_UNUSED(userdata);

        MainWindow::self()->playlistView()->setCurrentIndex(QModelIndex());
        TrackModel *const trackModel = MainWindow::self()->mainWidget()->trackModel();
        trackModel->removeRows(0, trackModel->rowCount());
        trackModel->insertRows(0, sp_search_num_tracks(result));
        for (int i = 0; i < sp_search_num_tracks(result); ++i) {
            sp_track *const tr = sp_search_track(result, i);
            if (!tr) {
                continue;
            }
            {
                const QModelIndex &index = trackModel->index(i, TrackModel::Title);
                trackModel->setData(index, QString::fromUtf8(sp_track_name(tr)));
            }
            {
                const QModelIndex &index = trackModel->index(i, TrackModel::Artist);
                sp_artist *const artist = sp_track_artist(tr, 0);
                if (artist) {
                    trackModel->setData(index, QString::fromUtf8(sp_artist_name(artist)));
                }
            }
            {
                const QModelIndex &index = trackModel->index(i, TrackModel::Album);
                sp_album *const album = sp_track_album(tr);
                if (album) {
                    trackModel->setData(index, QString::fromUtf8(sp_album_name(album)));
                }
            }
            {
                const QModelIndex &index = trackModel->index(i, TrackModel::Popularity);
                trackModel->setData(index, sp_track_popularity(tr));
            }
            {
                const QModelIndex &index = trackModel->index(i, TrackModel::Title);
                trackModel->setData(index, QVariant::fromValue<sp_track*>(tr), TrackModel::SpotifyNativeTrack);
            }
        }
        MainWindow::self()->showTemporaryMessage(i18n("Search complete"));
    }

    static void dummySearchComplete(sp_search *result, void *userdata)
    {
        Q_UNUSED(userdata);

        QString *query = static_cast<QString*>(userdata);

        const int res = sp_search_total_tracks(result);
        sp_search_create(MainWindow::self()->session(), query->toUtf8().data(), 0, res, 0, 0, 0, 0, &SpotifySearch::searchComplete, 0);

        delete query;
    }

}
//END: SpotifySearch - application bridge

//BEGIN: SpotifyImage - application bridge
namespace SpotifyImage {

    void imageLoaded(sp_image *image, void *userdata)
    {
        QModelIndex index = *static_cast<QModelIndex*>(userdata);
        size_t dataSize = 0;
        const void *imageData = sp_image_data(image, &dataSize);
        MainWindow::self()->setCurrentCover(QImage::fromData(static_cast<const uchar*>(imageData), dataSize, "JPEG"));
    }

}
//END: SpotifyImage - application bridge

MainWindow::MainWindow(QWidget *parent)
    : KXmlGuiWindow(parent)
    , m_soundFeeder(new SoundFeeder(this))
    , m_isPlaying(false)
    , m_checkSpotifyEvents(false)
    , m_pc(0)
    , m_statusLabel(new QLabel(i18n("Ready"), this))
    , m_progress(new QProgressBar(this))
    , m_notifierItem(new KStatusNotifierItem(i18n("Spokify"), this))
    , m_loggedIn(false)
    , m_mainWidget(new MainWidget(this))
    , m_playlistModel(new PlaylistModel(this))
    , m_playlistView(new QListView(this))
{
    s_self = this;

    m_notifierItem->setCategory(KStatusNotifierItem::ApplicationStatus);
    m_notifierItem->setAssociatedWidget(this);
    m_notifierItem->setToolTip("preferences-desktop-text-to-speech", "Spokify", KGlobal::mainComponent().aboutData()->shortDescription());
    m_notifierItem->setStatus(KStatusNotifierItem::Active);
    m_notifierItem->setIconByName("preferences-desktop-text-to-speech");

    connect(m_soundFeeder, SIGNAL(pcmWritten(int)), this, SLOT(pcmWrittenSlot(int)));
    connect(m_mainWidget, SIGNAL(trackRequest(QModelIndex)), this, SLOT(trackRequested(QModelIndex)));
    connect(m_mainWidget, SIGNAL(seekPosition(int)), this, SLOT(seekPosition(int)));

    setCentralWidget(m_mainWidget);
    setupActions();

    //BEGIN: Spotify session init
    {
        m_config.api_version = SPOTIFY_API_VERSION;
        m_config.cache_location = "tmp";
        m_config.settings_location = "tmp";
        m_config.application_key = g_appkey;
        m_config.application_key_size = g_appkey_size;
        m_config.user_agent = "spokify";
        m_config.callbacks = &SpotifySession::spotifyCallbacks;

        sp_session_init(&m_config, &m_session);
    }
    //END: Spotify session init

    //BEGIN: set up playlists widget
    {
        m_playlistView->setAlternatingRowColors(true);
        m_playlistView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_playlistView->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
        m_playlistView->setMouseTracking(true);
        m_playlistView->setModel(m_playlistModel);
        QDockWidget *playlists = new QDockWidget(i18n("Playlists"), this);
        playlists->setObjectName("playlists");
        playlists->setWidget(m_playlistView);
        addDockWidget(Qt::LeftDockWidgetArea, playlists);
        connect(m_playlistView, SIGNAL(activated(QModelIndex)), this, SLOT(playListChanged(QModelIndex)));
    }
    //END: set up playlists widget

    //BEGIN: set up search widget
    {
        QDockWidget *search = new QDockWidget(i18n("Search"), this);
        search->setObjectName("search");
        search->setWidget(createSearchWidget());
        addDockWidget(Qt::LeftDockWidgetArea, search);
    }
    //END: set up search widget

    //BEGIN: set up search widget
    {
        QDockWidget *cover = new QDockWidget(i18n("Cover"), this);
        cover->setObjectName("cover");
        cover->setWidget(createCoverWidget());
        addDockWidget(Qt::LeftDockWidgetArea, cover);
    }
    //END: set up search widget

    m_progress->setMinimum(0);
    m_progress->setMaximum(0);
    m_progress->setVisible(false);

    startTimer(500);
    statusBar()->insertWidget(0, m_statusLabel);
    statusBar()->insertWidget(1, new QWidget(this), 1);
    statusBar()->insertWidget(2, m_progress);

    clearAllWidgets();

    initSound();
    m_soundFeeder->start();
}

MainWindow::~MainWindow()
{
    m_soundFeeder->terminate();
    if (m_loggedIn) {
        sp_session_logout(m_session);
    }
}

sp_session *MainWindow::session() const
{
    return m_session;
}

MainWindow *MainWindow::self()
{
    return s_self;
}

MainWidget *MainWindow::mainWidget() const
{
    return m_mainWidget;
}

QListView *MainWindow::playlistView() const
{
    return m_playlistView;
}

void MainWindow::setIsPlaying(bool isPlaying)
{
    m_isPlaying = isPlaying;
    m_play->setVisible(!isPlaying);
    m_pause->setVisible(isPlaying);
}

bool MainWindow::isPlaying() const
{
    return m_isPlaying;
}

void MainWindow::setCheckSpotifyEvents(bool checkSpotifyEvents)
{
    m_checkSpotifyEvents = checkSpotifyEvents;
}

void MainWindow::setCurrentCover(const QImage &cover)
{
    m_cover->setPixmap(QPixmap::fromImage(cover));
}

void MainWindow::spotifyLoggedIn()
{
    m_loggedIn = true;
    m_login->setVisible(false);
    m_login->setEnabled(true);
    m_logout->setVisible(true);
    showTemporaryMessage(i18n("Logged in"));
    fillPlaylistModel();
}

void MainWindow::spotifyLoggedOut()
{
    if (!m_loggedIn) {
        return;
    }
    m_loggedIn = false;
    m_login->setVisible(true);
    m_logout->setVisible(false);
    m_logout->setEnabled(true);
    showTemporaryMessage(i18n("Logged out"));
    clearAllWidgets();
}

void MainWindow::showTemporaryMessage(const QString &message)
{
    m_progress->setVisible(false);
    m_statusLabel->setText(message);
    QTimer::singleShot(2000, this, SLOT(restoreStatusBarSlot()));
}

void MainWindow::showRequest(const QString &request)
{
    m_progress->setVisible(true);
    m_statusLabel->setText(request);
}

snd_pcm_t *MainWindow::pcmHandle() const
{
    return m_snd;
}

QMutex &MainWindow::pcmMutex()
{
    return m_pcmMutex;
}

QMutex &MainWindow::dataMutex()
{
    return m_dataMutex;
}

QWaitCondition &MainWindow::pcmWaitCondition()
{
    return m_pcmWaitCondition;
}

QWaitCondition &MainWindow::playCondition()
{
    return m_playCondition;
}

void MainWindow::newChunk(const Chunk &chunk)
{
    m_data.enqueue(chunk);
}

Chunk MainWindow::nextChunk()
{
    return m_data.dequeue();
}

bool MainWindow::hasChunk() const
{
    return !m_data.isEmpty();
}

void MainWindow::restoreStatusBarSlot()
{
    m_progress->setVisible(false);
    m_statusLabel->setText(i18n("Ready"));
}

bool MainWindow::event(QEvent *event)
{
    //BEGIN: Spotify event processing
    switch (event->type()) {
        case QEvent::Timer: {
            if (m_checkSpotifyEvents) {
                m_checkSpotifyEvents = false;
                int timeout = -1;
                sp_session_process_events(m_session, &timeout);
                event->accept();
                return true;
            }
        }
        default:
            break;
    }
    //END: Spotify event processing
    return KXmlGuiWindow::event(event);
}

void MainWindow::loginSlot()
{
    Login *login = new Login(this);
    if (login->exec() == KDialog::Accepted) {
        m_login->setEnabled(false);
        showRequest(i18n("Logging in..."));
    }
    delete login;
}

void MainWindow::logoutSlot()
{
    m_logout->setEnabled(false);
    showRequest(i18n("Logging out..."));
    //BEGIN: Spotify logout
    sp_session_logout(m_session);
    //END: Spotify logout
}

void MainWindow::previousSlot()
{
}

void MainWindow::playSlot()
{
    setIsPlaying(true);
    m_playCondition.wakeAll();
    sp_session_player_play(m_session, true);
}

void MainWindow::pauseSlot()
{
    sp_session_player_play(m_session, false);
    setIsPlaying(false);
}

void MainWindow::nextSlot()
{
}

void MainWindow::shuffleSlot()
{
}

void MainWindow::repeatSlot()
{
}

void MainWindow::performSearch()
{
    QString query;
    switch (m_searchCategory->currentIndex()) {
        case 0: // All
            query = m_searchField->text();
            break;
        case 1: // Tracks
            query = QString("track:%1").arg(m_searchField->text());
            break;
        case 2: // Artist
            query = QString("artist:%1").arg(m_searchField->text());
            break;
        case 3: // Album
            query = QString("album:%1").arg(m_searchField->text());
            break;
        case 4: // Year
            query = QString("year:%1").arg(m_searchField->text());
            break;
        case 5: // Record Company
            query = QString("label:%1").arg(m_searchField->text());
            break;
        default:
            Q_ASSERT(false);
            return;
    }
    showRequest(i18n("Searching..."));
    sp_search_create(m_session, query.toUtf8().data(), 0, 1, 0, 0, 0, 0, &SpotifySearch::dummySearchComplete, new QString(query));
}

void MainWindow::pcmWrittenSlot(int frames)
{
    m_mainWidget->advanceCurrentTrackTime(frames);
}

void MainWindow::playListChanged(const QModelIndex &index)
{
    TrackModel *const trackModel = m_mainWidget->trackModel();
    trackModel->removeRows(0, trackModel->rowCount());

    sp_playlist *const curr = index.data(PlaylistModel::SpotifyNativePlaylist).value<sp_playlist*>();
    const int numTracks = sp_playlist_num_tracks(curr);
    trackModel->insertRows(0, numTracks);
    for (int i = 0; i < numTracks; ++i) {
        sp_track *const tr = sp_playlist_track(curr, i);
        if (!tr) {
            continue;
        }
        {
            const QModelIndex &index = trackModel->index(i, TrackModel::Title);
            trackModel->setData(index, QString::fromUtf8(sp_track_name(tr)));
        }
        {
            const QModelIndex &index = trackModel->index(i, TrackModel::Artist);
            sp_artist *const artist = sp_track_artist(tr, 0);
            if (artist) {
                trackModel->setData(index, QString::fromUtf8(sp_artist_name(artist)));
            }
        }
        {
            const QModelIndex &index = trackModel->index(i, TrackModel::Album);
            sp_album *const album = sp_track_album(tr);
            if (album) {
                trackModel->setData(index, QString::fromUtf8(sp_album_name(album)));
            }
        }
        {
            const QModelIndex &index = trackModel->index(i, TrackModel::Popularity);
            trackModel->setData(index, sp_track_popularity(tr));
        }
        {
            const QModelIndex &index = trackModel->index(i, TrackModel::Title);
            trackModel->setData(index, QVariant::fromValue<sp_track*>(tr), TrackModel::SpotifyNativeTrack);
        }
    }
}

void MainWindow::trackRequested(const QModelIndex &index)
{
    m_dataMutex.lock();
    if (m_isPlaying) {
        sp_session_player_play(m_session, false);
        sp_session_player_unload(m_session);
        m_pcmMutex.lock();
        snd_pcm_drop(m_snd);
        m_pcmMutex.unlock();
        while (!m_data.isEmpty()) {
            Chunk c = m_data.dequeue();
            free(c.m_data);
        }
    }
    m_dataMutex.unlock();
    setIsPlaying(true);
    m_pcmMutex.lock();
    snd_pcm_prepare(m_snd);
    m_pcmMutex.unlock();
    sp_track *const tr = index.data(TrackModel::SpotifyNativeTrack).value<sp_track*>();
    sp_album *const album = sp_track_album(tr);
    const byte *image = sp_album_cover(album);
    sp_image *const cover = sp_image_create(m_session, image);
    sp_image_add_load_callback(cover, &SpotifyImage::imageLoaded, m_session);
    sp_session_player_load(m_session, tr);
    m_mainWidget->setTotalTrackTime(sp_track_duration(tr));
    sp_session_player_play(m_session, true);
}

void MainWindow::seekPosition(int position)
{
    m_pcmMutex.lock();
    snd_pcm_drop(m_snd);
    while (!m_data.isEmpty()) {
        Chunk c = m_data.dequeue();
        free(c.m_data);
    }
    snd_pcm_prepare(m_snd);
    m_pcmMutex.unlock();
    sp_session_player_seek(m_session, position);
}

void MainWindow::clearAllWidgets()
{
    m_playlistModel->removeRows(0, m_playlistModel->rowCount());
    m_playlistView->setEnabled(false);
    TrackModel *trackModel = m_mainWidget->trackModel();
    trackModel->removeRows(0, trackModel->rowCount());
}

void MainWindow::initSound()
{
    int d = 0;
    snd_pcm_uframes_t periodSize = 1024;
    snd_pcm_uframes_t bufferSize = periodSize * 4;

    snd_pcm_hw_params_t *hwParams;
    snd_pcm_open(&m_snd, "default", SND_PCM_STREAM_PLAYBACK, 0);
    snd_pcm_hw_params_malloc(&hwParams);
    snd_pcm_hw_params_any(m_snd, hwParams);
    snd_pcm_hw_params_set_access(m_snd, hwParams, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(m_snd, hwParams, SND_PCM_FORMAT_S16_LE);
    snd_pcm_hw_params_set_rate(m_snd, hwParams, 44000, 0);
    snd_pcm_hw_params_set_channels(m_snd, hwParams, 2);
    snd_pcm_hw_params_set_period_size_near(m_snd, hwParams, &periodSize, &d);
    snd_pcm_hw_params_set_buffer_size_near(m_snd, hwParams, &bufferSize);
    snd_pcm_hw_params(m_snd, hwParams);
    snd_pcm_hw_params_free(hwParams);

    snd_pcm_sw_params_t *swParams;
    snd_pcm_sw_params_malloc(&swParams);
    snd_pcm_sw_params_current(m_snd, swParams);
    snd_pcm_sw_params_set_avail_min(m_snd, swParams, periodSize);
    snd_pcm_sw_params_set_start_threshold(m_snd, swParams, 0);
    snd_pcm_sw_params(m_snd, swParams);
    snd_pcm_sw_params_free(swParams);

    snd_pcm_prepare(m_snd);
}

QWidget *MainWindow::createSearchWidget()
{
    QWidget *searchWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout;
    {
        m_searchCategory = new KComboBox(this);
        m_searchCategory->addItem(i18n("All"));
        m_searchCategory->addItem(i18n("Tracks"));
        m_searchCategory->addItem(i18n("Artists"));
        m_searchCategory->addItem(i18n("Album"));
        m_searchCategory->addItem(i18n("Year"));
        m_searchCategory->addItem(i18n("Record Company"));
        m_searchField = new KLineEdit(this);
        m_searchField->setClickMessage(i18n("Search"));
        m_searchField->setClearButtonShown(true);
        layout->addWidget(m_searchCategory);
        layout->addWidget(m_searchField);
    }
    {
        QHBoxLayout *innerLayout2 = new QHBoxLayout;
        KPushButton *searchButton = new KPushButton(KIcon(), i18n("Search"), this);
        connect(searchButton, SIGNAL(clicked()), this, SLOT(performSearch()));
        innerLayout2->addStretch();
        innerLayout2->addWidget(searchButton);
        layout->addLayout(innerLayout2);
    }
    layout->addStretch();
    searchWidget->setLayout(layout);
    return searchWidget;
}

QWidget *MainWindow::createCoverWidget()
{
    QWidget *coverWidget = new QWidget(this);
    m_cover = new QLabel(coverWidget);
    m_cover->setMinimumSize(1, 1);
    m_cover->setScaledContents(true);
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addStretch();
    layout->addWidget(m_cover);
    layout->addStretch();
    coverWidget->setLayout(layout);
    return coverWidget;
}

void MainWindow::setupActions()
{
    m_login = new KAction(this);
    m_login->setText(i18n("&Login"));
    m_login->setIcon(KIcon("user-online"));
    m_login->setShortcut(Qt::CTRL + Qt::Key_L);
    actionCollection()->addAction("login", m_login);
    connect(m_login, SIGNAL(triggered(bool)), this, SLOT(loginSlot()));

    m_logout = new KAction(this);
    m_logout->setVisible(false);
    m_logout->setText(i18n("L&ogout"));
    m_logout->setIcon(KIcon("user-offline"));
    m_logout->setShortcut(Qt::CTRL + Qt::Key_O);
    actionCollection()->addAction("logout", m_logout);
    connect(m_logout, SIGNAL(triggered(bool)), this, SLOT(logoutSlot()));    

    m_previous = new KAction(this);
    m_previous->setText(i18n("&Previous"));
    m_previous->setIcon(KIcon("media-skip-backward"));
    m_previous->setShortcut(Qt::CTRL + Qt::Key_P);
    actionCollection()->addAction("previous", m_previous);
    connect(m_previous, SIGNAL(triggered(bool)), this, SLOT(previousSlot()));

    m_play = new KAction(this);
    m_play->setText(i18n("P&lay"));
    m_play->setIcon(KIcon("media-playback-start"));
    m_play->setShortcut(Qt::CTRL + Qt::Key_L);
    actionCollection()->addAction("play", m_play);
    connect(m_play, SIGNAL(triggered(bool)), this, SLOT(playSlot()));

    m_pause = new KAction(this);
    m_pause->setVisible(false);
    m_pause->setText(i18n("Pau&se"));
    m_pause->setIcon(KIcon("media-playback-pause"));
    m_pause->setShortcut(Qt::CTRL + Qt::Key_S);
    actionCollection()->addAction("pause", m_pause);
    connect(m_pause, SIGNAL(triggered(bool)), this, SLOT(pauseSlot()));
    
    m_next = new KAction(this);
    m_next->setText(i18n("&Next"));
    m_next->setIcon(KIcon("media-skip-forward"));
    m_next->setShortcut(Qt::CTRL + Qt::Key_N);
    actionCollection()->addAction("next", m_next);
    connect(m_logout, SIGNAL(triggered(bool)), this, SLOT(nextSlot()));

    m_shuffle = new KAction(this);
    m_shuffle->setText(i18n("Shu&ffle"));
    m_shuffle->setIcon(KIcon("tools-wizard"));
    m_shuffle->setShortcut(Qt::CTRL + Qt::Key_F);
    actionCollection()->addAction("shuffle", m_shuffle);
    connect(m_shuffle, SIGNAL(triggered(bool)), this, SLOT(shuffleSlot()));

    m_repeat = new KAction(this);
    m_repeat->setText(i18n("R&epeat"));
    m_repeat->setIcon(KIcon("view-refresh"));
    m_repeat->setShortcut(Qt::CTRL + Qt::Key_E);
    actionCollection()->addAction("repeat", m_repeat);
    connect(m_repeat, SIGNAL(triggered(bool)), this, SLOT(repeatSlot()));

    KStandardAction::quit(kapp, SLOT(quit()), actionCollection());

    setupGUI(Default, "spokifyui.rc");
}

void MainWindow::fillPlaylistModel()
{
    if (!m_pc) {
        m_pc = sp_session_playlistcontainer(m_session);
    }
    m_playlistView->setEnabled(true);
    const int numPlaylists = sp_playlistcontainer_num_playlists(m_pc);
    m_playlistModel->removeRows(0, m_playlistModel->rowCount());
    m_playlistModel->insertRows(0, numPlaylists);
    for (int i = 0; i < numPlaylists; ++i) {
        sp_playlist *pl = sp_playlistcontainer_playlist(m_pc, i);
        const QModelIndex &index = m_playlistModel->index(i);
        m_playlistModel->setData(index, QString::fromUtf8(sp_playlist_name(pl)));
        m_playlistModel->setData(index, QVariant::fromValue<sp_playlist*>(pl), PlaylistModel::SpotifyNativePlaylist);
    }
}
