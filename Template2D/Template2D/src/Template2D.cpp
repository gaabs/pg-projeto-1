/*
-----------------------------------------------------------------------------
P2-11: Gráfico da Curvatura da Curva de Bézier Interativo:
Computer Science Center - CIn
Federal University of Pernambuco - UFPE

@author Giovanni Barros (gaabs) e Pedro Sereno (psg2)

Descrição: o usuário entra via mouse com os pontos de controle de uma curva de Bézier.
O número de pontos de controle é arbitrário, sem limite. O sistema desenha a curva correspondente,
e, numa outra janela, desenha o gráfico da curvatura da curva (normalizada para caber na janela).
O usuário pode modificar o posicionamento dos pontos, deletar e inserir pontos, e o sistema
responder em tempo real de forma adequada, reconstruindo a curva correspondente.
O usuário poderá suprimir os pontos de controle, a poligonal de controle, e os pontos da curva.
O usuário também poderá determinar o número de avaliações que deverá ser usado para então o
sistema calcular os correspondentes pontos da curva e ligá-los por retas.
As avaliações deverão ser feitas obrigatoriamente com o Algoritmo de de Casteljau,
tanto para a curva quanto para o cálculo das derivadas.

Funcionalidades:
1- Criar um Ponto com tamanho e cor aleatoria ao clicar com o botao
direito do mouse. A posicao e definida pela posicao atual do mouse.
2- Movimentar o Ponto segurando o botao esquerdo do mouse e arrastando.
3- Apagar o Ponto ao clicar com o botao do meio do mouse.
4- Esconder/mostrar os pontos de controle com a tecla 1.
5- Esconder/mostrar a poligonal de controle com a tecla 2.
6- Esconder/mostrar a curva de bezier com a tecla 3.
7- Alterar o número de avaliações com a tecla ' e escrevendo no console o novo número.
8- Limpar a tela apertando a tecla F5.
9- Sair do programa apertando a tecla ESC.

Referencias:
OpenGL Tutorial da VOXAR Labs
Curves and Surfaces for CAGD - Gerald Farin
http://www.math24.net/curvature-of-plane-curves.html
http://www.cs.mtu.edu/~shene/COURSES/cs3621/NOTES/spline/Bezier/bezier-sub.html
http://algorithmist.net/docs/subdivision.pdf
http://link.periodicos.capes.gov.br.ez16.periodicos.capes.gov.br/sfxlcl41?frbrVersion=5&ctx_ver=Z39.88-2004&ctx_enc=info:ofi/enc:UTF-8&ctx_tim=2015-11-20T09%3A33%3A38IST&url_ver=Z39.88-2004&url_ctx_fmt=infofi/fmt:kev:mtx:ctx&rfr_id=info:sid/primo.exlibrisgroup.com:primo3-Article-sciversesciencedirect_elsevier&rft_val_fmt=info:ofi/fmt:kev:mtx:&rft.genre=article&rft.atitle=Curvature%20of%20singular%20B%C3%A9zier%20curves%20and%20surfaces&rft.jtitle=Computer%20Aided%20Geometric%20Design&rft.btitle=&rft.aulast=Sederberg&rft.auinit=&rft.auinit1=&rft.auinitm=&rft.ausuffix=&rft.au=Sederberg,%20Thomas%20W.&rft.aucorp=&rft.date=2011&rft.volume=28&rft.issue=4&rft.part=&rft.quarter=&rft.ssn=&rft.spage=233&rft.epage=244&rft.pages=233-244&rft.artnum=&rft.issn=0167-8396&rft.eissn=&rft.isbn=&rft.sici=&rft.coden=&rft_id=info:doi/10.1016/j.cagd.2011.03.004&rft.object_id=&svc_val_fmt=info:ofi/fmt:kev:mtx:sch_svc&rft.eisbn=&rft_dat=%3Csciversesciencedirect_elsevier%3ES0167-8396(11)00030-6%3C/sciversesciencedirect_elsevier%3E%3Cgrp_id%3E6461839330533267273%3C/grp_id%3E%3Coa%3E%3C/oa%3E&rft_id=info:oai/&svc.fulltext=yes&req.language=por

-----------------------------------------------------------------------------
*/

#include "Template2D.h"
#include <stdio.h>
#include <math.h>
#include <algorithm>

#define maxPontos 1000

int qtdPontos;
int estado;
GLfloat mouse_x, mouse_y;
Ponto pontos[maxPontos];
Ponto pontosCurva[maxPontos];
Ponto pontosDivisao[maxPontos][maxPontos];
Ponto derivada1[maxPontos];
Ponto derivada2[maxPontos];
Ponto aux[maxPontos];
GLfloat window_width = 800.0;
GLfloat window_height = 600.0;
GLfloat window2_width = 400.0;
GLfloat window2_height = 300.0;

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

void myreshape2(GLsizei w, GLsizei h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	window2_width = (GLfloat)w;
	window2_height = (GLfloat)h;
	glOrtho(0, window2_width, 0, window2_height, -1.0, -1.0);
}

