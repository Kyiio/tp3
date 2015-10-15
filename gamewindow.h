#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include "openglwindow.h"
#include "Camera.h"

#define ETE 0
#define AUTOMNE 1
#define HIVER 2
#define PRINTEMPS 3


struct point
{
    float x, y ,z;
};

#define MAX_PARTICLES 10000

typedef struct
{
    float x;
    float y;
    float z;
} tParticle1;

class GameWindow : public OpenGLWindow
{
public:
    GameWindow(int fps, Camera* camera, int type);

    void initialize();
    void render();
    bool event(QEvent *event);

    void keyPressEvent(QKeyEvent *event);

    void displayTriangles();
    void displayLines();
    void displayTrianglesC();
    void displayPoints();
    void displayTrianglesTexture();

    void displayColor(float);
    void displayColorSeasons(float);

    void loadMap(QString localPath);

    void initall();
    void initentity(int index);
    void renderMeteo();
    void update();

protected:

    tParticle1 Particles[MAX_PARTICLES];
    int m_type;

    Camera* m_camera;
    int m_frame;
    QImage m_image;
    point *p;
    int carte=1;
    bool m_isRotating;
    QTimer * m_timer;
    int m_fps;

};


#endif // GAMEWINDOW_H
