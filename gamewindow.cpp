#include "gamewindow.h"

#include <QtGui/QGuiApplication>
#include <QtGui/QMatrix4x4>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QScreen>

#include <QtCore/qmath.h>
#include <QMouseEvent>
#include <QKeyEvent>
#include <time.h>
#include <sys/time.h>
#include <iostream>

#include <QtCore>
#include <QtGui>

using namespace std;

GameWindow::GameWindow(int fps, Camera* camera, int type)
{
    m_type = type;
    m_fps = fps;
    m_isRotating = false;
    m_timer = new QTimer(this);
    m_timer->connect(m_timer, SIGNAL(timeout()),this, SLOT(renderNow()));
    m_timer->start(1000/fps);
    m_camera = camera;
}

void GameWindow::initialize()
{
    const qreal retinaScale = devicePixelRatio();

    glViewport(0, 0, width() * retinaScale, height() * retinaScale);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, -100.0, 100.0);

    loadMap(":/heightmap-2.png");

    if(m_type == HIVER || m_type == AUTOMNE)
        initall();

}

void GameWindow::loadMap(QString localPath)
{

    if (QFile::exists(localPath)) {
        m_image = QImage(localPath);
    }

    uint id = 0;
    p = new point[m_image.width() * m_image.height()];
    QRgb pixel;
    for(int i = 0; i < m_image.width(); i++)
    {
        for(int j = 0; j < m_image.height(); j++)
        {

            pixel = m_image.pixel(i,j);

            id = i*m_image.width() +j;

            p[id].x = (float)i/(m_image.width()) - ((float)m_image.width()/2.0)/m_image.width();
            p[id].y = (float)j/(m_image.height()) - ((float)m_image.height()/2.0)/m_image.height();
            p[id].z = 0.001f * (float)(qRed(pixel));
        }
    }
}

void GameWindow::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(m_isRotating)
        m_camera->set_rotY(m_camera->get_rotY() + 1.0f);

    glLoadIdentity();
    m_camera->scale();
    glRotatef(m_camera->get_rotX(),1.0f,0.0f,0.0f);
    glRotatef(m_camera->get_rotY(),0.0f,0.0f,1.0f);

    if(m_type == HIVER || m_type == AUTOMNE){
        renderMeteo();
        update();
    }

    switch(m_camera->get_etat())
    {
    case 0:
        displayPoints();
        break;
    case 1:
        displayLines();
        break;
    case 2:
        displayTriangles();
        break;
    case 3:
        displayTrianglesC();
        break;
    case 4:
        displayTrianglesTexture();
        break;
    case 5:

        displayTrianglesTexture();
        displayLines();
        break;
    default:
        displayPoints();
        break;
    }


    ++m_frame;
}

bool GameWindow::event(QEvent *event)
{
    switch (event->type())
    {
    case QEvent::UpdateRequest:

        renderNow();
        return true;
    default:
        return QWindow::event(event);
    }
}

void GameWindow::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
    case 'C':
    {
        m_isRotating = !m_isRotating;
        break;
    }
    case 'P':
    {
        if(m_fps < 1000){
            m_timer->stop();
            m_fps *= 2;
            m_timer->start(1000/m_fps);
        }
        break;
    }
    case 'M':
    {
        if(m_fps >= 2){
            m_timer->stop();
            m_fps /= 2;
            m_timer->start(1000/m_fps);
        }
        break;
    }
    case 'Z':
        m_camera->set_ss(m_camera->get_ss()+0.10f);
        break;
    case 'S':
        m_camera->set_ss(m_camera->get_ss()-0.10f);
        break;
    case 'A':
        m_camera->set_rotX(m_camera->get_rotX()+1.0f);
        break;
    case 'E':
        m_camera->set_rotX(m_camera->get_rotX()-1.0f);
        break;
    case 'Q':
        m_camera->set_rotY(m_camera->get_rotY()+1.0f);
        break;
    case 'D':
        m_camera->set_rotY(m_camera->get_rotY()-1.0f);
        break;
    case 'W':
        m_camera->set_etat(m_camera->get_etat()+1);
        if(m_camera->get_etat() > 5)
            m_camera->set_etat(0);
        break;
    case 'X':
        carte ++;
        if(carte > 3)
            carte = 1;
        QString depth (":/heightmap-");
        depth += QString::number(carte) ;
        depth += ".png" ;

        loadMap(depth);
        break;

    }
    //renderNow();
}


void GameWindow::initall()
{

    srand(time(NULL));
    for(int i = 0; i < MAX_PARTICLES; i++)
    {
        Particles[i].x = (-0.5f) + ((float)(rand())/ (float)(RAND_MAX));
        Particles[i].y = (-0.5f) + ((float)(rand())/ (float)(RAND_MAX));
        Particles[i].z = ((float)(rand())/ (float)(RAND_MAX));
    }
}

