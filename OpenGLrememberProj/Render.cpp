#include "Render.h"




#include <windows.h>

#include <GL\gl.h>
#include <GL\glu.h>
#include "GL\glext.h"

#include "MyOGL.h"
#include <mmsystem.h>
#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "MyShaders.h"

#include "ObjLoader.h"
#include "GUItextRectangle.h"

#include "Texture.h"
#include <sstream>
#pragma comment(lib, "winmm.lib")
GuiTextRectangle rec;

bool textureMode = true;
bool lightMode = true;

//��������� ������ ��� ��������� ����
#define POP glPopMatrix()
#define PUSH glPushMatrix()
int selectId = 0;
bool selectmode = false;
char flag = '0';
bool flagU = true;
bool flagI = true;
bool flagO = true;
bool flagP = true;
bool flagK = true;
ObjFile *model;

Texture texture1;
Texture sTex;
Texture rTex;
Texture tBox;

Shader s[10];  //��������� ��� ������ ��������
Shader frac;
Shader cassini;

void Player_Select(HWND );
void Game_show();


//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;
	double posX, posY;
	
	//������� ������ �� ���������
	CustomCamera()
	{
		posX = pos.X();
		posY = pos.Y();
		camDist = 10;
		fi1 = 1;
		fi2 = 1;
	}
	
	
	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	virtual void SetUpCamera()
	{

		pos.setCoords(0, 0, 3);

		lookPoint.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));
			normal.setCoords(0, 0, 1);
		WASD();
		pos.setCoords(posX, posY, 3);

		LookAt();
	}
	void CustomCamera::WASD() {
		PlaySound("Sounds\\step.wav", NULL, SND_ASYNC);
		Sleep(13);

		float ugol = -fi1 / 180 * M_PI;
		float speed = 0;
		if (OpenGL::isKeyPressed('D')) speed = 0.1;
		if (OpenGL::isKeyPressed('A')) speed = -0.1;
		if (OpenGL::isKeyPressed('W')) {
			speed = 0.1; ugol -= M_PI * 0.5;
		}
		if (OpenGL::isKeyPressed('S')) { speed = 0.1; ugol += M_PI * 0.5; }
		if (speed != 0) {
			posX += sin(ugol) * speed;
			posY += cos(ugol) * speed;
			
			
		}
	
	}
	void CustomCamera::Check_angle() {

	}
	void CustomCamera::LookAt()
	{
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������

//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}

	
	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		Shader::DontUseShaders();
		bool f1 = glIsEnabled(GL_LIGHTING);
		glDisable(GL_LIGHTING);
		bool f2 = glIsEnabled(GL_TEXTURE_2D);
		glDisable(GL_TEXTURE_2D);
		bool f3 = glIsEnabled(GL_DEPTH_TEST);
		
		glDisable(GL_DEPTH_TEST);
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();

		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
				glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}
		/*
		if (f1)
			glEnable(GL_LIGHTING);
		if (f2)
			glEnable(GL_TEXTURE_2D);
		if (f3)
			glEnable(GL_DEPTH_TEST);
			*/
	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����



//������ ���������� ����
int mouseX = 0, mouseY = 0;




float offsetX = 0, offsetY = 0;
float zoom=1;
float Time = 0;
int tick_o = 0;
int tick_n = 0;

//���������� �������� ����
void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += dx/100.0;
		if (camera.fi1 < 0) camera.fi1 += 360;
		if (camera.fi1 > 360) camera.fi1 -= 360;
		camera.fi2 += -dy / 100.0;
		if (camera.fi2 < 0) camera.fi2 = 0;
		if (camera.fi2 > 180) camera.fi2 = 180;
	}


	if (OpenGL::isKeyPressed(VK_LBUTTON))
	{
		offsetX -= 1.0*dx/ogl->getWidth()/zoom;
		offsetY += 1.0*dy/ogl->getHeight()/zoom;

		

	}


	
	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y,60,ogl->aspect);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

//���������� �������� ������  ����
void mouseWheelEvent(OpenGL *ogl, int delta)
{


	float _tmpZ = delta*0.003;
	if (ogl->isKeyPressed('Z'))
		_tmpZ *= 10;
	zoom += 0.2*zoom*_tmpZ;


	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;
}

