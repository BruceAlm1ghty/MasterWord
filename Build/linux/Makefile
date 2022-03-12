CC=g++
CPPFLAGS=-std=c++11 -I../../include -I/usr/lib/x86_64-linux-gnu/wx/include/gtk2-unicode-3.0 -I/usr/include/wx-3.0 -D_FILE_OFFSET_BITS=64 -DWXUSINGDLL -D__WXGTK__ -pthread
DEPS = 
ODIR=obj
LIBS=
SDIR=../../src

# echo | g++ -dM -E -x c++ -

_OBJ = App.o CheckingGameCtrl.o Frame.o Game.o GameCtrl.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SDIR)/%.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CPPFLAGS)

MasterWord: $(OBJ)
	$(CC) -o $@ $^ $(CPPFLAGS) $(LIBS)


