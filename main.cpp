#include <GL/gl.h>
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>

//variavel que armazena o tamanho da janela
GLfloat win = 50.0f;
//medidas padrão dos objetos
//ao dizer padrão significa inicial, os objetos podem sofrer escalonamentos
// e então podem haver na cena objetos menores ou maiores que essas medidas
GLfloat comprimentoBase = 25,//medida padrão do comprimento da base do robô
        alturaBase = 7,//medida padrão da altura da base do robô
        diametroCirculo = 10,//medida padrão do diametro das circunferencias
        alturaBraco = (diametroCirculo *8.0f);// medida padrão da altura dos braços do robô

//váriaveis que armazenam os angulos de rotação das juntas dos braços e dedos, são valores iniciais que podem
//sofrer alterações devido à  rotações durante a execução do projeto
GLfloat anguloPrimeiroBraco = 0,//angulo da base com o primeiro braço
        anguloSegundoBraco = -125, //angulo de rotação entre o segundo e o primeiro braço
        anguloTerceiroBraco = 55, // angulo de rotação entre o terceiro e o segundo braço
        anguloDedo1 = 40,//angulo de rotação entre o dedo e mão(circulo que indica a mão)
        anguloDedo2 = 140,//angulo de rotação entre o dedo e mão(circulo que indica a mão)
        anguloPontaDedo1 = -30,//angulo de rotação entre a ponta do dedo 1 e o inicio do dedo 1
        anguloPontaDedo2 = -160;// angulo de rotação entre a ponta do dedo 2 e o inicio do dedo 2

//variavel que armazena um valor simplificado de PI para desenhar circulos
const int PI = 3.14159;

GLfloat inicioBaseRoboX = 0.0f,
        inicioBaseRoboY = 0.0f;

GLfloat posicaoBraco2 = 37.0f;
GLfloat posicaoBraco3 = 21.5f;


//função que remapeia as coordenadas ao sofrer um redimensionamento de tela
void RedimensionaJanela(GLsizei w, GLsizei h)
{
    // Especifica as dimens�es da Viewport
    glViewport(0, 0, w, h);

    // Inicializa o sistema de coordenadas
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D (-win, win, -win, win);
}


// equação para dois pontos
float equacao2(int p1, int p2, float t) {
    float resultado = 0.0;
    resultado = ((1 - t) * p1) + (t * p2);
    return resultado;
}

//equação para 3 pontos
float equacao3(int p1, int p2, int p3, float t) {
    float resultado = 0.0;
    resultado = (pow((1 - t),2) * p1) + (((2*t)*(1 - t)) * p2) + (pow(t,2)*p3);
    return resultado;
}

//equação para 4 pontos.
float equacao4(int p1, int p2, int p3, int p4, float t) {
    float resultado = 0.0;
    resultado = (pow((1 - t),3) * p1) + (((3*t)*pow((1 - t),2)) * p2) + ((3*pow(t,2))*((1-t)*p3)) + (pow(t,3) * p4);
    return resultado;
}

// função que desenha a linha entre dois pontos,
//recebe as coordenadas de cada ponto, chama as equações para cada ponto com t variando de 0 a 1
//e desenha a linha com o retorndo das equações
static void desenhaEq2(int x1, int x2, int y1, int y2) {
    glBegin(GL_LINE_STRIP);
        for(float t = 0.0f; t <= 1.0f; t = t + 0.001f){
            float coordx = equacao2(x1,x2, t);
            float coordy = equacao2(y1,y2, t);
            glVertex2d(coordx,coordy);
        }
    glEnd();
    glFlush( );
}

// função que desenha a linha entre três pontos,
//recebe as coordenadas de cada ponto, chama as equações passando as coordenadas dos pontos com t variando de 0 a 1
//e desenha a linha com o retorndo das equações
static void desenhaEq3(int x1, int x2, int x3, int y1, int y2, int y3) {
    glBegin(GL_LINE_STRIP);
        for(float t = 0.0f; t <= 1.0f; t = t + 0.001f){
            float coordx = equacao3(x1,x2,x3, t);
            float coordy = equacao3(y1,y2,y3, t);
            glVertex2d(coordx,coordy);
        }
    glEnd();
    glFlush( );
}

