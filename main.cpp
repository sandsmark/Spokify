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

#include <KAboutData>
#include <KCmdLineArgs>
#include <KStandardDirs>
#include <KUniqueApplication>
#include <time.h>
 
#include "mainwindow.h"

int main(int argc, char **argv)
{
    // Get a new seed when starting application
    srand((unsigned)time(0));
    KAboutData aboutData("spokify", "spokify",
                         ki18n("Spokify"), "1.0",
                         ki18n("A Free Spotify Client"),
                         KAboutData::License_GPL_V3,
                         ki18n("Copyright (C) 2010 Rafael Fernández López"));
    aboutData.setProgramIconName("preferences-desktop-text-to-speech");
    aboutData.setHomepage("http://www.ereslibre.es/projects/spokify");

    aboutData.addAuthor(ki18n("Rafael Fernández López"), ki18n("Developer and maintainer"), "ereslibre@kde.org");

    KCmdLineArgs::init(argc, argv, &aboutData);
    KUniqueApplication app;
    setlocale(LC_NUMERIC, "C");
    QCoreApplication::setApplicationVersion("1.0");

    aboutData.setOtherText(ki18n("This product uses SPOTIFY CORE but is not endorsed, certified or otherwise approved in any way by Spotify. Spotify is the registered trade mark of the Spotify Group.\n\n<html><img src=\"%1\"></html>").subs(KStandardDirs::locate("appdata", "images/spotify-core-logo-128x128.png")));

    KGlobal::activeComponent().setAboutData(aboutData);

    MainWindow *window = new MainWindow();
    if (window->session() == NULL) {
        delete window;
        return EXIT_FAILURE;
    }

    window->show();

    return app.exec();
}
