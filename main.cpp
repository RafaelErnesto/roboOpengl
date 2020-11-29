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


//função que desenha um circulo preenchido
void circulo(){
    //o raio do circulo é baseado na variavel global que define o diametro, o raio é sempre a metade do diamentro
    float raio = diametroCirculo / 2;

    double val = PI/ 180.0;//180 graus é igual a PI radianos, portando PI/180 é o valor em radiano por grau
    glBegin(GL_TRIANGLE_FAN);
        //define 360 pontos para a circunferencia
        for(int i = 0; i <= 360; i++){
            //a cada iteração as coordenadas são definidas utilizando radianos e então os pontos são desenhados na tela.
            //cosseno da altura * raio, seno da altura * raio, nos dão as coordenadas de cada ponto da circunferencia.
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

//desenha um triangulo
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
        //cor roxa
        glColor3f(0.5f, 0.0f, 0.5f);
        //escalonamento para atender a especificação(a especificação diz 70% da altura da base, porém fica desproporcional)
        glScaled(comprimentoBase*0.2, alturaBase*0.3,0);
        quadrado();
    glPopMatrix();
}

//função que desenha um objeto no formato de "+" nas juntas em que há rotação e translação dos braços.
void ligacaoEmCruz() {
    //cor roxa
    glColor3f(0.5f, 0.0f, 0.5f);
    //são dois pequenos retangulos obtidos através de escalonamento, e eles são rotacionados de modo que formem um +
     glPushMatrix();
        glRotated(125.0f,0.0f,0.0f,1.0f);
        glScaled(comprimentoBase*0.15, alturaBase*0.2,0);//a especificação não fiz nada sobre a medida do objeto +,portanto assumi um valor que achei adequado como base
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
        //escalona para atender a especificação
        glScaled(comprimentoBase,alturaBase,0);
        //cor azul
        glColor3f(1.0,0.5,0.0);
        quadrado();
    glPopMatrix();
}

//função que desenha a junta(circulo) entre base e braço, e entre braços do robô
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
        //escalonamento par atender a especificação
        glScaled(diametroCirculo,alturaBraco,0);
        triangulo();
    glPopMatrix();
}
//função que desenha a primeira junta(circulo) entre a base e o primeiro braço do robô e já a diciona à junta o simbolo de "-"
void juntaBraco1() {
    //desenha a junta
    junta();
    glPushMatrix();
        //move a ligação("-") para a origem do sistema de coordenadas da junta da base.
        glTranslated(0,0,0);
        ligacaoBase();
    glPopMatrix();
}

//função que desenha o segundo braço do robô
void braco2() {
    //rotação com uma variavel que quarda o valor do angulo de rotação do braço 2
    glRotated(anguloSegundoBraco,0,0,1);
    glPushMatrix();
        //cor azul
        glColor3f(0.0f, 0.5f, 1.0f);
        //escalona o triangulo para ter as proporçoes da especificação
        glScaled(diametroCirculo * 0.7f,alturaBraco * 0.6f,0);
        triangulo();
    glPopMatrix();
}

//função que desenha a junta do segundo braço com o primeiro braço
void juntaBraco2() {
    glPushMatrix();
        //escalonamento para atender a especificação
        glScaled(0.7f,0.7f,0);
        junta();
    glPopMatrix();
    //desenha a ligação em cruz(+), no centro da junta.
    ligacaoEmCruz();
}

