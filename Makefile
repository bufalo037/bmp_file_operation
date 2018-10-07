FLAGS=-Wall
EXE=imgproc
EXEv=imgprocg
CC=gcc
build: $(EXE)
$(EXE): main.o functii.o
	$(CC) $(FLAGS) $^ -o $(EXE) 
main.o: main.c functii.h bmp_header.h
	$(CC) $(FLAGS) -c $< -o $@
functii.o: functii.c functii.h bmp_header.h
	$(CC) $(FLAGS) -c $< -o $@
run: build
	./$(EXE)
buildv: $(EXEv)
$(EXEv): main.c functii.c
	$(CC) $(FLAGS) $^ -g -o $(EXEv)
.PHONY: clean

clean:
	rm *.o $(EXE) $(EXEv)