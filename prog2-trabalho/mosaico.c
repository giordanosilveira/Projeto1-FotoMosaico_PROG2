#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include "libmosaico.h"

int main (int argc, char *argv[]) {

    DIR *diretorio;
    t_ppm *imputppm, *outputppm;
    t_pixel *cor_media_bloco;
    t_vetor_pastilhas *vetor;
    struct dirent *file_directory;
    char *nome_entrada, *nome_saida, *nome_diretorio;
    int opcao;
    int flagI, flagP, flagO;
    int past_lar, past_alt;


    flagI = 0;
    flagP = 0;
    flagO = 0;


    opterr = 0;

    //Bloco: trata das opções da linha de comando
    while ((opcao = getopt (argc, argv, "i:o:p:")) != -1){

        //Usuário habilita a opção -i
        if (opcao == 'i') {
            nome_entrada = optarg;
            flagI = 1;
        }
        //Usuário habilita a opção -o
        else if (opcao == 'o') {
            nome_saida = optarg;
            flagO = 1;
        }
        //Usuário habilita a opção -p
        else if (opcao == 'p') {
            nome_diretorio = optarg;
            flagP = 1;
        }
        //Usuário entrou com uma opção inválida
        else {
            fprintf (stderr, "Opção inválida");
            exit (1);
        }
    }

    //Caso o usuário não tenha digitado nenhuma/algumas das opções, pega a entrada padrão, ou a saída padrão e/ou o
    //diretório ./tiles/
    if (! flagI){
        nome_entrada = aloca_vetor (TAM_ENTRADA);
        strcpy (nome_entrada, "/dev/stdin");
    }
    if (! flagO) {
        nome_saida = aloca_vetor (TAM_ENTRADA);
        strcpy (nome_saida, "/dev/stdout");
    }
    if (! flagP) {
        nome_diretorio = aloca_vetor (TAM_ENTRADA);
        strcpy (nome_diretorio, "./tiles/");
    }                                                       //Fim do bloco.

    fprintf (stderr, "Abrindo o diretorio %s\n", nome_diretorio);
    diretorio = opendir (nome_diretorio);
    if (! diretorio) {
        perror ("Error");
        fprintf (stderr, "Não foi possível abrir o diretorio %s\n", nome_diretorio);
        exit (1);
    }

    file_directory = readdir (diretorio);

    while (! (strcmp(file_directory->d_name, ".")) || ! (strcmp(file_directory->d_name, ".."))) {

        if ( ! (file_directory = readdir (diretorio))) {
            perror ("Error");
            fprintf (stderr, "Existe apenas o diretório corrente e o diretório anterior nesta pasta\n");
            exit (1);
        }

    }

    padrao_pastilhas (file_directory, nome_diretorio, &past_lar, &past_alt);
    rewinddir (diretorio);

    
    vetor = abrir_pastilhas (diretorio, file_directory, nome_diretorio, past_lar, past_alt);

    fprintf (stderr, "%d tiles read\n", vetor->tam);    
    fprintf (stderr, "Tile size is %dx%d\n", past_lar, past_alt);
    
    cor_media_bloco = malloc (sizeof(t_pixel)*vetor->tam);
    if (!cor_media_bloco) {
        perror ("Error");
        fprintf (stderr, "Não foi possível alocar memória para o vetor de ponteiros\n");
        exit (1);
    }

    fprintf (stderr, "Calculating tiles' average colors\n");
    for (int i = 0; i < vetor->tam; i++) {
        cor_media_bloco[i] = media_bloco (vetor->vetor[i].imagem->matrix, past_lar, past_alt, ZERO, ZERO);
    }

    FILE * entradappm;
    int largura_imagem, altura_imagem;

    fprintf (stderr, "Reading imput imagem\n");
    entradappm = fopen (nome_entrada, "r");
    if (! entradappm ){
        perror ("Error");
        fprintf (stderr, "Não foi possível abrir a imagem %s\n", nome_entrada);
        exit (1);
    }

    tamanho_imagem (entradappm, &largura_imagem, &altura_imagem);
    imputppm = inicializa_imagem (largura_imagem, altura_imagem);

    //rewind (entradappm);
    fclose(entradappm);

    ler_imagem (imputppm, nome_entrada);

    if (imputppm->tipo == 0)
        fprintf (stderr, "Imput imagem is PPM P6, %dx%d pixels\n", imputppm->largura, imputppm->altura);
    else
        fprintf (stderr, "Imput imagem is PPM P3, %dx%d pixels\n", imputppm->largura, imputppm->altura);

    int lixo;
    fprintf (stderr, "Building mosaic imagem\n");
    outputppm = inicializa_imagem (largura_imagem, altura_imagem);
    lixo = fotomosaico (vetor, imputppm, outputppm, cor_media_bloco);
    
    fprintf (stderr, "Writing output file\n");
    escrever_imagem (outputppm, nome_saida, lixo);
    
    
    //imprimir_pastilhas (vetor, cor_media_bloco);



    fprintf (stderr, "Fechando o diretorio %s\n", nome_diretorio);
    (void)closedir(diretorio);
    
    free (cor_media_bloco);
    free (imputppm->matrix[0]);
    free (imputppm->matrix);
    free (imputppm->nome_arquivo);
    free (imputppm);

    free (outputppm->matrix[0]);
    free (outputppm->matrix);
    free (outputppm->nome_arquivo);
    free (outputppm);

    for (int i = 0; i < vetor->tam; i++) {
        free (vetor->vetor[i].imagem->nome_arquivo);
        free (vetor->vetor[i].imagem->matrix[0]);
        free (vetor->vetor[i].imagem->matrix);
        free (vetor->vetor[i].imagem);
    }
    free (vetor->vetor);
    free (vetor);

    return 0;
}