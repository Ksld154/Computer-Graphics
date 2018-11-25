#define _USE_MATH_DEFINES
#include<Windows.h>
#include"glew.h"
#include"glut.h"
#include"FreeImage.h"
#include<iostream>
#include<math.h>
#include<string>
using namespace std;

int window_width = 600, window_height = 600;
int cutoff = 0, exponent = 0;
//GLuint texture_id;

float radius = 1.0;
int slice = 360;    //earth longtitude
int stack = 180;    //earth latitude
float spin_deg = 0.1;    // X = spin_deg*365

float degree_Earth_Rotate = 0;
float degree_Moon = 0;

int pause = 0;
float spin_deg_save = 0;

// int lighting_mode = 0;	//0:point light, 1:direcitonal light, 2: spotlight

unsigned int EarthTexture, MoonTexture;

void Init();
void Display();
void Idle();
void Keyboard(unsigned char key, int x, int y);
void Lighting();
void DrawSphere(float r, int stack, int slice, bool axis);
unsigned int InitTexture(const char* filename);
float DegreeCalc(float deg, float spin_angle);

int main(int argc, char** argv){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(window_width, window_height);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("0516215_Project2");

    Init();
	EarthTexture = InitTexture("./earth.jpg");
	MoonTexture  = InitTexture("./moon.jpg");
    Lighting();

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
	gluLookAt(0.0f, 5.0f, 10.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	//Projection Matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, window_width / (GLfloat)window_height, 0.1, 1000);
	//Viewport Matrix
	glViewport(0, 0, window_width, window_height);

    //clear
	glMatrixMode(GL_MODELVIEW);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);



	// float red[]   = { 1.0f, 0.0f, 0.0f, 1.0f };
	// float green[] = { 0.0f, 1.0f, 0.0f, 1.0f };
	// float blue [] = { 0.0f, 0.0f, 1.0f, 1.0f };


	// glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	// glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	// glEnable(GL_TEXTURE_GEN_S);
	// glEnable(GL_TEXTURE_GEN_T);



	// draw earth
	glPushMatrix();
	glRotatef(-23.5, 0.0f, 0.0f, 1.0f);
	glRotatef(degree_Earth_Rotate, 0.0f, 1.0f, 0.0f);
	glBindTexture(GL_TEXTURE_2D, EarthTexture);
	DrawSphere(radius, slice, stack, true);
	glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();

	// draw moon
	glPushMatrix();

	glRotatef(degree_Moon, 0.0f, 1.0f, 0.0f);
	glTranslatef(3*radius, 0.0, 0.0);
	glRotatef(degree_Moon, 0.0f, 1.0f, 0.0f);

	glBindTexture(GL_TEXTURE_2D, MoonTexture);
	DrawSphere(radius/2, 240, 60, true);
	glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();

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

	//enable 2D texture
	glEnable(GL_TEXTURE_2D);
}

void Idle() {
	Sleep(20);	//about 50 fps

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

void Keyboard(unsigned char key, int x, int y) {
	if (key == 'o') {
		if (slice == 360 && stack == 180) {
			slice = 4;
			stack = 2;
		}
		else if (slice == 4 && stack == 2) {
			slice = 360;
			stack = 180;
		}
	}
	else if (key == 'p') {
		// do pause!!!!!
		if(pause == 0){ // if not paused, then pause it
			pause = 1;
			spin_deg_save = spin_deg;
			spin_deg = 0;
		}
		else if (pause == 1) {
			pause = 0;
			spin_deg = spin_deg_save;
			spin_deg_save = 0;
		}
	}
}

void Lighting() {
	GLfloat ambient[]  = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat position[] = { 0.0f, 10.0f, 0.0f, 1.0f };

	glLightfv(GL_LIGHT0, GL_AMBIENT,  ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glEnable(GL_LIGHT0);
}


void DrawSphere(float r, int slice, int stack, bool axis) {
	//F(u, v) = [cos(u)*sin(v)*r, cos(v)*r, sin(u)*sin(v)*r]
	//float x, y, z;
	// glBindTexture(GL_TEXTURE_2D, texture_id);
	
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
			
			glTexCoord2f(1.0f-tex_X, 1.0f-tex_Y);
			glVertex3f(SphereSurface[0][0], SphereSurface[0][1], SphereSurface[0][2]);
			//glTexCoord2f(tex_X, tex_Y2);			
			glVertex3f(SphereSurface[1][0], SphereSurface[1][1], SphereSurface[1][2]);
						
			//glTexCoord2f(tex_X2, tex_Y2);
			glVertex3f(SphereSurface[3][0], SphereSurface[3][1], SphereSurface[3][2]);
			//glTexCoord2f(tex_X2, tex_Y);			
			glVertex3f(SphereSurface[2][0], SphereSurface[2][1], SphereSurface[2][2]);

			float v1[3] = { SphereSurface[3][0] - SphereSurface[0][0], SphereSurface[3][1] - SphereSurface[0][1], SphereSurface[3][2] - SphereSurface[0][2] };
			float v2[3] = { SphereSurface[1][0] - SphereSurface[0][0], SphereSurface[1][1] - SphereSurface[0][1], SphereSurface[1][2] - SphereSurface[0][2] };
			
			// calculate the normal of the surface (for coloring)
			float normal[3]  = {v1[1]*v2[2]-v2[1]*v1[2], v2[0]*v1[2]-v1[0]*v2[2], v1[0]*v2[1]-v2[0]*v1[1]};
			float normal_len = sqrt(normal[0]*normal[0] + normal[1]*normal[1] + normal[2]*normal[2]);
			
			normal[0] = normal[0] / normal_len;
			normal[1] = normal[1] / normal_len;
			normal[2] = normal[2] / normal_len;
			
			glNormal3fv(normal);
			
			// glBindTexture(GL_TEXTURE_2D, 0);
			glEnd();
		}
	}
	if(axis){
		glBegin(GL_LINE_STRIP);
		// glColor3f (1.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, -2*radius, 0.0f);
		// glColor3f (1.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 2*radius, 0.0f);
		float red[]   = { 1.0f, 0.0f, 0.0f, 1.0f };
		// glMaterialfv(GL_FRONT, GL_AMBIENT, red);
		glEnd();
	}
}

unsigned int InitTexture(const char* filename){

	unsigned int texture_id;
	FreeImage_Initialise(0);

    FIBITMAP* pImage = FreeImage_Load(FreeImage_GetFileType(filename, 0), filename);
	FIBITMAP* p32BitsImage = FreeImage_ConvertTo32Bits(pImage);
	int iWidth  = FreeImage_GetWidth(p32BitsImage);
	int iHeight = FreeImage_GetHeight(p32BitsImage);
	
    glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);

	// glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, iWidth, iHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void *)FreeImage_GetBits(p32BitsImage));
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glBindTexture(GL_TEXTURE_2D, 0); 
	FreeImage_Unload(pImage);
	FreeImage_Unload(p32BitsImage);
	FreeImage_DeInitialise();

	return texture_id;
}