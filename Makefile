INCLUDE_PATH = /usr/local/include/libxml2
#INCLUDE_PATH = /usr/local/lib
LIB_PATH = /home/hakyu/Project/lib

all: html_parser_by_dart tags

dartpage: dartpage.c
	gcc -Wl,-rpath -Wl,LIBDIR -g -o dartpage dartpage.c -I${INCLUDE_PATH} -lxml2
#gcc -g -Wl,-rpath -Wl,LIBDIR -o dartpage dartpage.c -L ${INCLUDE_PATH}

html_parser_by_dart: html_parser_by_dart.c
	gcc -g -o html_parser_by_dart html_parser_by_dart.c -L$(LIB_PATH) -lfnstring

tags:
	ctags html_parser_by_dart

clean:
	rm html_parser_by_dart

