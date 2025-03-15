saveload: main.c floorgenerator.c saveloader.c
	gcc -o dungeon main.c floorgenerator.c saveloader.c -lm
clean: dungeon
	rm -f dungeon
	rm -rf build 
debug: main.c floorgenerator.c saveloader.c
	gcc -o dungeon main.c floorgenerator.c saveloader.c -lm -g
