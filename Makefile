
# Uncomment one of these to hard-code a platform:
#
#OS:=LINUX
#OS:=WINDOWS
#OS:=OSX

ifeq ($(OS), WINDOWS)
EXE=prevector_bench.exe
else
EXE=prevector_bench
endif
ifndef OS
$(error OS is not defined.  Please run make with OS=LINUX or OS=OSX or OS=WINDOWS)
endif
LIBS=
ifeq ($(OS), LINUX)
LIBS=-pthread -lpthread
endif

all: $(EXE)

$(EXE): main.cpp prevector.h
	$(CXX) -o $(EXE) -O3 -fomit-frame-pointer -std=c++11 -D$(OS) $(LIBS) -DNDEBUG main.cpp

clean:
	rm -f $(EXE)
