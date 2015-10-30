#pragma once

#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>

#include <openGL/glwidget277.h>
#include <la.h>
#include <openGL/shaderprogram.h>
#include <scene/camera.h>
#include <scene/scene.h>
#include <la.h>
#include <scene/xmlreader.h>
#include <raytracing/integrator.h>
#include <scene/xmlreader.h>
#include <raytracing/integrator.h>

#include <raytracing/samplers/pixelsampler.h>
#include <raytracing/samplers/uniformpixelsampler.h>
#include <raytracing/samplers/randompixelsampler.h>
#include <raytracing/samplers/stratifiedpixelsampler.h>
#include <raytracing/samplers/imagewidestratifiedsampler.h>

#include <memory>


class MyGL
    : public GLWidget277
{
    Q_OBJECT
private:
    QOpenGLVertexArrayObject vao;

    ShaderProgram prog_lambert;
    ShaderProgram prog_flat;

    Camera gl_camera;//This is a camera we can move around the scene to view it from any angle.
                                //However, the camera defined in the config file is the one from which the scene will be rendered.
                                //If you move gl_camera, you will be able to see the viewing frustum of the scene's camera.

    Scene scene;
    XMLReader xml_reader;
    std::unique_ptr<Integrator> integrator;
    IntersectionEngine intersection_engine;

    bool kdtree_bbox_visible = true;
    bool object_bbox_visible = true;
    bool mesh_bbox_visible = true;
    bool triangle_bbox_visible = false;

    PixelSampler* current_sampler = nullptr;
    int current_sample_level = 4;

    // Samplers
    UniformPixelSampler uniform_sampler;
    StratifiedPixelSampler stratified_sampler;
    RandomPixelSampler random_sampler;
    ImageWideStratifiedSampler iw_stratified_sampler;



public:


    enum SamplerType{SAMPLER_UNIFORM, SAMPLER_RANDOM, SAMPLER_STRATIFIED, SAMPLER_IWS};
    enum AALevelType{AA_ONEONE, AA_TWOTWO, AA_FOURFOUR};

    explicit MyGL(QWidget *parent = 0);
    ~MyGL();

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    void SceneLoadDialog();
    void GLDrawScene();
    void ResizeToSceneCamera();

    void RaytraceScene();

    void setKDTreeBBoxVisible(bool value){this->kdtree_bbox_visible = value;}
    void setObjectBBoxVisible(bool value){this->object_bbox_visible = value;}
    void setMeshBBoxVisible(bool value){this->mesh_bbox_visible = value;}
    void setTriangleBBoxVisible(bool value);
    void setSampler(SamplerType samplertype);
    void setAALevel(AALevelType aaleveltype);


protected:
    void keyPressEvent(QKeyEvent *e);

signals:
    void sig_ResizeToCamera(int,int);
};
