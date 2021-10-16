#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include "libmosaico.h"

char *aloca_vetor (int tam) {

    char *string;
    string = malloc (sizeof (char) * tam);
    if ( ! string ) {
        perror ("Error");
        fprintf (stderr, "Não foi possível alocar espaço para uma string\n");
        exit (1);
    }

    return string;

}

t_ppm * inicializa_imagem (int largura, int altura) {

    t_ppm *imagem;
    imagem = malloc (sizeof (t_ppm));
    if (! imagem){
        perror ("Error");
        fprintf (stderr, "Não foi possível alocar um espaço na memória para o tipo imagem\n");
        exit (1);
    }

    imagem->nome_arquivo = aloca_vetor (TAM_ENTRADA);
    imagem->matrix = aloca_matriz_pixel (largura, altura);

    return imagem;

}

t_pixel ** aloca_matriz_pixel (int largura, int altura) {

    t_pixel **matriz;
    
    //Bloco: aloca a matriz e testa para ver se alocou
    matriz = malloc (sizeof(t_pixel*) * altura);
    if (! matriz) {
        perror ("Error");
        fprintf (stderr, "Não foi possivel alocar espaço para os ponteiros da matriz de pixel\n");
        exit (1);
    }                                                                  //Fim do bloco

    //Bloco: aloca espaço para a matriz de linhas contíguas
    matriz[0] = malloc (largura * altura * sizeof(t_pixel));
    if (! matriz[0]) {
        perror ("Error");
        fprintf (stderr, "Não foi possivel alocar espaço para a matriz\n");
        exit (1);
    }                                                                 //Fim do bloco

    //Arruma os ponteiros
    int i;
    for (i = 0; i < altura; i++)
        matriz[i] = matriz[0] + i*largura;

    return matriz;

}

void padrao_pastilhas (struct dirent * file, char *nome_diretorio, int *largura, int *altura) {

    FILE *arquivo;
    char *nome_arquivo;
    char tipo[MAX_TYPE + 1];
    int max_rgb;

    nome_arquivo = aloca_vetor (TAM_ENTRADA);

    strcpy (nome_arquivo, nome_diretorio);
    strcat (nome_arquivo, file->d_name);

    arquivo = fopen (nome_arquivo, "w");
    if (! arquivo) {
        perror ("Error");
        fprintf (stderr, "Não foi possível abrir o arquivo %s\n", nome_arquivo);
        exit (1);
    }

    if (! fgets (tipo, MAX_TYPE + 1, arquivo)) {
        perror ("Error");
        fprintf (stderr, "Não foi possível ler o tipo da imagem do padrão pastilhas\n");
        exit (1);
    }

    ignorando_comentario (arquivo);

    if ( fscanf(arquivo, "%d %d", largura, altura) != COMPONENTES_IMG) {
        perror ("Error");
        fprintf (stderr, "O número de componentes da imagem dentro da função pastilhas_padrão foi lido errado\n");
        exit (1);
    }

    fclose (arquivo);

}

