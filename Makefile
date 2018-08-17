CC     = gcc
CFLAGS = -Wall
INCLUDE= -I/usr/include
LIBS   = -L/usr/lib
LIBS   = -lz
SRC    = *.c
OUT    = m3g-reader


$(OUT):$(SRC)
	$(CC) $(CFLAGS) $(SRC) $(INCLUDE) $(LIBS) -o $(OUT)
