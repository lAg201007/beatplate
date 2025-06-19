# Compiler e flags
CXX = g++
CXXFLAGS = -std=c++17 -DSFML_STATIC -Ilibs/SFML-3.0.0/include

# Linker flags (release)
LDFLAGS = -Llibs/SFML-3.0.0/lib -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lopengl32 -lfreetype -lwinmm -lgdi32 -mwindows

# Linker flags (debug, sem -mwindows)
DEBUG_LDFLAGS = -Llibs/SFML-3.0.0/lib -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lopengl32 -lfreetype -lwinmm -lgdi32

# Arquivos fonte
SRC = main.cpp state_stack.cpp game_logic/note_objects/hold_plate.cpp game_logic/note_objects/plate.cpp game_states/game.cpp game_states/menu.cpp game_states/song_select.cpp shaders/shader_manager.cpp utils/tween_storage.cpp utils/utilities.cpp

# Objetos gerados
OBJ = $(SRC:.cpp=.o)

# Nome do executável
TARGET = game

# Alvo padrão
all: $(TARGET)

# Alvo release (com -mwindows)
$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET) $(LDFLAGS)

# Alvo debug (sem -mwindows)
debug: CXXFLAGS += -g
debug: $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET)-debug $(DEBUG_LDFLAGS)

# Regra de compilação de .cpp para .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Limpa arquivos gerados
clean:
	rm -f *.o game_states/*.o $(TARGET).exe $(TARGET)-debug.exe