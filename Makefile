
# Uncomment one of these to choose a platform:
#
OS:=LINUX
#OS:=WINDOWS
#OS:=OSX

ifeq ($(OS), WINDOWS)
	EXE=prevector_bench.exe
else
	EXE=prevector_bench
endif

all: $(EXE)

prevector_bench: main.cpp prevector.h
	$(CXX) -o $(EXE) -O2 -std=c++11 -D$(OS) -DNDEBUG main.cpp

clean:
	rm -f $(EXE)
