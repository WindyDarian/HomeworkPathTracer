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
    viewMenu->addAction(objectBBoxVisible);
    kdTreeBBoxVisible = new QAction("Bounding Box (BVH Tree)", viewMenu);
    kdTreeBBoxVisible->setCheckable(true);
    kdTreeBBoxVisible->setChecked(true);
    viewMenu->addAction(kdTreeBBoxVisible);

    connect(objectBBoxVisible, SIGNAL(toggled(bool)),
                     this, SLOT(on_objectbboxvisible_toggled(bool)));
    connect(kdTreeBBoxVisible, SIGNAL(toggled(bool)),
                     this, SLOT(on_kdtreebboxvisible_toggled(bool)));



    samplerMenu = new QMenu("Anti-Aliasing");
    this->menuBar()->addMenu(samplerMenu);
    QAction * a;

    selectSampler = new QActionGroup( this );

    a = new QAction("Uniform", viewMenu);
    a->setCheckable(true);
    samplerMenu->addAction(a);
    this->uniformSampler = a;
    a->setActionGroup(selectSampler);
    a->setChecked(true);

    a = new QAction("Random", viewMenu);
    a->setCheckable(true);
    samplerMenu->addAction(a);
    this->randomSampler = a;
    a->setActionGroup(selectSampler);

    a = new QAction("Stratified", viewMenu);
    a->setCheckable(true);
    samplerMenu->addAction(a);
    this->stratifiedSampler = a;
    a->setActionGroup(selectSampler);

    a = new QAction("Image-Wide Stratified", viewMenu);
    a->setCheckable(true);
    samplerMenu->addAction(a);
    this->iwsSampler = a;
    a->setActionGroup(selectSampler);

    samplerMenu->addSeparator();

    selectAALevel = new QActionGroup( this );

    a = new QAction("1 x 1", viewMenu);
    a->setCheckable(true);
    samplerMenu->addAction(a);
    this->aalevelOne = a;
    a->setActionGroup(selectAALevel);
    a->setChecked(true);

    a = new QAction("2 x 2", viewMenu);
    a->setCheckable(true);
    samplerMenu->addAction(a);
    this->aalevelTwo = a;
    a->setActionGroup(selectAALevel);

    a = new QAction("4 x 4", viewMenu);
    a->setCheckable(true);
    samplerMenu->addAction(a);
    this->aalevelFour = a;
    a->setActionGroup(selectAALevel);


    bvhMenu = new QMenu("BVH Settings");
    this->menuBar()->addMenu(bvhMenu);

    selectBVH = new QActionGroup( this );

    a = new QAction("No BVH", viewMenu);
    a->setCheckable(true);
    bvhMenu->addAction(a);
    this->noBVH = a;
    a->setActionGroup(selectBVH);

    a = new QAction("SPLIT_EQUAL_COUNTS", viewMenu);
    a->setCheckable(true);
    bvhMenu->addAction(a);
    this->splitEqualCounts = a;
    a->setActionGroup(selectBVH);

    a = new QAction("SPLIT_SAH", viewMenu);
    a->setCheckable(true);
    bvhMenu->addAction(a);
    this->splitSAH = a;
    a->setActionGroup(selectBVH);
    a->setChecked(true);

}

MainWindow::~MainWindow()
{
    delete bvhMenu;
    delete samplerMenu;
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
