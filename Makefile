# definitions

AR          := $(shell which ar)
CXX         := $(shell which c++)
INCLUDES    = -Isrc
DEBUG_FLAGS = -g
OPT_FLAGS   = -O3
WARN_FLAGS  = -Wall
CXXFLAGS    = $(DEBUG_FLAGS) $(OPT_FLAGS) $(WARN_FLAGS) $(INCLUDES) -std=c++11
LDFLAGS     = -L/usr/local/lib -Lbuild/lib -lbitvector


BITVECTOR_OBJS    = build/obj/bitvector.o

# build targets

all: libs

libs: build/lib/libbitvector.a

tests: build/bin/test_1 build/bin/test_2 build/bin/bench_1

clean: ; rm -fr build

backup: clean
	tar czf ../$(shell basename $(shell pwd)).tar.gz .

# build rules

.PRECIOUS: build/obj/%.o

build/obj/%.o: src/%.cpp
	@echo CXX $@ ; mkdir -p $(@D) ; $(CXX) $(CXXFLAGS) -c -o $@ $<

build/obj/%.o: tests/%.cpp
	@echo CXX $@ ; mkdir -p $(@D) ; $(CXX) $(CXXFLAGS) -c -o $@ $<

build/obj/%.o: demo/%.cpp
	@echo CXX $@ ; mkdir -p $(@D) ; $(CXX) $(CXXFLAGS) $(EDIT_CFLAGS) -c -o $@ $<

build/lib/libbitvector.a: $(BITVECTOR_OBJS)
	@echo AR $@ ; mkdir -p $(@D) ; $(AR) cr $@ $^

build/bin/%: build/obj/%.o build/lib/libbitvector.a
	@echo LD $@ ; mkdir -p $(@D) ; $(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS) -lgmp
