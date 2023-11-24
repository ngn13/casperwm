build/casperwm: src/*
	mkdir -p build
	gcc -lX11 -linih -o build/casperwm src/*.c

install:
	cp build/casperwm /usr/bin/casperwm
	cp install/casperwm.desktop /usr/share/xsessions/casperwm.desktop
	mkdir -p ~/.config/casperwm/
	cp install/config.default ~/.config/casperwm/config 
	cp install/start ~/.config/casperwm/start 
	chown +x ~/.config/casperwm/start
	chown -R $USER:$USER ~/.config/casperwm

uninstall:
	rm /usr/bin/casperwm
	rm /usr/share/xsessions/casperwm.desktop

dev:
	Xephyr -br -ac -noreset -screen 1800x1000 :1

.PHONY: uninstall install dev test
