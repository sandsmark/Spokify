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

#include <KApplication>
#include <KAboutData>
#include <KCmdLineArgs>
 
#include "mainwindow.h"
 
int main(int argc, char **argv)
{
  KAboutData aboutData("spokify", "spokify",
                       ki18n("Spokify"), "1.0",
                       ki18n("A Free Spotify Client"),
                       KAboutData::License_GPL,
                       ki18n("Copyright (c) 2010 Rafael Fernández López"));

  KCmdLineArgs::init(argc, argv, &aboutData);
  KApplication app;
 
  MainWindow* window = new MainWindow();
  window->show();

  return app.exec();
}
