#ABSOLUTESOLNDIRBOU=/mnt/d/PG/RetroGraph/
#DEVENVBOU=wrun /mnt/d/ProgramFiles/Visual\ Studio\ 2017/Common7/IDE/devenv.com
DEVENVBOU=devenv.com
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
	../Debug/RetroGraph.exe

brr: debug
	../Release/RetroGraph.exe

rund: FORCE
	$(SOLNDIR)Debug/RetroGraph.exe

runr: FORCE
	$(SOLNDIR)Release/RetroGraph.exe

FORCE:

rebuildd:
	$(DEVENVBOU) $(SOLN) -rebuild Debug

rebuildr:
	$(DEVENVBOU) $(SOLN) -rebuild Release

clean: cleand cleanr

cleand:
	$(DEVENVBOU) $(SOLN) -clean Debug

cleanr:
	$(DEVENVBOU) $(SOLN) -clean Release