void GameWindow::initentity(int index)
{
    Particles[index].x = (-0.5f) + ((float)(rand())/ (float)(RAND_MAX));
    Particles[index].y = (-0.5f) + ((float)(rand())/ (float)(RAND_MAX));
    Particles[index].z = ((float)(rand())/ (float)(RAND_MAX));
}

void GameWindow::renderMeteo()
{
    glBegin(GL_POINTS);

    if(m_type == HIVER)
        glColor3f(1.0f,1.0f,1.0f);
    else//automne
        glColor3f(44.0f/255.0f,117.0f/255.0f,1.0f);

    for(int i = 0; i < MAX_PARTICLES; i++)
    {
        glVertex3f(Particles[i].x, Particles[i].y, Particles[i].z);
    }
    glEnd();
}

void GameWindow::update()
{
    for(int i = 0; i < MAX_PARTICLES; i++)
    {

       if(m_type == AUTOMNE)
            Particles[i].z -= ((float)(rand())/(float)(RAND_MAX)) * 0.15f;
       else{//hiver
            Particles[i].z -= ((float)(rand())/(float)(RAND_MAX))* 0.025f;
            //Particles[i].x += ((float)(rand())/(float)(RAND_MAX));
            //Particles[i].y += ((float)(rand())/(float)(RAND_MAX));
        }

        if(Particles[i].z < 0)
        {
            initentity(i);
        }
    }
}


void GameWindow::displayPoints()
{
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_POINTS);
    uint id = 0;
    for(int i = 0; i < m_image.width(); i++)
    {
        for(int j = 0; j < m_image.height(); j++)
        {
            displayColorSeasons(p[id].z);
            id = i*m_image.width() +j;
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);

        }
    }
    glEnd();
}

void GameWindow::displayTriangles()
{
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_TRIANGLES);
    uint id = 0;

    for(int i = 0; i < m_image.width()-1; i++)
    {
        for(int j = 0; j < m_image.height()-1; j++)
        {

            id = i*m_image.width() +j;
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);
            id = i*m_image.width() +(j+1);
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);
            id = (i+1)*m_image.width() +j;
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);



            id = i*m_image.width() +(j+1);
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);
            id = (i+1)*m_image.width() +j+1;
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);
            id = (i+1)*m_image.width() +j;
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);
        }
    }

    glEnd();
}

void GameWindow::displayTrianglesC()
{
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_TRIANGLES);
    uint id = 0;

    for(int i = 0; i < m_image.width()-1; i++)
    {
        for(int j = 0; j < m_image.height()-1; j++)
        {
            glColor3f(0.0f, 1.0f, 0.0f);
            id = i*m_image.width() +j;
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);
            id = i*m_image.width() +(j+1);
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);
            id = (i+1)*m_image.width() +j;
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);


            glColor3f(1.0f, 1.0f, 1.0f);
            id = i*m_image.width() +(j+1);
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);
            id = (i+1)*m_image.width() +j+1;
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);
            id = (i+1)*m_image.width() +j;
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);
        }
    }
    glEnd();
}


void GameWindow::displayLines()
{
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_LINES);
    uint id = 0;

    for(int i = 0; i < m_image.width()-1; i++)
    {
        for(int j = 0; j < m_image.height()-1; j++)
        {

            id = i*m_image.width() +j;
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);
            id = i*m_image.width() +(j+1);
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);

            id = (i+1)*m_image.width() +j;
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);
            id = i*m_image.width() +j;
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);

            id = (i+1)*m_image.width() +j;
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);
            id = i*m_image.width() +(j+1);
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);

            id = i*m_image.width() +(j+1);
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);
            id = (i+1)*m_image.width() +j+1;
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);

            id = (i+1)*m_image.width() +j+1;
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);

            id = (i+1)*m_image.width() +(j);
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);
        }
    }

    glEnd();
}

void GameWindow::displayTrianglesTexture()
{
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_TRIANGLES);
    uint id = 0;

    for(int i = 0; i < m_image.width()-1; i++)
    {
        for(int j = 0; j < m_image.height()-1; j++)
        {

            id = i*m_image.width() +j;
            //displayColor(p[id].z);
            displayColorSeasons(p[id].z);

            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);
            id = i*m_image.width() +(j+1);
            //displayColor(p[id].z);
            displayColorSeasons(p[id].z);
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);
            id = (i+1)*m_image.width() +j;
            //displayColor(p[id].z);
            displayColorSeasons(p[id].z);
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);



            id = i*m_image.width() +(j+1);
            //displayColor(p[id].z);
            displayColorSeasons(p[id].z);
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);
            id = (i+1)*m_image.width() +j+1;
            //displayColor(p[id].z);
            displayColorSeasons(p[id].z);
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);
            id = (i+1)*m_image.width() +j;
            //displayColor(p[id].z);
            displayColorSeasons(p[id].z);
            glVertex3f(
                        p[id].x,
                        p[id].y,
                        p[id].z);
        }
    }
    glEnd();
}


