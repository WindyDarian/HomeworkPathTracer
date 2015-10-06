#include "mainwindow.h"
#include <ui_mainwindow.h>
#include <cameracontrolshelp.h>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mygl->setFocus();

    viewMenu = new QMenu("View");
    this->menuBar()->addMenu(viewMenu);
    objectBBoxVisible = new QAction("Bounding Box (Object)", viewMenu);
    objectBBoxVisible->setCheckable(true);
    objectBBoxVisible->setChecked(true);
    kdTreeBBoxVisible = new QAction("Bounding Box (K-D Tree)", viewMenu);
    kdTreeBBoxVisible->setCheckable(true);
    kdTreeBBoxVisible->setChecked(false);

    connect(objectBBoxVisible, SIGNAL(toggled(bool)),
                     this, SLOT(on_objectbboxvisible_toggled(bool)));
    connect(kdTreeBBoxVisible, SIGNAL(toggled(bool)),
                     this, SLOT(on_kdtreebboxvisible_toggled(bool)));

    viewMenu->addAction(objectBBoxVisible);
    viewMenu->addAction(kdTreeBBoxVisible);



}

MainWindow::~MainWindow()
{
    delete kdTreeBBoxVisible;
    delete objectBBoxVisible;
    delete viewMenu;
    delete ui;
}

void MainWindow::on_actionQuit_triggered()
{
    QApplication::exit();
}

void MainWindow::on_actionLoad_Scene_triggered()
{
    ui->mygl->SceneLoadDialog();
}

void MainWindow::slot_ResizeToCamera(int w, int h)
{
    setFixedWidth(w);
    setFixedWidth(h);
}

void MainWindow::on_actionRender_triggered()
{
    ui->mygl->RaytraceScene();
}

void MainWindow::on_actionCamera_Controls_triggered()
{
    CameraControlsHelp* c = new CameraControlsHelp();
    c->show();
}

void MainWindow::on_kdtreebboxvisible_toggled(bool value)
{
    ui->mygl->setKDTreeBBoxVisible(value);
}

void MainWindow::on_objectbboxvisible_toggled(bool value)
{
    ui->mygl->setObjectBBoxVisible(value);
}
