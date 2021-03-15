CC = "gcc"
CFLAGS = "-Wall"
RESULT = "so-cpp"

MAIN_FILE = "PreProcess-Main.c"
HASHMAP_FILE = "hashmap.c"

default: so-main clean_obj

so-main: hashmap.o main.o
	$(CC) $(CFLAGS) -o $(RESULT) $^

main.o:
	$(CC) -o $@ -c $(MAIN_FILE)

hashmap.o:
	$(CC) -c $(HASHMAP_FILE)

clean_obj:
	rm *.o

clean:
	rm $(RESULT)