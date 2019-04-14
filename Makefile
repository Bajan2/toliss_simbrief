.SUFFIXES: .obj

TARGET=lin.xpl sbfetch_test

HEADERS=$(wildcard *.h)
OBJECTS=tlsb.o log_msg.o tlsb_http_get.o tlsb_ofp_get_parse.o clipboard.o
SDK=../SDK
PLUGDIR=/media/hd2/steam2/steamapps/common/X-Plane 11/Resources/plugins/toliss_simbrief

CC=gcc
LD=gcc

CFLAGS+=-O2 -Wall -fPIC -fdiagnostics-color -fvisibility=hidden \
	-I$(SDK)/CHeaders/XPLM -I$(SDK)/CHeaders/Widgets -DLIN=1 \
    -DXPLM200 -DXPLM210 -DXPLM300 -DXPLM301 $(DEFINES)

LDFLAGS=-shared -rdynamic -nodefaultlibs -undefined_warning -lpthread
LIBS= -lcurl


all: $(TARGET)

.c.o: $(HEADERS)
	$(CC) $(CFLAGS) -c $<

sbfetch_test: sbfetch_test.c tlsb_http_get.c tlsb_ofp_get_parse.c log_msg.c $(HEADERS)
	$(CC) $(CFLAGS) -DLOCAL_DEBUGSTRING -o sbfetch_test sbfetch_test.c tlsb_http_get.c tlsb_ofp_get_parse.c log_msg.c -lcurl

lin.xpl: $(OBJECTS)
	$(LD) -o lin.xpl $(LDFLAGS) $(OBJECTS) $(LIBS)

clean:
	rm $(OBJECTS) $(TARGET)

# install the just compiled target
install: $(TARGET)
	mkdir -p "$(PLUGDIR)/64"
	cp -p lin.xpl "$(PLUGDIR)/64"

# create the whole plugin directory
plugin: $(TARGET)
	mkdir -p "$(PLUGDIR)/64"
	cp -p lin.xpl "$(PLUGDIR)/64"
