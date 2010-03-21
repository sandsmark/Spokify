#include "mainwindow.h"
 
#include <KApplication>
#include <KAction>
#include <KLocale>
#include <KActionCollection>
#include <KStandardAction>
 
MainWindow::MainWindow(QWidget *parent)
  : KXmlGuiWindow(parent)
{
  setCentralWidget(new QWidget(this));
  setupActions();
}
 
void MainWindow::setupActions()
{
  setupGUI(Default, "spokifyui.rc");
}
