#ifndef __LBM__
#define __LBM__

#include <stdio.h>
#include <dirent.h>

#define TAM_ENTRADA 128
#define MAX_TYPE 2
#define COMPONENTES_IMG 2
#define N_PASTILHAS 1024
#define ZERO 0

typedef struct {
    int vermelho, verde, azul;
} t_pixel;

typedef struct {
    char *nome_arquivo;
    int tipo;
    int largura, altura;
    int componente_rgb;
    t_pixel **matrix;
} t_ppm;

typedef struct {
    t_ppm *imagem;
}t_pastilha;

typedef struct {
    int tam;
    t_pastilha *vetor;
}t_vetor_pastilhas;

//Aloca um vetor de char, uma string.
//É passado um inteiro, o tamanho do vetor que você quer
//É retornado um char *, o vetor
char * aloca_vetor (int );

//Inicializa o tipo imagem
//É passado a altura e a largura da matriz de pixel usada dentro do t_ppm
//Retorna um t_ppm ponteiro com as estruturas inicializadas
t_ppm * inicializa_imagem (int , int );

//Aloca um vetor do t_pixel
//É passado para a função a largura e a altura da função
//Retorna o pontreiro da matriz alocada
t_pixel ** aloca_matriz_pixel (int largura, int altura);

//Descobre o tamanho das pastilhas que vão ser usadas
//É passado uma struct dirent, o nome do diretório e dois inteiros que são o retorno da largura e altura das pastilhas
void padrao_pastilhas (struct dirent *, char *, int *, int *);

//Carrega as pastilhas da pasta para a memória
//É passado um sctruct dirente *, o nome do diretório e dois inteiros que são o tamanho das pastilhas
//É retonado o verto de pastilhas com as pastilhas carregadas
t_vetor_pastilhas * abrir_pastilhas (DIR * , struct dirent * , char * , int , int );

//Lê uma imagem.ppm
//É passado a um t_ppm para os dados da imagem e um tipo char * o nome da imagem
void ler_imagem (t_ppm *, char * );

//Carrega os pixels do arquivo para a memória
//É passado um FILE *, o arquivo, um int, o tipo da imagem, e dois int's, as dimensões da matriz
void carrega_pixels (FILE *, t_pixel **, int , int , int );

//Carrega a média de um bloco
//É passada a matriz de pixel do bloco; a largura e a altura, até onde o bloco vai; e a posisão aonde começa o bloco
//É retornada um t_pixel que é a média calculada
t_pixel media_bloco (t_pixel **, int , int , int, int);

#endif