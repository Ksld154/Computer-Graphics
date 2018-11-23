#define _USE_MATH_DEFINES
#include<Windows.h>
#include"glew.h"
#include"glut.h"
#include"FreeImage.h"
#include<iostream>
#include<math.h>
using namespace std;

int window_width = 600, window_height = 600;
int cutoff = 0, exponent = 0;
GLuint texture_id;

int degree = 20;
float radius = 15.0;
int slice = 360;    //earth longtitude
int stack = 180;    //earth latitude

float degree_Earth_Rotate = 0;
// int lighting_mode = 0;	//0:point light, 1:direcitonal light, 2: spotlight

void Init();
void Display();
void Idle();
void Keyboard(unsigned char key, int x, int y);
void Lighting();
void DrawSphere(float r, int stack, int slice);
void InitTexture();

int main(int argc, char** argv){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(window_width, window_height);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("0516215_Project2");

    Init();
    InitTexture();

	glutDisplayFunc(Display);
	glutIdleFunc(Idle);
	glutKeyboardFunc(Keyboard);

	glutMainLoop();
	return 0;
}

void Display(){
    //ModelView Matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0f, 50.0f, 50.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	//Projection Matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, window_width / (GLfloat)window_height, 0.1, 1000);
	//Viewport Matrix
	glViewport(0, 0, window_width, window_height);

    //clear
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
    Lighting();

	float red[]   = { 1.0f, 0.0f, 0.0f, 1.0f };
	float green[] = { 0.0f, 1.0f, 0.0f, 1.0f };
	float blue [] = { 0.0f, 0.0f, 1.0f, 1.0f };

	// draw earth
	glPushMatrix();
	

	// 自轉(rotate itself)
	//glRotatef(degree_Earth_Rotate, sin(23.5f * (2.0f*M_PI/360.0f)), cos(23.5f * (2.0f*M_PI/360.0f)), 0.0f);
	glRotatef(-23.5, 0.0f, 0.0f, 1.0f);
	glRotatef(degree_Earth_Rotate, 0.0f, 1.0f, 0.0f);

	// glMaterialfv(GL_FRONT, GL_DIFFUSE, blue);
	// glMaterialfv(GL_FRONT, GL_AMBIENT, blue);
	
	DrawSphere(radius/3, slice, stack);
	glPopMatrix();


	// glPushMatrix();
	// //glRotatef(degree, 0, 1, 0);
	// // glMaterialfv(GL_FRONT, GL_DIFFUSE, red);
	// // glMaterialfv(GL_FRONT, GL_SPECULAR, green);
	// glMaterialfv(GL_FRONT, GL_AMBIENT, blue);
	// glMaterialf(GL_FRONT, GL_SHININESS, 10);
	// glutSolidSphere(3.5, 72, 36);
	// glPopMatrix();

	glutSwapBuffers();

}
void Init() {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	//enable depth test
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearDepth(1.0);

	//enable lighting
	glEnable(GL_LIGHTING);
	glShadeModel(GL_SMOOTH);
}

void Idle() {
	Sleep(20);	//about 50 fps
	glutPostRedisplay();
}

void Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	default:
		break;
	}
}

void Lighting() {
	GLfloat diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat ambient[]  = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	GLfloat none[] = { 0.0f, 0.0f, 0.0f, 1.0f };

	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  diffuse);
	glLightfv(GL_LIGHT0, GL_AMBIENT,  ambient);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
}


