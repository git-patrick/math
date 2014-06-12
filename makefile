
all: build

build:

install:
	rm -R /usr/local/include/math
	mkdir /usr/local/include/math
	cp include/*.h /usr/local/include/math