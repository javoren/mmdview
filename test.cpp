
#include <GL/gl.h>
#include <GL/glut.h>

#include <math.h>
#include <string>
#include <stdio.h>
#include <memory.h>

#include "mmd_model.h"
#include "mmd_motion.h"

#define REDRAW_DELAY    (33)


// とりあえずここにプロトタイプ書いておく

// とりあえず
MMD_File    mmdfile;
VMD_File    vmdfile;
Texture     madoka_magic;

int WindowWidth = 512;
int WindowHeight = 512;
int  motion_index = 0;


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

    // 視点, ビューポートの設定
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
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // テクスチャの有効化
    glEnable(GL_TEXTURE_2D);

    // Zバッファの有効化
    glEnable(GL_DEPTH_TEST);

    // モデル座標系の設定を開始することを示す
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // vmdモーションを進め、適用する
    vmdfile.setMmdMotion(&mmdfile, motion_index);
//    motion_index++;
    printf("motion frame number = %d\n", motion_index);

    // パーツごとに描画する
    mmdfile.draw();

    // ここまででMMDモデルの描画が完了
    // 行列スタックを元に戻す
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
//    mmdfile.setpath("/home/catalina/workspace/opengl/madoka/");
//    mmdfile.load("md_m.pmd");

    // GUMI
    mmdfile.setpath("/home/catalina/workspace/opengl/Model/");
    mmdfile.load("gumi.pmd");

    vmdfile.read("/home/catalina/workspace/opengl/madoka/koikito.vmd");
//    return 0;

    // ほむら
    // ※: 魔法少女服のテクスチャ形式がtgaのため、読み込み失敗する。
    // opencvの画像読み込み関数を使えば一発でいけそう
//    mmdfile.setpath("/home/catalina/workspace/opengl/homura/");
//    mmdfile.load("hm_m.pmd");

    glEnable( GL_NORMALIZE );
    glutMainLoop();

    return 0;
}

