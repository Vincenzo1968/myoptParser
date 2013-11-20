#AM_CFLAGS = -Wall -W -pedantic -O2

lib_LTLIBRARIES = libmyoptParser.la
libmyoptParser_la_SOURCES = myoptParser.c myoptParser.h myoptLexer.c myoptLexer.h myoptSymtab.c myoptSymtab.h
libmyoptParser_la_CFLAGS = -Wall -W -pedantic -O2
libmyoptParser_la_LDFLAGS = $(LTLIBINTL)
include_HEADERS = myoptParser.h

localedir = $(datadir)/locale
DEFS += -DLOCALEDIR=\"$(localedir)\" @DEFS@
