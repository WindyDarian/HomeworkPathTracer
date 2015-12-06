#include "mygl.h"
#include <la.h>

#include <iostream>
#include <QApplication>
#include <QKeyEvent>
#include <QXmlStreamReader>
#include <QFileDialog>
#include <QElapsedTimer>
#include <QPainter>
#include <scene/geometry/mesh.h>
#include <ctime>
#include <raytracing/directlightingintegrator.h>
#include <QOpenGLTexture>


#include <iomanip>


float const screen_quad_pos[] = {
1.0f, -1.0f, 0.0f, 1.0f, 1.0f,
1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
-1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
-1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
};



MyGL::MyGL(QWidget *parent)
    : GLWidget277(parent),
      uniform_sampler(2),
      stratified_sampler(2,static_cast<unsigned int>(std::time(nullptr))),
      random_sampler(2, static_cast<unsigned int>(std::time(nullptr)) + 1 ),
      iw_stratified_sampler(2, static_cast<unsigned int>(std::time(nullptr)) + 2)
{
    setFocusPolicy(Qt::ClickFocus);

//    setAALevel(AA_TWOTWO);
//    setSampler(SAMPLER_UNIFORM);
}

MyGL::~MyGL()
{
    makeCurrent();

    vao.destroy();
}

void MyGL::initializeGL()
{
    // Create an OpenGL context
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Set a few settings/modes in OpenGL rendering
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    // Set the size with which points should be rendered
    glPointSize(5);
    // Set the color with which the screen is filled at the start of each render call.
    glClearColor(0.5, 0.5, 0.5, 1);

    printGLErrorLog();

    // Create a Vertex Attribute Object
    vao.create();

    // Create and set up the diffuse shader
    prog_lambert.create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    // Create and set up the flat-color shader
    prog_flat.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");

    this->progressive_render_program.addShaderFromSourceFile(QOpenGLShader::Vertex  , ":/glsl/renderview.vert.glsl");
    this->progressive_render_program.addShaderFromSourceFile(QOpenGLShader::Fragment,  ":/glsl/renderview.frag.glsl");
    this->progressive_render_program.link();


    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
    vao.bind();

    //Test scene data initialization
    scene.CreateTestScene();
    integrator.reset(new DirectLightingIntegrator());
    integrator->scene = &scene;
    integrator->intersection_engine = &intersection_engine;
    intersection_engine.scene = &scene;
    ResizeToSceneCamera();




    // create full screen quad for progressive rendering
    glGenBuffers(1, &progressive_position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, progressive_position_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screen_quad_pos), screen_quad_pos, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MyGL::resizeGL(int w, int h)
{
    gl_camera = Camera(w, h);

    glm::mat4 viewproj = gl_camera.getViewProj();

    // Upload the projection matrix
    prog_lambert.setViewProjMatrix(viewproj);
    prog_flat.setViewProjMatrix(viewproj);

    printGLErrorLog();
}

// This function is called by Qt any time your GL window is supposed to update
// For example, when the function updateGL is called, paintGL is called implicitly.
void MyGL::paintGL()
{
    // Clear the screen so that we only see newly drawn images
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    if (!is_rendering)
    {

        // Update the viewproj matrix
        prog_lambert.setViewProjMatrix(gl_camera.getViewProj());
        prog_flat.setViewProjMatrix(gl_camera.getViewProj());

        GLDrawScene();
    }
    else
    {


        // if rendering, draw progressive rendering scene;
        progressive_render_program.bind();

        QOpenGLTexture *texture = new QOpenGLTexture(QImage(progressive_scene));
        texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
        texture->setMagnificationFilter(QOpenGLTexture::Linear);

        // the initial texture is from the opengl frame buffer before start rendering
        texture->bind();


        glBindBuffer(GL_ARRAY_BUFFER, progressive_position_buffer);

        int vertexLocation = progressive_render_program.attributeLocation("position");
        progressive_render_program.enableAttributeArray(vertexLocation);
        int vertexTextureCoord = progressive_render_program.attributeLocation("texcoord");
        progressive_render_program.enableAttributeArray(vertexTextureCoord);

        glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), 0);
        glVertexAttribPointer(vertexTextureCoord, 2, GL_FLOAT, GL_TRUE, 5*sizeof(GLfloat), (const GLvoid*)(3 * sizeof(GLfloat)));

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        texture->destroy();
        delete texture;

    }
}