t_vetor_pastilhas * abrir_pastilhas (DIR * diretorio, struct dirent * file, char *nome_diretorio, int largura, int altura) {

    t_vetor_pastilhas * ptr;
    int mult;

    mult = 0;

    ptr = malloc (sizeof (t_vetor_pastilhas));
    if (! ptr) {
        perror ("Error");
        fprintf (stderr, "Não foi possível alocar espaço para o t_vetor_pastilhas\n");
        exit (1);
    }

    ptr->tam = 0;
    ptr->vetor = malloc (sizeof (t_pastilha) * N_PASTILHAS);
    if (! ptr->vetor) {
        perror ("Error");
        fprintf (stderr, "Não foi possível alocar espaço para o vetor t_pastilhas dentro de t_vetor_pastilhas\n");
        exit (1);
    }

    for (int i = 0; i < N_PASTILHAS; i++) {
        ptr->vetor[i].imagem = inicializa_imagem (largura, altura);    
    }

    while ((file = readdir (diretorio))) {

        if ( ! (strcmp (file->d_name, ".")))
            continue;
        if ( ! (strcmp (file->d_name, "..")))
            continue;

        strcpy (ptr->vetor[ptr->tam].imagem->nome_arquivo, nome_diretorio);
        strcat (ptr->vetor[ptr->tam].imagem->nome_arquivo, file->d_name);

        ler_imagem (ptr->vetor[ptr->tam].imagem, ptr->vetor[ptr->tam].imagem->nome_arquivo);
        ptr->tam++;

        if (ptr->tam >= N_PASTILHAS*mult) {
            mult++;
            ptr->vetor = realloc (ptr->vetor, sizeof(t_pastilha) * (N_PASTILHAS * mult));
            for (int i = 0; i < N_PASTILHAS*mult; i++) {
                ptr->vetor[i].imagem = inicializa_imagem (largura, altura); 
            }
        }        
    }
    return ptr;

}
void ignorando_comemtarios (FILE * arquivo) {

    char caractere;

    //Bloco: enquanto for # (comentarios) vai pegando caractere do arquivo
    caractere = getc (arquivo);
    while ((caractere == '#') || (caractere == '\n')) {
        while ( caractere != '\n') 
            caractere = getc (arquivo);
        caractere = getc (arquivo);
    }                                                                //Fim do bloco

    //Quando ele sai do laço mais interno ele pega mais um caractere para a verificação, essa função devolve ele para o
    //arquivo
    ungetc (caractere, arquivo);

}
void ler_imagem (t_ppm *imagem, char * nome_arquivo) {

    FILE *arquivo;
    char *tipo_ppm;

    tipo_ppm = aloca_vetor (MAX_TYPE + 1);

    //Bloco: abrindo a imagem e testanta para ver se abriu o arquivo
    arquivo = fopen (nome_arquivo, "r");
    if (! arquivo) {
        perror ("Error");
        fprintf (stderr, "Não foi possivel abrir o arquivo %s\n", nome_arquivo);
        exit (1);
    }                                                                //Fim do bloco

    //Bloco: Lendo o tipo da imagem e testando para ver se foi lido
    if (! fgets (tipo_ppm, MAX_TYPE + 1, arquivo)) {
        perror ("Error");
        fprintf (stderr, "Não foi possível ler o tipo da imagem\n");
        exit (1);
    }                                                                //Fim do bloco

    if ( strcmp (tipo_ppm, "P6") == 0)
        imagem->tipo = 0;
    else
        imagem->tipo = 1;
    
    //tratando os comentários
    ignorando_comentarios (arquivo);

    //Bloco: lendo o tamanho da imagem
    if ( fscanf (arquivo, "%d %d", &imagem->largura, &imagem->altura) != COMPONENTES_IMG) {
        perror ("Error");
        fprintf (stderr, "Não foi possivel ler o tamanho da imagem\n");
        exit (1);
    }                                                                //Fim do bloco
   
    //tratando os comentários
    ignorando_comentarios (arquivo);

    //Bloco: Lendo o componete RGB e testando para ver se foi lido
    if (fscanf(arquivo, "%d", &imagem->componente_rgb) != 1){
        perror ("Error");
        fprintf (stderr, "Não foi possivel ler o componente RGB\n");
        exit (1);
    }                                                                //Fim do bloco

    //Bloco: aloca a matriz de pixel na memória e carrega os pixels na memória
    carrega_pixels (arquivo, imagem->matrix, imagem->tipo, imagem->largura, imagem->altura);                                             
                                                                     //Fim do bloco

    fclose (arquivo);

}
void carrega_pixels (FILE *arquivo, t_pixel ** matriz, int tipo, int largura, int altura) {
    
    //Se a imagem for P6
    if ( ! tipo ) {
        for (int i = 0; i < largura; i ++) {
            for (int j = 0; j < altura; j++) {
                fread (matriz[i][j].vermelho, tipo + 1, 1, arquivo);
                fread (matriz[i][j].verde, tipo + 1, 1, arquivo);
                fread (matriz[i][j].azul, tipo + 1, 1, arquivo);
            }
        }
    }
    //Se a imagem for P3
    else {
        for (int i = 0; i < largura; i ++) {
            for (int j = 0; j < altura; j++) {
                fscanf (arquivo, "%d ", &matriz[i][j].vermelho);
                fscanf (arquivo, "%d ", &matriz[i][j].verde);
                fscanf (arquivo, "%d ", &matriz[i][j].azul);
            }
        }
    }  
}