# CHARMBASE = $(HOME)/charm
CHARMBASE = /Users/shudler1/projects/charm

CONDUIT_DIR = /Users/shudler1/projects/chrm/ascent/_uberenv_build/spack/opt/spack/darwin-mojave-x86_64/clang-10.0.0-apple/conduit-0.7.2-2j6ix27s5veicwrajkbyuvhjonzkeiw7
ASCENT_DIR = /Users/shudler1/install/ascent
BFLOW_DIR = /Users/shudler1/install/babelflow

OPTS = -O3
CHARMC = $(CHARMBASE)/bin/charmc $(OPTS)

OUT := lulesh
C_CI := $(wildcard *.ci)
C_SRCS := $(wildcard *.cc)
C_OBJS := $(C_SRCS:.cc=.o)
DECL := $(C_CI:.ci=.decl.h)
DEF := $(C_CI:.ci=.def.h)
DECL_DEF := $(DECL) $(DEF)

INC_FLAGS := -I$(CONDUIT_DIR)/include/conduit -I$(ASCENT_DIR)/include/ascent -I$(BFLOW_DIR)/include/BabelFlow/charm 
LD_FLAGS := -L$(CONDUIT_DIR)/lib -L$(ASCENT_DIR)/lib -lconduit -lconduit_blueprint -lascent -lascent_flow

.PHONY: all projections loadbalance clean distclean

all: $(OUT)

$(OUT): $(C_OBJS) 
	$(CHARMC) -language charm++ $(C_OBJS) $(LD_FLAGS) -lm -o $(OUT)

%.o: %.cc $(DECL_DEF)
	$(CHARMC) -language charm++ $(INC_FLAGS) $< -o $@

projections: $(C_SRCS) $(DECL_DEF)
	$(CHARMC) -tracemode projections -tracemode summary -module CommonLBs -language charm++ $(C_SRCS) -lm -o $(OUT)

loadbalance: $(C_SRCS) $(DECL_DEF)
	$(CHARMC) -language charm++ -module CommonLBs $(C_SRCS) -lm -o $(OUT)

$(DECL_DEF): $(C_CI)
	$(CHARMC) $(C_CI)

clean:
	rm -f *.decl.h *.def.h *.o charmrun $(OUT)

distclean: clean