void MyGL::GLDrawScene()
{
    for(Geometry *g : scene.objects)
    {
        if(g->drawMode() == GL_TRIANGLES)
        {
            prog_lambert.setModelMatrix(g->transform.T());
            prog_lambert.draw(*this, *g);
        }
        else if(g->drawMode() == GL_LINES)
        {
            prog_flat.setModelMatrix(g->transform.T());
            prog_flat.draw(*this, *g);
        }

        if (this->mesh_bbox_visible)
        {

            if (auto mesh = dynamic_cast<Mesh*>(g))
            {

                prog_flat.setModelMatrix(g->transform.T());
                auto bboxes = mesh->getVisibleBoundingBoxesBVH();
                for(BoundingBoxFrame* b: bboxes)
                {
                    prog_flat.draw(*this, *b);
                }
            }
        }
        if (this->triangle_bbox_visible)
        {

            if (auto mesh = dynamic_cast<Mesh*>(g))
            {

                prog_flat.setModelMatrix(g->transform.T());
                auto bboxes = mesh->getVisibleBoundingBoxesTriangle();
                for(BoundingBoxFrame* b: bboxes)
                {
                    prog_flat.draw(*this, *b);
                }
            }
        }

    }
    for(Geometry *l : scene.lights)
    {
        prog_flat.setModelMatrix(l->transform.T());
        prog_flat.draw(*this, *l);
    }
    if (this->object_bbox_visible)
    {
        for (auto b :scene.boundingbox_objects)
        {

            prog_flat.setModelMatrix(glm::mat4(1.0f));
            prog_flat.draw(*this, *b);

        }
    }
    if (this->kdtree_bbox_visible)
    {
        for (auto b :scene.boundingbox_bvh)
        {
            prog_flat.setModelMatrix(glm::mat4(1.0f));
            prog_flat.draw(*this, *b);
        }
    }

    prog_flat.setModelMatrix(glm::mat4(1.0f));
    prog_flat.draw(*this, scene.camera);
}

void MyGL::ResizeToSceneCamera()
{
    this->setFixedWidth(scene.camera.width);
    this->setFixedHeight(scene.camera.height);
    gl_camera = Camera(scene.camera);
}

void MyGL::keyPressEvent(QKeyEvent *e)
{
    float amount = 2.0f;
    if(e->modifiers() & Qt::ShiftModifier){
        amount = 10.0f;
    }
    // http://doc.qt.io/qt-5/qt.html#Key-enum
    if (e->key() == Qt::Key_Escape) {
        QApplication::quit();
    } else if (e->key() == Qt::Key_Right) {
        gl_camera.RotateAboutUp(-amount);
    } else if (e->key() == Qt::Key_Left) {
        gl_camera.RotateAboutUp(amount);
    } else if (e->key() == Qt::Key_Up) {
        gl_camera.RotateAboutRight(-amount);
    } else if (e->key() == Qt::Key_Down) {
        gl_camera.RotateAboutRight(amount);
    } else if (e->key() == Qt::Key_1) {
        gl_camera.fovy += amount;
    } else if (e->key() == Qt::Key_2) {
        gl_camera.fovy -= amount;
    } else if (e->key() == Qt::Key_W) {
        gl_camera.TranslateAlongLook(amount);
    } else if (e->key() == Qt::Key_S) {
        gl_camera.TranslateAlongLook(-amount);
    } else if (e->key() == Qt::Key_D) {
        gl_camera.TranslateAlongRight(amount);
    } else if (e->key() == Qt::Key_A) {
        gl_camera.TranslateAlongRight(-amount);
    } else if (e->key() == Qt::Key_Q) {
        gl_camera.TranslateAlongUp(-amount);
    } else if (e->key() == Qt::Key_E) {
        gl_camera.TranslateAlongUp(amount);
    } else if (e->key() == Qt::Key_F) {
        gl_camera.CopyAttributes(scene.camera);
    } else if (e->key() == Qt::Key_R) {
        scene.camera = Camera(gl_camera);
        scene.camera.recreate();
    }
    gl_camera.RecomputeAttributes();
    update();  // Calls paintGL, among other things
}



void MyGL::SceneLoadDialog()
{
    QString filepath = QFileDialog::getOpenFileName(0, QString("Load Scene"), QString("../scene_files"), tr("*.xml"));
    if(filepath.length() == 0)
    {
        return;
    }

    QFile file(filepath);
    int i = filepath.length() - 1;
    while(QString::compare(filepath.at(i), QChar('/')) != 0)
    {
        i--;
    }
    QStringRef local_path = filepath.leftRef(i+1);
    //Reset all of our objects
    scene.Clear();

    intersection_engine = IntersectionEngine();
    //Load new objects based on the XML file chosen.
    Integrator* new_integrator = nullptr;
    xml_reader.LoadSceneFromFile(file, local_path, scene, new_integrator);

    integrator.reset(new_integrator);
    integrator->scene = &scene;
    integrator->intersection_engine = &intersection_engine;
    intersection_engine.scene = &scene;

    scene.recomputeBVH();


    if(triangle_bbox_visible)
    {
        for(Geometry *g : scene.objects)
        {
            if (auto mesh = dynamic_cast<Mesh*>(g))
            {
                mesh->createVisibleBoundingBoxes();
            }
        }
    }

    ResizeToSceneCamera();
    update();
}

