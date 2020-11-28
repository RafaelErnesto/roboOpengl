#include <GL/gl.h>
#include <GL/glut.h>
#include <math.h>
#include <stdbool.h>
//variavel que armazena o tamanho da janela
extern GLfloat win = 600.0f;
//medidas padrão dos objetos
//ao dizer padrão significa inicial, os objetos podem sofrer escalonamentos
// e então podem haver na cena objetos menores ou maiores que essas medidas
extern GLfloat comprimentoBase = 25,//medida padrão do comprimento da base do robô
        alturaBase = 7,//medida padrão da altura da base do robô
        diametroCirculo = 10,//medida padrão do diametro das circunferencias
        alturaBraco = 80.0f;// medida padrão da altura dos braços do robô

//váriaveis que armazenam os angulos de rotação das juntas dos braços e dedos, são valores iniciais que podem
//sofrer alterações devido à  rotações durante a execução do projeto
extern GLfloat anguloPrimeiroBraco = 0,//angulo da base com o primeiro braço
        anguloSegundoBraco = -125, //angulo de rotação entre o segundo e o primeiro braço
        anguloTerceiroBraco = 55, // angulo de rotação entre o terceiro e o segundo braço
        anguloDedo1 = 40,//angulo de rotação entre o dedo e mão(circulo que indica a mão)
        anguloDedo2 = 140,//angulo de rotação entre o dedo e mão(circulo que indica a mão)
        anguloPontaDedo1 = -30,//angulo de rotação entre a ponta do dedo 1 e o inicio do dedo 1
        anguloPontaDedo2 = -160;// angulo de rotação entre a ponta do dedo 2 e o inicio do dedo 2

//variavel que armazena um valor simplificado de PI para desenhar circulos
extern const int PI = 3.14159;

extern GLfloat inicioBaseRoboX = 300.0f,
        inicioBaseRoboY = -300.0f;

extern GLfloat posicaoBraco2 = 37.0f;
extern GLfloat posicaoBraco3 = 21.5f;

extern GLfloat anguloReflexao = 0.0f;

extern GLboolean realizarReflexaoRobo = false;

extern GLboolean selecaoPontosAtivado = false;
extern int quantidadePontosEsperados = 0;
extern int quantidadePontosSelecionados = 0;
extern GLfloat pontosSelecionados[4][2] = {0,0};

