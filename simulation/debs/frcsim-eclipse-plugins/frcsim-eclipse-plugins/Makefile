prefix = /usr
eclipsedir = $(prefix)/share/eclipse/dropins/frcsim
plugindir = $(eclipsedir)/plugins
featuredir = $(eclipsedir)/features

all: clean
	sh ./genfeatures.sh

clean:
	rm -rf build

install: all
	mkdir -p $(DESTDIR)$(plugindir)
	mkdir -p $(DESTDIR)$(featuredir)
	install plugins/** $(DESTDIR)$(plugindir)
	cp -r build/features/** $(DESTDIR)$(featuredir)
