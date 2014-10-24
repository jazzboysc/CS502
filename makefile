CC = gcc
CFLAGS  = -c -Wall
LDFLAGS = -Wno-write-strings \
          -Wno-int-to-pointer-cast \
          -Wno-pointer-to-int-cast

SOURCES = base.c \
          critical_section.c \
          heap.c \
          interrupt_handler.c \
          list.c \
          priority_queue.c \
          process_manager.c \
          sample.c \
          scheduler.c \
          state_printer.c \
          svc.c \
          test.c \
          mytest.c \
          z502.c

OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = cs502.exe

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@