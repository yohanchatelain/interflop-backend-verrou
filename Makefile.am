ACLOCAL_AMFLAGS=-I m4
lib_LTLIBRARIES = libinterflop_verrou.la

libinterflop_verrou_la_SOURCES = interflop_verrou.cxx
libinterflop_verrou_la_CFLAGS = -flto -O2 -march=native -fno-stack-protector
libinterflop_verrou_la_CXXFLAGS = -flto -O2 -march=native -fno-stack-protector 
libinterflop_verrou_la_LDFLAGS = -flto -O2 -march=native

libinterflop_verrou_la_CFLAGS +=-DVERROU_DET_HASH=vr_@vg_cv_verrou_det_hash@_hash
libinterflop_verrou_la_CXXFLAGS +=-DVERROU_DET_HASH=vr_@vg_cv_verrou_det_hash@_hash
libinterflop_verrou_la_CFLAGS +=-DVERROU_NUM_AVG=@VERROU_NUM_AVG@
libinterflop_verrou_la_CXXFLAGS +=-DVERROU_NUM_AVG=@VERROU_NUM_AVG@

if WALL_CFLAGS
libinterflop_verrou_la_CFLAGS += -Wall -Wextra -Wno-varargs -g
endif
libinterflop_verrou_la_LIBADD = \
    @INTERFLOP_STDLIB_PATH@/lib/libinterflop_prng.la \
    @INTERFLOP_STDLIB_PATH@/lib/libinterflop_fma.la 
    
if LINK_INTERFLOP_STDLIB
libinterflop_verrou_la_LIBADD += @INTERFLOP_STDLIB_PATH@/lib/libinterflop_stdlib.la
endif
libinterflop_verrou_la_includedir =$(includedir)/
include_HEADERS = interflop_verrou.h