VPATH=src

CPP=g++
CPPFLAGS += -Iinclude -Wall -Werror -fopenmp -O2
LDFLAGS += -lmpfr

default: ciccomp_impulse.png ciccomp_spectrum.png

ciccomp_impulse.png ciccomp_spectrum.png: ciccomp.dat analyze.py
	./analyze.py

ciccomp.dat: main
	./main

main: main.o band.o cheby.o eigenvalue.o barycentric.o pm.o
	$(CPP) $(CPPFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CPP) $(CPPFLAGS) -c -o $@ $<

clean:
	-rm -f *.o main ciccimp.dat *.png *.pdf