void GameWindow::displayColor(float alt)
{

    float R;
    float G;
    float B;

    if(alt > 0.15f){//blanc
       R = 1.0f;
       G = 1.0f;
       B = 1.0f;
    }
    else if (alt > 0.08f){//marron
        R = 88.0f/255.0f;
        G = 41.0f/255.0f;
        B = 0.0f;
    }
    else if (alt > 0.04f){//vert
        R = 20.0f/255.0f;
        G = 148.0f/255.0f;
        B = 20.0f/255.0f;
    }
    else if (alt > 0.030f){//jaune
        R = 255.0f/255.0f;
        G = 222.0f/255.0f;
        B = 117.0f/255.0f;
    }
    else if (alt > 0.015f){//bleu clair
        R = 44.0f/255.0f;
        G = 117.0f/255.0f;
        B = 1.0f;
    }
    else{//bleu foncé
        R = 15.0f/255.0f;
        G = 5.0f/255.0f;
        B = 107.0f/255.0f;
    }

    glColor3f(R, G, B);
 /*
    if (alt > 0.2)
    {
        glColor3f(01.0f, 1.0f, 1.0f);
    }
    else     if (alt > 0.1)
    {
        glColor3f(alt, 1.0f, 1.0f);
    }
    else     if (alt > 0.05f)
    {
        glColor3f(01.0f, alt, alt);
    }
    else
    {
        glColor3f(0.0f, 0.0f, 1.0f);
    }
*/
}

void GameWindow::displayColorSeasons(float alt)
{
    float R;
    float G;
    float B;

    if(m_type == ETE){

        if (alt > 0.08f){//marron
            R = 143.0f/255.0f;
            G = 89.0f/255.0f;
            B = 34.0f/255.0f;
        }
        else if (alt > 0.04f){//vert
            R = 20.0f/255.0f;
            G = 148.0f/255.0f;
            B = 20.0f/255.0f;
        }
        else if (alt > 0.020f){//jaune
            R = 255.0f/255.0f;
            G = 222.0f/255.0f;
            B = 117.0f/255.0f;
        }
        else {//bleu clair
            R = 44.0f/255.0f;
            G = 117.0f/255.0f;
            B = 1.0f;
        }
    }
    else if(m_type == AUTOMNE){

        if (alt > 0.08f){//marron
            R = 88.0f/255.0f;
            G = 41.0f/255.0f;
            B = 0.0f;
        }
        else if (alt > 0.04f){//vert
            R = 20.0f/255.0f;
            G = 148.0f/255.0f;
            B = 20.0f/255.0f;
        }
        else if (alt > 0.030f){//orange
            R = 223.0f/255.0f;
            G = 120.0f/255.0f;
            B = 20.0f/255.0f;
        }
        else if (alt > 0.025f){//bleu clair
            R = 44.0f/255.0f;
            G = 117.0f/255.0f;
            B = 1.0f;
        }
        else{//bleu foncé
            R = 15.0f/255.0f;
            G = 5.0f/255.0f;
            B = 107.0f/255.0f;
        }
    }
    else if(m_type == HIVER){

        if(alt > 0.15f){//blanc
           R = 1.0f;
           G = 1.0f;
           B = 1.0f;
        }
        else if (alt > 0.08f){//marron
            R = 88.0f/255.0f;
            G = 41.0f/255.0f;
            B = 0.0f;
        }
        else if (alt > 0.04f){//blanc
            R = 1.0f;
            G = 1.0f;
            B = 1.0f;
        }
        else if (alt > 0.015f){//bleu clair
            R = 44.0f/255.0f;
            G = 117.0f/255.0f;
            B = 1.0f;
        }
        else{//bleu foncé
            R = 15.0f/255.0f;
            G = 5.0f/255.0f;
            B = 107.0f/255.0f;
        }
    }
    else{//PRINTEMPS

        if(alt > 0.2f){//blanc
           R = 1.0f;
           G = 1.0f;
           B = 1.0f;
        }
        else if (alt > 0.08f){//marron
            R = 88.0f/255.0f;
            G = 41.0f/255.0f;
            B = 0.0f;
        }
        else if (alt > 0.03f){//vert
            R = 20.0f/255.0f;
            G = 148.0f/255.0f;
            B = 20.0f/255.0f;
        }
        else if (alt > 0.015f){//bleu clair
            R = 44.0f/255.0f;
            G = 117.0f/255.0f;
            B = 1.0f;
        }
        else{//bleu foncé
            R = 15.0f/255.0f;
            G = 5.0f/255.0f;
            B = 107.0f/255.0f;
        }
    }

    glColor3f(R, G, B);

}
