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

    nome_arquivo = aloca_string (TAM_ENTRADA);

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

    //ignorando_comentario (arquivo);

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
                
            }
        }        
    }

}