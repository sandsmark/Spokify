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
