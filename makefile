COMP = g++
FLAG = -o
INPT = main_test.cpp Color.cpp Drawer.cpp Line.cpp Point.cpp Wireframe.cpp Map.cpp Window.cpp
OUTP = app

all : program1

program1 : 
	@echo "Compile your program"
	${COMP} ${FLAG} ${OUTP} ${INPT}
	@echo "Your program are ready"

clean :
	@echo "Cleaning up ..."
	rm ${OUTP}
