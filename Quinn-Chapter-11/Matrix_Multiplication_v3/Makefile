CPP_APPS=Matrix_Multiplication_v3
CPP_OBJS=

all: ${CPP_APPS}

clean:
	rm -f ${CPP_APPS}

Matrix_Multiplication_v3: Matrix_Multiplication_v3.cpp
	mpic++ -g -lmpichfort -o $@ $<
	