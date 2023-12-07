build/casperwm: src/*
	mkdir -p build
	gcc -lX11 -linih -o build/casperwm src/*.c

install:
	cp build/casperwm /usr/bin/casperwm
	cp install/casperwm.desktop /usr/share/xsessions/casperwm.desktop

install_cfg:
	mkdir -p ~/.config/casperwm/
	cp install/config.default ~/.config/casperwm/config 
	cp install/start ~/.config/casperwm/start 
	chmod +x ~/.config/casperwm/start

uninstall:
	rm /usr/bin/casperwm
	rm /usr/share/xsessions/casperwm.desktop

dev:
	Xephyr -br -ac -noreset -screen 1800x1000 :1

.PHONY: uninstall install install_cfg dev test
