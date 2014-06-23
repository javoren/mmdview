
#include <GL/gl.h>
#include <GL/glut.h>

#include "Mesh.h"

int WindowWidth = 512;
int WindowHeight = 512;

// テスト用頂点配列
const GLfloat test_vertex[] = {
	-0.9 , 0.9 , 0.9 , 0.9 , 0 , -0.9
};

void Reshape(int x, int y)
{
	WindowWidth = x;
	WindowHeight = y;
	if ( WindowWidth < 1 ) WindowWidth = 1;
	if ( WindowHeight < 1 ) WindowHeight = 1;
}


void disp(void)
{
//	glClearColor(1 , 1 , 1 , 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (double)WindowWidth/(double)WindowHeight, 0.1, 1000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glViewport(0, 0, WindowWidth, WindowHeight);

	//　視点の設定
	gluLookAt( 
		0.0, 0.0, -5.0,
		0.0, 0.0, 0.0,
		0.0, 1.0, 0.0 );

	printf("passed\n");

#if 1
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2 , GL_FLOAT , 0 , test_vertex);
	glBegin(GL_POLYGON); {
		int i;
		for(i = 0 ; i < 3 ; i++) glArrayElement(i);
	} glEnd();
#endif

#if 0
	///　三角形を描画　///
	glBegin(GL_TRIANGLES);

	//　頂点1
	glColor3d(1.0, 1.0, 0.0);		
	glVertex3d(0.0, 0.825, 0.0);

	//　頂点2
	glColor3d(0.0, 1.0, 1.0);
	glVertex3d(-1.0, -0.825, 0.0);

	//　頂点3
	glColor3d(1.0, 0.0, 1.0);
	glVertex3d(1.0, -0.825, 0.0);

	///　描画終了　///
	glEnd();
#endif

	glutSwapBuffers();
}

int main(int argc , char ** argv)
{
	PmdMesh	mesh((char*)"/home/catalina/workspace/opengl/madoka/md_m.pmd");

	glutInit(&argc , argv);
	glutInitWindowPosition(100 , 50);
	glutInitWindowSize(WindowWidth , WindowHeight);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);

	glutCreateWindow("Window caption");
	glutDisplayFunc(disp);
	glutReshapeFunc(Reshape);
	glutMainLoop();
	return 0;
}

