# With GNU libc (glibc), librt is needed for clock_gettime().
# Comment out if this is not needed on your platform.
LIBS = -lrt

CFLAGS = -O2 -g -Wall -pedantic

clocktest: clocktest.c
	${CC} ${CFLAGS} -o $@ $< ${LIBS}

clean:
	-rm clocktest
