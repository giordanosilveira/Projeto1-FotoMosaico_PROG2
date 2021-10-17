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
    string = malloc (sizeof (char) * tam);  //Aloca um vetor de char
    if ( ! string ) {                       //Teste para ver se alocou
        perror ("Error");
        fprintf (stderr, "Não foi possível alocar espaço para uma string\n");
        exit (1);
    }

    return string;

}
//Aloca uma matriz do t_pixel
//É passado para a função a largura e a altura da função
//Retorna o pontreiro da matriz alocada
t_pixel ** aloca_matriz_pixel (int largura, int altura) {

    t_pixel **matriz;
    
    //Bloco: aloca a matriz e testa para ver se alocou
    matriz = malloc (sizeof(t_pixel*) * altura);
    if (! matriz) {
        perror ("Error");
        fprintf (stderr, "Não foi possivel alocar espaço para os ponteiros da matriz de pixel\n");
        exit (1);
    }                                                                   //Fim do bloco

    //Bloco: aloca espaço para a matriz de linhas contíguas (método 3 na wiki do Maziero)
    matriz[0] = malloc (largura * altura * sizeof(t_pixel));
    if (! matriz[0]) {
        perror ("Error");
        fprintf (stderr, "Não foi possivel alocar espaço para a matriz\n");
        exit (1);
    }                                                                   //Fim do bloco

    //Arruma os ponteiros
    int i;
    for (i = 0; i < altura; i++)                                        
        matriz[i] = matriz[0] + i*largura;

    return matriz;

}
t_ppm * inicializa_imagem (int largura, int altura) {

    t_ppm *imagem;
    imagem = malloc (sizeof (t_ppm));       //Aloca um vetor t_ppm (tipo ppm)
    if (! imagem){                          //Teste para ver se alocou
        perror ("Error");
        fprintf (stderr, "Não foi possível alocar um espaço na memória para o tipo imagem\n");
        exit (1);
    }

    imagem->nome_arquivo = aloca_vetor (TAM_ENTRADA);       //Aloca um vetor de char
    imagem->matrix = aloca_matriz_pixel (largura, altura);  //Aloca uma matriz de pixel

    return imagem;

}
//Trata dos comentários do imagem.ppm se ela tiver
//É passado um FILE *, o arquivo
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
void tamanho_imagem (FILE *entradappm, int *largura, int *altura) {

    char type[MAX_TYPE + 1];
    
    //Bloco: Lê o tipo da imagem (P6 ou P3) e testa para ver se o fgets leu corretamente
    if (! (fgets (type, MAX_TYPE + 1, entradappm))) {
        perror ("Error");
        fprintf (stderr, "Não foi possível ler o tipo da imagem do padrão pastilhas\n");
        exit (1);
    }                                                               //Fim do bloco

    ignorando_comentarios (entradappm);

    //Bloco: Lê a largura e a altura da imagem e testa para ver se o scanf leu corretamente
    if ( fscanf(entradappm, "%d %d", largura, altura) != COMPONENTES_IMG) {
        perror ("Error");
        fprintf (stderr, "O número de componentes da imagem dentro da função pastilhas_padrão foi lido errado\n");
        exit (1);   
    }                                                               //Fim do bloco

}