// função que desenha a linha entre quatro pontos,
//recebe as coordenadas de cada ponto, chama as equações passando as coordenadas dos pontos com t variando de 0 a 1
//e desenha a linha com o retorndo das equações
static void desenhaEq4(int x1, int x2, int x3, int x4, int y1, int y2, int y3, int y4) {
    glBegin(GL_LINE_STRIP);
        for(float t = 0.0f; t <= 1.0f; t = t + 0.001f){
            float coordx = equacao4(x1,x2,x3,x4, t);
            float coordy = equacao4(y1,y2,y3,y4, t);
            glVertex2d(coordx,coordy);
        }
    glEnd();
    glFlush();
}
//função para desenhar eixos na origem dos sistemas de coordenadas
void eixo() {
    glColor3f(0.0,0.0,0.0);
    glBegin(GL_LINES);
        glVertex2f(-win,0);
        glVertex2f(win,0);
    glEnd();
    glBegin(GL_LINES);
        glVertex2f(0,-win);
        glVertex2f(0,win);
    glEnd();
    glFlush();
}

//função que desenha um circulo preenchido
void circulo(){
    //o raio do circulo é baseado na variavel global que define o diametro, o raio é sempre a metade do diamentro
    float raio = diametroCirculo / 2;

    double val = PI/ 180.0;
    glBegin(GL_TRIANGLE_FAN);
        //define 360 pontos para a circunferencia
        for(int i = 0; i <= 360; i++){
            glVertex2f(cos(i * val) * raio,sin(i * val) * raio);
        }
    glEnd();
}

//desenha uma primitiva de quadrado de tamano 1x1
void quadrado(){
    glBegin(GL_QUADS);
        glVertex2f(0.5f,-0.5f);
        glVertex2f(0.5f,0.5f);
        glVertex2f(-0.5f,0.5f);
        glVertex2f(-0.5f,-0.5f);
    glEnd();
}

//desenha um triangulo isoceles de base 1 e
void triangulo(){
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(-0.5f,0.0f);
        glVertex2f(0.5f,0.0f);
        glVertex2f(0.0f,0.5f);
    glEnd();
}

//objeto que fica no centro da junta da base
void ligacaoBase() {
    glPushMatrix();
    glColor3f(1.0f, 0.0f, 1.0f);
    glScaled(comprimentoBase*0.2, alturaBase*0.3,0);
    quadrado();
    glPopMatrix();
}

void ligacaoEmCruz() {
    //eixo();
    glColor3f(2.0f, 0.5f, 1.0f);
    glPushMatrix();
        glRotated(125.0f,0.0f,0.0f,1.0f);
        glScaled(comprimentoBase*0.15, alturaBase*0.2,0);
        quadrado();
    glPopMatrix();
    glPushMatrix();
        //eixo();
        glRotated(35.0f,0.0f,0.0f,1.0f);
        glScaled(comprimentoBase*0.15, alturaBase*0.2,0);
        quadrado();
    glPopMatrix();
}

void base() {
    glPushMatrix();
        glScaled(comprimentoBase,alturaBase,0);
        glColor3f(1.0,0.5,0.0);
        quadrado();
    glPopMatrix();
}

void inicioDedo() {
    glPushMatrix();
        glTranslated(3.5f,0.0f,0.0f);
        glScaled(comprimentoBase * 0.3f,alturaBase * 0.3f,0);
        glColor3f(0.0f, 0.5f, 1.0f);
        quadrado();
    glPopMatrix();
}

void junta() {
    glPushMatrix();
    circulo();
    glPopMatrix();
}

void braco1() {
    glPushMatrix();
        glColor3f(0.0f, 0.5f, 1.0f);
        glScaled(diametroCirculo,alturaBraco,0);
        triangulo();
    glPopMatrix();
}

