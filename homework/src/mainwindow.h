#pragma once

#include <QMainWindow>
#include <QActionGroup>


namespace Ui
{
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionQuit_triggered();

    void on_actionLoad_Scene_triggered();
    void slot_ResizeToCamera(int,int);

    void on_actionRender_triggered();

    void on_actionCamera_Controls_triggered();

    void on_objectbboxvisible_toggled(bool);
    void on_kdtreebboxvisible_toggled(bool);
    void on_meshkdtreebboxvisible_toggled(bool);
    void on_meshtrianglebboxvisible_toggled(bool);

    void on_selectSampler_triggered(QAction*);
    void on_selectAALevel_triggered(QAction*);
    void on_selectBVH_triggered(QAction*);


private:
    Ui::MainWindow *ui;
    QMenu *viewMenu = nullptr;
    QAction *objectBBoxVisible = nullptr;
    QAction *kdTreeBBoxVisible = nullptr;
    QAction *meshKdTreeBBoxVisible = nullptr;
    QAction *meshTriangleBBoxVisible = nullptr;
    QMenu *samplerMenu = nullptr;
    QActionGroup *selectSampler = nullptr;
    QAction *uniformSampler = nullptr;
    QAction *randomSampler = nullptr;
    QAction *stratifiedSampler = nullptr;
    QAction *iwsSampler = nullptr;
    QAction *sceneSampler = nullptr;
    QActionGroup *selectAALevel = nullptr;
    QAction *aalevelOne = nullptr;
    QAction *aalevelTwo = nullptr;
    QAction *aalevelFour = nullptr;
    QMenu *bvhMenu = nullptr;
    QActionGroup *selectBVH = nullptr;
    QAction *noBVH = nullptr;
    QAction *splitEqualCounts = nullptr;
    QAction *splitSAH = nullptr;
};
