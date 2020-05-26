# Makefile
CXX = g++
LINKDIR =	-L/usr/X11R6/lib \
			-L/opt/local/lib
####################
LINUXDEFS =	-D__LINUX__
MACDEFS =	-D__MAC__
####################
MACINC =	-I/opt/local/include \
			-I/usr/local/include \
			-I/usr/X11R6/include
LINUXINC =  -I/usr/include/glpk/ \
	 		-I/usr/X11R6/include
####################
MACLIBS =	`pkg-config --libs opencv` \
			`gsl-config --cflags --libs` \
			-ltinyxml\
			-framework OpenGL -framework GLUT \
			-ljsoncpp

LINUXLIBS =	`pkg-config --libs opencv` \
			-lboost_regex \
			-lgsl -lgslcblas \
			-lglut -lGLU \
			-ljsoncpp -ltinyxml

####################
#DEBUG = -g -Wall -Wno-deprecated
DEBUG = -O -Wall -Wno-deprecated -std=gnu++11
CFLAGS = $(DEBUG)

#Graph.cpp, Graph.h

SRC =	main.cpp \

HDR =	\

OBJ = $(SRC:.cpp=.o)
BIN = receipt

.cpp.o: $(SRC)
	$(CXX) -c $(CFLAGS) $(DEFS) $(INC) $<

mac:
	make $(OBJ) "DEFS = $(MACDEFS)" "INC = $(MACINC)"
	$(CXX) -o $(BIN) $(CFLAGS) $(MACDEFS) $(OBJ) $(LINKDIR) $(MACINC) $(MACLIBS)

linux:
	make $(OBJ) "DEFS = $(LINUXDEFS)" "INC = $(LINUXINC)"
	$(CXX) -o $(BIN) $(CFLAGS) $(LINUXDEFS) $(OBJ) $(LINKDIR) $(LINUXINC) $(LINUXLIBS)

clean:
	rm -f *.o *~* core dot/*.dot pdf/*.pdf $(BIN)

.PHONY:linux mac clean

#
