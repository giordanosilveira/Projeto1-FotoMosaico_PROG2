#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

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
void ignorando_comentarios (FILE * arquivo) {

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
void padrao_pastilhas (struct dirent * file, char *nome_diretorio, int *largura, int *altura) {

    FILE *arquivo;
    char *nome_arquivo;
    char type[MAX_TYPE + 1];

    nome_arquivo = aloca_vetor (TAM_ENTRADA);

    strcpy (nome_arquivo, nome_diretorio);
    strcat (nome_arquivo, file->d_name);

    arquivo = fopen (nome_arquivo, "r");
    if (! arquivo) {
        perror ("Error");
        fprintf (stderr, "Não foi possível abrir o arquivo %s\n", nome_arquivo);
        exit (1);
    }

    if (! (fgets (type, MAX_TYPE + 1, arquivo))) {
        perror ("Error");
        fprintf (stderr, "Não foi possível ler o tipo da imagem do padrão pastilhas\n");
        exit (1);
    }

    ignorando_comentarios (arquivo);

    if ( fscanf(arquivo, "%d %d", largura, altura) != COMPONENTES_IMG) {
        perror ("Error");
        fprintf (stderr, "O número de componentes da imagem dentro da função pastilhas_padrão foi lido errado\n");
        exit (1);
    }

    fclose (arquivo);

}

void tamanho_imagem (FILE *entradappm, int *largura, int *altura) {

    char type[MAX_TYPE + 1];
    
    if (! (fgets (type, MAX_TYPE + 1, entradappm))) {
        perror ("Error");
        fprintf (stderr, "Não foi possível ler o tipo da imagem do padrão pastilhas\n");
        exit (1);
    }

    ignorando_comentarios (entradappm);

    if ( fscanf(entradappm, "%d %d", largura, altura) != COMPONENTES_IMG) {
        perror ("Error");
        fprintf (stderr, "O número de componentes da imagem dentro da função pastilhas_padrão foi lido errado\n");
        exit (1);
    }

}

t_vetor_pastilhas * abrir_pastilhas (DIR * diretorio, struct dirent * file, char *nome_diretorio, int largura, int altura) {

    t_vetor_pastilhas * ptr;
    int mult;

    mult = 1;

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

        if (ptr->tam == N_PASTILHAS*mult) {
            mult++;
            ptr->vetor = realloc (ptr->vetor, sizeof(t_pastilha) * (N_PASTILHAS * mult));
            for (int i = ptr->tam; i < N_PASTILHAS*mult; i++) {
                ptr->vetor[i].imagem = inicializa_imagem (largura, altura); 
            }
        }        
    }
    return ptr;

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
        for (int i = 0; i < altura; i ++) {
            for (int j = 0; j < largura; j++) {
                fread (&matriz[i][j].vermelho, tipo + 1, 1, arquivo);
                fread (&matriz[i][j].verde, tipo + 1, 1, arquivo);
                fread (&matriz[i][j].azul, tipo + 1, 1, arquivo);
            }
        }
    }
    //Se a imagem for P3
    else {
        for (int i = 0; i < altura; i ++) {
            for (int j = 0; j < largura; j++) {
                fscanf (arquivo, "%d ", &matriz[i][j].vermelho);
                fscanf (arquivo, "%d ", &matriz[i][j].verde);
                fscanf (arquivo, "%d ", &matriz[i][j].azul);
            }
        }
    }  
}

t_pixel media_bloco (t_pixel **matriz, int largura, int altura, int largura_ini, int altura_ini) {

    int soma_vermelho, soma_verde, soma_azul;
    t_pixel media;

    soma_vermelho = 0;
    soma_verde = 0;
    soma_azul = 0;

    //fprintf (stderr, "dentro do media bloco\n");
    //Bloco: percorre a matriz de pixels e soma os componentes RGB
    for (int i = altura_ini; i < altura + altura_ini; i++) {
        for (int j = largura_ini; j < largura + largura_ini; j++) {
            soma_vermelho += matriz[i][j].vermelho;
            soma_verde += matriz[i][j].verde;
            soma_azul += matriz[i][j].azul;

        }
    }                                                                   //Fim do bloco                                                                    

    //Bloco: calcula a média das somas dos componentes 
    media.vermelho = soma_vermelho/(largura * altura);
    media.verde = soma_verde/(largura * altura);
    media.azul = soma_azul/(largura * altura);                          //Fim do bloco

    return media;

}