//inline void _renderpixel_normal(int x, int y, Scene& scene, IntersectionEngine& intersection_engine)
//{
//    Ray r(scene.camera.Raycast(static_cast<float>(x), static_cast<float>(y)));

//    Intersection intersection(intersection_engine.GetIntersection(r));

//    if(intersection.object_hit == NULL || intersection.object_hit == nullptr)
//    {
//        scene.film.pixels[x][y] = glm::vec3(0.f);
//    }
//    else
//    {
//        scene.film.pixels[x][y] = glm::abs(intersection.normal);
//        //scene.film.pixels[x][y] = intersection.normal;
//    }
//}

//inline void _renderpixel(int x, int y, Scene& scene, Integrator& integrator, PixelSampler* sampler = nullptr)
//{
//    if (sampler == nullptr)
//    {
//        Ray r(scene.camera.Raycast(static_cast<float>(x), static_cast<float>(y)));
//        scene.film.pixels[x][y] = integrator.TraceRay(r, 0);
//    }
//    else
//    {
//        auto samples = sampler->GetSamples(x, y);
//        glm::vec3 color_total(0.f);

//        for (glm::vec2 sample : samples)
//        {
//            Ray r = scene.camera.Raycast(sample.x, sample.y);
//            color_total += integrator.TraceRay(r, 0);
//        }

//        scene.film.pixels[x][y] = color_total / static_cast<float>(samples.size());
//    }
//}

/* old
void MyGL::RaytraceScene()
{
    QString filepath = QFileDialog::getSaveFileName(0, QString("Save Image"), QString("../rendered_images"), tr("*.bmp"));
    if(filepath.length() == 0)
    {
        return;
    }



    PixelSampler* sampler = this->current_sampler;

    sampler->initialize(scene.camera.width, scene.camera.height);
    //iw_stratified_sampler.recalculateSamples(scene.camera.width, scene.camera.height);


    std::cout<< "Render started:" << std::endl;
    QElapsedTimer render_timer;
    render_timer.start();


    //#define TBB //Uncomment this line out to render your scene with multiple threads.
    //This is useful when debugging your raytracer with breakpoints.
    #ifdef TBB
        parallel_for(0, (int)scene.camera.width, 1, [=](unsigned int i)
        {
            for(unsigned int j = 0; j < scene.camera.height; j++)
            {
                //_renderpixel_normal(i,j,scene,intersection_engine);
                _renderpixel(i, j, this->scene, this->integrator, sampler);
            }
        });
    #else

        for(unsigned int i = 0; i < scene.camera.width; i++)
        {
            for(unsigned int j = 0; j < scene.camera.height; j++)
            {
                //_renderpixel_normal(i,j,scene,intersection_engine);
                //_renderpixel(i,j,this->scene,this->integrator);

                //_renderpixel(i, j, this->scene, this->integrator, &stratified_sampler);
                //_renderpixel(i, j, this->scene, this->integrator, &random_sampler);
                //_renderpixel(i, j, this->scene, this->integrator, &simple_sampler);
                //_renderpixel(i, j, this->scene, this->integrator, &iw_stratified_sampler);

                _renderpixel(i, j, this->scene, this->integrator, sampler);

            }
        }
    #endif

    int render_time = render_timer.elapsed();
    std::cout << "Render completed. Total time: "
              << std::setprecision(9) << render_time/1000.0
              << " seconds. (" << render_time << " millseconds.)"
              << std::endl;

    scene.film.WriteImage(filepath);
}
*/