//função que desenha o terceiro braço
void braco3() {
    //adiciona um angulo de rotação entre o terceiro braço e o segundo praço
    glRotated(anguloTerceiroBraco,0,0,1);
    glPushMatrix();
        //cor azul claro
        glColor3f(0.0f, 0.5f, 1.0f);
        //escalonamento para atender a especificação
        glScaled(diametroCirculo * 0.5f,alturaBraco * 0.4f,0);
        triangulo();
    glPopMatrix();
}
//função que desenha a junta entre o terceiro e o segundo braço, e também adiciona o objeto "+" no centro da junta
void juntaBraco3() {
    //tudo dentro de um push/pop para as transformações locais ficarem restritas.
    glPushMatrix();
        //escalonamento para a tender a especificação
        glScaled(0.5f,0.5f,0);
        junta();
    glPopMatrix();
    glPushMatrix();
        //escalonamento para atender a especificação
        glScaled(0.7f,0.7f,0);
        ligacaoEmCruz();
    glPopMatrix();
}
//função que desenha a parte inicial do dedo(retangular)
void inicioDedo() {
    glPushMatrix();
        //move o sistema para a origem da junta da mão do robo
        glTranslated(10.5f,0.0f,0.0f);
        //escalonamento para atender a especificação
        glScaled(comprimentoBase * 0.3f,alturaBase * 0.3f,0);
        //cor azul claro
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
            //escalona o triangulo para atender a especificação(na especificação diz que a base deve ser 30% do diametro base do circulo,
            // porém fica desproporcional com a junta do dedo(circulo) cujo diametro na especificação é 20% do diametro base do circulo)
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
        //transladamos o sistema para a origem da junta da mão
        glTranslated(0.0f,0.0f,0.0f);
        //adicionamos um angulo de rotação entre o dedo e a junta da mão
        glRotated(anguloDedo2,0.0f,0.0f,1.0f);
        //desenhamos o inicio do dedo(parte retangular do dedo)
        inicioDedo();
         //ponta do dedo 2
         //movemos o sistema para a ponta do inicio do dedo
        glTranslated(20.0f, 0.0f, 0.0f);
        //push matrix para que as transformações aqui dentro fiquem restritas
        glPushMatrix();
            glColor3f(0.0f, 0.5f, 1.0f);
            //adiciona um angulo entre a ponta do dedo e a parte inferior do dedo
            glRotated(anguloPontaDedo2,0.0f,0.0f,1.0f);
            //escalona a ponta do dedo(triangulo) de acordo com a especificação
            glScaled(diametroCirculo * 0.2f,alturaBraco * 0.2f,0);
            triangulo();
        glPopMatrix();

        // junta do dedo 2
        glPushMatrix();
            //escalonamento de acordo com a especificação
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
        //segundo a especificação o diametro dessa junta é 30% do diametro base do circulu, porém acho que nao fica
        // como no desenho, talvez 40% seria mais adequado.
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
        //isso fará o robo girar no proprio eixo y ao ser invocado o comando de rotação abaixo.
        anguloReflexao += 180;
    }
    //translada a base do robo(e conseguentemente o robo inteiro) para onde os valores das varaiveis estao
    glTranslated(inicioBaseRoboX, inicioBaseRoboY,0.0f);
    //rotaciona o robo(aqui ele só sofrerá uma rotação se o valor tiver sido alterado no condicional logo acima)
    glRotated(anguloReflexao,0.0f,1.0f,0.0f);
    //chama a função que desenha a base do robô
    base();

    //translada um pouco o sistema para cima
    glTranslated(0.0f,6.0f,0.0f);

    //e então desenhamos o primeiro braço adicionando um angulo de rotação entre o primeiro braço e a base do robô
    glRotated(anguloPrimeiroBraco,0,0,1);
    braco1();
    //agora chamamos a função que desenha a primeira junta, entre o braço 1 e  a base do robô
    juntaBraco1();

    //agora fazemos uma translação no eixo y de acordo com o valor da variavel abaixo,ela por padrão ja vem definida no
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
//função que printa informações na tela
void info(){
    //essas 4 frases abaixo serão printadas na tela, a ultima tem uma condição para ser printada
    char info[4][100] = {
        "IMPORTANTE",
        "Para reflexao do robo, presione a tecla r",
        "Para ativar o menu pressione o botao direito do mouse, e selecione quantos pontos deseja selecionar",
        "Modo de selecao de pontos ativado"
    };
    //espaço entre linhas
    float linha = -50;
    //por padrão cor preta para as linhas
    glColor3f(0,0,0);
    //percorre o array
     for(int j = 0; j < 4; j++ ){

        //se for a ultima linha e o modo de seleção de pontos estiver ativado...
        if(j == 3 && selecaoPontosAtivado) {
            //cor vermelha
            glColor3f(1.0f,0.0f,0.0f);
        //se for a ultima linha e não tiver o modo de seleção de pontos ativados...
        } else if( j == 3 && !selecaoPontosAtivado) {
            //não printa nada.
            continue;
        }
        //posição da linha em x e y
        glRasterPos2f(2,linha);//posição da linha
        //variavel qua guarda o comprimento da linha
        int len;
        len = strlen(info[j]);
        //cria um array como comprimento da linha que vai ser printada
        char texto[len];
        //adiciona a linha do array de info para o texto
        strcpy(texto,info[j]);
        //percorre o array texto printando caractere a caractere.
        for ( int i = 0; i < len; ++i ){
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, texto[i]);
        }
        //decrementa linha para printar a próxima linha mais abaixo da atual
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
//função que chama as funções que calculam as coordenadas de acordo com os pontos selecionados e translada o robo nessas
//coordenadas fazendo o robô percorrer o trajeto das curvas
void fazRoboAndarNaCurva() {
            //enquanto t for menor igual a 1
            while(t <= 1.0) {
                    //quantos pontos o usuario selecionou?
                switch(quantidadePontosSelecionados){
                    case 2://se 2, as variaveis que posicionam a base do robo, receberam o resultado da equação de 2 pontos
                        inicioBaseRoboX = equacao2(pontosSelecionados[0][0],pontosSelecionados[1][0], t);
                        inicioBaseRoboY = equacao2(pontosSelecionados[0][1],pontosSelecionados[1][1], t);
                    break;
                    case 3://se 3, as variaveis que posicionam a base do robo, receberam o resultado da equação de 3 pontos
                        inicioBaseRoboX = equacao3(pontosSelecionados[0][0],pontosSelecionados[1][0],pontosSelecionados[2][0], t);
                        inicioBaseRoboY = equacao3(pontosSelecionados[0][1],pontosSelecionados[1][1],pontosSelecionados[2][1], t);
                    break;
                    case 4://se 4, as variaveis que posicionam a base do robo, receberam o resultado da equação de 4 pontos
                         inicioBaseRoboX = equacao4(pontosSelecionados[0][0],pontosSelecionados[1][0],pontosSelecionados[2][0],pontosSelecionados[3][0], t);
                        inicioBaseRoboY  = equacao4(pontosSelecionados[0][1],pontosSelecionados[1][1],pontosSelecionados[2][1],pontosSelecionados[3][1], t);
                    break;
                }
                //limpa a tela para dar o efeito de movimento.
                glClear(GL_COLOR_BUFFER_BIT);
                desenhaPontos();//desenha os pontos selecionados
                desenhaCurvas();//desenha a curva nos pontos

                glPushMatrix();//push matrix para as alterações do desenho do robo ficarem restritas apenas ao robo
                    //desenha o robo.
                    robo();
                glPopMatrix();
                //troca os buffers para imprimir imediatamente o desenho na tela
                glutSwapBuffers();
                //incrementa a variavel
                 t += 0.001f;
            }
            //neste ponto, o robo já percorreu todo o trajeto da curva.
            //então vamos resetar as variaveis de controle
            quantidadePontosEsperados = 0;//não há mais pontos esperados, porque saimos do modo de pontos
            quantidadePontosSelecionados = 0;//não há mais pontos selecionados, porque o robo já percorreu os pontos selecionados
            t = 0.0f;//variavel da equação zerada novamente
}

//variavel responsavel por controlar o que será desenhado na tela
void desenhaCena(void){
    //limmpa o buffer
    glClear(GL_COLOR_BUFFER_BIT);
    //printa as informações na tela
    info();
    //desenha os pontos selecionados
    desenhaPontos();
    // se estamos no modo de seleção de pontos e já selecionamos todos os pontos que precisavamos,
    //esta na hora do robo percorrer o trajeto da curva
    if((quantidadePontosEsperados == quantidadePontosSelecionados) && quantidadePontosSelecionados > 0) {
        //chama função que faz o robo percorrer a curva
        fazRoboAndarNaCurva();
    } else {
        //se estamos no modo normal
        //push matrix para todas as transformações aplicadas para o desenho do robo, ficarem restritas somente ao robo
        glPushMatrix();
            //desenha o robo.
            robo();
        glPopMatrix();
        //troca os buffers para desenhar na tela.
        glutSwapBuffers();
    }
}
//função que com base na tecla pressionada faz parte correspondente do robo rotacionar.
//os botões e os sentidos das rotações estão conforme o especificado no trabalho
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
            anguloDedo2++;
            desenhaCena();
            break;
        case 'F':
            anguloDedo2--;
            desenhaCena();
            break;
        case 'g':
            anguloPontaDedo2++;
            desenhaCena();
            break;
        case 'G':
            anguloPontaDedo2--;
            desenhaCena();
            break;
        case 'h':
            anguloDedo1++;
            desenhaCena();
            break;
        case 'H':
            anguloDedo1--;
            desenhaCena();
            break;
        case 'j':
            anguloPontaDedo1++;
            desenhaCena();
            break;
        case 'J':
            anguloPontaDedo1--;
            desenhaCena();
            break;
    }
}