void copia_dados (t_ppm *destino, t_ppm* origem) {

    destino->tipo = origem->tipo;
    destino->largura = origem->largura;
    destino->altura =  origem->altura;
    destino->componente_rgb = origem->componente_rgb;

}


int compara_blocos (int tam_vetor, t_pixel *media_bloco_pastilhas, t_pixel bloco_imagem) {

    int indice_menor;
    float deltaC, menor_deltaC, delta_r, delta_g, delta_b, media_r;

    indice_menor = 0;
    
    delta_r = media_bloco_pastilhas[0].vermelho - bloco_imagem.vermelho;
    delta_g = media_bloco_pastilhas[0].verde - bloco_imagem.verde;
    delta_b = media_bloco_pastilhas[0].azul - bloco_imagem.azul;

    //fprintf (stderr, "%f %f %f\n", delta_r, delta_g, delta_b);
    media_r = (media_bloco_pastilhas[0].vermelho + bloco_imagem.vermelho)/2;
    //fprintf (stderr, "%f\n", media_r);
    menor_deltaC = sqrt((2 + media_r/(RGB + 1))*pow(delta_r, 2) + 4*pow(delta_g,2) + (2 + (RGB - media_r)/(RGB + 1))*pow(delta_b, 2));
    //fprintf (stderr, "%f %d\n", menor_deltaC, tam_vetor);

    if (menor_deltaC < 0)
            menor_deltaC = menor_deltaC * -1;

    for (int i = 1; i < tam_vetor; i++) {

        delta_r = media_bloco_pastilhas[i].vermelho - bloco_imagem.vermelho;
        delta_g = media_bloco_pastilhas[i].verde - bloco_imagem.verde;
        delta_b = media_bloco_pastilhas[i].azul - bloco_imagem.azul;
        //fprintf (stderr, "%f %f %f\n", delta_r, delta_g, delta_b);

        media_r = (media_bloco_pastilhas[i].vermelho + bloco_imagem.vermelho)/2;
        //fprintf (stderr, "%f\n", media_r);
        deltaC = sqrt((2 + media_r/(RGB + 1))*pow(delta_r, 2) + 4*pow(delta_g,2) + (2 + (RGB - media_r)/(RGB + 1))*pow(delta_b, 2));

        if (deltaC < 0)
            deltaC = deltaC * -1;

        //fprintf (stderr, "%f\n", deltaC);
        if (deltaC < menor_deltaC) {
            menor_deltaC = deltaC;
            indice_menor = i;
        }
        //fprintf (stderr, "%f %f %d\n", deltaC, menor_deltaC, indice_menor);
    }

    return indice_menor;

}
void substitui_bloco (t_pixel ** matriz_saida, t_pixel ** matriz_pastilha, int linha_inicial, int linha_final, int coluna_inicial, int coluna_final) {

    //fprintf (stderr, "dentro do substitui bloco\n");
    int i, j;
    i = 0;
    for (int k = linha_inicial; k < linha_final; k++) {
        j = 0;
        for (int l = coluna_inicial; l < coluna_final; l++) {
            matriz_saida[k][l].vermelho = matriz_pastilha[i][j].vermelho;
            matriz_saida[k][l].verde = matriz_pastilha[i][j].verde;
            matriz_saida[k][l].azul = matriz_pastilha[i][j].azul;
            j++;
        }
        i++;
    }

}
int condicoes_imagem (int linha_ini, int coluna_ini, int altura_imagem, int largura_imagem, int tam_bloco) {

    if ((linha_ini + tam_bloco > altura_imagem) && (coluna_ini + tam_bloco > largura_imagem))
        return 0;
    else if ((linha_ini + tam_bloco > altura_imagem))
        return 1;
    else if ((coluna_ini + tam_bloco > largura_imagem))
        return 2;
    else
        return 3;

}
int fotomosaico (t_vetor_pastilhas *vetor, t_ppm *entrada, t_ppm *saida, t_pixel *media_pastlhas) {

    int tipo;
    int retorno, tam_bloco, pos_menor, size;
    t_pixel bloco_imagem;

    size = vetor->tam;
    tam_bloco = vetor->vetor[0].imagem->altura;

    copia_dados (saida, entrada);
    for (int i = 0; i < saida->altura; i += tam_bloco ) {
        for (int j = 0; j < saida->largura; j += tam_bloco ) {
            
            retorno = condicoes_imagem (i, j, saida->altura, saida->largura, tam_bloco);
            switch (retorno) {
                case 0 :
                    bloco_imagem = media_bloco (entrada->matrix, saida->largura - j, saida->altura - i, j, i);
                    pos_menor = compara_blocos (size, media_pastlhas, bloco_imagem);
                    substitui_bloco (saida->matrix, vetor->vetor[pos_menor].imagem->matrix, i, saida->altura, j, saida->largura);
                    break;
                case 1 :
                    bloco_imagem = media_bloco (entrada->matrix, tam_bloco, saida->altura - i, j, i);
                    pos_menor = compara_blocos (size ,media_pastlhas, bloco_imagem);
                    substitui_bloco (saida->matrix, vetor->vetor[pos_menor].imagem->matrix, i, saida->altura, j, j + tam_bloco);
                    break;
                case 2 :
                    bloco_imagem = media_bloco (entrada->matrix, saida->largura - j, tam_bloco, j, i);
                    pos_menor = compara_blocos (size, media_pastlhas, bloco_imagem);
                    substitui_bloco (saida->matrix, vetor->vetor[pos_menor].imagem->matrix, i, i + tam_bloco, j, saida->largura);
                    break;
                default:
                    bloco_imagem = media_bloco (entrada->matrix, tam_bloco, tam_bloco, j, i);
                    pos_menor = compara_blocos (size, media_pastlhas, bloco_imagem);
                    substitui_bloco (saida->matrix, vetor->vetor[pos_menor].imagem->matrix, i, i + tam_bloco, j, j + tam_bloco);
                    if ( (i == 0) && (j == 0)) {
                        if (vetor->vetor[pos_menor].imagem->tipo == 1)
                            tipo = 1;
                    }
                    break;
            }

        }
    }
    return tipo;

}
void escrever_imagem (t_ppm *outputppm, char *name_output, int teste) {

    FILE *arquivo;
    arquivo = fopen (name_output, "w");
    if (! arquivo) {
        perror ("Error");
        fprintf (stderr, "Não foi possível abrir o arquivo %s\n", name_output);
        exit (1);
    }

    if (outputppm->tipo == 0) 
        fprintf (arquivo, "P6\n");
    else
        fprintf (arquivo, "P3\n");

    fprintf (arquivo, "# Autor: Giordano Henrique Silveira\n");
    fprintf (arquivo, "%d %d\n", outputppm->largura, outputppm->altura);

    if (outputppm->tipo)
        fprintf (arquivo, "%d\n", outputppm->componente_rgb);
    else {
        if (teste)
            fprintf (arquivo, "%d  \n", outputppm->componente_rgb);
        else
            fprintf (arquivo, "%d", outputppm->componente_rgb);    
    }

    if (outputppm->tipo == 0) {
        for (int i = 0; i < outputppm->altura; i++) {
            for (int j = 0; j < outputppm->largura; j++) {
                fwrite (&outputppm->matrix[i][j].vermelho, outputppm->tipo + 1, 1, arquivo);
                fwrite (&outputppm->matrix[i][j].verde, outputppm->tipo + 1, 1, arquivo);
                fwrite (&outputppm->matrix[i][j].azul, outputppm->tipo + 1, 1, arquivo);
            }
        }
    }
    else {
        for (int i = 0; i < outputppm->altura; i++) {
            for (int j = 0; j < outputppm->largura; j++) {
                fprintf (arquivo,"%d ", outputppm->matrix[i][j].vermelho);
                fprintf (arquivo,"%d ", outputppm->matrix[i][j].verde);
                fprintf (arquivo,"%d ", outputppm->matrix[i][j].azul);
            }
        }
    }

    fclose (arquivo);

}
void imprimir_pastilhas (t_vetor_pastilhas * vetor, t_pixel *media) {

    FILE *arquivo;
    arquivo = fopen ("teste.txt", "w");
    if (! arquivo) {
        perror ("Error");
        fprintf (stderr, "Não foi possível abrir o arquivo teste.txt\n");
        exit (1);
    }

    for (int i = 0; i < vetor->tam; i++) {
        fprintf (arquivo, "%s\n", vetor->vetor[i].imagem->nome_arquivo);
        fprintf (arquivo, "red %d green %d blue %d\n", media[i].vermelho, media[i].verde, media[i].azul);
    }

    fclose (arquivo);
}