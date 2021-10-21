# Projeto1-FotoMosaico_PROG2
Projeto que tem como intenção pegar um arquivo.ppm e transformar sua imagem em um Foto Mosaico.

O programa recebe da linha de comando os parâmentros i, o e p que representam, respectivamente, o nome de entrada do arquivo.ppm,
o nome de saída do Foto Moisaico e a pasta que contêm as pastilhas necessárias para fazer o foto mosaico.

Sobre as pastilhas, são pequenas imagens de tamanho NxN - 32x32, 30x30, 20x20 - que ficam dentro de um repositório do computador.
As pastilhas são carregadas na memória, e a partir daí é calculada suas cores médias.

A imagem passada, também é carregada na memória. É calculada a média de cada bloco NxN da imagem grande (principal) e esse bloco é
substituído pela pastilha que tem a cor mais próxima.

Por fim, é montada a imagem de saída e fim do programa. :)
