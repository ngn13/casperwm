build/casperwm: src/*
	mkdir -p build 
	gcc -lX11 -o build/casperwm src/*.c

install:
	cp build/casperwm /usr/bin/casperwm
	cp install/casperwm.desktop /usr/share/xsessions/casperwm.desktop

uninstall:
	rm /usr/bin/casperwm 
	rm /usr/share/xsessions/casperwm.desktop

dev:
	Xephyr -br -ac -noreset -screen 1800x1000 :1

.PHONY: uninstall install dev
