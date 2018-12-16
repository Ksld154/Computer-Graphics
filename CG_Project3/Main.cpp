#include "FreeImage.h"
#include "glew.h"
#include "glut.h"
#include "shader.h"
#include <iostream>
#include <cmath>
using namespace std;

#define PI 3.14159265358

struct VertexAttribute{
	GLfloat position[3];
	GLfloat normal[3];
    GLfloat texcoord[2];
};

int windowSize[2] = { 800, 600 };
int slice = 360, stack = 180;
GLuint program;
GLuint vboName;

bool start;

VertexAttribute *drawPlane();
void display();
void reshape(GLsizei w, GLsizei h);
void init();

void Idle();
void Keyboard(unsigned char key, int x, int y);
void Lighting();
VertexAttribute *mySphere(int slice, int stack);
unsigned int InitTexture(const char* filename);
float DegreeCalc(float deg, float spin_angle);

int main(int argc, char** argv){

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(windowSize[0], windowSize[1]);
	glutInitWindowPosition(0, 0);
    glutCreateWindow("0516215_Project3");
	
    glewInit();
	init();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMainLoop();

	return 0;
}

void init() {
	GLuint vert = createShader("Shaders/example.vert", "vertex");
	GLuint geom = createShader("Shaders/example.geom", "geometry");
	GLuint frag = createShader("Shaders/example.frag", "fragment");
	program = createProgram(vert, geom, frag);

	glGenBuffers(1, &vboName);
	glBindBuffer(GL_ARRAY_BUFFER, vboName);

	VertexAttribute *vertices;
	// vertices = drawPlane();
	vertices = mySphere(slice, stack);
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexAttribute) * slice*(stack+1)*2, vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, position)));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, normal)));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, texcoord)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vboName);
	glBindBuffer(GL_ARRAY_BUFFER, vboName);

}

void display() {
    
	//Clear the buffer
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// viewport transformation
	glViewport(0, 0, windowSize[0], windowSize[1]);
	// projection transformation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (GLfloat)512 / (GLfloat)512, 1, 500);
	// viewing and modeling transformation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//lights();
	gluLookAt(0.5f, 1.5f, -0.5f,// eye
		0.5f, 0.5f, 0.5f,// center
		0.0f, 1.0f, 0.0f);// up

	GLfloat pmtx[16];
	GLfloat mmtx[16];
	glGetFloatv(GL_PROJECTION_MATRIX, pmtx);
	glGetFloatv(GL_MODELVIEW_MATRIX, mmtx);
	GLint pmatLoc = glGetUniformLocation(program, "Projection");
	GLint mmatLoc = glGetUniformLocation(program, "ModelView");

	glUseProgram(program);

	glUniformMatrix4fv(pmatLoc, 1, GL_FALSE, pmtx);
	glUniformMatrix4fv(mmatLoc, 1, GL_FALSE, mmtx);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, slice*(stack+1)*2);
	glUseProgram(0);
	
	glPopMatrix();
	glutSwapBuffers();

}


VertexAttribute *mySphere(int slice, int stack) {
	VertexAttribute *vertices;
	vertices = new VertexAttribute[slice*(stack+1)*2];

	double r = 5.0f;
	double x, y, z;
	double slice_step = 2 * PI / slice, stack_step = PI / stack;
	for (int i = 0; i < slice; i++) {
		// glBegin(GL_TRIANGLE_STRIP);
		for (int j = 0; j < stack + 1; j++) {
			x = r * sin(j * stack_step) * cos(i*slice_step);
			y = r * cos(j * stack_step);
			z = r * sin(j * stack_step) * sin(i*slice_step);
			// glNormal3d(x, y, z);
			// glVertex3d(x, y, z);

			vertices[i*stack + 2*j].position[0] = x; 
			vertices[i*stack + 2*j].position[1] = y; 
			vertices[i*stack + 2*j].position[2] = z;
			vertices[i*stack + 2*j].normal[0] = 0.0f;
			vertices[i*stack + 2*j].normal[1] = 0.0f;
			vertices[i*stack + 2*j].normal[2] = 1.0f;
			vertices[i*stack + 2*j].texcoord[0] = 1.0f-stack_step;
			vertices[i*stack + 2*j].texcoord[1] = 1.0f-slice_step;




			x = r * sin(j * stack_step) * cos((i + 1)*slice_step);
			y = r * cos(j * stack_step);
			z = r * sin(j * stack_step) * sin((i + 1)*slice_step);
			// glNormal3d(x, y, z);
			// glVertex3d(x, y, z);
			vertices[i*stack + 2*j + 1].position[0] = x; 
			vertices[i*stack + 2*j + 1].position[1] = y; 
			vertices[i*stack + 2*j + 1].position[2] = z;
			vertices[i*stack + 2*j + 1].normal[0] = 0.0f;
			vertices[i*stack + 2*j + 1].normal[1] = 0.0f;
			vertices[i*stack + 2*j + 1].normal[2] = 1.0f;
			vertices[i*stack + 2*j + 1].texcoord[0] = 1.0f-stack_step;
			vertices[i*stack + 2*j + 1].texcoord[1] = 1.0f-(slice_step+1);
		}
		// glEnd();
	}
	return vertices;
}

void reshape(GLsizei w, GLsizei h){
	windowSize[0] = w;
	windowSize[1] = h;
}