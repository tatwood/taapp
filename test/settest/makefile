EXE=../bin/settest
EXED=../bin/settestd
OBJS=obj/make.o
OBJSD=objd/make.o
INCLUDES=-I../../include
LIBS=
CC=g++
CCFLAGS=-Wall -msse -O3 -fno-rtti -fno-exceptions $(INCLUDES)
CCFLAGSD=-Wall -msse -O0 -ggdb2 -fno-rtti -fno-exceptions -D_DEBUG $(INCLUDES)
LD=g++
LDFLAGS=$(LIBS)

$(EXE): obj ../bin $(OBJS)
	$(LD) $(OBJS) $(LDFLAGS) -o $(EXE)

$(EXED): objd ../bin $(OBJSD)
	$(LD) $(OBJSD) $(LDFLAGS) -o $(EXED)

obj:
	mkdir obj

objd:
	mkdir objd

../bin:
	mkdir ../bin

obj/make.o : make.cpp
	$(CC) $(CCFLAGS) -c $< -o $@

objd/make.o : make.cpp
	$(CC) $(CCFLAGSD) -c $< -o $@

all: $(EXE) $(EXED)

clean:
	rm -rf $(EXE) $(EXED) obj objd

debug: $(EXED)

release: $(EXE)	
