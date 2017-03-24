ABSOLUTESOLNDIRBOU=/mnt/c/PG/RetroGraph/
DEVENVCYG=/cygdrive/d/ProgramFiles/Visual\ Studio\ 2015/Common7/IDE/devenv.com
DEVENVBOU=wrun /mnt/d/ProgramFiles/Visual\ Studio\ 2015/Common7/IDE/devenv.com
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
