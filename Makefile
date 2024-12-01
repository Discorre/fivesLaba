# Имя исполняемого файла
TARGET = marketplace

# Путь к компилятору
CXX = g++

# Флаги компиляции
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic

# Исходные файлы
SRCS = main.cpp

# Объектные файлы
OBJS = $(SRCS:.cpp=.o)

# Правило по умолчанию
all: $(TARGET)
	rm -f $(OBJS)
	clear
	./marketplace

# Правило для сборки исполняемого файла
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Правило для компиляции каждого .cpp файла в .o файл
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Правило для очистки объектных файлов и исполняемого файла
clean:
	rm -f $(OBJS) $(TARGET)

# Правило для перекомпиляции всего проекта
rebuild: clean all