void juntaBraco1() {
    glColor3f(1.0f, 0.5f, 0.0f);
    junta();
    glPushMatrix();
        glTranslated(0,0,0);
        ligacaoBase();
    glPopMatrix();
}

void braco2() {
    glRotated(anguloSegundoBraco,0,0,1);
    glPushMatrix();
        glColor3f(0.0f, 0.5f, 1.0f);
        glScaled(diametroCirculo * 0.7f,alturaBraco * 0.6f,0);
        triangulo();
    glPopMatrix();
}

void juntaBraco2() {
    glColor3f(1.0f, 0.5f, 0.0f);
    glPushMatrix();
        glScaled(0.7f,0.7f,0);
        junta();
    glPopMatrix();

    ligacaoEmCruz();
}

void braco3() {
    glRotated(anguloTerceiroBraco,0,0,1);
    glPushMatrix();
        glColor3f(0.0f, 0.5f, 1.0f);
        glScaled(diametroCirculo * 0.5f,alturaBraco * 0.4f,0);
        triangulo();
    glPopMatrix();
}

void juntaBraco3() {
    glColor3f(1.0f, 0.5f, 0.0f);
    glPushMatrix();
        glScaled(0.5f,0.5f,0);
        junta();
    glPopMatrix();
    glPushMatrix();
        glScaled(0.7f,0.7f,0);
        ligacaoEmCruz();
    glPopMatrix();
}

void dedo1() {
     //dedo 1
    glPushMatrix();
        glTranslated(0.0f,0.0f,0.0f);
        glRotated(anguloDedo1,0.0f,0.0f,1.0f);
        inicioDedo();
     //ponta do dedo 1
        glTranslated(7.0f, 0.0f, 0.0f);

        glPushMatrix();
            glColor3f(0.0f, 0.5f, 1.0f);
            glRotated(anguloPontaDedo1,0.0f,0.0f,1.0f);
            glScaled(diametroCirculo * 0.2f,alturaBraco * 0.2f,0);
            triangulo();
        glPopMatrix();

        // junta do dedo 1
        glColor3f(1.0f, 0.5f, 0.0f);
        glPushMatrix();
            glScaled(0.2f,0.2f,0);
            junta();
        glPopMatrix();
    glPopMatrix();
}

void dedo2() {
    glPushMatrix();
        glTranslated(0.0f,0.0f,0.0f);
        glRotated(anguloDedo2,0.0f,0.0f,1.0f);
        inicioDedo();
         //ponta do dedo 2
        glTranslated(7.0f, 0.0f, 0.0f);
        //glRotated(anguloSegundoBraco,0,0,1);
        glPushMatrix();
            glColor3f(0.0f, 0.5f, 1.0f);
            glRotated(anguloPontaDedo2,0.0f,0.0f,1.0f);
            glScaled(diametroCirculo * 0.2f,alturaBraco * 0.2f,0);
            triangulo();
        glPopMatrix();

        // junta do dedo 2
        glColor3f(1.0f, 0.5f, 0.0f);
        glPushMatrix();
            glScaled(0.2f,0.2f,0);
            junta();
        glPopMatrix();
    glPopMatrix();

}

void mao() {
    dedo1();
    dedo2();
     // junta da mão
    glPushMatrix();
        glColor3f(1.0f, 0.5f, 0.0f);
        glScaled(0.3f,0.3f,0);
        junta();
    glPopMatrix();
}

void robo(){
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslated(inicioBaseRoboX, inicioBaseRoboY,0.0f);
    base();
    //primeiro braço
    glTranslated(0.0f,2.0f,0.0f);
    glRotated(anguloPrimeiroBraco,0,0,1);
    braco1();
    //primeira junta
    juntaBraco1();
    //segundo braço
    glTranslated(0, posicaoBraco2, 0);
    braco2();
    // junta do proximo braço
    juntaBraco2();
    //terceiro braco
    glTranslated(0, posicaoBraco3, 0);
    braco3();
    // junta do terceiro braço
    juntaBraco3();
    glTranslated(0.0f,15.0f,0.0f);
    mao();
}

