#include "gamewindow.h"
#include "gamewindowserveur.h"
#include "gamewindowclient.h"
#include "Camera.h"
#include "mythread.h"

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

int main(int argc, char **argv)
{
    srand(time(NULL));
    QGuiApplication app(argc, argv);

    Camera* camera = new Camera();

    QSurfaceFormat format;
    format.setSamples(16);

    /**** Test threads ****/

    /** **/
    GameWindowServeur serveur_window(50,camera);
    serveur_window.setFormat(format);
    serveur_window.setTitle("Serveur");
    serveur_window.resize(640, 480);
    serveur_window.show();

    MyThread threadClient((*camera),"Client 1");
    threadClient.start();

    MyThread threadClient2((*camera),"Client 2");
    threadClient2.start();

    MyThread threadClient3((*camera),"Client 3");
    threadClient3.start();

    MyThread threadClient4((*camera),"Client 4");
    threadClient4.start();

    /** **/

    /************* Test saisons ***********/
    /** ** /
    GameWindow ete (50,camera,ETE);
    ete.setFormat(format);
    ete.setTitle("ete");
    ete.resize(640, 480);
    ete.show();

    GameWindow printemps(50,camera,PRINTEMPS);
    printemps.setFormat(format);
    printemps.setTitle("printemps");
    printemps.resize(640, 480);
    printemps.show();

    GameWindow hiver (50,camera,HIVER);
    hiver.setFormat(format);
    hiver.setTitle("hiver");
    hiver.resize(640, 480);
    hiver.show();

    GameWindow automne (50,camera,AUTOMNE);
    automne.setFormat(format);
    automne.setTitle("automne");
    automne.resize(640, 480);
    automne.show();

    /** **/

    return app.exec();
}



