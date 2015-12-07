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

    meshKdTreeBBoxVisible = new QAction("Bounding Box (BVH Tree of Mesh, top 5 layers)", viewMenu);
    meshKdTreeBBoxVisible->setCheckable(true);
    meshKdTreeBBoxVisible->setChecked(true);
    viewMenu->addAction(meshKdTreeBBoxVisible);

    meshTriangleBBoxVisible = new QAction("Bounding Box (Triangle, may crash on big model)", viewMenu);
    meshTriangleBBoxVisible->setCheckable(true);
    meshTriangleBBoxVisible->setChecked(false);
    viewMenu->addAction(meshTriangleBBoxVisible);

    interactiveRenderSwitch = new QAction("Interactive_Render", viewMenu);
    interactiveRenderSwitch->setCheckable(true);
    interactiveRenderSwitch->setChecked(false);
    viewMenu->addAction(interactiveRenderSwitch);

    connect(objectBBoxVisible, SIGNAL(toggled(bool)),
                     this, SLOT(on_objectbboxvisible_toggled(bool)));
    connect(kdTreeBBoxVisible, SIGNAL(toggled(bool)),
                     this, SLOT(on_kdtreebboxvisible_toggled(bool)));
    connect(meshKdTreeBBoxVisible, SIGNAL(toggled(bool)),
            this, SLOT(on_meshkdtreebboxvisible_toggled(bool)));
    connect(meshTriangleBBoxVisible, SIGNAL(toggled(bool)),
            this, SLOT(on_meshtrianglebboxvisible_toggled(bool)));
    connect(interactiveRenderSwitch, SIGNAL(toggled(bool)),
            this, SLOT(on_interactiverendervisible_toggled(bool)));



//    samplerMenu = new QMenu("Anti-Aliasing");
//    this->menuBar()->addMenu(samplerMenu);
//    QAction * a;

//    selectSampler = new QActionGroup( this );

//    a = new QAction("Uniform", viewMenu);
//    a->setCheckable(true);
//    samplerMenu->addAction(a);
//    this->uniformSampler = a;
//    a->setActionGroup(selectSampler);
//    a->setChecked(true);

//    a = new QAction("Random", viewMenu);
//    a->setCheckable(true);
//    samplerMenu->addAction(a);
//    this->randomSampler = a;
//    a->setActionGroup(selectSampler);

//    a = new QAction("Stratified", viewMenu);
//    a->setCheckable(true);
//    samplerMenu->addAction(a);
//    this->stratifiedSampler = a;
//    a->setActionGroup(selectSampler);

//    a = new QAction("Image-Wide Stratified", viewMenu);
//    a->setCheckable(true);
//    samplerMenu->addAction(a);
//    this->iwsSampler = a;
//    a->setActionGroup(selectSampler);


//    samplerMenu->addSeparator();

//    selectAALevel = new QActionGroup( this );

//    a = new QAction("1 x 1", viewMenu);
//    a->setCheckable(true);
//    samplerMenu->addAction(a);
//    this->aalevelOne = a;
//    a->setActionGroup(selectAALevel);

//    a = new QAction("2 x 2", viewMenu);
//    a->setCheckable(true);
//    samplerMenu->addAction(a);
//    this->aalevelTwo = a;
//    a->setActionGroup(selectAALevel);
//    a->setChecked(true);

//    a = new QAction("4 x 4", viewMenu);
//    a->setCheckable(true);
//    samplerMenu->addAction(a);
//    this->aalevelFour = a;
//    a->setActionGroup(selectAALevel);

//    connect(selectSampler, SIGNAL(triggered(QAction*)),
//                     this, SLOT(on_selectSampler_triggered(QAction*)));
//    connect(selectAALevel, SIGNAL(triggered(QAction*)),
//                     this, SLOT(on_selectAALevel_triggered(QAction*)));


    bvhMenu = new QMenu("BVH Settings");
    this->menuBar()->addMenu(bvhMenu);

    selectBVH = new QActionGroup( this );
    QAction * a;
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



    connect(selectBVH, SIGNAL(triggered(QAction*)),
                     this, SLOT(on_selectBVH_triggered(QAction*)));

}

MainWindow::~MainWindow()
{
    if(bvhMenu) delete bvhMenu;
    if(samplerMenu) delete samplerMenu;
    if(viewMenu) delete viewMenu;
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

void MainWindow::on_meshkdtreebboxvisible_toggled(bool value)
{
    ui->mygl->setMeshBBoxVisible(value);
}

void MainWindow::on_meshtrianglebboxvisible_toggled(bool value)
{
    ui->mygl->setTriangleBBoxVisible(value);
}

void MainWindow::on_interactiverendervisible_toggled(bool value)
{
    ui->mygl->setInteractiveRenderEnabled(value);
}

void MainWindow::on_selectSampler_triggered(QAction * action)
{
    MyGL::SamplerType choice;
    if (action == this->uniformSampler)
    {
        choice = MyGL::SAMPLER_UNIFORM;
    }
    else if (action == this->randomSampler)
    {
        choice = MyGL::SAMPLER_RANDOM;
    }
    else if (action == this->stratifiedSampler)
    {
        choice = MyGL::SAMPLER_STRATIFIED;
    }
    else if (action == this->iwsSampler)
    {
        choice = MyGL::SAMPLER_IWS;
    }

    ui->mygl->setSampler(choice);
}

void MainWindow::on_selectAALevel_triggered(QAction * action)
{
    MyGL::AALevelType choice;
    if (action == this->aalevelOne)
    {
        choice = MyGL::AA_ONEONE;
    }
    else if (action == this->aalevelTwo)
    {
        choice = MyGL::AA_TWOTWO;
    }
    else
    {
        choice = MyGL::AA_FOURFOUR;
    }

    ui->mygl->setAALevel(choice);
}

void MainWindow::on_selectBVH_triggered(QAction * action)
{
    if (action == this->noBVH)
    {
        BVHNode::BVHIntersectionDisabled = true;
        std::cout << "BVH intersection test for scene and meshes disabled."<<
                      std::endl;
        return;
    }
    if (action == this->splitEqualCounts)
    {
        BVHNode::CurrentSplitMethodSettings = BVHNode::SPLIT_EQUAL_COUNTS;

    }
    else if (action == this->splitSAH)
    {
        BVHNode::CurrentSplitMethodSettings = BVHNode::SPLIT_SAH;
    }

    if (BVHNode::BVHIntersectionDisabled)
    {
        BVHNode::BVHIntersectionDisabled = false;
        std::cout << "BVH intersection test enabled."<< std::endl;
    }
    std::cout << "BVH set to " << BVHNode::GetCurrentSplitMethodText() << std::endl;
    std::cout << "Please RELOAD scene file to reconstruct BVH!" << std::endl;

}

void MainWindow::on_objectbboxvisible_toggled(bool value)
{
    ui->mygl->setObjectBBoxVisible(value);
}