t_vetor_pastilhas * abrir_pastilhas (DIR * diretorio, struct dirent * file, char *nome_diretorio, int largura, int altura) {

    t_vetor_pastilhas * ptr;
    int mult;

    mult = 1;

    //Bloco: aloca um ptr do tipo t_vetor_pastilhas e testa para ver se alocou
    ptr = malloc (sizeof (t_vetor_pastilhas));
    if (! ptr) {
        perror ("Error");
        fprintf (stderr, "Não foi possível alocar espaço para o t_vetor_pastilhas\n");
        exit (1);
    }                                                               //Fim do bloco

    //Bloco: inicializa as estruturas do tipo t_vetor_pastilhas
    ptr->tam = 0;                                                   //Tamanho do vetor de pastilhas = 0                                                       
    ptr->vetor = malloc (sizeof (t_pastilha) * N_PASTILHAS);        //Aloca espaço para um vetor t_pastilha e testa para ver se alocou
    if (! ptr->vetor) {
        perror ("Error");
        fprintf (stderr, "Não foi possível alocar espaço para o vetor t_pastilhas dentro de t_vetor_pastilhas\n");
        exit (1);
    }                                                               //Fim do bloco

    //Inicializa a estrutura t_ppm dentro do tipo t_pastilha
    for (int i = 0; i < N_PASTILHAS; i++) {
        ptr->vetor[i].imagem = inicializa_imagem (largura, altura);    
    }

    //Bloco 1: Anda pelos arquivos do diretório, monta o nome do arquivo e passa ele para ser lido na função ler_imagem
    while ((file = readdir (diretorio))) {

        //Bloco: testa para ver se não é o diretório corrente ou o anterior
        if ( ! (strcmp (file->d_name, ".")))
            continue;
        if ( ! (strcmp (file->d_name, "..")))
            continue;                                               //Fim do bloco

        //Bloco: monta o nome do arquivo para ser aberto
        //Primeiro o nome da pasta e depois o nome do arquivo da pasta
        strcpy (ptr->vetor[ptr->tam].imagem->nome_arquivo, nome_diretorio);
        strcat (ptr->vetor[ptr->tam].imagem->nome_arquivo, file->d_name);
                                                                    //Fim do bloco

        //Função que lê a iamgem e carrega ela na memória
        ler_imagem (ptr->vetor[ptr->tam].imagem, ptr->vetor[ptr->tam].imagem->nome_arquivo);
        ptr->tam++;

        //Bloco 2: realoca mais espaço na memória se necessário
        //Se ptr->tam for igual a N_PASTILHAS (1024) vezes mult (1, ele é incrementado depois) pede mais espaço para a memória 
        if (ptr->tam == N_PASTILHAS*mult) {                 
            mult++;
            ptr->vetor = realloc (ptr->vetor, sizeof(t_pastilha) * (N_PASTILHAS * mult));
            for (int i = ptr->tam; i < N_PASTILHAS*mult; i++) {
                ptr->vetor[i].imagem = inicializa_imagem (largura, altura); //Para as novas estruturas alocadas aloca um tipo imagem 
            }
        }                                                           //Fim do bloco 2
    }                                                               //Fim do bloco 1
    return ptr;

}
//Carrega os pixels do arquivo para a memória
//É passado um FILE *, o arquivo, um int, o tipo da imagem, e dois int's, as dimensões da matriz
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

    //Se a imagem for P6, o imagem->tipo recebe 0, senão imagem->tipo recebe 1
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

    if (imagem->componente_rgb != 255) {
        perror ("Error");
        fprintf (stderr, "Componente RGB: %d diferente de 255\n", imagem->componente_rgb);
        exit (1);
    }
    
    //Bloco: aloca a matriz de pixel na memória e carrega os pixels na memória
    carrega_pixels (arquivo, imagem->matrix, imagem->tipo, imagem->largura, imagem->altura);                                             
                                                                     //Fim do bloco

    fclose (arquivo);

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

//Copia os dados da imagem origem para a imagem destino
void copia_dados (t_ppm *destino, t_ppm* origem) {

    destino->tipo = origem->tipo;
    destino->largura = origem->largura;
    destino->altura =  origem->altura;
    destino->componente_rgb = origem->componente_rgb;

}

