parser: bison.c flex.c AST.c AST.h Mysemantic.h Mysemantic.c
	gcc -o parser bison.c Mysemantic.c AST.c

bison.c: bison.y
	bison -o bison.c -dvt bison.y
bison.h: bison.y
	bison -o bison.c -dvt bison.y
flex.c: flex.l bison.h
	flex -o flex.c flex.l

.PHONY: clean
clean: 
	-rm bison.c bison.h bison.output parser flex.c

# =声明变量（宏） $()使用变量