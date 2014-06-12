prefix = /usr
frcsimdir = $(prefix)/share/frcsim
modeldir = $(frcsimdir)/models
worlddir = $(frcsimdir)/worlds

all:
	echo Should download files

clean:
	echo Cleaning

install: all
	mkdir -p $(DESTDIR)$(modeldir)
	mkdir -p $(DESTDIR)$(worlddir)
	cp -r models/** $(DESTDIR)$(modeldir)
	cp -r worlds/** $(DESTDIR)$(worlddir)
