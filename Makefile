.PHONY: total, clean

# Disable implicit rules
.SUFFIXES:

total :
	make all -C src
	make install -C src

clean:
	make clean -C src
