#include <Windows.h>
#include "glew.h"
#include "glut.h"
#include "shader.h"
#include "FreeImage.h"
#include <iostream>
#include <cmath>
using namespace std;

#define PI 3.14159265358

struct VertexAttribute{
	GLfloat position[3];
	GLfloat normal[3];
    GLfloat texcoord[2];
};

int windowSize[2] = { 600, 600 };

float radius = 1.0f;
int slice = 360; 
int stack = 180;
float EarthDegree = 0;
float light_rev_degree = 0;
float spin_deg = 3;

GLuint program;
GLuint vboName;
unsigned int EarthTexture, EarthNormal, EarthSpecular;

bool texture_on  = true;
bool normal_on   = true;
bool specular_on = true;
bool pause = false;

// VertexAttribute *drawPlane();
void Init();
void Idle();
void Display();
void Lighting();

void Keyboard(unsigned char key, int x, int y);
unsigned int InitTexture(const char* filename);
VertexAttribute *DrawSphere(float r, int slice, int stack);
float DegreeCalc(float deg, float spin_angle);
void Reshape(GLsizei w, GLsizei h);


int main(int argc, char** argv){

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(windowSize[0], windowSize[1]);
	glutInitWindowPosition(0, 0);
    glutCreateWindow("0516215_Project3");
	
    glewInit();
	Init();

	EarthTexture  = InitTexture("./earth_texture_map.jpg");
	EarthNormal   = InitTexture("./earth_normal_map.tif");
	EarthSpecular = InitTexture("./earth_specular_map.tif"); 

	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutIdleFunc(Idle);
	// glutKeyboardFunc(Keyboard);

	glutMainLoop();
	return 0;
}

void Init() {
	GLuint vert = createShader("Shaders/example.vert", "vertex");
	GLuint geom = createShader("Shaders/example.geom", "geometry");
	GLuint frag = createShader("Shaders/example.frag", "fragment");
	program = createProgram(vert, geom, frag);

	glGenBuffers(1, &vboName);
	glBindBuffer(GL_ARRAY_BUFFER, vboName);

	VertexAttribute *vertices;
	vertices = DrawSphere(radius, slice, stack);
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexAttribute) * slice*(stack+1)*2, vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, position)));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, normal)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, texcoord)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vboName);
	glBindBuffer(GL_ARRAY_BUFFER, vboName);
	
	// glEnable(GL_LIGHTING);
	// glShadeModel(GL_SMOOTH);
	
	glEnable(GL_TEXTURE_2D);
}

void Display() {
    
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
	gluLookAt(0.0f, 0.0f, 3.0f,// eye
		0.0f, 0.0f, 0.0f,// center
		0.0f, 1.0f, 0.0f // up
	);


	glRotatef(-23.5, 0.0f, 0.0f, 1.0f);
	glRotatef(EarthDegree, 0.0f, 1.0f, 0.0f);


	GLfloat pmtx[16];
	GLfloat mmtx[16];
	glGetFloatv(GL_PROJECTION_MATRIX, pmtx);
	glGetFloatv(GL_MODELVIEW_MATRIX, mmtx);
	
	GLint pmatLoc = glGetUniformLocation(program, "Projection");
	GLint mmatLoc = glGetUniformLocation(program, "ModelView");
	GLint texLoc  = glGetUniformLocation(program, "Texture");
	GLint normalLoc   = glGetUniformLocation(program, "NormalMap");
	GLint specularLoc = glGetUniformLocation(program, "SpecularMap");	
	GLint degreeLoc   = glGetUniformLocation(program, "EarthDegree");
	GLint lightrevLoc = glGetUniformLocation(program, "light_rev_degree");

	glUseProgram(program);
	
	/*Texture Mapping*/
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, EarthTexture);
	glUniform1i(texLoc, 0);

	/*Normal Mapping*/
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, EarthNormal);
	glUniform1i(normalLoc, 1);

	/*Specular Mapping*/
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, EarthSpecular);
	glUniform1i(specularLoc, 2);

	glUniform1f(degreeLoc, EarthDegree);
	glUniform1f(lightrevLoc, light_rev_degree);


	glUniformMatrix4fv(pmatLoc, 1, GL_FALSE, pmtx);
	glUniformMatrix4fv(mmatLoc, 1, GL_FALSE, mmtx);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, slice*(stack+1)*2);
	
	glBindTexture(GL_TEXTURE_2D, NULL);
	glUseProgram(0);
	
	glPopMatrix();
	glutSwapBuffers();
}


VertexAttribute *DrawSphere(float r, int slice, int stack) {
	VertexAttribute *vertices;
	vertices = new VertexAttribute[slice*(stack+1)*2];

	// double r = 1.0f;
	double x, y, z;
	double slice_step = 2 * PI / slice, stack_step = PI / stack;
	int cnt = 0;
	float u = 1.0f / (float)slice;
	float v = 1.0f / (float)stack;

	for (int i = 0; i < slice; i++) {
		for (int j = 0; j < stack + 1; j++) {
			x = r * sin(j * stack_step) * cos(i*slice_step);
			y = r * cos(j * stack_step);
			z = r * sin(j * stack_step) * sin(i*slice_step);

			vertices[cnt].position[0] = x; 
			vertices[cnt].position[1] = y; 
			vertices[cnt].position[2] = z;
			vertices[cnt].normal[0] = x;
			vertices[cnt].normal[1] = y;
			vertices[cnt].normal[2] = z;
			vertices[cnt].texcoord[0] = 1.0f - i * u;
			vertices[cnt].texcoord[1] = 1.0f - j * v;
			cnt++;


			x = r * sin(j * stack_step) * cos((i + 1)*slice_step);
			y = r * cos(j * stack_step);
			z = r * sin(j * stack_step) * sin((i + 1)*slice_step);

			vertices[cnt].position[0] = x; 
			vertices[cnt].position[1] = y; 
			vertices[cnt].position[2] = z;
			vertices[cnt].normal[0] = x;
			vertices[cnt].normal[1] = y;
			vertices[cnt].normal[2] = z;
			vertices[cnt].texcoord[0] = 1.0f - (i+1) * u;
			vertices[cnt].texcoord[1] = 1.0f - j * v;
			cnt++;
		}
	}
	return vertices;
}

void Idle() {
	
	if(!pause){
		Sleep(20);	//about 50 fps
		EarthDegree = DegreeCalc(EarthDegree, spin_deg);
		light_rev_degree = DegreeCalc(light_rev_degree, 0.1f*spin_deg);
		// EarthDegree = (EarthDegree + spin_deg) % 3600;
		// cout << EarthDegree << endl;
	}
	
	glutPostRedisplay();
}
float DegreeCalc(float deg, float spin_angle){
	deg += spin_angle;
	while(deg >= 360.0f)
		deg -= 360.0f;
	return deg;
}

void Reshape(GLsizei w, GLsizei h){
	windowSize[0] = w;
	windowSize[1] = h;
}


unsigned int InitTexture(const char* filename){

	unsigned int texture_id;

    FIBITMAP* pImage = FreeImage_Load(FreeImage_GetFileType(filename, 0), filename);
	FIBITMAP* p32BitsImage = FreeImage_ConvertTo32Bits(pImage);
	int iWidth  = FreeImage_GetWidth(p32BitsImage);
	int iHeight = FreeImage_GetHeight(p32BitsImage);
	
    glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, iWidth, iHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void *)FreeImage_GetBits(p32BitsImage));
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glBindTexture(GL_TEXTURE_2D, 0); 
	FreeImage_Unload(pImage);
	FreeImage_Unload(p32BitsImage);

	return texture_id;
}
