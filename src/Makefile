GCC = gcc
FLAGS = -Wall -v
FLAGS_STATIC = -static
DEBUG?=
RM = rm -f
CP = cp -v -f

WIN_GCC = i686-w64-mingw32-gcc
WIN_FLAGS = -Wall -v

OBJECTS=diffscgen.c diffscgen.h

all: diffscgen win32 linux32 linux64

diffscgen.o: $(OBJECTS)
	$(GCC) $(FLAGS) $(DEBUG) -c -o $@ $<
	@echo $(OBJECTS)

diffscgen: diffscgen.o
	$(GCC) $(FLAGS) $(DEBUG) -o $@ $<
	$(CP) $@ ../bin/

win32: diffscgen-w32
	$(CP) $< ../win32/diffscgen.exe

diffscgen-w32.o: $(OBJECTS)
	$(WIN_GCC) $(WIN_FLAGS) -c -o $@ $<
	@echo $(OBJECTS)

diffscgen-w32: diffscgen-w32.o
	$(WIN_GCC) $(WIN_FLAGS) -o $@ $<

linux32: diffscgen-linux32
	$(CP) $< ../linux32/diffscgen

diffscgen-linux32.o: $(OBJECTS)
	$(GCC) $(FLAGS) $(FLAGS_STATIC) -m32 -c -o $@ $<
	@echo $(OBJECTS)

diffscgen-linux32: diffscgen-linux32.o
	$(GCC) $(FLAGS) $(FLAGS_STATIC) -m32 -o $@ $<

linux64: diffscgen-linux64
	$(CP) $< ../linux64/diffscgen

diffscgen-linux64.o: $(OBJECTS)
	$(GCC) $(FLAGS) $(FLAGS_STATIC) -c -o $@ $<
	@echo $(OBJECTS)

diffscgen-linux64: diffscgen-linux64.o
	$(GCC) $(FLAGS) $(FLAGS_STATIC) -o $@ $<

clean:
	$(RM) diffscgen diffscgen.o diffscgen-w32 diffscgen-w32.o diffscgen-linux32 diffscgen-linux32.o diffscgen-linux64 diffscgen-linux64.o
	$(RM) ../bin/diffscgen
