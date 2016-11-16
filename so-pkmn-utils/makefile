all: 
	-cd src && $(MAKE) all
	-cd tests && $(MAKE) all

clean:
	-cd src && $(MAKE) clean
	-cd tests && $(MAKE) clean

debug:
	-cd src && $(MAKE) debug
	-cd tests && $(MAKE) debug

test: all
	-cd tests && $(MAKE) test

install: test
	-cd src && $(MAKE) install

uninstall:
	-cd src && $(MAKE) uninstall
