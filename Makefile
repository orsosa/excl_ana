.DELETE_ON_ERROR:

ROOTCFLAGS  := $(shell root-config --cflags)
ROOTLDFLAGS := $(shell root-config --ldflags)
ROOTLIBS    := $(shell root-config --libs) -lEG
ROOTGLIBS   := $(shell root-config --glibs)

INC_DIR   := -I.

CXX       := g++
CXXFLAGS  += -Wall -fPIC $(ROOTCFLAGS) -std=c++11 #-g -O0
LD        := g++
LDFLAGS   := $(ROOTLDFLAGS)

AR	  = ar
ARFLAGS	  = -cvr #create,verbose,quick (don't check for replacement, otherwise use r instead)

all: checkdirs slib/libexcl.so run_excl

checkdirs: dict slib

dict slib:
	@mkdir -p $@

%: %.o
	$(CXX) -o $@ $< $(ROOTCFLAGS) $(ROOTLDFLAGS) $(ROOTLIBS) -Lslib -lexcl

%.o: %.cxx 
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(ROOTCFLAGS) $(INC_DIR)


dict/dictexcl.cxx: excl_ana.h 
	rootcling -f $@ -c $(ROOTCFLAGS) $(HIPOCFLAGS) -p $^


slib/libexcl.so: excl_ana.cxx
	g++ -shared -fPIC -o $@ $(ROOTLDFLAGS) $(ROOTCFLAGS) $(HIPOCFLAGS) $(INC_DIR) $(ROOTCFLAGS) $^ #-g -O0
#	cp dict/datadict_rdict.pcm slib/.

