
#include <GL/gl.h>
#include <GL/glut.h>

#include <math.h>
#include <string>
#include <stdio.h>
#include <memory.h>

#include "mmd_model.h"

#define REDRAW_DELAY    (33)


// とりあえずここにプロトタイプ書いておく

// とりあえず
MMD_File    mmdfile;
Texture     madoka_magic;

int WindowWidth = 512;
int WindowHeight = 512;



void Reshape(int x, int y)
{
    WindowWidth = x;
    WindowHeight = y;
    if ( WindowWidth < 1 ) WindowWidth = 1;
    if ( WindowHeight < 1 ) WindowHeight = 1;
}


void disp(void)
{
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
    glPushMatrix();

    // mmd側のコードをここに再現しておき、
    // 問題点がどこにあるか確認する
    // --------ここから--------
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)WindowWidth/(double)WindowHeight, 0.1, 1000.0);
    glViewport(0, 0, WindowWidth, WindowHeight);

    gluLookAt( 
        0.0, 10.0, -32.0,
        0.0, 10.0, 0.0,
        0.0, 1.0, 0.0 );

    // 光源の設定
    const GLfloat lightPos[] = { 0 , 0 , -2 , 0 };
    const GLfloat lightCol[] = { 1 , 1 , 1 , 1 };
    const GLfloat ambient [] = { 0.3f, 0.3f, 0.3f, 1.0f};
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_POSITION , lightPos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE , lightCol);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    static int rot_r = 0;
    rot_r += 3;
//    glTranslatef(10.0f, 0.0f, -30.0f);        // 右手座標系。z+が画面手前方向
    glRotatef(rot_r, 0.0f, 1.0f, 0.0f);
    // -------- ここまで --------

    // パーツごとに描画する
    mmdfile.draw();

    glPopMatrix();
    glutSwapBuffers();
}

void timer(int value)
{
    glutPostRedisplay();
    glutTimerFunc(REDRAW_DELAY , timer , 0);
}


int main(int argc , char ** argv)
{
    glutInit(&argc , argv);
    glutInitWindowPosition(100 , 50);
    glutInitWindowSize(WindowWidth , WindowHeight);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

    glutCreateWindow("Window caption");
    glutDisplayFunc(disp);
    glutReshapeFunc(Reshape);
    glutTimerFunc(REDRAW_DELAY , timer , 0);

    // まどか
    mmdfile.setpath("/home/catalina/workspace/opengl/madoka/");
    mmdfile.load("md_m.pmd");


    glEnable( GL_NORMALIZE );
    glutMainLoop();

    return 0;
}