//função que com base na tecla pressionada faz o robo ter uma rotação em torno do eixo y(reflexão).
//os botões e os sentidos das rotações estão conforme o especificado no trabalho
void comandoReflexao(unsigned char key) {
    switch(key){
        case 'r':
            realizarReflexaoRobo = true;
            desenhaCena();
            break;
    }
}

//função que com base na tecla pressionada faz o robo transladar.
//os botões e os sentidos das rotações estão conforme o especificado no trabalho
//as variavies inicioBaseRoboX e inicioBaseRoboY, especificam o ponto central da base do robo, quando elas mudam, o robo translada
//conforme o valor delas.
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

//função que com base na tecla pressionada faz o robo andar na diagonal
//os botões e os sentidos das rotações estão conforme o especificado no trabalho
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

//função que com base na tecla pressionada faz o braço correspondente a tecla transladar sobre o eixo y do braço em que esta apoiado.
void comandosTranslacaoBracos(unsigned char key) {
    switch(key) {
    case 'w':
            if(posicaoBraco2 > 2) {//o braço 2 pode ir até y = 2 do braço 1(menos que isso ele já fica abaixo da base)
                posicaoBraco2--;
            }
            desenhaCena();
            break;
        case 'W':
            if(posicaoBraco2 <= 70) {//o limite para o braço 2  transladar no braço 1 é 70, mais que isso ele fica acima do final do braço 1
                posicaoBraco2++;
            }
            desenhaCena();
            break;
        case 'e':
            if(posicaoBraco3 > 0) { // limite minimo que o braço 3 pode transladar no braço 2(até a origem do braço 2)
                posicaoBraco3--;
            }
            desenhaCena();
            break;
        case 'E':
            if(posicaoBraco3 <= 41.0) {//limite maximo que o braço 3 pode transladar no braço 2(até o final do braço 2)
                posicaoBraco3++;
            }
            desenhaCena();
            break;
    }
}
//função que é passada como callback para a função glutKeyboardFunc, ela chama quase todas as funções que gerenciam movimento
// a unica que ela nao chama é a função que gerencia os inputs das setas direcionais do teclado, essa precisa ser chamada
//a partir de outra função da glut
void movimentosTeclado(unsigned char key,int x, int y) {
    comandosRotacao(key);
    comandosDiagonais(key);
    comandosTranslacaoBracos(key);
    comandoReflexao(key);
}

