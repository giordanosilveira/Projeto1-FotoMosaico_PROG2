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
    //t_ppm *entradappm, *saidappm;
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

    for (int i = 0; i < vetor->tam; i++) {
        cor_media_bloco[i] = media_bloco (vetor->vetor[i].imagem->matrix, past_lar, past_alt, ZERO, ZERO);
    }
    
    //imprimir_pastilhas (vetor);

    fprintf (stderr, "Fechando o diretorio %s\n", nome_diretorio);
    (void)closedir(diretorio);
    return 0;
}