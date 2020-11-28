#include <GL/gl.h>
#include <GL/glut.h>
#include <math.h>
#include <stdbool.h>
//variavel que armazena o tamanho da janela
extern GLfloat win = 600.0f;
//medidas padr�o dos objetos
//ao dizer padr�o significa inicial, os objetos podem sofrer escalonamentos
// e ent�o podem haver na cena objetos menores ou maiores que essas medidas
extern GLfloat comprimentoBase = 25,//medida padr�o do comprimento da base do rob�
        alturaBase = 7,//medida padr�o da altura da base do rob�
        diametroCirculo = 10,//medida padr�o do diametro das circunferencias
        alturaBraco = 80.0f;// medida padr�o da altura dos bra�os do rob�

//v�riaveis que armazenam os angulos de rota��o das juntas dos bra�os e dedos, s�o valores iniciais que podem
//sofrer altera��es devido �  rota��es durante a execu��o do projeto
extern GLfloat anguloPrimeiroBraco = 0,//angulo da base com o primeiro bra�o
        anguloSegundoBraco = -125, //angulo de rota��o entre o segundo e o primeiro bra�o
        anguloTerceiroBraco = 55, // angulo de rota��o entre o terceiro e o segundo bra�o
        anguloDedo1 = 40,//angulo de rota��o entre o dedo e m�o(circulo que indica a m�o)
        anguloDedo2 = 140,//angulo de rota��o entre o dedo e m�o(circulo que indica a m�o)
        anguloPontaDedo1 = -30,//angulo de rota��o entre a ponta do dedo 1 e o inicio do dedo 1
        anguloPontaDedo2 = -160;// angulo de rota��o entre a ponta do dedo 2 e o inicio do dedo 2

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