//Calcula a menor distancia entre as cores do bloco da imagem principal analizado e as pastilhas
//Contas baseados no texto da wikipedia sobre distancia entre cores
static int compara_blocos (int tam_vetor, t_pixel *media_bloco_pastilhas, t_pixel bloco_imagem) {

    int indice_menor;
    float deltaC, menor_deltaC, delta_r, delta_g, delta_b, media_r;

    indice_menor = 0;
    
    //Bloco: Inicializa as variáveis usando a primeira primeira pastilha do vetor de pastilhas
    delta_r = media_bloco_pastilhas[0].vermelho - bloco_imagem.vermelho;
    delta_g = media_bloco_pastilhas[0].verde - bloco_imagem.verde;
    delta_b = media_bloco_pastilhas[0].azul - bloco_imagem.azul;

    media_r = (media_bloco_pastilhas[0].vermelho + bloco_imagem.vermelho)/2;
    menor_deltaC = sqrt((2 + media_r/(RGB + 1))*pow(delta_r, 2) + 4*pow(delta_g,2) + (2 + (RGB - media_r)/(RGB + 1))*pow(delta_b, 2));
                                                                        //Fim do bloco
    if (menor_deltaC < 0)   //Se for negativo o resultado (nunca vai ser, mas por precaução...), torna ele positivo
            menor_deltaC = menor_deltaC * -1;

    //Bloco: Percorre o vetor de pastilhas e compara todas as pastilhas com o bloco da imagem analizado. 
    //Acha a menor distancia e salva a posição que ela foi achada
    for (int i = 1; i < tam_vetor; i++) {

        delta_r = media_bloco_pastilhas[i].vermelho - bloco_imagem.vermelho;
        delta_g = media_bloco_pastilhas[i].verde - bloco_imagem.verde;
        delta_b = media_bloco_pastilhas[i].azul - bloco_imagem.azul;

        media_r = (media_bloco_pastilhas[i].vermelho + bloco_imagem.vermelho)/2;
        deltaC = sqrt((2 + media_r/(RGB + 1))*pow(delta_r, 2) + 4*pow(delta_g,2) + (2 + (RGB - media_r)/(RGB + 1))*pow(delta_b, 2));

        if (deltaC < 0)
            deltaC = deltaC * -1;

        if (deltaC < menor_deltaC) {
            menor_deltaC = deltaC;
            indice_menor = i;
        }
    }                                                                   //Fim do bloco

    return indice_menor;

}
//substitui o bloco da imagem analizada pela pastilha com a menor distância entre as cores
//É passada a matriz da imagem de saída, a matriz origem da pastilha, e quatro inteiros, que são as posições finais e iniciais
//da matriz da imagem de saída
void substitui_bloco (t_pixel ** matriz_saida, t_pixel ** matriz_pastilha, int linha_inicial, int linha_final, int coluna_inicial, int coluna_final) {

    int i, j;
    i = 0;
    //Bloco: basicamente percorre a matriz e substitui
    for (int k = linha_inicial; k < linha_final; k++) {
        j = 0;
        for (int l = coluna_inicial; l < coluna_final; l++) {
            matriz_saida[k][l].vermelho = matriz_pastilha[i][j].vermelho;
            matriz_saida[k][l].verde = matriz_pastilha[i][j].verde;
            matriz_saida[k][l].azul = matriz_pastilha[i][j].azul;
            j++;
        }
        i++;
    }                                                                   //Fim do bloco

}
//Analiza os casos que podem ocorrer quando se anda em blocos pela matriz da imagem de saída
//Retorna um inteiro mostrando qual caso é
static int condicoes_imagem (int linha_ini, int coluna_ini, int altura_imagem, int largura_imagem, int tam_bloco) {

    //Se for o último bloco e não couber uma pastilha de NxN ali
    if ((linha_ini + tam_bloco > altura_imagem) && (coluna_ini + tam_bloco > largura_imagem))
        return 0;
    //Se estiver na última linha e não couber uma pastilha de NxN ali
    else if ((linha_ini + tam_bloco > altura_imagem))
        return 1;
    //Se estiver na última coluna e não couber uma pastilhas de NxN ali
    else if ((coluna_ini + tam_bloco > largura_imagem))
        return 2;
    //Caso padrão onde cabe uma pastilhas de NxN
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
    //Bloco: anda a matriz de entrada por blocos e sustitui, na matriz da imagem de saída, pelo bloco com a menor distância
    //Contas facilmente explicadas 
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
    }                                                                       //Fim do bloco
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

    //Bloco: Escreve o cabeçalho do tipo PPM
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
    }                                                                       //Fim do bloco

    //Se for tipo P6
    if (outputppm->tipo == 0) {
        for (int i = 0; i < outputppm->altura; i++) {
            for (int j = 0; j < outputppm->largura; j++) {
                fwrite (&outputppm->matrix[i][j].vermelho, outputppm->tipo + 1, 1, arquivo);
                fwrite (&outputppm->matrix[i][j].verde, outputppm->tipo + 1, 1, arquivo);
                fwrite (&outputppm->matrix[i][j].azul, outputppm->tipo + 1, 1, arquivo);
            }
        }
    }
    //Se for tipo P3
    else {
        for (int i = 0; i < outputppm->altura; i++) {
            for (int j = 0; j < outputppm->largura; j++) {
                fprintf (arquivo,"%d ", outputppm->matrix[i][j].vermelho);
                fprintf (arquivo,"%d ", outputppm->matrix[i][j].verde);
                fprintf (arquivo,"%d ", outputppm->matrix[i][j].azul);
            }
            //fprintf (arquivo,"\n");
        }
    }

    fclose (arquivo);

}
/*void imprimir_pastilhas (t_vetor_pastilhas *ptr) {

    FILE * arquivo;
    arquivo = fopen ("teste.txt", "w");
    if (! arquivo) {
        perror ("Error");
        exit (1);
    }

    for (int i = 0; i < ptr->tam; i++) {
        fprintf (arquivo, "%d\n", ptr->vetor[i].imagem->componente_rgb);
    }

    fclose (arquivo);
}*/
