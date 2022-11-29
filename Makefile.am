ACLOCAL_AMFLAGS=-I m4
lib_LTLIBRARIES = libinterflop_verrou.la

libinterflop_verrou_la_SOURCES = interflop_verrou.cxx
libinterflop_verrou_la_CFLAGS = -fno-stack-protector -fPIC
libinterflop_verrou_la_CXXFLAGS = -fno-stack-protector -fPIC

libinterflop_verrou_la_CFLAGS +=-DVERROU_DET_HASH=vr_@vg_cv_verrou_det_hash@_hash
libinterflop_verrou_la_CXXFLAGS +=-DVERROU_DET_HASH=vr_@vg_cv_verrou_det_hash@_hash
libinterflop_verrou_la_CFLAGS +=-DVERROU_NUM_AVG=@VERROU_NUM_AVG@
libinterflop_verrou_la_CXXFLAGS +=-DVERROU_NUM_AVG=@VERROU_NUM_AVG@

if WALL_CFLAGS
libinterflop_verrou_la_CFLAGS += -Wall -Wextra -Wno-varargs
endif
libinterflop_verrou_la_LIBADD = @INTERFLOP_STDLIB_PATH@/libinterflop_prng.la
if BUILD_INTERFLOP_STDLIB
libinterflop_verrou_la_LIBADD += @INTERFLOP_STDLIB_PATH@/libinterflop_stdlib.la
endif
libinterflop_verrou_la_includedir =$(includedir)/
