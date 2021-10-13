//biblioteca do programa mosaico.c
//Aluno: Giordano Henrique Silveira; GRR20197154

#ifndef __LBM__
#define __LBM__

#include <stdio.h>

#define LARGURA_MAX 1024
#define ZERO 0
#define MAX_TYPE 2
#define COMPONENTES_IMG 2
#define COMPONENTES_RGB 3
#define RGB 255
#define TAM_ENTRADA 128

//Essa struct é referente às cores do pixel
typedef struct {
    unsigned char red, green, blue;
} t_pixel;

//Essa struct contém tudo referente a imagem
typedef struct {
    t_pixel *cor_media_bloco;
    t_pixel **matriz_pixels;
    char tipo[MAX_TYPE + 1];
    char *nome_imagem;
    int largura, altura, componente_rgb;
} t_imagem;

//Struct responsável para carregar na memória as pastilhas
typedef struct {
    int size;
    t_imagem **vetor;
} t_tiles;

//Aloca uma string
//É passado um inteiro, o tamanho da string, e é retornado o ponteiro para esse vetor alocado
char *aloca_vetor (int );

//Aloca uma matriz de pixel
//É passado dois inteiros, as dimensões da matriz, e é retornado o ponteiro para essa matriz alocada
t_pixel ** aloca_matriz_pixel (int , int );

//Aloca um vetor de pixel
//É passado o tamanho desse vetor, é retornado o ponteiro do vetor alocado
t_pixel * aloca_vetor_pixel (int );

//Aloca um ponteiro para o tipo imagem
//É passada um int, o tamanho do vetor, e é retornado o ponteiro para esse vetor alocado
t_imagem* inicializa_tipo_imagem (int );

//Aloca o ponteiro para o tipo pastilhas
//É passado dois ints, um para o tamanho do vetor pastilhas e outro para o tamanho da matriz de imagem que está dentro do tipo pastilhas
//Retorna o ponteiro alocado
t_tiles* inicializa_pastilhas (int , int );

//Carrega os pixels do arquivo para dentro da matriz de pixel  
void carrega_pixels (FILE *, t_pixel **, char *, int , int );

//Trata dos comentário da imagem .ppm
void tratando_comentarios (FILE *);

//Abre a imagem e carrega os dados dela na memória
void ler_imagens (t_imagem *, char *);

//Abre o diretório e carrega na memórias as pastilhas
t_tiles * abrir_pastilhas (char *);

//Libera o que foi alocado para o tipo pastilha
void liberando_pastilhas (t_tiles * );

//Calcula a media do bloco
//É passada a matriz de pixels, as dimensões do bloco e as posições que o bloco começa
//Retorna a media do bloco
t_pixel * media_bloco (t_pixel **, int, int, int, int);

//Trata da imagem foto moisaico
//É passada as patilhas, a imagem de entrada e a imagem de saída
void fotomosaico (t_tiles *, t_imagem *, t_imagem *);

//Compara a media das cores do bloco da imagem grande com o vetor de pastilhas
//Retorna o índice da pastilha que tem as cores média mais próxima do bloco da imagem grande
int compara_blocos (t_tiles *, t_pixel *);

//Substitui o bloco de cores mais próxima na matriz da imagem de saída
//É passado a matriz de pixel da pastilha, a matriz de pixel da imagem grande, dois inteiros representando a referência de onde começar no bloco da imagem grande
//e dois int's representando até onde vai a substituição
void substitui_bloco (t_pixel **, t_pixel **, int , int , int , int);

//Escreve o arquivo output.ppm
//É passado os dados necessários para o arquivo
void escrever_imagem (t_imagem *, char *, char *);
//Libera as strings dos nomes do arquivo
void liberando_string (char *);

#endif
