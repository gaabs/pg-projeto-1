/*
-----------------------------------------------------------------------------
OpenGL Tutorial
VOXAR Labs
Computer Science Center - CIn
Federal University of Pernambuco - UFPE
http://www.cin.ufpe.br/~voxarlabs

@author Tullio Lucena - tjsl

Este programa foi feito para mostrar conceitos basicos de OpenGl e glut para
a turma de Processamento Grafico do Centro de Informatica da UFPE.

Funcionalidades:
1-	Criar um Ponto com tamanho e cor aleatoria ao clicar com o botao
direito do mouse. A posicao e definida pela posicao atual do mouse.
2-	Movimentar o Ponto segurando o botao esquerdo do mouse e arrastando.
3-	Limpar a tela apertando a tecla F5.
4-	Sair do programa apertando a tecla ESC.

Referencias:
Funcoes de C/C++:
http://www.cplusplus.com/
Copia online do Red Book (OpenGL Programming Guide):
http://fly.cc.fer.hr/~unreal/theredbook/
Referencia online para os comandos OpenGL (Man pages):
http://www.opengl.org/sdk/docs/man/

-----------------------------------------------------------------------------
*/

#include "Template2D.h"
#include <windows.h>
#include <math.h>
 

#define maxPontos 1000

int qtdPontos;
int estado;
GLfloat mouse_x, mouse_y;
Ponto pontos[maxPontos];
Ponto derivada1[maxPontos];
Ponto derivada2[maxPontos];
Ponto aux[maxPontos];
GLfloat window_width = 800.0;
GLfloat window_height = 600.0;
int UM, DOIS;
GLfloat LADO = (GLfloat) 1.0 / 50;
int numeroAvaliacoes = 100;

bool SHOW_CONTROL_POINT;
bool SHOW_POLIGONAL;
bool SHOW_CURVE;

void myinit()
{
	srand(time(NULL));
	qtdPontos = 0;
	estado = MODIFIED;
	SHOW_CONTROL_POINT = SHOW_POLIGONAL = SHOW_CURVE = 1;
	loop(0);
}

void myreshape(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	window_width = (GLfloat)w;
	window_height = (GLfloat)h;
	glOrtho(0, window_width, 0, window_height, -1.0, -1.0);
}

void bezier() {
	//printf("entrou!\n");

	glBegin(GL_LINE_STRIP);
	glVertex2f(pontos[0].x, pontos[0].y);

	int N = numeroAvaliacoes;

	double v = 1.0 / (N + 1);

	for (int n = 0; n < N; n++) {

		int q = qtdPontos - 1;
		double x, y;
		double t = (n + 1)*v;

		for (int i = 0; i < qtdPontos; i++) {
			aux[i] = pontos[i];
		}

		while (q > 0) {
			for (int i = 0; i < q; i++) {
				x = (1 - t)*aux[i].x + t*aux[i + 1].x;
				y = (1 - t)*aux[i].y + t*aux[i + 1].y;

				aux[i].x = x;
				aux[i].y = y;

			}
			q--;
		}

		glColor3f(1, 0, 0);
		glVertex2f(aux[0].x, aux[0].y);
	}
	glVertex2f(pontos[qtdPontos - 1].x, pontos[qtdPontos - 1].y);
	glEnd();
}

void calcDerivadas() {
	for (int i = 0; i < qtdPontos-1; i++) {
		derivada1[i] = pontos[i + 1] - pontos[i];
	}
	for (int i = 0; i < qtdPontos - 2; i++) {
		derivada2[i] = derivada1[i + 1] - derivada1[i];
	}
}

void mydisplay()
{
	glutSetWindow(UM);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	if (SHOW_CONTROL_POINT) {
		glPointSize(GLfloat(5.0));
		glBegin(GL_POINTS);
		for (int i = 0; i < qtdPontos; i++) {
			glColor3f(pontos[i].r, pontos[i].g, pontos[i].b);
			glVertex2f(pontos[i].x, pontos[i].y);
		}
		glEnd();
	}


	if (SHOW_POLIGONAL) {
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glBegin(GL_LINE_STRIP);
		for (int i = 0; i < qtdPontos; i++) {
			glColor3f(pontos[i].r, pontos[i].g, pontos[i].b);
			glVertex2f(pontos[i].x, pontos[i].y);
		}
		glEnd();
	}

	if (qtdPontos > 2 && SHOW_CURVE) bezier();

	glFlush();

}

void mydisplay2()
{
	calcDerivadas();
	//http://www.math24.net/images/13sodi7.gif

	glutSetWindow(DOIS);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	// k = (x'y'' - y'x'')/
	//	((x')^2 + (y')^2)^(3/2)
	double K[maxPontos];

	double maiorAbs = 0;

	for (int i = 0; i < qtdPontos-2; i++) {
		K[i] = (derivada1[i].x*derivada2[i].y - derivada1[i].y*derivada2[i].x) / ((derivada1[i].x*derivada1[i].x) + (derivada1[i].y*derivada1[i].y));
		//printf("i:%d k:%lf\n", i, K[i]);
		maiorAbs = max(maiorAbs, fabs(K[i]));
	}

	glPointSize(GLfloat(5.0));
	glBegin(GL_POINTS);
	for (int i = 0; i < qtdPontos - 2; i++) {
		glColor3f(0, 1, 0);
		glVertex2f(-1 + 2.0*i / (qtdPontos - 3), K[i] / maiorAbs);
		//printf("i:%d qtd:%d div:%lf\n", i, qtdPontos-3, (1.0*i)/(qtdPontos - 3) );
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < qtdPontos - 2; i++) {
		glColor3f(0, 1, 0);
		glVertex2f(-1 + 2.0*i/(qtdPontos-3), K[i]/maiorAbs);
	}
	glEnd();

	glFlush();
}

