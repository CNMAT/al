AL_BASENAMES = al_obj al_pv al_region al_osctime al_type al_osc al_strfmt al_time al_atom al_list al_alist al_env al_boot al oscal_parse oscal_lex


AL_CFILES = $(foreach F, $(AL_BASENAMES), $(F).c) 
AL_HFILES = $(foreach F, $(AL_BASENAMES), $(F).h)
AL_OFILES = $(foreach F, $(AL_BASENAMES), $(F).o)

#LIBO_SCANNER_BASENAMES = osc_scanner osc_expr_scanner osc_legacy_scanner osc_lex
# LIBO_SCANNER_BASENAMES = osc_lex
# LIBO_SCANNER_LFILES = $(foreach OBJ, $(LIBO_SCANNER_BASENAMES), $(OBJ).l)
# LIBO_SCANNER_CFILES = $(foreach OBJ, $(LIBO_SCANNER_BASENAMES), $(OBJ).c)
# LIBO_SCANNER_HFILES = $(foreach OBJ, $(LIBO_SCANNER_BASENAMES), $(OBJ).h)
# LIBO_SCANNER_OBJECTS = $(foreach OBJ, $(LIBO_SCANNER_BASENAMES), $(OBJ).o)

#LIBO_PARSER_BASENAMES = osc_parser osc_expr_parser osc_legacy_parser osc_parse
# LIBO_PARSER_BASENAMES = osc_parse
# LIBO_PARSER_YFILES = $(foreach OBJ, $(LIBO_PARSER_BASENAMES), $(OBJ).y)
# LIBO_PARSER_CFILES = $(foreach OBJ, $(LIBO_PARSER_BASENAMES), $(OBJ).c)
# LIBO_PARSER_HFILES = $(foreach OBJ, $(LIBO_PARSER_BASENAMES), $(OBJ).h)
# LIBO_PARSER_OBJECTS = $(foreach OBJ, $(LIBO_PARSER_BASENAMES), $(OBJ).o)

AL_OBJECTS = $(AL_OFILES) #$(LIBO_SCANNER_OBJECTS) $(LIBO_PARSER_OBJECTS)

RELEASE-CFLAGS += -Wall -Wno-trigraphs -fno-strict-aliasing -O3 -funroll-loops -std=c99
DEBUG-CFLAGS += -Wall -Wno-trigraphs -fno-strict-aliasing -O0 -g -std=c99

MAC-CFLAGS = -arch i386 -arch x86_64
MAC-DEBUG-CFLAGS = -arch x86_64
ARM-CFLAGS = -arch armv7 -arch armv7s
WIN-CFLAGS = -mno-cygwin -DWIN_VERSION -DWIN_EXT_VERSION -U__STRICT_ANSI__ -U__ANSI_SOURCE -std=c99

MAC-INCLUDES = -I/usr/include
WIN-INCLUDES = 

all: CFLAGS += $(RELEASE-CFLAGS)
all: CFLAGS += $(MAC-CFLAGS)
all: CC = clang
all: I = $(MAC-INCLUDES)
all: $(AL_CFILES) $(AL_HFILES) libal.a #$(LIBO_SCANNER_CFILES) $(LIBO_PARSER_CFILES) libo.a
all: STATIC-LINK = libtool -static -o libal.a $(AL_OBJECTS) /usr/local/lib/libfl.a
all: DYNAMIC-LINK = clang -dynamiclib $(MAC-CFLAGS) -single_module -compatibility_version 1 -current_version 1 -o libal.dylib $(AL_OBJECTS)
all: al_test

debug: CFLAGS += $(DEBUG-CFLAGS)
debug: CFLAGS += $(MAC-DEBUG-CFLAGS)
debug: CC = clang
debug: I = $(MAC-INCLUDES)
debug: $(AL_CFILES) $(AL_HFILES) libal.a #$(LIBO_SCANNER_CFILES) $(LIBO_PARSER_CFILES) libo.a
debug: STATIC-LINK = libtool -static -o libal.a $(AL_OBJECTS) /usr/local/lib/libfl.a
debug: al_test

# win: CFLAGS += $(RELEASE-CFLAGS)
# win: CFLAGS += $(WIN-CFLAGS)
# win: CC = i686-w64-mingw32-gcc
# win: I = $(WIN-INCLUDES)
# win: $(LIBO_PARSER_CFILES) $(LIBO_SCANNER_CFILES) libo.a 
# win: STATIC-LINK = ar cru libo.a $(LIBO_OBJECTS) /usr/lib/libfl.a

linux: CC = clang
linux: CFLAGS += -std=c99 -fPIC -DLINUX_VERSION -D_XOPEN_SOURCE=500
linux: $(AL_CFILES) $(AL_HFILES) libal.a #$(LIBO_SCANNER_CFILES) $(LIBO_PARSER_CFILES) libo.a
linux: STATIC-LINK = ar cru libal.a $(AL_OBJECTS) /usr/lib/libfl.a

# swig: CC = clang
# swig: CFLAGS += -std=c99
# swig: libo.i libo_wrap.c libo.py _libo.so

# libo_wrap.c libo.py:
# 	swig -python libo.i

# _libo.so: libo.py libo_wrap.c setup.py
# 	python setup.py build_ext --inplace

# node: odot_wrap.cxx build/Release/odot.node

# odot_wrap.cxx:
# 	swig -javascript -node -c++ odot.i

# build/Release/odot.node:
# 	node-gyp rebuild


libal.a: $(AL_OBJECTS)
	rm -f libal.a
	$(STATIC-LINK)

libal.dylib: $(AL_OBJECTS)
	rm -f libal.dylib
	$(DYNAMIC-LINK)

%.o: %.c 
	$(CC) $(CFLAGS) $(I) -c -o $(basename $@).o $(basename $@).c

oscal_lex.c: oscal_lex.l oscal_parse.c
	flex -o $(basename $@).c --prefix=$(basename $@)_ --header-file=$(basename $@).h $(basename $@).l

oscal_parse.c: oscal_parse.y
	bison -p $(basename $@)_ -d -v --report=itemset -o $(basename $@).c $(basename $@).y

# %_scanner.c: %_scanner.l %_parser.c
# 	flex -o $(basename $@).c --prefix=$(basename $@)_ --header-file=$(basename $@).h $(basename $@).l
# osc_lex.c: osc_lex.l osc_parse.c
# 	flex -o $(basename $@).c --prefix=$(basename $@)_ --header-file=$(basename $@).h $(basename $@).l

# %_parser.c: %_parser.y
# 	bison -p $(basename $@)_ -d -v --report=itemset -o $(basename $@).c $(basename $@).y
# osc_parse.c: osc_parse.y
# 	bison -p $(basename $@)_ -d -v --report=itemset -o $(basename $@).c $(basename $@).y

al_test: al_test.c $(AL_CFILES) #osc_lex.c osc_parse.c
	$(CC) $(CFLAGS) $(I) -L. -lal  -o al_test al_test.c

.PHONY: doc
doc:
	cd doc && doxygen Doxyfile

.PHONY: clean
clean:
	rm -rf *.o libal.a libal.dylib *~ $(AL_PARSER_CFILES) $(AL_PARSER_HFILES) $(AL_SCANNER_CFILES) $(AL_SCANNER_HFILES) *.output al_test *.dSYM
	cd doc && rm -rf html latex man
	cd test && $(MAKE) clean
	cd contrib && rm -rf *.o

# .PHONY: swig-clean
# swig-clean:
# 	rm -rf libo.py libo_wrap.c _libo.so
# node-clean:
# 	rm -rf build odot_wrap.cxx
