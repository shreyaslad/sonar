default: all

.PHONY:	all		\
		symlist	\

C_SOURCES	= $(shell find . -type f -name '*.c')
H_SOURCES	= $(shell find . -type f -name '*.h')
ASM_SOURCES	= $(shell find . -type f -name '*.asm')

OBJ			= ${C_SOURCES:.c=.o} ${ASM_SOURCES:.asm=.o}

CFLAGS		+=							\
			-I.							\
			-Ilib						\
			-Ithirdparty/lai/include	\

all: ${SONAR_KNL_TARGET}

${SONAR_KNL_TARGET}: ${OBJ} symlist
	${LD} ${LDFLAGS} ${OBJ} sys/symlist.o -o $@
	sh gensyms.sh
	${CC} -x c ${CFLAGS} -c sys/symlist.gen -o sys/symlist.o
	${LD} ${LDFLAGS} ${OBJ} sys/symlist.o -o $@

symlist:
	echo '#include <sys/symlist.h>' > sys/symlist.gen
	echo 'struct symlist_t symlist[] = {{0xffffffffffffffff,""}};' >> sys/symlist.gen
	${CC} -x c ${CFLAGS} -c sys/symlist.gen -o sys/symlist.o

%.o: %.c
	${CC} ${CFLAGS} -c $< -o $@

%.o: %.asm
	nasm -f elf64 -F dwarf -g -o $@ $<