//���������� ������� ������ ����������
void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}	   

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}

	if (key == 'S')
	{
		frac.LoadShaderFromFile();
		frac.Compile();

		s[0].LoadShaderFromFile();
		s[0].Compile();

		cassini.LoadShaderFromFile();
		cassini.Compile();
	}

	if (key == 'Q')
		Time = 0;
	if (OpenGL::isKeyPressed('U')) flag ='U';
	if (OpenGL::isKeyPressed('I')) flag ='I';
	if (OpenGL::isKeyPressed('O')) flag ='O';
	if (OpenGL::isKeyPressed('P')) flag ='P';
	if (OpenGL::isKeyPressed('K')) flag ='K';
}

void keyUpEvent(OpenGL *ogl, int key)
{

}


void DrawQuad()
{
	double A[] = { 0,0 };
	double B[] = { 1,0 };
	double C[] = { 1,1 };
	double D[] = { 0,1 };
	glBegin(GL_QUADS);
	glColor3d(.5, 0, 0);
	glNormal3d(0, 0, 1);
	glTexCoord2d(0, 0);
	glVertex2dv(A);
	glTexCoord2d(1, 0);
	glVertex2dv(B);
	glTexCoord2d(1, 1);
	glVertex2dv(C);
	glTexCoord2d(0, 1);
	glVertex2dv(D);
	glEnd();
}


ObjFile objModel,monkey,CourseWork_main,table,note;
ObjFile bat,chairs_front, chairs_front_, chairs_right, chairs_right_, divan, door_exit, door_win, kitchen, main_, main_exit, ref, wash, win;
Texture monkeyTex,woodTex,refTex,wallTex,leatherTex,active,none_active;
 void Game_show();
//����������� ����� ������ ��������
void initRender(OpenGL *ogl)
{

	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	//glEnable(GL_TEXTURE_2D);
	
	


	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	//ogl->mainCamera = &WASDcam;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH); 


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	/*
	//texture1.loadTextureFromFile("textures\\texture.bmp");   �������� �������� �� �����
	*/


	frac.VshaderFileName = "shaders\\v.vert"; //��� ����� ���������� ������
	frac.FshaderFileName = "shaders\\frac.frag"; //��� ����� ������������ �������
	frac.LoadShaderFromFile(); //��������� ������� �� �����
	frac.Compile(); //�����������

	cassini.VshaderFileName = "shaders\\v.vert"; //��� ����� ���������� ������
	cassini.FshaderFileName = "shaders\\cassini.frag"; //��� ����� ������������ �������
	cassini.LoadShaderFromFile(); //��������� ������� �� �����
	cassini.Compile(); //�����������
	

	s[0].VshaderFileName = "shaders\\v.vert"; //��� ����� ���������� ������
	s[0].FshaderFileName = "shaders\\light.frag"; //��� ����� ������������ �������
	s[0].LoadShaderFromFile(); //��������� ������� �� �����
	s[0].Compile(); //�����������

	s[1].VshaderFileName = "shaders\\v.vert"; //��� ����� ���������� ������
	s[1].FshaderFileName = "shaders\\textureShader.frag"; //��� ����� ������������ �������
	s[1].LoadShaderFromFile(); //��������� ������� �� �����
	s[1].Compile(); //�����������

	

	 //��� ��� ��� ������� ������ *.obj �����, ��� ��� ��� ��������� �� ���������� � ���������� �������, 
	 // ������������ �� ����� ����������, � ������������ ������ � *.obj_m
	
	loadModel("models\\bat.obj", &bat);
	loadModel("models\\chairs_front.obj", &chairs_front);
	loadModel("models\\chairs_front_+.obj", &chairs_front_);
	loadModel("models\\chairs_right.obj", &chairs_right);
	loadModel("models\\chairs_right_+.obj", &chairs_right_);
	loadModel("models\\divan.obj", &divan);
	loadModel("models\\door_exit.obj", &door_exit);
	loadModel("models\\door_win.obj", &door_win);
	loadModel("models\\kitchen.obj", &kitchen);
	loadModel("models\\main.obj", &main_);
	loadModel("models\\main_exit.obj", &main_exit);
	loadModel("models\\ref.obj", &ref);
	loadModel("models\\table.obj", &table);
	loadModel("models\\wash.obj", &wash);
	loadModel("models\\win.obj", &win);
	loadModel("models\\notebook.obj", &note);

	glActiveTexture(GL_TEXTURE0);
	
	woodTex.loadTextureFromFile("textures//wood.bmp");
	leatherTex.loadTextureFromFile("textures//divan.bmp");
	refTex.loadTextureFromFile("textures//refrigerator.bmp");
	wallTex.loadTextureFromFile("textures//wall.bmp");
	active.loadTextureFromFile("textures//d.bmp");
	none_active.loadTextureFromFile("textures//a.bmp");
	


	tick_n = GetTickCount();
	tick_o = tick_n;

	rec.setSize(300, 100);
	rec.setPosition(10, ogl->getHeight() - 100-10);
	rec.setText("T - ���/���� �������\nL - ���/���� ���������\nF - ���� �� ������\nG - ������� ���� �� �����������\nG+��� ������� ���� �� ���������",0,0,0);
	
	
}
void Player_Select(HWND hwnd) {
	
	selectmode = true;
	Game_show();
	selectmode = false;

	RECT rct;
	GLubyte clr[3];
	GetClientRect(hwnd, &rct);
	glReadPixels(rct.right / 2.0, rct.bottom / 2.0, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, clr);

	
	selectId = clr[0];

}



