#include "lib_mosaico.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <math.h>

char *aloca_vetor (int tam_vetor) {

    char * vetor;
    vetor = malloc (sizeof(char)*tam_vetor);
    if (! vetor) {
        perror ("Error:");
        fprintf (stderr, "Não foi possível alocar o espaço para o vetor\n");
        exit (1);
    }
    return vetor;
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

t_pixel * aloca_vetor_pixel (int tam_vetor) {

    t_pixel * pixel;

    pixel = malloc (sizeof(t_pixel)*tam_vetor);
    if ( ! pixel ) {
        perror ("Error");
        fprintf (stderr, "Não foi possivel alocar espaço para o vetor de pixel\n");
        exit (1);
    }

    return pixel;

}
t_imagem** inicializa_matriz_imagem (int tam) {

    t_imagem **imagem;

    imagem = malloc (sizeof(t_imagem*)*tam);
    if (! imagem) {
        perror ("Error:");
        fprintf (stderr, "Não foi possível alocar a matriz de imagem\n");
        exit (1);
    }

    for (int i = 0; i < tam; i++) {
        
        imagem[i] = malloc (sizeof(t_imagem)*1);
        if (! imagem[i] ) {
            perror ("Error:");
            fprintf (stderr, "Não foi possível alocar a matriz de imagem\n");
            exit (1);
        }
    }
    return imagem;
}
t_imagem* inicializa_tipo_imagem (int tam_vetor){

   t_imagem * vetor;

   //Bloco: inicializando o tipo imagem e verificando se ele foi alocado
   vetor = malloc (sizeof(t_imagem)*tam_vetor);
   if (! vetor) {
      perror ("Error:");
      fprintf (stderr, "Não foi possível alocar o espaço para o vetor de 'imagens\n'");
      exit (1);
   }                                                                   //Fim do bloco
   return vetor;
}

t_tiles* inicializa_pastilhas (int n, int tam_vetor) {

    t_tiles * pastilhas;

    //Bloco: inicializando o tipo pastilha e verificando se ele foi alocado
    pastilhas = malloc (sizeof(t_tiles)*n);
    if (! pastilhas) {
        perror ("Error:");
        fprintf (stderr, "Não foi possível alocar o espaço para o ponteiros 'pastilhas\n'");
        exit (1);
    }                                                                  //Fim do bloco

    pastilhas->vetor = inicializa_matriz_imagem (tam_vetor);
    pastilhas->size = 0;

    return pastilhas;
}

void inicializa_mosaico (t_imagem * entrada, t_imagem *saida) {

    strcpy (saida->tipo, entrada->tipo);
    saida->largura = entrada->largura;
    saida->altura = entrada->altura;
    saida->componente_rgb = entrada->componente_rgb;
    saida->matriz_pixels = aloca_matriz_pixel (saida->largura, saida->altura);
    
}

void carrega_pixels (FILE *arquivo, t_pixel **pixels, char *tipo, int largura, int altura) {

    int red, green, blue;

    //Bloco: Se a imagem for do tipo P3
    if ( ! strcmp (tipo, "P3")) {
        for (int i = 0; i < altura; i++ ){
            for (int j = 0; j < largura; j++){

                //Bloco: lendo os pixels do arquivo e testando para ver se leu corretamente
                fscanf (arquivo, "%d ", &red);
                fscanf (arquivo, "%d ", &green);
                fscanf (arquivo, "%d ", &blue);                         //Fim do bloco

                // if ( fscanf (arquivo, "%d %d %d", &red, &green, &blue) != 3) {
                    //perror ("Error:");
                    //fprintf (stderr, "Não foi possivel ler os pixels da imagem P3\n");
                    //exit (1);
                //}                                                     

                //Bloco: carregando os pixels retirados do arquivo na matriz de pixel
                pixels[i][j].red = red;
                pixels[i][j].green = green;
                pixels[i][j].blue = blue;
            }                                                           //Fim do bloco
        }                                                               //Fim do bloco
    }

    //Bloco: Se a imagem for P6 carrega tudo na memória com fread
    else {
        for (int i = 0; i < altura; i++) {
            for (int j = 0; j < largura; j++) {
                fread (&pixels[i][j].red, 1, 1, arquivo);
                fread (&pixels[i][j].green, 1, 1, arquivo);
                fread (&pixels[i][j].blue, 1, 1, arquivo);
            }
        }                                                           //Fim do bloco
    }
}
void tratando_comentarios (FILE *arquivo){

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
void ler_imagens (t_imagem * imagem, char * nome_arquivo) {
    
    FILE *arquivo;
    //Bloco: abrindo a imagem e testanta para ver se abriu o arquivo
    arquivo = fopen (nome_arquivo, "r");
    if (! arquivo) {
        perror ("Error");
        fprintf (stderr, "Não foi possivel abrir o arquivo %s\n", nome_arquivo);
        exit (1);
    }                                                                //Fim do bloco

    //Bloco: Lendo o tipo da imagem e testando para ver se foi lido
    if (! fgets (imagem->tipo, MAX_TYPE + 1, arquivo)) {
        perror ("Error");
        fprintf (stderr, "Não foi possível ler o tipo da imagem\n");
        exit (1);
    }                                                                //Fim do bloco

    //tratando os comentários
    tratando_comentarios (arquivo);

    //Bloco: lendo o tamanho da imagem
    if ( fscanf (arquivo, "%d %d", &imagem->largura, &imagem->altura) != COMPONENTES_IMG) {
        perror ("Error");
        fprintf (stderr, "Não foi possivel ler o tamanho da imagem\n");
        exit (1);
    }                                                                //Fim do bloco
   
    //tratando os comentários
    tratando_comentarios (arquivo);

    //Bloco: Lendo o componete RGB e testando para ver se foi lido
    if (fscanf(arquivo, "%d", &imagem->componente_rgb) != 1){
        perror ("Error");
        fprintf (stderr, "Não foi possivel ler o componente RGB\n");
        exit (1);
    }                                                                //Fim do bloco

    //Bloco: aloca a matriz de pixel na memória e carrega os pixels na memória
    imagem->matriz_pixels = aloca_matriz_pixel (imagem->largura, imagem->altura);
    carrega_pixels (arquivo, imagem->matriz_pixels, imagem->tipo, imagem->largura, imagem->altura);                                             
                                                                     //Fim do bloco

    fclose (arquivo);
}
t_tiles * abrir_pastilhas (char *nome_diretorio) {

    DIR * dir;
    t_tiles *pastilhas;
    struct dirent *arquivo;
    //char nome_imagem[TAM_ENTRADA + 1];
    int multiplicador = 1;

    //inicializando o tipo pastilhas
    pastilhas = inicializa_pastilhas (1, LARGURA_MAX);

   //Abrindo o diretório e testando para ver se ele foi aberto
   dir = opendir (nome_diretorio);
   if (! dir){
       perror ("Error:");
       fprintf (stderr, "Não foi possível abrir o diretório %s\n", nome_diretorio);
       exit (1);
   }

   //Laço que vai abrir cada arquivo da pasta e guardar ele na memória
   //Realoc se necessário - se o tamanho do vetor ultrapassar a LARGURA_MAX
   while ((arquivo = readdir (dir))) {

        pastilhas->vetor[pastilhas->size]->nome_imagem = aloca_vetor (TAM_ENTRADA);
       //Teste para ver se não é o diretório corrente ou o pai
       if ( ! strcmp (arquivo->d_name, "."))
            continue;
       if ( ! strcmp (arquivo->d_name, ".."))
            continue;

        //Torna o arquivo legível e carrega na memória todas as imagens
        strcpy (pastilhas->vetor[pastilhas->size]->nome_imagem, nome_diretorio);
        strcat (pastilhas->vetor[pastilhas->size]->nome_imagem, arquivo->d_name);

        //Abre o arquivo e carrega na memória
        ler_imagens (pastilhas->vetor[pastilhas->size], pastilhas->vetor[pastilhas->size]->nome_imagem);
        pastilhas->size += 1;


        //Realoca espaço se necessário
        if (pastilhas->size >= LARGURA_MAX*multiplicador) {

            multiplicador++;
            pastilhas->vetor = realloc (pastilhas->vetor, sizeof(t_imagem) * (LARGURA_MAX * multiplicador));
            for (int i = pastilhas->size; i  < LARGURA_MAX*multiplicador; i++) {
                pastilhas->vetor[i] = malloc (sizeof(t_imagem) * 1);
                if (! pastilhas->vetor[i]) {
                    perror ("Error:");
                    fprintf (stderr, "Não foi possível alocar a matriz de imagem\n");
                    exit (1);
                }
            }

        }

   }

   (void) closedir (dir);
   return pastilhas;

}
t_pixel * media_bloco (t_pixel ** pixels, int largura, int altura, int ini_lin, int ini_col) {

    t_pixel * media;
    int soma_red, soma_green, soma_blue;

    media = aloca_vetor_pixel (1);
    soma_red = 0;
    soma_green = 0;
    soma_blue = 0;

    //fprintf (stderr, "%d %d %d %d\n", largura, altura, ini_lin, ini_col);
    //Bloco: percorre a matriz de pixels e soma os componentes RGB
    for (int i = ini_lin; i < altura; i++) {
        for (int j = ini_col; j < largura; j++) {
            soma_red = soma_red + pixels[i][j].red;
            soma_green = soma_green + pixels[i][j].green;
            soma_blue = soma_blue + pixels[i][j].blue;

        }
    }                                                                   //Fim do bloco                                                                    

    //Bloco: calcula a média das somas dos componentes 
    media->red = soma_red/((largura - ini_col)*(altura - ini_lin));
    media->green = soma_green/((largura - ini_col)*(altura - ini_lin));
    media->blue = soma_blue/((largura - ini_col)*(altura - ini_lin));   //Fim do bloco

    return media;
    
}
void fotomosaico (t_tiles* pastilhas, t_imagem * entradappm, t_imagem * saidappm) {

    int pos_menor;
    inicializa_mosaico (entradappm, saidappm);
    for (int i = 0; i < entradappm->altura; i += pastilhas->vetor[0]->altura) {
        for (int j = 0; j < entradappm->largura; j += pastilhas->vetor[0]->largura) {
            if ((i + pastilhas->vetor[0]->altura > entradappm->altura) && (j + pastilhas->vetor[0]->largura > entradappm->largura)) {
                saidappm->cor_media_bloco = media_bloco ( entradappm->matriz_pixels, entradappm->largura, entradappm->altura, i, j);
                pos_menor = compara_blocos (pastilhas, saidappm->cor_media_bloco);
                substitui_bloco (pastilhas->vetor[pos_menor]->matriz_pixels, saidappm->matriz_pixels, i, j, entradappm->altura, entradappm->largura);
            }
            else if (j + pastilhas->vetor[0]->largura > entradappm->largura) {
                saidappm->cor_media_bloco = media_bloco ( entradappm->matriz_pixels, entradappm->largura, i + pastilhas->vetor[0]->altura, i, j);
                pos_menor = compara_blocos (pastilhas, saidappm->cor_media_bloco);
                substitui_bloco (pastilhas->vetor[pos_menor]->matriz_pixels, saidappm->matriz_pixels, i, j, i + pastilhas->vetor[0]->altura, entradappm->largura);
            }
            else if (i + pastilhas->vetor[0]->altura > entradappm->altura) {
                saidappm->cor_media_bloco = media_bloco ( entradappm->matriz_pixels, j + pastilhas->vetor[0]->largura, entradappm->altura, i, j);
                pos_menor = compara_blocos (pastilhas, saidappm->cor_media_bloco);
                substitui_bloco (pastilhas->vetor[pos_menor]->matriz_pixels, saidappm->matriz_pixels, i, j, entradappm->altura, j + pastilhas->vetor[0]->largura);
            }
            else {
                saidappm->cor_media_bloco = media_bloco ( entradappm->matriz_pixels, j + pastilhas->vetor[0]->largura, i + pastilhas->vetor[0]->altura, i, j);
                pos_menor = compara_blocos (pastilhas, saidappm->cor_media_bloco);
                substitui_bloco (pastilhas->vetor[pos_menor]->matriz_pixels, saidappm->matriz_pixels, i, j, i + pastilhas->vetor[0]->altura, j + pastilhas->vetor[0]->largura);
            }
            free (saidappm->cor_media_bloco);
        }
    }

}

int compara_blocos (t_tiles *pastilhas, t_pixel * media_bloco) {

    int ind_menor;
    float deltaC, deltaC_menor, delta_r, delta_g, delta_b, media_r;

    delta_r = pastilhas->vetor[0]->cor_media_bloco->red - media_bloco->red;
    delta_g = pastilhas->vetor[0]->cor_media_bloco->green - media_bloco->green;   
    delta_b = pastilhas->vetor[0]->cor_media_bloco->blue - media_bloco->blue;
    
    ind_menor = 0;
    media_r = (pastilhas->vetor[0]->cor_media_bloco->red + media_bloco->red)/2;
    deltaC_menor = sqrt((2 + media_r/(RGB + 1)) * pow(delta_r,2) + 4*pow(delta_g,2) + (2 + (RGB - media_r)/(RGB + 1)) * pow(delta_b,2));
    
    if (deltaC_menor < 0)
            deltaC_menor = deltaC_menor * -1;

    for (int i = 1; i < pastilhas->size; i++) {

        delta_r = pastilhas->vetor[i]->cor_media_bloco->red - media_bloco->red;
        delta_g = pastilhas->vetor[i]->cor_media_bloco->green - media_bloco->green;   
        delta_b = pastilhas->vetor[i]->cor_media_bloco->blue - media_bloco->blue;

        media_r = (pastilhas->vetor[i]->cor_media_bloco->red + media_bloco->red)/2;
        deltaC = sqrt((2 + media_r/(RGB + 1)) * pow(delta_r,2) + 4*pow(delta_g,2) + (2 + (RGB - media_r)/(RGB + 1)) * pow(delta_b,2));

        if (deltaC < 0)
            deltaC = deltaC * -1;

        if (deltaC < deltaC_menor) {
            deltaC_menor = deltaC;
            ind_menor = i;
        }
    }

    return ind_menor;

}

void substitui_bloco (t_pixel ** matriz_pastilha, t_pixel ** matriz_imagem, int linha_ini, int col_ini, int altura, int largura) {

    int l, k;
    l = 0;
    for (int i = linha_ini; i < altura; i++) {
        k = 0;
        for (int j = col_ini; j < largura; j++) {
            matriz_imagem[i][j].red = matriz_pastilha[l][k].red;
            matriz_imagem[i][j].green = matriz_pastilha[l][k].green;
            matriz_imagem[i][j].blue = matriz_pastilha[l][k].blue;
            k++;
        }
        l++;
    }
}
void escrever_imagem (t_imagem * imagem_saida, char *nome_saida, char* tipo) {

    FILE * arquivo;
    int i;

    i = strcmp (tipo, "P3");

    arquivo = fopen (nome_saida, "w");
    if (! arquivo) {
        perror ("Error:");
        fprintf (stderr, "Não foi possível criar a imagem de saída\n");
        exit (1);
    }

    //Bloco: Cabeçalho da função
    fprintf (arquivo, "%s\n", imagem_saida->tipo);
    fprintf (arquivo, "# Autor: Giordano Henrique Silveira\n");
    fprintf (arquivo, "# Não copia comédia\n");
    fprintf (arquivo, "%d %d\n", imagem_saida->largura, imagem_saida->altura);
    //fprintf (arquivo, "%d\n", imagem_saida->componente_rgb);            //Fim do bloco

    if ( ! strcmp (imagem_saida->tipo, "P6")) {
        
        if (i == 0)
            fprintf (arquivo, "%d  \n", imagem_saida->componente_rgb);
        else
            fprintf (arquivo, "%d", imagem_saida->componente_rgb);

        for (int i = 0; i < imagem_saida->altura; i++){
            for (int j = 0; j < imagem_saida->largura; j++) {
                fwrite (&imagem_saida->matriz_pixels[i][j].red, 1, 1, arquivo);
                fwrite (&imagem_saida->matriz_pixels[i][j].green, 1, 1, arquivo);
                fwrite (&imagem_saida->matriz_pixels[i][j].blue, 1, 1, arquivo);
            }
        }
    }
    else {
        fprintf (arquivo, "%d\n", imagem_saida->componente_rgb);
        for (int i = 0; i < imagem_saida->altura; i++) {
            for (int j = 0; j < imagem_saida->largura; j++) {
                fprintf (arquivo, "%d %d %d ", imagem_saida->matriz_pixels[i][j].red, imagem_saida->matriz_pixels[i][j].green, imagem_saida->matriz_pixels[i][j].blue);
            }
        }
    }

    fclose (arquivo);
}
void liberando_pastilhas (t_tiles * pastilhas) {

    int i;
    for (i = pastilhas->size - 1; i >= ZERO; i--) {
        free (pastilhas->vetor[i]->cor_media_bloco);
        free (pastilhas->vetor[i]->nome_imagem);
        free (pastilhas->vetor[i]->matriz_pixels[0]);
        free (pastilhas->vetor[i]->matriz_pixels);
    }
    for (int i = 0; i < pastilhas->size; i++)
        free (pastilhas->vetor[i]);
    
    free (pastilhas->vetor);
    free (pastilhas);

}

void liberando_string (char *string) {

    if (string)
        free (string);

}

void liberando_imagem (t_imagem * imagem) {

    //free (imagem->nome_imagem);
    //free (imagem->cor_media_bloco);
    free (imagem->matriz_pixels[0]);
    free (imagem->matriz_pixels);
    free (imagem);

}
  
