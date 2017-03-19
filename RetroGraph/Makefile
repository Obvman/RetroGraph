DEVENV=/cygdrive/d/ProgramFiles/Visual\ Studio\ 2015/Common7/IDE/devenv.com
SOLNDIR=../
SOLN=$(SOLNDIR)RetroGraph.sln
DEBUGEXE=$(SOLNDIR)Debug/RetroGraph.exe
RELEASEEXE=$(SOLNDIR)Release/RetroGraph.exe

all: debug release

debug: FORCE
	$(DEVENV) $(SOLN) -build Debug

release: FORCE
	$(DEVENV) $(SOLN) -build Release

runD: FORCE
	$(DEBUGEXE)$

runR:

FORCE:
