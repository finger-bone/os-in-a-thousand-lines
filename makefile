all:
	./run.sh

clean:
	rm -rf ./*.o ./*.elf ./*.bin ./*.map

format:
	clang-format -i ./src/*.c ./src/*.h