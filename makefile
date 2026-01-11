all:
	# build embeded text files
	xxd -i ./source/scourge/standard/print.scourge > ./build_temps/scourge-print.c
	xxd -i ./source/scourge/standard/cast.scourge > ./build_temps/scourge-cast.c
	xxd -i ./source/scourge/standard/buffer.scourge > ./build_temps/scourge-buffer.c
	xxd -i ./source/scourge/standard/current.scourge > ./build_temps/scourge-current.c
	xxd -i ./source/scourge/standard/list.scourge > ./build_temps/scourge-list.c
	xxd -i ./source/scourge/standard/context.scourge > ./build_temps/scourge-context.c
	xxd -i ./source/scourge/standard/check.scourge > ./build_temps/scourge-check.c
	xxd -i ./source/scourge/standard/error.scourge > ./build_temps/scourge-error.c
	xxd -i ./source/scourge/standard/json.scourge > ./build_temps/scourge-json.c
	xxd -i ./source/scourge/standard/time.scourge > ./build_temps/scourge-time.c
	xxd -i ./source/scourge/standard/anvil.scourge > ./build_temps/scourge-anvil.c
	xxd -i ./source/scourge/standard/compile.scourge > ./build_temps/scourge-compile.c
	xxd -i ./source/scourge/standard/just_run.scourge > ./build_temps/scourge-just_run.c

	# compile executables
	gcc ./source/main.c -Wall -Wextra -fsanitize=address -g -o ./../scourge-debug.elf
	gcc ./source/main.c -o ./../scourge.elf
