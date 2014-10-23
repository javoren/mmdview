
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

#if 0
    // モデルの回転を行う実験コード
    // TODO : 本来は不要なコードなので実験終わったら消す
    static int rot_r = 0;
    rot_r += 3;
    glRotatef(rot_r, 0.0f, 1.0f, 0.0f);
#else
    static int  motion_index = 0;
    VMD_Motion* motion_addr = vmdfile.vmd_motion;
    while(motion_index == motion_addr->FrameNumber){
        // モーションデータを取得して、表示するための情報に反映させる
        // 次のモーションへ
        motion_addr++;
    }
//    vmdfile.vmd_motion;
//    float       px,py,pz;               // 位置
//    Quaternion  rq;                     // 回転角(クォータニオン)
    motion_index++;
#endif

    // パーツごとに描画する
    mmdfile.draw();

    // ここまででMMDモデルの描画が完了
    // 行列スタックを元に戻す
    glPopMatrix();

    // アルファブレンディングの実験
    // この部分は演出部分の実装実験で使う。
    // TODO : 実験終了後に削除する
/*
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ZERO);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
*/

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

