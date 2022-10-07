CC=g++

###

debug: 
	${CC} -I./header main.c source/stack.c source/memory_functions.c source/hash.c -DDEBUG -O0 -Og -Wall -Wextra -Werror -o debug

release:
	${CC} -I./header main.c source/stack.c source/memory_functions.c   -O3 -Wall -Wextra -Werror -o release

test:
	${CC} -I./header main.c source/stack.c source/memory_functions.c  -O3 -Wall -Wextra -Werror -o test

