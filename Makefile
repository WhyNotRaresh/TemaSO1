CC=cl
LINK=link
CFLAGS="/MD /Wall"
RESULT=so-cpp

MAIN_FILE=PreProcess-Main.c
HASHMAP_FILE=hashmap.c
ARRAY_FILE=immutable_array.c
STR_AUX_FILE=text_processing.c

default: build

build: so-main clean_obj

so-main: hashmap.obj array.obj text_processing.obj main.obj
	$(LINK) $(CFLAGS) /Fe$(RESULT) hashmap.obj array.obj text_processing.obj main.obj

main.obj:
	$(CC) /Fomain.obj /c $(MAIN_FILE)

hashmap.obj:
	$(CC) /Fohashmap.obj /c $(HASHMAP_FILE)

array.obj:
	$(CC) /Foarray.obj /c $(ARRAY_FILE)

text_processing.obj:
	$(CC) /Fotext_processing.obj /c $(STR_AUX_FILE)


clean_obj:
	rm *.o

clean:
	rm $(RESULT)
