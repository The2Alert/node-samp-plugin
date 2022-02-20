build-start:
	cd ./build/current && cmake ../.. && cd ../..
build-win32-start:
	cd ./build/current && cmake ../.. -A Win32 && cd ../..
build:
	cmake --build ./build/current --config Release
build-linux-start:
	docker build . -t node-samp-plugin-linux-builder
build-linux:
	docker run --rm -v ${CURDIR}:/plugin --name builder node-samp-plugin-linux-builder