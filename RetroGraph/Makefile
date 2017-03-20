DEVENV=/cygdrive/d/ProgramFiles/Visual\ Studio\ 2015/Common7/IDE/devenv.com
SOLNDIR=../
SOLN=$(SOLNDIR)RetroGraph.sln

# This just delegates building to the Visual Studio build system

all: debug

both: release debug 

debug: FORCE
	$(DEVENV) $(SOLN) -build Debug

release: FORCE
	$(DEVENV) $(SOLN) -build Release

brd: debug
	exec ../Debug/RetroGraph.exe

brr: debug
	exec ../Release/RetroGraph.exe

rund: FORCE
	exec ../Debug/RetroGraph.exe

runr: FORCE
	cygstart --action=runas ../Release/RetroGraph.exe

FORCE:
