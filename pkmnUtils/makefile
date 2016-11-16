all: 
	-cd src && $(MAKE) all
	-cd test && $(MAKE) all

clean:
	-cd src && $(MAKE) clean
	-cd test && $(MAKE) clean

debug:
	-cd src && $(MAKE) debug
	-cd test && $(MAKE) debug

test: all
	-cd test && $(MAKE) test

install: test
	-cd src && $(MAKE) install

uninstall:
	-cd src && $(MAKE) uninstall
