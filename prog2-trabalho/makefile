CFLAGS = -Wall -g  # gerar "warnings" detalhados e infos de depuração
LDLIBS = -lm

objs = mosaico.o lib_mosaico.o
 
# regra default (primeira regra)
all: mosaico
 
# regras de ligacao
mosaico: $(objs)
 
# regras de compilação
mosaico.o: mosaico.c lib_mosaico.h
lib_mosaico.o: lib_mosaico.c lib_mosaico.h
 
# remove arquivos temporários
clean:
	-rm -f $(objs) *~ *.o
 
# remove tudo o que não for o código-fonte
purge: clean
	-rm -f mosaico
