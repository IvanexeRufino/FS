# Makefile de todos los proyectos y librerias
# Utiliza los makes de cada proyecto que crea el eclipse

all: bibliotecaCompartida entrenador mapa pkmnUtils pokedexCliente pokedexServidor so-commons-library so-nivel-gui-library so-pkmn-utils

so-commons-library:
	mkdir -p commons/Debug
	mkdir -p commons/Debug/commons
	mkdir -p commons/Debug/commons/collections
	mkdir -p commons/Debug/commons/LibUtils
	make -C commons/ all

bibliotecaCompartida: 
	mkdir -p BibliotecaCompartida/Debug
	make -C BibliotecaCompartida/ all

entrenador:
	mkdir -p Entrenador/Debug
	make -C Entrenador/ all

mapa: commons nivel-gui
	mkdir -p Mapa/Debug	
	make -C Mapa/ all

pkmnUtils: commons
	mkdir -p pkmnUtils/Debug
	make -C pkmnUtils/ all

pokedexCliente: commons
	mkdir -p PokedexCliente/Debug
	make -C PokedexCliente/ all
	
pokedexServidor: commons
	mkdir -p PokedexServidor/Debug
	make -C PokedexServidor/ all
	
so-commons-library: commons
	mkdir -p so-commons-library/Debug
	make -C so-commons-library/ all
	
so-nivel-gui-library: commons
	mkdir -p so-nivel-gui-library/Debug
	make -C so-nivel-gui-library/ all

so-pkmn-utils: commons
	mkdir -p so-pkmn-utils/Debug
	make -C so-pkmn-utils/ all

clean:
	make -C bibliotecaCompartida/ clean
	make -C entrenador/ clean
	make -C mapa/ clean
	make -C pkmnUtils/ clean
	make -C pokedexCliente/ clean
	make -C pokedexServidor/ clean.
	make -C so-commons-library/ clean
	make -C so-nivel-gui-library/ clean
	make -C so-pkmn-utils/ clean

.PHONY: all bibliotecaCompartida entrenador mapa pkmnUtils pokedexCliente pokedexServidor so-commons-library so-nivel-gui-library so-pkmn-utils clean




