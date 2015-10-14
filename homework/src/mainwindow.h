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
    QMenu *viewMenu;
    QAction *objectBBoxVisible;
    QAction *kdTreeBBoxVisible;
    QAction *meshKdTreeBBoxVisible;
    QAction *meshTriangleBBoxVisible;
    QMenu *samplerMenu;
    QActionGroup *selectSampler;
    QAction *uniformSampler;
    QAction *randomSampler;
    QAction *stratifiedSampler;
    QAction *iwsSampler;
    QAction *sceneSampler;
    QActionGroup *selectAALevel;
    QAction *aalevelOne;
    QAction *aalevelTwo;
    QAction *aalevelFour;
    QMenu *bvhMenu;
    QActionGroup *selectBVH;
    QAction *noBVH;
    QAction *splitEqualCounts;
    QAction *splitSAH;
};
