#ifndef MAINWINDOW_H
#define MAINWINDOW_H
 
#include <KXmlGuiWindow>
 
class MainWindow
    : public KXmlGuiWindow
{
public:
    MainWindow(QWidget *parent = 0);

private:
    void setupActions();
};
 
#endif
