# Name of the executable
EXEC = note-detector

# Include PortAudio and FFTW headers and library archives
CLIB = -I/mingw64/include -L/mingw64/lib -lportaudio -lfftw3 -pthread

SRC = main.cpp audio_utils.cpp note_detector.cpp

$(EXEC): $(SRC)
	g++ -o $@ $^ $(CLIB)

clean:
	rm -f $(EXEC)
.PHONY: clean

clean-all: clean
	rm -rf lib
.PHONY: clean-all