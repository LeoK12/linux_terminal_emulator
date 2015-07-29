test : test.c
	gcc -o test -g test.c -lutil
clean:
	rm -f test
