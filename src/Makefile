CC = g++
DEFINES = 
override CFLAGS += -Wall -Wextra -pedantic -std=c++11 -MD -MP -O3 $(DEFINES)
LFLAGS = 
INCLUDES =
LIBS = -lpthread -lstdc++ -lboost_program_options
SRC = $(wildcard *.cpp)
HEADERS = $(wildcard *.h *.inl)
OBJS = $(SRC:.cpp=.o)
TARGET = cut
RM = rm -f *.o *.d $(TARGET) 

ifdef SystemRoot # Windows
	LIBS += -lpsapi
	RM = del /Q *.o $(TARGET).exe
	BUILD = $(TARGET)
else 
	ifeq ($(shell uname -o),Cygwin)
		LIBS += -lpsapi
		BUILD = $(TARGET)
		# BUILD = msbuild
		# RM = MSBuild.exe /p:Configuration=Release /t:clean
	else 
		BUILD = $(TARGET)
	endif
endif

all: $(BUILD)
	
msbuild:
	MSBuild.exe /p:Configuration=Release

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(OBJS) $(LFLAGS) $(LIBS)

-include $(SRC:.cpp=.d)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

rebuild: clean all

clean:
	$(RM)
