# Compiler e flags
CXX = g++
CXXFLAGS = -std=c++17 -DSFML_STATIC -Ilibs/SFML-3.0.0/include
LDFLAGS = -Llibs/SFML-3.0.0/lib -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lopengl32 -lfreetype -lwinmm -lgdi32 -mwindows

# Arquivos fonte
SRC = main.cpp state_stack.cpp game_states/menu.cpp

# Geração de objetos a partir dos fontes
OBJ = $(SRC:.cpp=.o)

# Nome do executável final
TARGET = game

# Alvo principal
all: $(TARGET)

# Regra para compilar o executável
$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET) $(LDFLAGS)

# Regra para compilar arquivos .cpp em .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Limpa os arquivos gerados
clean:
	del /Q *.o $(TARGET).exe 2> NUL || true
