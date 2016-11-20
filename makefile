# Makefile de todos los proyectos y librerias
# Utiliza los makes de cada proyecto que crea el eclipse

all: so-commons-library bibliotecaCompartida nivel-gui pkmnUtils entrenador mapa pokedexCliente pokedexServidor  

so-commons-library:
	mkdir -p so-commons-library/src/Debug
	make -C so-commons-library/src/ all
	sudo make install -C so-commons-library/src
	sudo apt-get install libncurses5-dev

bibliotecaCompartida: so-commons-library
	
	make -C BibliotecaCompartida/Debug/ all

nivel-gui: so-commons-library
	mkdir -p nivel-gui/nivel-gui/Debug
	make -C nivel-gui/nivel-gui
	sudo make install -C nivel-gui/nivel-gui

pkmnUtils: so-commons-library
	mkdir -p pkmnUtils/src/Debug
	make -C pkmnUtils/src/ all
	sudo make install -C pkmnUtils/src

entrenador: so-commons-library BibliotecaCompartida 
	
	make -C Entrenador/Debug/ all

mapa: so-commons-library nivel-gui pkmnUtils BibliotecaCompartida 
		
	make -C Mapa/Debug/ all

pokedexCliente: so-commons-library BibliotecaCompartida 
	
	make -C PokedexCliente/Debug/ all
	
pokedexServidor: so-commons-library BibliotecaCompartida 

	make -C PokedexServidor/Debug/ all

clean:
	make -C so-commons-library/ clean
	make -C BibliotecaCompartida/Debug/ clean
	make -C nivel-gui/ clean
	make -C pkmnUtils/ clean
	make -C Entrenador/Debug/ clean
	make -C Mapa/Debug/ clean
	make -C PokedexCliente/Debug/ clean
	make -C PokedexServidor/Debug/ clean

.PHONY: all so-commons-library bibliotecaCompartida nivel-gui pkmnUtils entrenador mapa pokedexCliente pokedexServidor clean
