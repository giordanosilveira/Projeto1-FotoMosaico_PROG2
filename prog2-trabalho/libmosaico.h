#ifndef __LBM__
#define __LBM__

#include <stdio.h>
#include <dirent.h>

#define TAM_ENTRADA 128
#define MAX_TYPE 2
#define COMPONENTES_IMG 2
#define N_PASTILHAS 1024
#define ZERO 0
#define RGB 255

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
//É passado a largura e a altura da matriz de pixel usada dentro do t_ppm
//Retorna um t_ppm ponteiro com as estruturas inicializadas
t_ppm * inicializa_imagem (int , int );

//Descobre o tamanho da imagem.ppm
//Retornar as dimensões nos dois inteiros passados como parâmetro
void tamanho_imagem (FILE *, int *, int *);

//Carrega as pastilhas da pasta para a memória
//É passado um sctruct dirente *, o nome do diretório e dois inteiros que são o tamanho das pastilhas
//É retonado o verto de pastilhas com as pastilhas carregadas
t_vetor_pastilhas * abrir_pastilhas (DIR * , struct dirent * , char * , int , int );

//Lê uma imagem.ppm
//É passado a um t_ppm para os dados da imagem e um tipo char * o nome da imagem
void ler_imagem (t_ppm *, char * );

//Função responsável por "montar" o foto mosaico na memória. Ela faz a media dos blocos da imagem, compara e substitui eles na memória
//É passado o vetor de pastilhas, os dados da imagem de entrada, os de saída e a media dos blocos das pastilhas 
int fotomosaico (t_vetor_pastilhas *, t_ppm *, t_ppm *, t_pixel *);

//Criar o arquivo de saída
//É passada os dados da imagem, em formato t_ppm *, e o nome do arquivo de saída, um char * 
void escrever_imagem (t_ppm *, char *, int);

//Carrega a média de um bloco
//É passada a matriz de pixel do bloco; a largura e a altura do bloco e a posisão aonde começa o bloco
//É retornada um t_pixel que é a média calculada
t_pixel media_bloco (t_pixel **, int , int , int, int);


#endif