CC=cl
CFLAGS=/MD
RESULT=so-cpp

MAIN_FILE=PreProcess-Main.c
HASHMAP_FILE=hashmap.c
ARRAY_FILE=immutable_array.c
STR_AUX_FILE=text_processing.c

default: so-main clean_obj

so-main: hashmap.o array.o text_processing.o main.o
	$(CC) $(CFLAGS) /Fe$(RESULT) $^

main.o:
	$(CC) /Fo$@ /c $(MAIN_FILE)

hashmap.o:
	$(CC) /Fo$@ /c $(HASHMAP_FILE)

array.o:
	$(CC) /Fo$@ /c $(ARRAY_FILE)

text_processing.o:
	$(CC) /Fo$@ /c $(STR_AUX_FILE)


clean_obj:
	rm *.o

clean:
	rm $(RESULT)