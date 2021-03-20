CC=cl
LINK=link
CFLAGS=/MD /Wall
RESULT=so-cpp.exe

MAIN_FILE=PreProcess-Main.c
HASHMAP_FILE=hashmap.c
ARRAY_FILE=immutable_array.c
STR_AUX_FILE=text_processing.c

default: build

build: so-main

so-main: hashmap.obj array.obj text_processing.obj main.obj
	$(LINK) /out:$(RESULT) hashmap.obj array.obj text_processing.obj main.obj

main.obj:
	$(CC) $(CFLAGS) /Fomain.obj /c $(MAIN_FILE)

hashmap.obj:
	$(CC) $(CFLAGS) /Fohashmap.obj /c $(HASHMAP_FILE)

array.obj:
	$(CC) $(CFLAGS) /Foarray.obj /c $(ARRAY_FILE)

text_processing.obj:
	$(CC) $(CFLAGS) /Fotext_processing.obj /c $(STR_AUX_FILE)


clean_obj:
	del *.obj

clean:
	del $(RESULT)
