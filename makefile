help:
	@echo "Usage:"
	@echo "make test    - Builds the main test"
	@echo "make packtest - Builds the packaged audio test"
test:
	@if [ -d build ];then\
		rm -rf build;\
	fi
	@mkdir build
	gcc src/audio.c src/test.c -lSDL2 -o build/test.out
packtest:
	@if [ -d build ];then\
		rm -rf build;\
	fi
	@mkdir build

	@echo "Building c sourcefiles..."
	gcc src/packagedAudioTest.c -o build/test.o -c
	gcc src/audio.c -o build/audio.o -c

	@echo "Generating linkable objects for the audio files..."

	ld -b binary -r music/highlands.wav -o build/highlands.o
	@echo "# Accessible at the following symbols: #"
	objdump -x build/highlands.o|grep "_*binary.*" -o

	ld -b binary -r music/road.wav -o build/road.o
	@echo "# Accessible at the following symbols: #"
	objdump -x build/road.o|grep "_*binary.*" -o

	ld -b binary -r sounds/door1.wav -o build/door1.o
	@echo "# Accessible at the following symbols: #"
	objdump -x build/door1.o|grep "_*binary.*" -o

	ld -b binary -r sounds/door2.wav -o build/door2.o
	@echo "# Accessible at the following symbols: #"
	objdump -x build/door2.o|grep "_*binary.*" -o
	@ echo "Linking everything..."
	gcc build/* -lSDL2 -o build/test.out
	
	@#Comment this line if you want to analyse the ".o" files with "objdump"
	@rm build/*.o