//função que gerencia o que o usuario selecionou no menu
void menuCurvas(int opcao) {
    switch(opcao) {
        case 2:
            quantidadePontosEsperados = 2;
            selecaoPontosAtivado = true;//sempre que uma opção é selecionada esta variavel indica que o modo de seleção de pontos esta ativado
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
    //função que força o programa a redesenhar a tela.
    //esta aqui para que o aviso de que o modo de seleção de pontos apareça na tela em vermelho.
    glutPostRedisplay();
}
//função que trata o evento de click do botão esquerdo do mouse
void selecionaPontos(int x, int y) {
    if(quantidadePontosSelecionados < quantidadePontosEsperados){//se ainda não selecionamos todos os pontos que precisamos...
        //adiciona a coordenada do ponto ao array de pontos selecionados
        pontosSelecionados[quantidadePontosSelecionados][0] = x;
        //por causa da orientação do meu sistema de coordenadas, eu inverto o valor de y para que ele fique negativo, que é o que o meu sistema espera.
        pontosSelecionados[quantidadePontosSelecionados][1] = -y;
        quantidadePontosSelecionados++;//incrementa a quantidade de pontos selecionados
    }
    //se já selecionamos todos os pontos que precisamos, então saimos do modo de seleção de pontos.
    if((quantidadePontosSelecionados == quantidadePontosEsperados) && quantidadePontosSelecionados >= 2){
        selecaoPontosAtivado = false;
    }
}

//função que cria o menu e adiciona ele ao botão direito do mouse
void menu() {
    //função que vai tratar a opção selecionada
    glutCreateMenu(menuCurvas);
    //opções do menu com seus respectivos valores
    glutAddMenuEntry("2 PONTOS",2);
    glutAddMenuEntry("3 PONTOS",3);
    glutAddMenuEntry("4 PONTOS",4);

    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

//função de callback que é chamada quando um evento de click de mouse ocorre
void mouseEventos(int button, int state, int x, int y) {
    //se foi botão direito pressionado, entao abre o menu
    if (button == GLUT_RIGHT_BUTTON){
        if (state == GLUT_DOWN){
            menu();
         }
    }
    //se foi o botão esquerdo pressionado e estamos no modo de seleção de pontos, captura as coordenadas do ponto selecionado.
    if (button == GLUT_LEFT_BUTTON && selecaoPontosAtivado){
        if (state == GLUT_DOWN){
            if(quantidadePontosEsperados > 0) {
                selecionaPontos(x, y);
            }
         }
    }
    //força o redisplay
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
    glPointSize(5.0);//altera o tamanho do ponto para que possa ser visualizado na tela
    glClearColor(1,1,1,1);
    glutMainLoop();
    return 0;
}