void handleMotion(int x, int y)
{
	if ((estado != MODIFIED) && (estado != IDLE)) {
		pontos[estado].x = (((((GLfloat)x) / window_width)*2.0) - 1.0) + mouse_x;
		pontos[estado].y = (-(((((GLfloat)y) / window_height)*2.0) - 1.0)) + mouse_y;
	}
}

//x e y sao as coordenas onde cliquei na tela
//x2 e y2 eh onde cliquei so que ele converte pra as coordenadas de tela do OpenGL
void handleMouse(int btn, int state, int x, int y)
{
	if (estado == IDLE && btn == GLUT_LEFT_BUTTON) {
		mouse_x = ((((GLfloat)x) / window_width)*2.0) - 1.0;
		mouse_y = -(((((GLfloat)y) / window_height)*2.0) - 1.0);
		//printf("x:%lf y:%lf\n", mouse_x, mouse_y);
		if (state == GLUT_DOWN) {
			for (int i = qtdPontos - 1; i >= 0; i--) {
				if ((mouse_x >= pontos[i].x - LADO) && (mouse_x <= (pontos[i].x + LADO)) && (mouse_y <= pontos[i].y + LADO) && (mouse_y >= (pontos[i].y - LADO))) {
					mouse_x = pontos[i].x - mouse_x;
					mouse_y = pontos[i].y - mouse_y;
					estado = i;
					break;
				}
			}
		}
	}
	else if (btn == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		estado = MODIFIED;
	}

	else if (estado == IDLE && btn == GLUT_MIDDLE_BUTTON) {
		mouse_x = ((((GLfloat)x) / window_width)*2.0) - 1.0;
		mouse_y = -(((((GLfloat)y) / window_height)*2.0) - 1.0);
		if (state == GLUT_DOWN) {
			for (int i = qtdPontos - 1; i >= 0; i--) {
				if ((mouse_x >= pontos[i].x - LADO) && (mouse_x <= (pontos[i].x + LADO)) && (mouse_y <= pontos[i].y + LADO) && (mouse_y >= (pontos[i].y - LADO))) {
					mouse_x = pontos[i].x - mouse_x;
					mouse_y = pontos[i].y - mouse_y;
					for (int j = i; j < qtdPontos - 1; j++) {
						pontos[j] = pontos[j + 1];
					}
					qtdPontos--;

					estado = MODIFIED;
					break;
				}
			}
		}
	}


	else if (estado == IDLE && btn == GLUT_RIGHT_BUTTON) {
		if (state == GLUT_DOWN) {
			GLfloat x2 = ((((GLfloat)x) / window_width)*2.0) - 1.0;
			GLfloat y2 = -(((((GLfloat)y) / window_height)*2.0) - 1.0);

			pontos[qtdPontos++] = Ponto(x2, y2,
				((GLfloat)(rand() % 256)) / 255.0, ((GLfloat)(rand() % 256)) / 255.0, ((GLfloat)(rand() % 256)) / 255.0);
			estado = MODIFIED;
		}
	}
}

void hadleKeyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case (ESC) :
		exit(0);
		break;

	case ('1') :
		SHOW_CONTROL_POINT = !SHOW_CONTROL_POINT;
		estado = MODIFIED;
		break;

	case ('2') :
		SHOW_POLIGONAL = !SHOW_POLIGONAL;
		estado = MODIFIED;
		break;

	case ('3') :
		SHOW_CURVE = !SHOW_CURVE;
		estado = MODIFIED;
		break;

	case ('\'') :
		scanf("%d", &numeroAvaliacoes);
		estado = MODIFIED;
		break;

	}
}

void hadleSpecialKeyboard(int key, int x, int y)
{
	if (key == GLUT_KEY_F5) {
		myinit();
	}
}

void loop(int id)
{
	if (estado == MODIFIED) {
		mydisplay();
		mydisplay2();
		estado = IDLE;
	}
	else if (estado != IDLE) {
		mydisplay();
		mydisplay2();
	}
	glutTimerFunc(1000 / FPS, loop, id);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(window_width, window_height);
	UM = glutCreateWindow("Curva de Bézier");

	HDC hDC = NULL;
	//HDC hDC = GetDC(win1); /* get the device context for a particular window */
	/* snip */
	HGLRC hRC;
	hRC = wglCreateContext(hDC); /* get a render context for the same window */
								 /* repeat with hDC2 and hRC2 with another window handle*/

	glutDisplayFunc(mydisplay);
	glutReshapeFunc(myreshape);
	glutMouseFunc(handleMouse);
	glutMotionFunc(handleMotion);
	glutKeyboardUpFunc(hadleKeyboard);
	glutSpecialUpFunc(hadleSpecialKeyboard);


	//GLuint win2 = glutCreateSubWindow(win1, 0, 0, 200, 200);
	glutInitWindowSize(window_width / 2, window_height / 2);
	DOIS = glutCreateWindow("Gráfico");

	glutPositionWindow(900, 40);
	glutDisplayFunc(mydisplay2);
	glutReshapeFunc(myreshape);

	myinit();
	glutMainLoop();
	return 0;
}
