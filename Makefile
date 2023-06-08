CC := gcc
CFLAGS := -ggdb -Wall -Wextra -pedantic
LDLIBS += -lraylib -lm

LIBOBJ := sprite.o cutscene.o tween.o wordWrap.o textbox.o
LOCALLIBDIR := build
LOCALINCLUDEDIR := include
SRCDIR := src

INSTALL_PROGRAM := install --debug
INSTALL_FLAGS := -o root
INSTALL_FLAGS_DIR := -m 755
INSTALL_FLAGS_FILE := -m 644
prefix := /usr/local
includedir := $(prefix)/include/customLib
libdir := $(prefix)/lib

# ------

LDFLAGS += -L$(LOCALLIBDIR) 
CFLAGS += -I$(LOCALINCLUDEDIR)

INSTALL_DATA := $(INSTALL_PROGRAM) $(INSTALL_FLAGS)

GPATH=$(LOCALLIBDIR)

vpath %.o $(LOCALLIBDIR)
vpath %.a $(LOCALLIBDIR)
vpath %.h $(LOCALINCLUDEDIR)
vpath %.c $(SRCDIR)


.PHONY: main clean install


main: libcutscene.a main.c cutsceneData.o
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) -o main $^ -lcutscene

clean:
	@$(RM) -vr $(LOCALLIBDIR)/
	@$(RM) -v *.o

install: libcutscene.a | cutscene.h
	$(INSTALL_DATA) $(INSTALL_FLAGS_DIR) -d $(DESTDIR)$(libdir)
	$(INSTALL_DATA) $(INSTALL_FLAGS_FILE) -C $(LOCALLIBDIR)/libcutscene.a -t $(DESTDIR)$(libdir)

	$(INSTALL_DATA) $(INSTALL_FLAGS_DIR) -d $(DESTDIR)$(includedir)
	$(INSTALL_DATA) $(INSTALL_FLAGS_FILE) -C $(LOCALINCLUDEDIR)/* -t $(DESTDIR)$(includedir)/

$(LOCALLIBDIR)/libcutscene.a: $(LIBOBJ)
	$(AR) rsv $@ $?

%.o: %.c $(LDLIBS) | $(LOCALLIBDIR)/
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $(LOCALLIBDIR)/$@ $<

$(LOCALLIBDIR)/:
	mkdir -v $@

