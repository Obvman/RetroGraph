ABSOLUTESOLNDIRBOU=/mnt/d/PG/RetroGraph/
DEVENVCYG=/cygdrive/d/ProgramFiles/Visual\ Studio\ 2015/Common7/IDE/devenv.com
#DEVENVBOU=wrun /mnt/d/ProgramFiles/Visual\ Studio\ 2015/Common7/IDE/devenv.com
DEVENVBOU=wrun /mnt/d/ProgramFiles/Visual\ Studio\ 2017/Common7/IDE/devenv.com
SOLNDIR=../
SOLN=$(SOLNDIR)RetroGraph.sln

# This just delegates building to the Visual Studio build system
all: debug

both: release debug 

debug: FORCE
	$(DEVENVBOU) $(SOLN) -build Debug

release: FORCE
	$(DEVENVBOU) $(SOLN) -build Release

brd: debug
	exec ../Debug/RetroGraph.exe

brr: debug
	exec ../Release/RetroGraph.exe

rund: FORCE
	wrun $(ABSOLUTESOLNDIRBOU)Debug/RetroGraph.exe

runr: FORCE
	cygstart --action=runas ../Release/RetroGraph.exe

FORCE:

rebuildd:
	$(DEVENVBOU) $(SOLN) -rebuild Debug

rebuildr:
	$(DEVENVBOU) $(SOLN) -rebuild Release

clean:
	$(DEVENVBOU) $(SOLN) -clean

# gcc building
# LINK_TARGET = gccRetroGraph.exe
# INCLUDE = -I..\deps\include -IC:\Program\ Files\ \(x86\)\Windows\ Kits\10\Include\10.0.15063\um
# OBJS = \
# 	main.o

# REBUILDABLES = $(OBJS) $(LINK_TARGET)

# gall: $(LINK_TARGET)
# 	echo done

# $(LINK_TARGET) : $(OBJS)
# 	g++ --std=c++14 -g -o $@ $^

# %.o : src/%.cpp
# 	g++ --std=c++14 $(INCLUDE) -g -o $@ -c $<

# main.o : src/main.cpp

