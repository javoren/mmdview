
#include <GL/gl.h>
#include <GL/glut.h>

#include <math.h>
#include <string>
#include <stdio.h>
#include <memory.h>

#include "mmd_model.h"


// とりあえずここにプロトタイプ書いておく

// とりあえず
MMD_File    mmdfile;
Texture     madoka_magic;

int WindowWidth = 512;
int WindowHeight = 512;

#if 0


void Reshape(int x, int y)
{
    WindowWidth = x;
    WindowHeight = y;
    if ( WindowWidth < 1 ) WindowWidth = 1;
    if ( WindowHeight < 1 ) WindowHeight = 1;
}


void disp(void)
{
    // プリミティブの描画を行えることの確認コード
    if(0){
        const GLfloat vertex[] = {
            -0.9 , 0.9 , 0.9 , 0.9 , 0 , -0.9
        };

        const GLuint indices[] = { 0 , 1 , 2 , 3, 4, 5};

        {
            glClear(GL_COLOR_BUFFER_BIT);

            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(2 , GL_FLOAT , 0 , vertex);

            glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, indices);
            glFlush();
        }
        return;
    }


//    glClearColor(1 , 1 , 1 , 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)WindowWidth/(double)WindowHeight, 0.1, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(0, 0, WindowWidth, WindowHeight);

    //　視点の設定
    gluLookAt( 
        0.0, 10.0, -32.0,
        0.0, 10.0, 0.0,
        0.0, 1.0, 0.0 );
#if 0
    gluLookAt( 
        0.0, 10.0, -32.0,
        0.0, 10.0, 0.0,
        0.0, 1.0, 0.0 );
#endif

    printf("draw start\n");

    // 光源の設定
    const GLfloat lightPos[] = { 3 , 0 , -2 , 0 };
    const GLfloat lightCol[] = { 1 , 1 , 1 , 1 };
    const GLfloat ambient [] = { 0.0f, 0.0f, 0.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_POSITION , lightPos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE , lightCol);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);

    glPushMatrix();
    glTranslated( 0, 0, 1);//平行移動値の設定(openGL座標系では画面手前がZ++の方向)
    glPushMatrix();

    // パーツごとに描画する
    mmdfile.draw();

    glPopMatrix();
    glPopMatrix();

    glutSwapBuffers();
}

void timer(int value)
{
    disp();
}


int main(int argc , char ** argv)
{
    glutInit(&argc , argv);
    glutInitWindowPosition(100 , 50);
    glutInitWindowSize(WindowWidth , WindowHeight);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);


    glutCreateWindow("Window caption");
    glutDisplayFunc(disp);
    glutReshapeFunc(Reshape);
    glEnable(GL_TEXTURE_2D);
//    glDisable(GL_CULL_FACE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glutTimerFunc(100 , timer , 0);

    mmdfile.setpath("/home/catalina/workspace/opengl/madoka/");
    mmdfile.load("md_m.pmd");

    glutMainLoop();

    return 0;
}
#else

const GLfloat vertex[] = {
    3.1 , 1.9 , -3 ,
    4.5 , 0.1 , -3 ,
    6 , 1.9 , -3 ,
    0.1 , 1.9 , -2 ,
    1.5 , 0.1 , -2 ,
    3 , 1.9 , -2 
};
const GLfloat color1[] = {
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
};
const GLfloat color2[] = {
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
};

const GLuint indices[] = {
    0, 1, 2, 3, 4, 5
};

void disp( void ) {
    glClear(GL_COLOR_BUFFER_BIT);
    glEnableClientState(GL_VERTEX_ARRAY);
//    glEnableClientState(GL_COLOR_ARRAY);
    glVertexPointer(3 , GL_FLOAT , 0 , vertex);
//    glColorPointer(3 , GL_FLOAT , 0 , color1);

    // mmd側のコードをここに再現しておき、
    // 問題点がどこにあるか確認する
    // --------ここから--------
//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    gluPerspective(45.0, (double)WindowWidth/(double)WindowHeight, 0.1, 1000.0);
//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();
//    glViewport(0, 0, WindowWidth, WindowHeight);

    //　視点の設定
//    gluLookAt( 
//        0.0, 10.0, -32.0,
//        0.0, 10.0, 0.0,
//        0.0, 1.0, 0.0 );

    // 光源の設定
    const GLfloat lightPos[] = { 3 , 0 , -2 , 0 };
    const GLfloat lightCol[] = { 1 , 1 , 1 , 1 };
    const GLfloat ambient [] = { 0.0f, 0.0f, 0.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_POSITION , lightPos);
    glLightfv(GL_LIGHT0, GL_DIFFUSE , lightCol);
//    glEnable(GL_TEXTURE_2D);
//    glEnable(GL_LIGHTING);
//    glEnable(GL_LIGHT0);
//    glEnable(GL_DEPTH_TEST);

//    glPushMatrix();
//    glTranslated( 0, 0, 1);//平行移動値の設定(openGL座標系では画面手前がZ++の方向)
//    glPushMatrix();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef(M_PI, 0.0f, 1.0f, 0.0f);
    // -------- ここまで --------

    // glbeginとglendの呼び出しをこのようにしてまとめることができる
    // 注意事項: glbegin, glendの間でこれらの関数を呼び出しても、何も表示されない。
        int primitive_count = 6;
        glDrawElements(GL_TRIANGLES,                    // プリミティブの種類
                        primitive_count,                // レンダリング要素の数
                        GL_UNSIGNED_INT,                // インデックス配列の型
                        indices);                       // インデックス配列をさすポインタ
        glDrawElements(GL_TRIANGLES,                    // プリミティブの種類
                        primitive_count,                // レンダリング要素の数
                        GL_UNSIGNED_INT,                // インデックス配列の型
                        indices);                       // インデックス配列をさすポインタ

    glFlush();
}

int main(int argc , char ** argv) {
    glutInit(&argc , argv);
    glutInitWindowPosition(100 , 50);
    glutInitWindowSize(400 , 300);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);

    glutCreateWindow("Kitty on your lap");
    glutDisplayFunc(disp);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(0 , 4 , 2 , 0 , 2 , 10);

    glutMainLoop();
    return 0;
}
#endif
