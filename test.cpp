
#include <GL/gl.h>
#include <GL/glut.h>

#include <math.h>
#include <string>
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>

// 外部のライブラリ
// http://www.syuhitu.org/other/bmp/BmpIoLib_h.html
#include "BmpIoLib.h"


#define REDRAW_DELAY    (33)

// とりあえずここにプロトタイプ書いておく

int WindowWidth = 512;
int WindowHeight = 512;
const int   particle_num = 128;
IBMP*       pBmp;
GLuint      texture_id;


void init_vertex()
{
    // RGBAでカラーを設定する
    static const GLbyte triangleColors[] = {
        128,   128,   128, 255,
        128,   128,   128, 255,
        128,   128,   128, 255,
        128,   128,   128, 255,
    };
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, triangleColors);

    static const GLfloat texuv[] = {
        0.0, 1.0,
        1.0, 1.0,
        1.0, 0.0,
        0.0, 0.0,
    };
    glTexCoordPointer(2, GL_FLOAT, 0, texuv);

    static const GLfloat vtx[] = {
        -1.0, -1.0,  0.0,   // 左下
         1.0, -1.0,  0.0,   // 右下
         1.0,  1.0,  0.0,   // 右上
        -1.0,  1.0,  0.0,   // 左上
    };
    glVertexPointer(3, GL_FLOAT, 0, vtx);
}



class Perticle{
private:
    float   px,  py,  pz;       // 位置
    float   mx,  my,  mz;       // 移動速度
    float   mxa, mya, mza;      // 移動速度に対する加速度
    float   sx,  sy,  sz;
public:
    void init()
    {
        px = py = pz = 0;
        mx = rand() % 50;
        my = rand() % 50;
        mx *= 0.01f;
        my *= 0.01f;
        if(rand() & 1){
            mx *= -1;
        }
        if(rand() & 1){
            my *= -1;
        }
        mz = 0;
//        mxa = mx*0.05f;
//        mya = my*0.05f;
        mxa = 0;
        mya = 0;
        mza = 0;

        mxa *= -1;      // 加速度は速度の逆方向に働かせる
        mya *= -1;      // 加速度は速度の逆方向に働かせる
        float scale = (rand() % 100) * 0.01f;
        sx = sy = sz = scale + 0.1f;
    }

    Perticle()
    {
        init();
    }

    void draw()
    {
        // 移動速度に対して加速度を加える
        mx += mxa;
        my += mya;
        mz += mza;

        // 現在位置に対して速度を加え、新しい位置を求める
        px += mx;
        py += my;

        // 画面外にはみ出ていたら最初から
        if(px < -10.0f || py < -10.0f || px > 10.0f || py > 10.0f){
            init();
        }

        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

        glTranslatef(px, py, pz);
        glScalef(sx, sy, sz);
        glDrawArrays(GL_QUADS, 0, 4);
    }
};


Perticle    g_particle[particle_num];

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

    init_vertex();

    // 視点, ビューポートの設定
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)WindowWidth/(double)WindowHeight, 0.1, 1000.0);
    glViewport(0, 0, WindowWidth, WindowHeight);
    gluLookAt( 
        0.0, 0.0, -32.0,
        0.0, 0.0, 0.0,
        0.0, 1.0, 0.0 );

    // テクスチャの有効化
    glEnable(GL_TEXTURE_2D);

    // Zバッファの有効化
//    glEnable(GL_DEPTH_TEST);

    // モデル座標系の設定を開始することを示す
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // アルファブレンディングの実験
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glDisable(GL_ALPHA_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);  // 加算半透明

    // パーティクルの原点を設定する
    glTranslatef(0, 0, 0);

    // パーティクスルの描画を行う
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glBindTexture(GL_TEXTURE_2D, texture_id );
    for(int i = 0;i < particle_num; i++){
        glPushMatrix();
        g_particle[i].draw();
        glPopMatrix();
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

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

    // テクスチャの読みこみを行う
    const char* filename = "test.bmp";
    FILE* fp = fopen(filename, "rb");
    if(fp == NULL){
        printf("テクスチャを読み込めませんでした\n");
    }

    pBmp = BmpIO_Load(fp);
    if(pBmp == NULL){
        printf("テクスチャを読み込めませんでした\n");
    }

    printf("テクスチャの形式を変換する\n");
    // RGB形式からRGBA形式に変換する。
    int pixel_num = pBmp->width * pBmp->height;
    printf("bmp=(%d x %d)\n", pBmp->width, pBmp->height);
    unsigned char *pix = new unsigned char[ pixel_num * 4];
    unsigned char *rp = (unsigned char *)pBmp->pColor;  // ピクセルのデータ
    unsigned char *wp = pix;
    printf("wp = %p, rp = %p\n", wp, rp);
    for(int i = 0;i < pixel_num; i++){
        *wp++ = *(rp);
        *wp++ = *(rp+1);
        *wp++ = *(rp+2);
        float sum = *(rp) + *(rp+1) + *(rp+2);
        float avg = sum /3.0f;
        *wp++ = 255.0f - avg;
        rp+=3;
    }
    printf("変換後のテクスチャの形式\n");

    // テクスチャのリソースを確保する
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(
        GL_TEXTURE_2D ,0 ,GL_RGB ,pBmp->width ,pBmp->height,
        0 ,GL_RGBA ,GL_UNSIGNED_BYTE, pix );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glBindTexture(GL_TEXTURE_2D, 0);

    glEnable( GL_NORMALIZE );
    glutMainLoop();

    return 0;
}

