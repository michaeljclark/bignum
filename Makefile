# definitions

AR          := $(shell which ar)
CXX         := $(shell which c++)
INCLUDES    = -Isrc
DEBUG_FLAGS = -g
OPT_FLAGS   = -O3
WARN_FLAGS  = -Wall
CXXFLAGS    = $(DEBUG_FLAGS) $(OPT_FLAGS) $(WARN_FLAGS) $(INCLUDES) -std=c++11
LDFLAGS     = -L/usr/local/lib -Lbuild/lib -lbignum


BIGNUM_OBJS    = build/obj/bignum.o

# build targets

all: libs

libs: build/lib/libbignum.a

tests: build/bin/test_1 build/bin/test_2 build/bin/bench_1

check: tests
	@build/bin/test_1 && echo "PASSED"

clean: ; rm -fr build

backup: clean
	tar czf ../$(shell basename $(shell pwd)).tar.gz .

# build rules

.PRECIOUS: build/obj/%.o

build/obj/%.o: src/%.cpp
	@echo CXX $@ ; mkdir -p $(@D) ; $(CXX) $(CXXFLAGS) -c -o $@ $<

build/obj/%.o: tests/%.cpp
	@echo CXX $@ ; mkdir -p $(@D) ; $(CXX) $(CXXFLAGS) -c -o $@ $<

build/lib/libbignum.a: $(BIGNUM_OBJS)
	@echo AR $@ ; mkdir -p $(@D) ; $(AR) cr $@ $^

build/bin/%: build/obj/%.o build/lib/libbignum.a
	@echo LD $@ ; mkdir -p $(@D) ; $(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS) -lgmp
