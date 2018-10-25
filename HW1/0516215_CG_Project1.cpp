
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
float radius = 6.0;
int earth_long = 360;    //slice
int earth_lati = 180;    //stack

int spin_degree = 1;
float spin_deg = 0.1;    // X = spin_deg*365

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

	glClearDepth(1.0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClear(GL_DEPTH_BUFFER_BIT);
	


	// draw
	glMatrixMode(GL_MODELVIEW);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	Lighting();


	float red[4]   = { 1.0f, 0.3f, 0.0f, 1.0f };
	float blue[4]  = { 0.1f, 0.6f, 1.0f, 1.0f };
	float moon[4]  = { 0.5f, 0.5f, 0.5f, 1.0f };


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
	glMaterialfv(GL_FRONT, GL_AMBIENT, blue);
	//glColor3f(0.0f, 0.0f, 1.0f);
	
	DrawSphere(radius/3, earth_long, earth_lati);
	glPopMatrix();
	


	// draw earth rotation axis
	glPushMatrix();

	// 公轉(revolution, i.e. rotate around the sun)
	glRotatef(degree_Earth_Revo, 0.0f, 1.0f, 0.0f);
	glTranslatef(20.0, 0.0, 0.0);
	// 自轉(rotate itself) 
	glRotatef(-degree_Earth_Revo, 0.0f, 1.0f, 0.0f);
	glRotatef(-23.5, 0.0f, 0.0f, 1.0f);
	
	glColor3f(1.0f, 0.0f, 0.0f);

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

	glMaterialfv(GL_FRONT, GL_DIFFUSE, moon);
	glMaterialfv(GL_FRONT, GL_AMBIENT, moon);
	//glColor3f(0.5f, 0.5f, 0.5f);

	DrawSphere(radius/6, 240, 60);
	glPopMatrix();


	// draw sun
	glMaterialfv(GL_FRONT, GL_DIFFUSE, red);
	glMaterialfv(GL_FRONT, GL_AMBIENT, red);
	//glColor3f(1.0f, 0.0f, 0.0f);
	//Rotatef(degree_Moon, 0.0f, 1.0f, 0.0f);
	DrawSphere(radius, 40, 20);

	glutSwapBuffers();
}


//4 2
void DrawSphere(float r, int n_long, int n_lati) {
	//F(u, v) = [cos(u)*sin(v)*r, cos(v)*r, sin(u)*sin(v)*r]
	//float x, y, z;
	
	for (float i = 0; i < n_lati; i++) {

		float SphereSurface[4][3] = { 0.0f, 0.0f, 0.0f };

		for (float j = 0; j < n_long; j++) {

			float m = i * (M_PI / n_lati);
			float n = j * (2 * M_PI / n_long);



			// x0, y0, z0
			SphereSurface[0][0] = r * sin(m) * cos(n);
			SphereSurface[0][1] = r * cos(m);
			SphereSurface[0][2] = r * sin(m) * sin(n);

			SphereSurface[1][0] = r * sin(m + (M_PI / n_lati)) * cos(n);
			SphereSurface[1][1] = r * cos(m + (M_PI / n_lati));
			SphereSurface[1][2] = r * sin(m + (M_PI / n_lati)) * sin(n);

			SphereSurface[2][0] = r * sin(m) * cos(n + (2 * M_PI / n_long));
			SphereSurface[2][1] = r * cos(m);
			SphereSurface[2][2] = r * sin(m) * sin(n + (2 * M_PI / n_long));

			SphereSurface[3][0] = r * sin(m + (M_PI / n_lati)) * cos(n + (2 * M_PI / n_long));
			SphereSurface[3][1] = r * cos(m + (M_PI / n_lati));
			SphereSurface[3][2] = r * sin(m + (M_PI / n_lati)) * sin(n + (2 * M_PI / n_long));
	
	
			
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glBegin(GL_POLYGON);
			
			glVertex3f(SphereSurface[0][0], SphereSurface[0][1], SphereSurface[0][2]);
			glVertex3f(SphereSurface[1][0], SphereSurface[1][1], SphereSurface[1][2]);
			glVertex3f(SphereSurface[3][0], SphereSurface[3][1], SphereSurface[3][2]);
			glVertex3f(SphereSurface[2][0], SphereSurface[2][1], SphereSurface[2][2]);

			float v2[3] = { SphereSurface[1][0] - SphereSurface[0][0], SphereSurface[1][1] - SphereSurface[0][1], SphereSurface[1][2] - SphereSurface[0][2] };
			float v1[3] = { SphereSurface[3][0] - SphereSurface[0][0], SphereSurface[3][1] - SphereSurface[0][1], SphereSurface[3][2] - SphereSurface[0][2] };

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
void Lighting() {
	// enable lighting
	//glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);

	//Add directed light
	GLfloat diffuse_color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat ambient_color[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat position[] = { 0.0f, 10.0f, 0.0f, 0.0f};
	
	glEnable(GL_LIGHT0);								//open light0
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_color);	//set diffuse color of light0
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_color);	//set ambient color of light0
	glLightfv(GL_LIGHT0, GL_POSITION, position);		//set position of light 0
}

void Idle() {
	Sleep(20);
	degree_Earth_Revo   = DegreeCalc(degree_Earth_Revo, spin_deg);
	degree_Earth_Rotate = DegreeCalc(degree_Earth_Rotate, spin_deg*365.0f);
	degree_Moon = DegreeCalc(degree_Moon, spin_deg*(28.0f*360.0f/365.0f));
	glutPostRedisplay();
}

float DegreeCalc(float deg, float spin_angle){
	deg += spin_angle;
	while(deg >= 360.0)
		deg -= 360.0;
	return deg;
}

void keyboard(unsigned char key, int x, int y) {
	
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
	else if (key == 'p') {
		// do pause
	}


}
