
#define _USE_MATH_DEFINES
#include <iostream>
#include <cstdlib>
#include <math.h>
#include <Windows.h>
#include "include/glut.h"
using namespace std;



int window_width  = 600;
int window_height = 600;
int degree = 20;
float radius = 4.0;
int earth_long = 360;
int earth_lati = 180;

int spin_degree = 1;
float spin_deg = 0.5;

float degree_Earth_Revo = 0;
float degree_Earth_Rotate = 0;
float degree_Moon = 0;

void display();
void DrawSphere(float r, int n_lati, int n_long);
void Lighting();
void Idle();
void keyboard(unsigned char key, int x, int y);
float DegreeCalc(float deg, float spin_deg);

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(window_width, window_height);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("0516215_Project1_SolarSystem");

	Lighting();

	glutDisplayFunc(display);
	glutIdleFunc(Idle);
	glutKeyboardFunc(keyboard);

	glutMainLoop();
	return 0;
}

void display() {

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



	// draw
	glMatrixMode(GL_MODELVIEW);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	float red[4]   = { 1.0f, 0.0f, 0.0f, 1.0f };
	float green[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
	float blue[4]  = { 0.0f, 0.0f, 1.0f, 1.0f };
	float white[4] = { 1.0f, 1.0f, 1.0f, 1.0f };


	// draw earth
	glPushMatrix();
	
	// 公轉(revolution, i.e. rotate around the sun)
	glRotatef(degree_Earth_Revo, 0.0f, 1.0f, 0.0f);
	glTranslatef(20.0, 0.0, 0.0);

	// 自轉(rotate itself)
	glRotatef(-degree_Earth_Revo, 0.0f, 1.0f, 0.0f);
	glRotatef(degree_Earth_Rotate, sin(23.5f * (2.0f*M_PI/360.0f)), cos(23.5f * (2.0f*M_PI/360.0f)), 0.0f);
	glRotatef(-23.5, 0.0f, 0.0f, 1.0f);

	glMaterialfv(GL_FRONT, GL_DIFFUSE, blue);
	glNormal3d(0, 10, 0);
	DrawSphere(radius/3, earth_long, earth_lati);
	//glutWireCube(1);
	glPopMatrix();
	


	// draw earth rotation axis
	glPushMatrix();

	// 公轉(revolution, i.e. rotate around the sun)
	//glRotatef(-degree_Earth_Revo, 0.0f, 0.0f, 1.0f);
	glRotatef(degree_Earth_Revo, 0.0f, 1.0f, 0.0f);
	glTranslatef(20.0, 0.0, 0.0);
	
	// 自轉(rotate itself) 
	glRotatef(-degree_Earth_Revo, 0.0f, 1.0f, 0.0f);
	glRotatef(-23.5, 0.0f, 0.0f, 1.0f);
	
	glMaterialfv(GL_FRONT, GL_DIFFUSE, red);
	glNormal3d(0, 10, 0);

	glBegin(GL_LINES);
	glColor3f (1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, -radius/2.0, 0.0f);
	glColor3f (1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, radius/2.0, 0.0f);
	glEnd();

	glPopMatrix();



	// draw moon
	glPushMatrix();

	// (3) 公轉(revolution, i.e. rotate around the SUN)
	glRotatef(degree_Earth_Revo, 0.0f, 1.0f, 0.0f);
	glTranslatef(20.0, 0.0, 0.0);

	// (2) 公轉(revolution, i.e. rotate around the EARTH)
	glRotatef(degree_Moon, 0.0f, 1.0f, 0.0f);
	glTranslatef(3.0, 0.0, 0.0);

	// (1) 自轉(rotate itself)
	glRotatef(degree_Moon, 0.0f, 1.0f, 0.0f);

	glMaterialfv(GL_FRONT, GL_DIFFUSE, white);
	glNormal3d(0, 10, 0);
	DrawSphere(radius/6, 240, 60);

	glPopMatrix();



	glMaterialfv(GL_FRONT, GL_DIFFUSE, red);
	glNormal3d(0, 10, 0);

	// draw sun
	DrawSphere(radius, 240, 60);

	glutSwapBuffers();
}


//4 2
void DrawSphere(float r, int n_long, int n_lati) {
	//F(u, v) = [cos(u)*sin(v)*r, cos(v)*r, sin(u)*sin(v)*r]
	float x, y, z;
	
	
	// draw latitudes  (horizontal)
	for(float i = 0; i < M_PI; i += M_PI/n_lati) {
		glBegin(GL_LINE_LOOP);
		
		y = (float)(r * cos(i));
		for(float j = 0; j < 2*M_PI; j += 2*M_PI/n_long) {
			x = (float)(r * sin(i) * cos(j));
			z = (float)(r * sin(i) * sin(j));
			glVertex3f(x, y, z);
		}
		glEnd();
	}
	
	
	// draw longtitudes (vertical)
	for(float i = 0; i < M_PI; i += 2*M_PI/n_long ){
		glBegin(GL_LINE_LOOP);
		for(float j = 0; j < 2*M_PI; j += M_PI/n_lati) {
			x = (float)(r * sin(j) * cos(i));
			y = (float)(r * cos(j));
			z = (float)(r * sin(j) * sin(i));
			glVertex3f(x, y, z);
		}
		glEnd();
	}
	
}
void Lighting() {
	// enable lighting
	glEnable(GL_LIGHTING);

	//Add directed light
	GLfloat diffuse_color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat ambient_color[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glEnable(GL_LIGHT0);								//open light0
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_color);	//set diffuse color of light0
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_color);	//set ambient color of light0
	glLightfv(GL_LIGHT0, GL_POSITION, position);		//set position of light 0
}

void Idle() {
	Sleep(20);
	//degree = (degree + spin_degree) % 360;
	degree_Earth_Revo = DegreeCalc(degree_Earth_Revo, spin_deg);
	degree_Earth_Rotate = DegreeCalc(degree_Earth_Rotate, spin_deg*365.0f);
	degree_Moon = DegreeCalc(degree_Moon, spin_deg*28.0f);
	glutPostRedisplay();
}

float DegreeCalc(float deg, float spin_angle){
	deg += spin_angle;
	while(deg >= 360.0)
		deg -= 360.0;
	return deg;
}

void keyboard(unsigned char key, int x, int y) {
	
	
	/*switch (key) {
		case 'o':
			if()
			earth_long = 4;
			earth_lati = 2;
			break;
		default:
			break;
	}*/
	if (key == 'o') {
		if (earth_long == 360 && earth_lati == 180) {
			earth_long = 4;
			earth_lati = 2;
		}
		else if (earth_long == 4 && earth_lati == 2) {
			earth_long = 360;
			earth_lati = 180;
		}
	}


}
