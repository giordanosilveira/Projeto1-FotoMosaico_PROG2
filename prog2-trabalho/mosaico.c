// Giordano Henrique Silveira. GRR:20197152

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include "lib_mosaico.h"

int main (int argc, char *argv[]) {

    char *nome_entrada, *nome_saida, *nome_diretorio;
    t_tiles *pastilhas;
    t_imagem * imputppm, *outputppm;

    int opcao;
    int flagI = 0;
    int flagP = 0;
    int flagO = 0;

    opterr = 0;

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
    }

    //Carregarando as pastilhas da memória e calculando a cor media de cada pastilha
    fprintf (stderr, "Reading tiles from %s\n", nome_diretorio);
    pastilhas = abrir_pastilhas (nome_diretorio);

    //max_pastilhas (pastilhas);

    fprintf (stderr, "%d tiles read\n", pastilhas->size);    
    fprintf (stderr, "Tile size is %dx%d\n", pastilhas->vetor[0]->largura, pastilhas->vetor[0]->altura);

    //Calculando as cores médias de cada bloco
    fprintf (stderr, "Calculating tiles' aberage colors\n");
    for (int i = 0; i < pastilhas->size; i++) {
        pastilhas->vetor[i]->cor_media_bloco = media_bloco (pastilhas->vetor[i]->matriz_pixels, pastilhas->vetor[i]->largura, pastilhas->vetor[i]->altura, ZERO, ZERO);

    //Abrindo a imagem principal
    fprintf (stderr, "Reading imput image\n");
    imputppm = inicializa_tipo_imagem (1);
    ler_imagens(imputppm, nome_entrada);
    //um_monte_de_print_besta (imputppm->matriz_pixels);
    fprintf (stderr, "Imput image is PPM %s, %dx%d pixels\n", imputppm->tipo, imputppm->largura, imputppm->altura);

    //Tratando da imagem de saída
    fprintf (stderr, "Building mosaic image\n");
    outputppm = inicializa_tipo_imagem (1);
    fotomosaico (pastilhas, imputppm, outputppm);

    //Escrevendo a imagem de saída
    fprintf (stderr, "Writing output file\n");
    escrever_imagem (outputppm, nome_saida, pastilhas->vetor[0]->tipo);


    //Dando Free nas estruturas usadas
    fprintf (stderr, "freeing memory spaces\n");
    //liberando_imagem ();
    //liberando_imagem ();
    liberando_pastilhas (pastilhas);
    //liberando_string (nome_entrada);
    //liberando_string (nome_saida);
    //liberando_string (nome_diretorio);

   return 0;
}
