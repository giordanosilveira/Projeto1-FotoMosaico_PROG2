CFLAGS = -Wall -g  # gerar "warnings" detalhados e infos de depuração
LDLIBS = -lm

objs = mosaico.o libmosaico.o
 
# regra default (primeira regra)
all: mosaico
 
# regras de ligacao
mosaico: $(objs)
 
# regras de compilação
mosaico.o: mosaico.c libmosaico.h
libmosaico.o: libmosaico.c libmosaico.h
 
# remove arquivos temporários
clean:
	-rm -f $(objs) *~ *.o
 
# remove tudo o que não for o código-fonte
purge: clean
	-rm -f mosaico