void DrawSphere(float r, int slice, int stack) {
	//F(u, v) = [cos(u)*sin(v)*r, cos(v)*r, sin(u)*sin(v)*r]
	//float x, y, z;
	glBindTexture(GL_TEXTURE_2D, texture_id);
	
	for (float i = 0; i <= stack; i++) {

		float SphereSurface[4][3] = { 0.0f, 0.0f, 0.0f };

		for (float j = 0; j <= slice; j++) {

			float m = i * (M_PI / stack);
			float n = j * (2 * M_PI / slice);

			// x0, y0, z0
			SphereSurface[0][0] = r * sin(m) * cos(n);
			SphereSurface[0][1] = r * cos(m);
			SphereSurface[0][2] = r * sin(m) * sin(n);

			SphereSurface[1][0] = r * sin(m + (M_PI / stack)) * cos(n);
			SphereSurface[1][1] = r * cos(m + (M_PI / stack));
			SphereSurface[1][2] = r * sin(m + (M_PI / stack)) * sin(n);

			SphereSurface[2][0] = r * sin(m) * cos(n + (2 * M_PI / slice));
			SphereSurface[2][1] = r * cos(m);
			SphereSurface[2][2] = r * sin(m) * sin(n + (2 * M_PI / slice));

			SphereSurface[3][0] = r * sin(m + (M_PI / stack)) * cos(n + (2 * M_PI / slice));
			SphereSurface[3][1] = r * cos(m + (M_PI / stack));
			SphereSurface[3][2] = r * sin(m + (M_PI / stack)) * sin(n + (2 * M_PI / slice));
	
			//glBindTexture(GL_TEXTURE_2D, texture_id);

			
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glBegin(GL_POLYGON);
			
			//float texX=float(j)/float(numQuads)+0.5f;
			float tex_X = (float) j / (float)slice;
			float tex_Y = (float) i / (float)stack;
			float tex_X2 = (float) j+1.0f / (float)slice;
			float tex_Y2 = (float) i+1.0f / (float)stack;
			
			glTexCoord2f(tex_X, tex_Y);
			glVertex3f(SphereSurface[0][0], SphereSurface[0][1], SphereSurface[0][2]);
			glTexCoord2f(tex_X, tex_Y2);			
			glVertex3f(SphereSurface[1][0], SphereSurface[1][1], SphereSurface[1][2]);
			glTexCoord2f(tex_X2, tex_Y);			
			glVertex3f(SphereSurface[3][0], SphereSurface[3][1], SphereSurface[3][2]);
			glTexCoord2f(tex_X2, tex_Y2);			
			glVertex3f(SphereSurface[2][0], SphereSurface[2][1], SphereSurface[2][2]);

			float v2[3] = { SphereSurface[1][0] - SphereSurface[0][0], SphereSurface[1][1] - SphereSurface[0][1], SphereSurface[1][2] - SphereSurface[0][2] };
			float v1[3] = { SphereSurface[3][0] - SphereSurface[0][0], SphereSurface[3][1] - SphereSurface[0][1], SphereSurface[3][2] - SphereSurface[0][2] };
			
			// calculate the normal of the surface (for coloring)
			float normal[3]  = {v1[1]*v2[2]-v2[1]*v1[2], v2[0]*v1[2]-v1[0]*v2[2], v1[0]*v2[1]-v2[0]*v1[1]};
			float normal_len = sqrt(normal[0]*normal[0] + normal[1]*normal[1] + normal[2]*normal[2]);
			
			normal[0] = normal[0] / normal_len;
			normal[1] = normal[1] / normal_len;
			normal[2] = normal[2] / normal_len;
			
			glNormal3fv(normal);
			
			glEnd();
		}
	}
}

void InitTexture(){

    //enable 2D texture
	glEnable(GL_TEXTURE_2D);
	
    FIBITMAP* pImage = FreeImage_Load(FreeImage_GetFileType("./earth.jpg", 0), "./earth.jpg");
	FIBITMAP* p32BitsImage = FreeImage_ConvertTo32Bits(pImage);
	int iWidth  = FreeImage_GetWidth(p32BitsImage);
	int iHeight = FreeImage_GetHeight(p32BitsImage);
	
	// glBindTexture(GL_TEXTURE_2D, textObj[obj]);
    glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, iWidth, iHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void *)FreeImage_GetBits(p32BitsImage));
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);


	//glBindTexture(GL_TEXTURE_2D, 0); 
}