void calcDerivadas(int n, double t) {
	int N = numeroAvaliacoes;


	// Se t está além da metade, a primeira metade é a maior e vice versa.
	// Na subdivisão, considera-se o ponto da subdivisão o inicial.
	if (t > 0.5) {
		derivada1[n] = pontosDivisao[qtdPontos - 2][0] - pontosDivisao[qtdPontos - 1][0];
		derivada1[n + 1] = pontosDivisao[qtdPontos - 3][0] - pontosDivisao[qtdPontos - 2][0];
		derivada2[n] = derivada1[n + 1] - derivada1[n];
	}
	else {
		derivada1[n] = pontosDivisao[qtdPontos - 2][1] - pontosDivisao[qtdPontos - 1][0];
		derivada1[n + 1] = pontosDivisao[qtdPontos - 3][2] - pontosDivisao[qtdPontos - 2][1];
		derivada2[n] = derivada1[n + 1] - derivada1[n];
	}
}

void bezier() {
	glBegin(GL_LINE_STRIP);

	//int N = 100*qtdQuadrados;
	int N = numeroAvaliacoes;

	glVertex2f(pontos[0].x, pontos[0].y);

	double v = 1.0 / (N + 1); // v define a distância entre dois Ts consecutivos da avaliacao da curva, sendo eles equidistantes.


	for (int n = 0; n < N; n++) {
		int q = qtdPontos - 1;
		double x, y;
		double t = (n + 1)*v;

		for (int i = 0; i < qtdPontos; i++) {
			aux[i] = pontos[i];
			pontosDivisao[0][i] = pontos[i];
			//if (t == v) printf("(%d,%d) ", 0, i);
		}
		//if (t == v)  puts("");

		for (int a = 0; a < q; a++) {
			for (int i = 0; i < q - a; i++) {
				x = (1 - t)*aux[i].x + t*aux[i + 1].x;
				y = (1 - t)*aux[i].y + t*aux[i + 1].y;

				aux[i].x = x;
				aux[i].y = y;

				pontosDivisao[a + 1][i] = aux[i]; // Os pontos utilizados para encontrar o ponto da curva são armazenados para utilizar os pontos da subdivisão necessarios posteriormente
				//if (t == v) printf("(%d,%d) ", a + 1, i);
			}
			//if (t == v) puts("");
		}

		glColor3f(1, 0, 0);
		glVertex2f(aux[0].x, aux[0].y);
		pontosCurva[n].x = aux[0].x;
		pontosCurva[n].y = aux[0].y;

		calcDerivadas(n, t); // Para cada ponto de avaliação, leva-se em conta sua subdivisão e calcula-se as derivadas da maior das duas curvas da subdivisão para utilizar no calculo da curvatura;
	}


	glVertex2f(pontos[qtdPontos - 1].x, pontos[qtdPontos - 1].y);
	glEnd();
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

	if (qtdPontos >= 2 && SHOW_CURVE) bezier();

	glFlush();

}

void mydisplay2()
{
	//calcDerivadas();
	glutSetWindow(DOIS);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	if (qtdPontos > 2) {
		//http://www.math24.net/images/13sodi7.gif
		// k = (x'y'' - y'x'')/
		//	((x')^2 + (y')^2)^(3/2)
		double K[maxPontos];

		double maiorAbs = 0;
		double num, den;

		int N = numeroAvaliacoes;

		// Para cada avaliação da curva, considera-se sua subdivisão nesse ponto e se calcula sua curvatura considerando suas derivadas, segundo a fórmula acima.
		for (int i = 0; i < N - 2; i++) {
			num = derivada1[i].x*derivada2[i].y - derivada1[i].y*derivada2[i].x;
			den = (derivada1[i].x*derivada1[i].x) + (derivada1[i].y*derivada1[i].y);
			den = pow(den, 3);
			den = sqrt(den);
			K[i] = num / den;
			//printf("i:%d num:%lf den:%lf k:%lf\n", i, num,den, K[i]);
			maiorAbs = max(maiorAbs, fabs(K[i]));
		}

		glPointSize(GLfloat(5.0));

		double x;
		double y;


		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glBegin(GL_LINE_STRIP);
		for (int i = 0; i < N - 2; i++) {
			glColor3f(0, 1, 0);

			x = -1 + 2.0*i / (N - 2);
			y = K[i] / maiorAbs;

			x /= 4;
			x *= 3;

			y /= 4;
			y *= 3;
			y = abs(y);

			glVertex2f(x, y);
		}
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
	UM = glutCreateWindow("Curva de Bezier");

	glutDisplayFunc(mydisplay);
	glutReshapeFunc(myreshape);
	glutMouseFunc(handleMouse);
	glutMotionFunc(handleMotion);
	glutKeyboardUpFunc(hadleKeyboard);
	glutSpecialUpFunc(hadleSpecialKeyboard);
	glutPositionWindow(0, 100);

	glutInitWindowSize(window2_width, window2_height);
	DOIS = glutCreateWindow("Grafico da Curvatura");

	glutPositionWindow(900, 40);
	glutDisplayFunc(mydisplay2);
	glutReshapeFunc(myreshape2);

	myinit();
	glutMainLoop();
	return 0;
}