void Game_show() {

	if (selectmode) {
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);

	}
	else
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);

	glBindTexture(GL_TEXTURE_2D, 0);


	glEnable(GL_DEPTH_TEST);

	//woodTex.bindTexture();

	PUSH;
	glRotated(90, 1, 0, 0);
	woodTex.bindTexture();
	table.DrawObj();
	POP;


	PUSH;
	glRotated(90, 1, 0, 0);
	woodTex.bindTexture();
	chairs_front.DrawObj();
	POP;
	PUSH;
	glRotated(90, 1, 0, 0);
	woodTex.bindTexture();
	chairs_right.DrawObj();
	POP;
	PUSH;
	glRotated(90, 1, 0, 0);
	woodTex.bindTexture();
	door_exit.DrawObj();
	

	
	if (flag == 'U') {
		if (flagU) {
			PlaySound("Sounds//open.wav", NULL, SND_SYNC);
			flagU = false;
		}
		
		glRotated(90, 0, 1, 0);
		glTranslated(-3.3, 0, 6.5);
		door_exit.DrawObj();

		
	}

	else flagU = true;
	POP;

	if (flag == 'I') {
		if (flagI) {
			PlaySound("Sounds//close.wav", NULL, SND_SYNC);
			flagI = false;
		}
	}
	else flagI = true;

		PUSH;
		glRotated(90, 1, 0, 0);
	refTex.bindTexture();
	kitchen.DrawObj();
	POP;

	PUSH;
	glRotated(90, 1, 0, 0);
	wallTex.bindTexture();
	chairs_front_.DrawObj();
	POP;

	PUSH;
	glRotated(90, 1, 0, 0);
	wallTex.bindTexture();
	chairs_right_.DrawObj();
	POP;


	PUSH;
	glRotated(90, 1, 0, 0);
	wallTex.bindTexture();
	main_.DrawObj();
	POP;
	PUSH;
	glRotated(90, 1, 0, 0);
	wallTex.bindTexture();
	main_exit.DrawObj();
	POP;
	

	if (flag == 'K') {
		if(flagK){
			PlaySound("Sounds//ref.wav", NULL, SND_ASYNC);
		Sleep(3000);
		flagK = false;
		}
	}
	else
		flagK = true;
	PUSH;
	glRotated(90, 1, 0, 0);
	refTex.bindTexture();
	ref.DrawObj();
	POP;

	PUSH;
	glRotated(90, 1, 0, 0);
	refTex.bindTexture();
	bat.DrawObj();
	POP;
	PUSH;
	glRotated(90, 1, 0, 0);
	refTex.bindTexture();
	wash.DrawObj();
	POP;
	PUSH;
	glRotated(90, 1, 0, 0);
	leatherTex.bindTexture();
	divan.DrawObj();
	POP;
	PUSH;
	glRotated(90, 1, 0, 0);
	leatherTex.bindTexture();
	door_win.DrawObj();
	POP;
	PUSH;
	glRotated(90, 1, 0, 0);
	leatherTex.bindTexture();
	win.DrawObj();
	POP;
	PUSH;
	
	glRotated(90, 1, 0, 0);
	note.DrawObj();
	if (flag == 'O') {
		if (flagO) {
			PlaySound("Sounds//active.wav", NULL, SND_SYNC);
			flagO = false;
		}
		active.bindTexture();
		note.DrawObj();
	}
	else 
		flagO = true;
		

	//none_active.bindTexture();
	POP;
	if (flag == 'P') {
		if (flagP) {
			PlaySound("Sounds//dis.wav", NULL, SND_SYNC);
			flagP = false;
		}
		none_active.bindTexture();
		note.DrawObj();
	}
	else flagP = true;
}