void MyGL::RaytraceScene()
{


    if (this->is_rendering)
    {
        std::cout << "Render Interrupted!" << std::endl;
        this->terminateRenderThreads();
    }

    output_file_path = QFileDialog::getSaveFileName(0, QString("Save Image"), QString("../rendered_images"), tr("*.bmp"));


    if(output_file_path.length() == 0)
    {
        return;
    }

    //test
    QImage p = this->grabFramebuffer();
    progressive_scene = p.scaled(this->scene.camera.width, this->scene.camera.height);
    this->is_rendering = true;


    std::cout<< "Render started:" << std::endl;

    render_timer.start();

    //Set up 16 (max) threads
    unsigned int width = scene.camera.width;
    unsigned int height = scene.camera.height;
    unsigned int x_block_size = (width >= 4 ? width/4 : 1);
    unsigned int y_block_size = (height >= 4 ? height/4 : 1);
    unsigned int x_block_count = width > 4 ? width/x_block_size : 1;
    unsigned int y_block_count = height > 4 ? height/y_block_size : 1;
    if(x_block_count * x_block_size < width) x_block_count++;
    if(y_block_count * y_block_size < height) y_block_count++;

    unsigned int num_render_threads = x_block_count * y_block_count;
    //RenderThread **render_threads = new RenderThread*[num_render_threads];
    render_threads.clear();
    render_threads.reserve(num_render_threads);
    for(int i = 0; i < num_render_threads; i++)
    {
        render_threads.push_back(nullptr);
    }

    //Launch the render threads we've made
    for(unsigned int Y = 0; Y < y_block_count; Y++)
    {
        //Compute the columns of the image that the thread should render
        unsigned int y_start = Y * y_block_size;
        unsigned int y_end = glm::min((Y + 1) * y_block_size, height);
        for(unsigned int X = 0; X < x_block_count; X++)
        {
            //Compute the rows of the image that the thread should render
            unsigned int x_start = X * x_block_size;
            unsigned int x_end = glm::min((X + 1) * x_block_size, width);
            //Create and run the thread
            render_threads[Y * x_block_count + X].reset(new RenderThread(x_start, x_end, y_start, y_end, scene.sqrt_samples, 5, &(scene.film), &(scene.camera), integrator.get()));
            render_threads[Y * x_block_count + X]->preview_image = &this->progressive_scene;
            render_threads[Y * x_block_count + X]->start();
        }
    }

    QTimer::singleShot(500, this, SLOT(on_RenderUpdate()));

}

void MyGL::on_RenderUpdate()
{
    this->renderUpdate();
}

void MyGL::renderUpdate()
{
    if (!this->is_rendering)
        return;

    update();
    bool still_running;

    still_running = false;
    for(int i = 0; i < render_threads.size(); i++)
    {
        if(render_threads[i]->isRunning())
        {
            still_running = true;
            QTimer::singleShot(500, this, SLOT(on_RenderUpdate()));
            return;
        }
    }
//        if(still_running)
//        {
//            //Free the CPU to let the remaining render threads use it
//            QThread::yieldCurrentThread();
//        }

    if (!still_running)
    {
        this->completeRender();
    }

}

void MyGL::completeRender()
{

    //Finally, clean up the render thread objects
//    for(unsigned int i = 0; i < num_render_threads; i++)
//    {
//        delete render_threads[i];
//    }
//    delete [] render_threads;

    this->terminateRenderThreads();


    int render_time = render_timer.elapsed();
    std::cout << "Render completed. Total time: "
              << std::setprecision(9) << render_time/1000.0
              << " seconds. (" << render_time << " millseconds.)"
              << std::endl;



    scene.film.WriteImage(output_file_path);
    this->is_rendering = false;
}

void MyGL::terminateRenderThreads()
{
    for (int i = 0; i< render_threads.size(); i++)
    {
        if (render_threads[i]->isRunning())
        {
            render_threads[i]->terminate(); //FIXME: UNSAFE
            render_threads[i]->wait();
        }
    }
    this->render_threads.clear();
}




void MyGL::setTriangleBBoxVisible(bool value)
{
    this->triangle_bbox_visible = value;
    for(Geometry *g : scene.objects)
    {
        if (auto mesh = dynamic_cast<Mesh*>(g))
        {
            mesh->createVisibleBoundingBoxes();
        }
    }
}

void MyGL::setSampler(MyGL::SamplerType samplertype)
{
    switch (samplertype)
    {
    case SAMPLER_UNIFORM:
        this->current_sampler = & this->uniform_sampler;
        std::cout<< "Sampler changed to Uniform."<<std::endl;
        break;

    case SAMPLER_RANDOM:
        this->current_sampler = & this->random_sampler;
        std::cout<< "Sampler changed to Random."<<std::endl;
        break;

    case SAMPLER_STRATIFIED:
        this->current_sampler = & this->stratified_sampler;
        std::cout<< "Sampler changed to Stratified."<<std::endl;
        break;

    case SAMPLER_IWS:
        this->current_sampler = & this->iw_stratified_sampler;
        std::cout<< "Sampler changed to Image-Wide Stratified."<<std::endl;
        break;

    default:
        this->current_sampler = & this->uniform_sampler;
        std::cout<< "Sampler changed to Uniform."<<std::endl;
        break;

    }

    if (this->current_sampler)
    {
        this->current_sampler->SetSampleCount(this->current_sample_level);
    }
}

void MyGL::setAALevel(MyGL::AALevelType aaleveltype)
{
    switch (aaleveltype)
    {
    case AA_ONEONE:
        this->current_sample_level = 1;
        break;
    case AA_TWOTWO:
        this->current_sample_level = 2;
        break;
    case AA_FOURFOUR:
        this->current_sample_level = 4;
        break;
    }
    if (this->current_sampler)
    {
        this->current_sampler->SetSampleCount(this->current_sample_level);
        std::cout<< "Sampling Level changed to "
                 << this->current_sample_level <<  " x "
                 << this->current_sample_level <<std::endl;
    }
}
