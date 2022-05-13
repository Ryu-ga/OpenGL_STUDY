DEPEND=glew glfw3 cglm

CFLAGS=-O3 -mtune=native `pkg-config --cflags ${DEPEND}` -pthread -std=c11
LDFLAGS=`pkg-config --libs ${DEPEND}`
all: opengl

opengl: main shader
	${CC} -g main.o shader.o -o opengl ${LDFLAGS}

main: main.c
	${CC} -c main.c ${CFLAGS}

shader: shader.c
	${CC} -c shader.c ${CFLAGS}
clean:
	rm -rf opengl
