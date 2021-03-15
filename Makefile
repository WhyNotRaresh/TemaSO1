GG = "gcc"
CFLAGS = "-Wall"
RESULT = "so-cpp"
FILE = "PreProcess-Main.c"

default:
	$(GG) $(CFLAGS) $(FILE) -o $(RESULT)

clean:
	rm $(RESULT)