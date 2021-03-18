CC = "gcc"
CFLAGS = "-Wall"
RESULT = "so-cpp"

MAIN_FILE = "PreProcess-Main.c"
HASHMAP_FILE = "hashmap.c"
ARRAY_FILE = "array.c"
STR_AUX_FILE = "text_processing.c"

default: so-main clean_obj

so-main: hashmap.o array.o text_processing.o main.o
	$(CC) $(CFLAGS) -o $(RESULT) $^

main.o:
	$(CC) -o $@ -c $(MAIN_FILE)

hashmap.o:
	$(CC) -c $(HASHMAP_FILE)

array.o:
	$(CC) -c $(ARRAY_FILE)

text_processing.o:
	$(CC) -c $(STR_AUX_FILE)

clean_obj:
	rm *.o

clean:
	rm $(RESULT)