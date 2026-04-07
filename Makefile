# NoteReader - Real-time audio pitch detection
# Supports Linux native, macOS, and Windows MSYS2 MinGW64 builds

# Detect OS
UNAME_S := $(shell uname -s)
IS_LINUX := $(filter Linux,$(UNAME_S))
IS_DARWIN := $(filter Darwin,$(UNAME_S))

# Executable name
EXEC = note-detector
TEST_EXEC = note-test

# Source files
SRC = main.cpp audio_utils.cpp note_detector.cpp dsp.cpp
TEST_SRC = test_dsp.cpp audio_utils.cpp note_detector.cpp dsp.cpp

# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g

# Platform-specific settings
ifeq ($(IS_LINUX),Linux)
    # Linux native build using local libraries
    INCLUDES = -I./lib/portaudio/include -I./lib/fftw-3.3.10/api
    LIBS = ./lib/portaudio/lib/.libs/libportaudio.a \
           ./lib/fftw-3.3.10/.libs/libfftw3.a \
           -lrt -lpthread -lm
else ifeq ($(IS_DARWIN),Darwin)
    # macOS
    INCLUDES = -I/usr/local/include
    LIBS = -lportaudio -lfftw3 -lpthread -lm
else
    # Windows MSYS2 MinGW64
    INCLUDES = -I/c/msys64/mingw64/include
    LIBS = -L/c/msys64/mingw64/lib -lportaudio -lfftw3 -lpthread -lm
endif

CXXFLAGS += $(INCLUDES)
LDFLAGS = $(LIBS)

# Pattern rule for all object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Targets
.PHONY: all clean test help info

all: $(EXEC)

$(EXEC): main.o audio_utils.o note_detector.o dsp.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

test: $(TEST_EXEC)
	./$(TEST_EXEC)

$(TEST_EXEC): test_dsp.o audio_utils.o note_detector.o dsp.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f *.o $(EXEC) $(TEST_EXEC)

info:
	@echo "Build configuration:"
	@echo "  OS: $(UNAME_S)"
	@echo "  Compiler: $(CXX)"
	@echo "  Includes: $(INCLUDES)"
	@echo "  Libraries: $(LIBS)"

help:
	@echo "NoteReader Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  all      - Build the main executable (default)"
	@echo "  test     - Build and run tests"
	@echo "  clean    - Remove build artifacts"
	@echo "  info     - Show build configuration"
	@echo "  help     - Show this help message"
	@echo ""
	@echo "Usage:"
	@echo "  make           # Build"
	@echo "  make test      # Build and run tests"
	@echo "  make clean     # Clean build artifacts"
	@echo ""
	@echo "Windows MSYS2 MinGW64: Libraries expected at C:\\msys64\\mingw64"
