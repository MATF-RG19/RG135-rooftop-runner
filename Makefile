main: main.c biblioteka.h
	gcc main.c -o main -lGL -lGLU -lglut -lm -Wall
biblioteka:
	gcc biblioteka.h -Wall