void Render(OpenGL *ogl)
{   
	
	tick_o = tick_n;
	tick_n = GetTickCount();
	Time += (tick_n - tick_o) / 1000.0;

	/*
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 1, 0, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	*/



	//��������������
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;

	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
	//������ �����
	glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//===================================
	//������� ���  


	//





	//s[0].UseShader();

	////�������� ���������� � ������.  ��� ���� - ���� ����� uniform ���������� �� �� �����. 
	//int location = glGetUniformLocationARB(s[0].program, "light_pos");
	////��� 2 - �������� �� ��������
	//glUniform3fARB(location, light.pos.X(), light.pos.Y(),light.pos.Z());

	//location = glGetUniformLocationARB(s[0].program, "Ia");
	//glUniform3fARB(location, 0.2, 0.2, 0.2);

	//location = glGetUniformLocationARB(s[0].program, "Id");
	//glUniform3fARB(location, 1.0, 1.0, 1.0);

	//location = glGetUniformLocationARB(s[0].program, "Is");
	//glUniform3fARB(location, .7, .7, .7);


	//location = glGetUniformLocationARB(s[0].program, "ma");
	//glUniform3fARB(location, 0.2, 0.2, 0.1);

	//location = glGetUniformLocationARB(s[0].program, "md");
	//glUniform3fARB(location, 0.4, 0.65, 0.5);

	//location = glGetUniformLocationARB(s[0].program, "ms");
	//glUniform4fARB(location, 0.9, 0.8, 0.3, 25.6);

	//location = glGetUniformLocationARB(s[0].program, "camera");
	//glUniform3fARB(location, camera.pos.X(), camera.pos.Y(), camera.pos.Z());

	Shader::DontUseShaders();
	Game_show();
	//������ ��������
	//objModel.DrawObj();

	
	//Shader::DontUseShaders();

	//������, ��� ��������
	//glPushMatrix();
	//	glTranslated(-5,15,0);
	//	//glScaled(-1.0,1.0,1.0);
	//	objModel.DrawObj();
	//glPopMatrix();



	//��������

	//s[1].UseShader();
	//int l = glGetUniformLocationARB(s[1].program,"tex"); 
	//glUniform1iARB(l, 0);     //��� ��� ����� �� ��������� �������� ������� �� GL_TEXTURE0

	
	





	


	
	

	//////��������� ��������

	
	/*
	{

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0,1,0,1,-1,1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		frac.UseShader();

		int location = glGetUniformLocationARB(frac.program, "size");
		glUniform2fARB(location, (GLfloat)ogl->getWidth(), (GLfloat)ogl->getHeight());

		location = glGetUniformLocationARB(frac.program, "uOffset");
		glUniform2fARB(location, offsetX, offsetY);

		location = glGetUniformLocationARB(frac.program, "uZoom");
		glUniform1fARB(location, zoom);

		location = glGetUniformLocationARB(frac.program, "Time");
		glUniform1fARB(location, Time);

		DrawQuad();

	}
	*/
	
	
	//////���� �������
	
	/*
	{

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0,1,0,1,-1,1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		cassini.UseShader();

		int location = glGetUniformLocationARB(cassini.program, "size");
		glUniform2fARB(location, (GLfloat)ogl->getWidth(), (GLfloat)ogl->getHeight());


		location = glGetUniformLocationARB(cassini.program, "Time");
		glUniform1fARB(location, Time);

		DrawQuad();
	}

	*/

	
	
	

	
	//Shader::DontUseShaders();

	
	
}   //����� ���� �������


bool gui_init = false;

//������ ���������, ��������� ����� �������� �������
void RenderGUI(OpenGL *ogl)
{
	
	Shader::DontUseShaders();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_LIGHTING);
	

	glActiveTexture(GL_TEXTURE0);
	rec.Draw();


		
	Shader::DontUseShaders(); 



	
}

void resizeEvent(OpenGL *ogl, int newW, int newH)
{
	rec.setPosition(10, newH - 100 - 10);
}