void info(){
    char info[6][80] = {
        "Teclas de controle: ",
        "A e S movem a base para esquerda ou direita.",
        "Q e W rotacionam o bloco azul.",
        "E e R rotacionam o bloco verde.",
        "T e Y rotacionam o bloco preto.",
        "U e I rotacionam o bloco laranja"
    };

    float linha = -1.3;
    glColor3f(0,0,0);
     for(int j = 0; j < 6; j++ ){
        glRasterPos2f(-2,linha);//posição da linha
        int len;
        len = strlen(info[j]);
        char texto[len];

        strcpy(texto,info[j]);
        for ( int i = 0; i < len; ++i ){
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, texto[i]);
        }
        linha = linha - 0.3;
     }

}

void desenhaRobo(void){
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-win,win,-win,win);
    robo();
    glFlush();
}

void comandosRotacao(unsigned char key) {
    switch(key){
        case 'a':
            anguloPrimeiroBraco++;
            desenhaRobo();
            break;
        case 'A':
                anguloPrimeiroBraco--;
            desenhaRobo();
            break;
        case 's':
            anguloSegundoBraco++;
            desenhaRobo();
            break;
        case 'S':
            anguloSegundoBraco--;
            desenhaRobo();
            break;
        case 'd':
            anguloTerceiroBraco++;
            desenhaRobo();
            break;
        case 'D':
            anguloTerceiroBraco--;
            desenhaRobo();
            break;
        case 'f':
            anguloDedo1++;
            desenhaRobo();
            break;
        case 'F':
            anguloDedo1--;
            desenhaRobo();
            break;
        case 'g':
            anguloPontaDedo1++;
            desenhaRobo();
            break;
        case 'G':
            anguloPontaDedo1--;
            desenhaRobo();
            break;
        case 'h':
            anguloDedo2++;
            desenhaRobo();
            break;
        case 'H':
            anguloDedo2--;
            desenhaRobo();
            break;
        case 'j':
            anguloPontaDedo2++;
            desenhaRobo();
            break;
        case 'J':
            anguloPontaDedo2--;
            desenhaRobo();
            break;
    }
}

void comandosTranslacaoRobo(int key,int x, int y) {
    switch(key){
        case 102:
            inicioBaseRoboX++;
            desenhaRobo();
            break;
        case 100:
            inicioBaseRoboX--;
            desenhaRobo();
            break;
        case 101:
            inicioBaseRoboY++;
            desenhaRobo();
            break;
        case 103:
            inicioBaseRoboY--;
            desenhaRobo();
            break;
    }
}

void comandosDiagonais(unsigned char key) {
    switch(key) {
        case 'p':
            inicioBaseRoboX++;
            inicioBaseRoboY++;
            desenhaRobo();
            break;
        case 'P':
            inicioBaseRoboX--;
            inicioBaseRoboY--;
            desenhaRobo();
            break;
    }
}

void comandosTranslacaoBracos(unsigned char key) {
    switch(key) {
    case 'w':
            if(posicaoBraco2 > 2) {
                posicaoBraco2--;
            }
            desenhaRobo();
            break;
        case 'W':
            if(posicaoBraco2 <= 37) {
                posicaoBraco2++;
            }
            desenhaRobo();
            break;
        case 'e':
            if(posicaoBraco3 > 0) {
                posicaoBraco3--;
            }
            desenhaRobo();
            break;
        case 'E':
            if(posicaoBraco3 <= 21.5) {
                posicaoBraco3++;
            }
            desenhaRobo();
            break;
    }
}

void movimentosTeclado(unsigned char key,int x, int y) {
    comandosRotacao(key);
    comandosDiagonais(key);
    comandosTranslacaoBracos(key);
}

int main(void){
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(600,600);
    glutInitWindowPosition(300,150);
    glutCreateWindow("Robo");
    glutDisplayFunc(desenhaRobo);
    glutReshapeFunc(RedimensionaJanela);
    glutKeyboardFunc(movimentosTeclado);
    glutSpecialFunc(comandosTranslacaoRobo);
    glClearColor(1,1,1,1);
    glutMainLoop();
    return 0;
}
