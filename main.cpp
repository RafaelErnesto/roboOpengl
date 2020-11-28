#include <GL/gl.h>
#include <GL/glut.h>
#include <math.h>
#include <stdio.h>

 GLfloat win = 600.0f;
//medidas padrão dos objetos
//ao dizer padrão significa inicial, os objetos podem sofrer escalonamentos
// e então podem haver na cena objetos menores ou maiores que essas medidas
 GLfloat comprimentoBase = 65,//medida padrão do comprimento da base do robô
        alturaBase = 20,//medida padrão da altura da base do robô
        diametroCirculo = 25,//medida padrão do diametro das circunferencias
        alturaBraco = 150.0f;// medida padrão da altura dos braços do robô

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

 //variaveis que guardam os valores das coordenadas da base do robo
 GLfloat inicioBaseRoboX = 300.0f,
        inicioBaseRoboY = -300.0f;

//variaveis que quardam o valor da coordenada y dos braços que podem ter translaçao.
 GLfloat posicaoBraco2 = 70.0f;
 GLfloat posicaoBraco3 = 41.5f;

 //variavel que armazena o angulo de reflexao do robô em relação a y
 GLfloat anguloReflexao = 0.0f;

 //flag que quando true, permite a reflexao do robõ
 GLboolean realizarReflexaoRobo = false;

 //flag que indica se o modo de seleção de pontos está ativado
 GLboolean selecaoPontosAtivado = false;
 //variavel que guarda o input do menu clicado pelo usuario, ela recebe a quantidade de pontos que o usuario quer selecionar.
 int quantidadePontosEsperados = 0;

 //variavel que marca quantos pontos já foram selecionados pelo usuario
 int quantidadePontosSelecionados = 0;
 //váriavel que quarda as coordenadas dos pontos selecionados.
 GLfloat pontosSelecionados[4][2] = {0,0};

 //variavel que quarda a altura da tela(até mesmo quando a redimensionamento), para ajustar o valor das coordenadas do eixo y
GLsizei alturaTela = 0;

