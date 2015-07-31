test : test.c
	gcc -o test -g test.c -lutil -lpthread
clean:
	rm -f test
