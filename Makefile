.PHONY: generate build run format clean

generate:
	cmake --preset default

build:
	cmake --build build --target App -j 18

run:
	./build/App.exe

format:
	clang-format -i ./src/*.c ./src/core/*.c ./src/game/*.c ./src/platform/*.c ./src/math/*.c ./src/render/*.c

clean:
	rm -rf build compile_commands.json .cache
