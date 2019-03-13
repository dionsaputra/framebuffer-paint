COMP = g++
FLAG = -o
INPT = main.cpp Color.cpp Drawer.cpp Point.cpp Wireframe.cpp PaintController.cpp Parser.cpp
OUTP = app

all : program1

program1 : 
	@echo "Compile your program"
	${COMP} ${FLAG} ${OUTP} ${INPT}
	@echo "Your program are ready"

clean :
	@echo "Cleaning up ..."
	rm ${OUTP}
