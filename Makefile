all:
	gcc -pthread blocking_sem.c -o blocking_sem
	gcc -pthread nonblocking_sem.c -o nonblocking_sem

clean:
	rm -f blocking_sem nonblocking_sem