//função que remapeia as coordenadas ao sofrer um redimensionamento de tela
void RedimensionaJanela(GLsizei w, GLsizei h){
    alturaTela = h;
    // Especifica as dimensões da Viewport
    glViewport(0, 0, w, h);
    // Inicializa o sistema de coordenadas
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //origem do sistema no lado superior esquerdo, com eixo orientado para baixo.
    gluOrtho2D(0.0f, w, -h, 0.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
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

//desenha uma primitiva de quadrado de tamanho 1x1
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

//objeto que desenha um "-" na junta da base com o primeiro braço
void ligacaoBase() {
    glPushMatrix();
        glColor3f(0.5f, 0.0f, 0.5f);
        glScaled(comprimentoBase*0.2, alturaBase*0.3,0);
        quadrado();
    glPopMatrix();
}

//função que desenha um objeto no formato de "+" nas juntas em que há rotação e translação dos braços.
void ligacaoEmCruz() {
    glColor3f(0.5f, 0.0f, 0.5f);
     glPushMatrix();
        glRotated(125.0f,0.0f,0.0f,1.0f);
        glScaled(comprimentoBase*0.15, alturaBase*0.2,0);
        quadrado();
    glPopMatrix();
    glPushMatrix();
        glRotated(35.0f,0.0f,0.0f,1.0f);
        glScaled(comprimentoBase*0.15, alturaBase*0.2,0);
        quadrado();
    glPopMatrix();
}

//função que desenha a base do robô
void base() {
    glPushMatrix();
        glScaled(comprimentoBase,alturaBase,0);
        glColor3f(1.0,0.5,0.0);
        quadrado();
    glPopMatrix();
}

//função que desenha a junta entre base e braço, e entre braços do robô
void junta() {
    glPushMatrix();
        //cor laranja para a junta
        glColor3f(1.0f, 0.5f, 0.0f);
        circulo();
    glPopMatrix();
}

//função que desenha o primeiro braço(o que está ligado à base do robô)
void braco1() {
    glPushMatrix();
        glColor3f(0.0f, 0.5f, 1.0f);
        glScaled(diametroCirculo,alturaBraco,0);
        triangulo();
    glPopMatrix();
}
//função que desenha a primeira junta entre a base e o primeiro braço do robô e já a diciona à junta o simbolo de "-"
void juntaBraco1() {
    //desenha a junta
    junta();
    glPushMatrix();
        //mode a ligação("-") para a origem do sistema de coordenadas da junta.
        glTranslated(0,0,0);
        ligacaoBase();
    glPopMatrix();
}

//função que desenha o segundo braço do robô
void braco2() {
    //rotação com uma variavel que quarda o valor do angulo de rotação do braço 2
    glRotated(anguloSegundoBraco,0,0,1);
    glPushMatrix();
        glColor3f(0.0f, 0.5f, 1.0f);
        //escalona o triangulo para ter as proporçoes da especificação
        glScaled(diametroCirculo * 0.7f,alturaBraco * 0.6f,0);
        triangulo();
    glPopMatrix();
}

//função que desenha a junta do segundo braço com o primeiro braço
void juntaBraco2() {
    glPushMatrix();
        glScaled(0.7f,0.7f,0);
        junta();
    glPopMatrix();
    //desenha a ligação em cruz, no centro da junta.
    ligacaoEmCruz();
}

//função que desenha o terceiro braço
void braco3() {
    //adiciona um angulo de rotação entre o terceiro braço e o segundo praço
    glRotated(anguloTerceiroBraco,0,0,1);
    glPushMatrix();
        glColor3f(0.0f, 0.5f, 1.0f);
        glScaled(diametroCirculo * 0.5f,alturaBraco * 0.4f,0);
        triangulo();
    glPopMatrix();
}
//função que desenha a junta entre o terceiro e o segundo braço, e também adiciona o objeto "+" no centro da junta
void juntaBraco3() {
    glPushMatrix();
        glScaled(0.5f,0.5f,0);
        junta();
    glPopMatrix();
    glPushMatrix();
        glScaled(0.7f,0.7f,0);
        ligacaoEmCruz();
    glPopMatrix();
}
//função que desenha a parte inicial do dedo
void inicioDedo() {
    glPushMatrix();
        glTranslated(10.5f,0.0f,0.0f);
        glScaled(comprimentoBase * 0.3f,alturaBase * 0.3f,0);
        glColor3f(0.0f, 0.5f, 1.0f);
        quadrado();
    glPopMatrix();
}
//função que desenha o primeiro dedo
void dedo1() {
    glPushMatrix();
        //mode a base da parte inicial do dedo na origem da junta da mão
        glTranslated(0.0f,0.0f,0.0f);
        //adiciona um angulo de rotação entre a junta da mão e o inicio do dedo.
        glRotated(anguloDedo1,0.0f,0.0f,1.0f);
        //desenha  a parte inicial do dedo
        inicioDedo();

        //agora vamos desenha a ponta do dedo
        //translação lara o final do inicio do dedo.
        glTranslated(20.0f, 0.0f, 0.0f);

        glPushMatrix();
            glColor3f(0.0f, 0.5f, 1.0f);
            //adiciona angulo de rotação entre a ponta do dedo e a parte inicial do dedo.
            glRotated(anguloPontaDedo1,0.0f,0.0f,1.0f);
            //escalona o triangulo para atender a especificação
            glScaled(diametroCirculo * 0.2f,alturaBraco * 0.2f,0);
            triangulo();
        glPopMatrix();

        //agora vamos colocar  a junta do dedo
        glPushMatrix();
            //escalona para atender a especificação
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
        glTranslated(20.0f, 0.0f, 0.0f);
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

//função que desenha a mão inteira do robo.
void mao() {
    //desenhamos 1 dedo
    dedo1();
    //depois o outro dedo.
    dedo2();
     // agora desenhamos a junta da mão do robo
    glPushMatrix();
        glScaled(0.3f,0.3f,0);
        junta();
    glPopMatrix();
}


//função que desenha o robô
void robo(){
    //aqui verificamos se é para realizar uma reflexão em y do robõ
    if(realizarReflexaoRobo) {
        //se verdade, setamos a variavel de controle para false, para não entrarmos em loop.
        realizarReflexaoRobo = false;
        //adicionamos ao valor atual de anguloReflexao mais 180 ao angulo de reflexão do robo.
        //isso fará o robo girar no proprio eixo y alguns comandos a abaixo.
        anguloReflexao += 180;
    }
    //translada a base do robo(e conseguentemente o robo inteiro) para onde os valores das varaiveis estao
    glTranslated(inicioBaseRoboX, inicioBaseRoboY,0.0f);
    //rotaciona o robo(aqui ele só sofrerá uma rotação se o valor tiver sido alterado no condicional logo acima)
    glRotated(anguloReflexao,0.0f,1.0f,0.0f);
    //chama a funçãoque desenha a base do robô
    base();

    //translada um pouco o sistema para cima
    glTranslated(0.0f,6.0f,0.0f);

    //e então desenhamos o primeiro braço adicionando um angulo de rotação entre o primeiro braço e a base do robô
    glRotated(anguloPrimeiroBraco,0,0,1);
    braco1();
    //agora chamamos a função que desenha a primeira junta, entre o braço 1 e  a base do robô
    juntaBraco1();


    //agora fazemos uma transação no eixo y de acordo com o valor da variavel abaixo,ela por padrão ja vem definida no
    //inicio do código, mas pode sofrer alteração no funcionamento do programa se o usuario quiser transladar o braço no eixo y.
    glTranslated(0, posicaoBraco2, 0);
    //função que desenha o braço 2
    braco2();
    // junta do braço2
    juntaBraco2();

    //agora vamos desenhar o terceiro braço, o que ocorre aqui é a mesma coisa do braço anterior, obviamente com uma váriavel diferente.
    glTranslated(0, posicaoBraco3, 0);
    braco3();
    // junta do terceiro braço
    juntaBraco3();
    //translação para posicionar a mão no final do terceiro braço
    glTranslated(0.0f,25.0f,0.0f);
    //chama a função que desenha a mão o robô
    mao();
}

void info(){
    char info[2][80] = {
        "IMPORTANTE",
        "Para reflexao do robo, presione a tecla r"
    };

    float linha = -50;
    glColor3f(0,0,0);
     for(int j = 0; j < 2; j++ ){
        glRasterPos2f(2,linha);//posição da linha
        int len;
        len = strlen(info[j]);
        char texto[len];

        strcpy(texto,info[j]);
        for ( int i = 0; i < len; ++i ){
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, texto[i]);
        }
        linha = linha - 25;
     }

}

//função que desenha os pontos selecionados na tela
static void desenhaPontos() {
    //quantidadePontosSelecionados é a quantidade de pontos que o usuario selecionou
    for(int i = 0; i < quantidadePontosSelecionados; i ++) {
        // pontos na cor preta
            glColor3f(0.0,0.0,0.0);
            glBegin(GL_POINTS);
                //a cada iteração, desenha um ponto selecionado pelo usuario.
                glVertex2d(pontosSelecionados[i][0],pontosSelecionados[i][1]);
            glEnd();
    }
}

//função que com base na quantidade de pontos que o usuario escolheu no menu para desenhar, chamará a função de desenho
//da curva correspondente passando a respectivas coordenadas dos pontos previamente selecionados pelo usuario.
void desenhaCurvas() {
    switch(quantidadePontosSelecionados){
        case 2:
            desenhaEq2(pontosSelecionados[0][0],pontosSelecionados[1][0],pontosSelecionados[0][1],pontosSelecionados[1][1]);
        break;
        case 3:
            desenhaEq3(pontosSelecionados[0][0],pontosSelecionados[1][0],pontosSelecionados[2][0],pontosSelecionados[0][1],pontosSelecionados[1][1],pontosSelecionados[2][1]);
        break;
        case 4:
            desenhaEq4(pontosSelecionados[0][0],pontosSelecionados[1][0],pontosSelecionados[2][0],pontosSelecionados[3][0],pontosSelecionados[0][1],pontosSelecionados[1][1],pontosSelecionados[2][1],pontosSelecionados[3][1]);
        break;
    }
}

//variavel necessária para as equações de curva, ela precisa variar de 0 à 1.
float t = 0.0f;
//variavel que chama as funções que calculam as coordenadas de acordo com os pontos selecionados e translada o robo nessas
//coordenadas fazendo o robô percorrer o trajeto das curvas
void fazRoboAndarNaCurva() {
            while(t <= 1.0) {
                switch(quantidadePontosSelecionados){
                    case 2:
                        inicioBaseRoboX = equacao2(pontosSelecionados[0][0],pontosSelecionados[1][0], t);
                        inicioBaseRoboY = equacao2(pontosSelecionados[0][1],pontosSelecionados[1][1], t);
                    break;
                    case 3:
                        inicioBaseRoboX = equacao3(pontosSelecionados[0][0],pontosSelecionados[1][0],pontosSelecionados[2][0], t);
                        inicioBaseRoboY = equacao3(pontosSelecionados[0][1],pontosSelecionados[1][1],pontosSelecionados[2][1], t);
                    break;
                    case 4:
                         inicioBaseRoboX = equacao4(pontosSelecionados[0][0],pontosSelecionados[1][0],pontosSelecionados[2][0],pontosSelecionados[3][0], t);
                        inicioBaseRoboY  = equacao4(pontosSelecionados[0][1],pontosSelecionados[1][1],pontosSelecionados[2][1],pontosSelecionados[3][1], t);
                    break;
                }
                //limpa a tela para da ro efeito de movimento.
                glClear(GL_COLOR_BUFFER_BIT);
                //glLoadIdentity();
                desenhaPontos();
                desenhaCurvas();
                glPushMatrix();
                    //desenha o robo.
                    robo();
                glPopMatrix();
                //troca os buffers para imprimir imediatamente o desenho na tela
                glutSwapBuffers();
                //incrementa a variavel
                 t += 0.001f;
            }

            quantidadePontosEsperados = 0;
            quantidadePontosSelecionados = 0;
            t = 0.0f;

             //enquanto a variavel t for menor ou igual a 1, glutimefunc chamará novamente esta mesma função(looping), após o tempo
             //especificado em milisegundos.
}

void desenhaCena(void){
    glClear(GL_COLOR_BUFFER_BIT);
    info();
    if((quantidadePontosEsperados == quantidadePontosSelecionados) && quantidadePontosSelecionados > 0) {
        if(quantidadePontosEsperados == quantidadePontosSelecionados) {
                fazRoboAndarNaCurva();
        }
    } else {
        glPushMatrix();
            robo();
        glPopMatrix();
        glutSwapBuffers();
    }
}

void comandosRotacao(unsigned char key) {
    switch(key){
        case 'a':
            anguloPrimeiroBraco++;
            desenhaCena();
            break;
        case 'A':
                anguloPrimeiroBraco--;
            desenhaCena();
            break;
        case 's':
            anguloSegundoBraco++;
            desenhaCena();
            break;
        case 'S':
            anguloSegundoBraco--;
            desenhaCena();
            break;
        case 'd':
            anguloTerceiroBraco++;
            desenhaCena();
            break;
        case 'D':
            anguloTerceiroBraco--;
            desenhaCena();
            break;
        case 'f':
            anguloDedo1++;
            desenhaCena();
            break;
        case 'F':
            anguloDedo1--;
            desenhaCena();
            break;
        case 'g':
            anguloPontaDedo1++;
            desenhaCena();
            break;
        case 'G':
            anguloPontaDedo1--;
            desenhaCena();
            break;
        case 'h':
            anguloDedo2++;
            desenhaCena();
            break;
        case 'H':
            anguloDedo2--;
            desenhaCena();
            break;
        case 'j':
            anguloPontaDedo2++;
            desenhaCena();
            break;
        case 'J':
            anguloPontaDedo2--;
            desenhaCena();
            break;
    }
}

void comandoReflexao(unsigned char key) {
    switch(key){
        case 'r':
            realizarReflexaoRobo = true;
            desenhaCena();
            break;
    }
}

void comandosTranslacaoRobo(int key,int x, int y) {
    switch(key){
        case 102:
            inicioBaseRoboX++;
            desenhaCena();
            break;
        case 100:
            inicioBaseRoboX--;
            desenhaCena();
            break;
        case 101:
            inicioBaseRoboY++;
            desenhaCena();
            break;
        case 103:
            inicioBaseRoboY--;
            desenhaCena();
            break;
    }
}

void comandosDiagonais(unsigned char key) {
    switch(key) {
        case 'p':
            inicioBaseRoboX++;
            inicioBaseRoboY++;
            desenhaCena();
            break;
        case 'P':
            inicioBaseRoboX--;
            inicioBaseRoboY--;
            desenhaCena();
            break;
    }
}

void comandosTranslacaoBracos(unsigned char key) {
    switch(key) {
    case 'w':
            if(posicaoBraco2 > 2) {
                posicaoBraco2--;
            }
            desenhaCena();
            break;
        case 'W':
            if(posicaoBraco2 <= 37) {
                posicaoBraco2++;
            }
            desenhaCena();
            break;
        case 'e':
            if(posicaoBraco3 > 0) {
                posicaoBraco3--;
            }
            desenhaCena();
            break;
        case 'E':
            if(posicaoBraco3 <= 21.5) {
                posicaoBraco3++;
            }
            desenhaCena();
            break;
    }
}

void movimentosTeclado(unsigned char key,int x, int y) {
    comandosRotacao(key);
    comandosDiagonais(key);
    comandosTranslacaoBracos(key);
    comandoReflexao(key);
}


void menuCurvas(int opcao) {
    switch(opcao) {
        case 2:
            quantidadePontosEsperados = 2;
            selecaoPontosAtivado = true;
        break;
        case 3:
            quantidadePontosEsperados = 3;
            selecaoPontosAtivado = true;
        break;
        case 4:
            quantidadePontosEsperados = 4;
            selecaoPontosAtivado = true;
        break;
    }
}

void selecionaPontos(int x, int y) {
    if(quantidadePontosSelecionados < quantidadePontosEsperados){
        pontosSelecionados[quantidadePontosSelecionados][0] = x;
        pontosSelecionados[quantidadePontosSelecionados][1] = -y;
        quantidadePontosSelecionados++;
    }

    if((quantidadePontosSelecionados == quantidadePontosEsperados) && quantidadePontosSelecionados >= 2){
        selecaoPontosAtivado = false;
    }
}

void menu() {
    glutCreateMenu(menuCurvas);
    glutAddMenuEntry("2 PONTOS",2);
    glutAddMenuEntry("3 PONTOS",3);
    glutAddMenuEntry("4 PONTOS",4);

    glutAttachMenu(GLUT_RIGHT_BUTTON);
}


void mouseEventos(int button, int state, int x, int y) {
    if (button == GLUT_RIGHT_BUTTON){
        if (state == GLUT_DOWN){
            menu();
         }
    }

    printf(" selecao de pontos ativada: %d \n", selecaoPontosAtivado);
    if (button == GLUT_LEFT_BUTTON && selecaoPontosAtivado){
        if (state == GLUT_DOWN){
            if(quantidadePontosEsperados > 0) {
                selecionaPontos(x, y);
            }
         }
    }

    glutPostRedisplay();
}
int main(void){
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(win,win);
    glutInitWindowPosition(10,10);
    glutCreateWindow("Robo");
    glutDisplayFunc(desenhaCena);
    glutReshapeFunc(RedimensionaJanela);
    glutKeyboardFunc(movimentosTeclado);
    glutSpecialFunc(comandosTranslacaoRobo);
    glutMouseFunc(mouseEventos);
    glPointSize(5.0);
    glClearColor(1,1,1,1);
    glutMainLoop();
    return 0;
}
