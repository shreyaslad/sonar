default: all

.PHONY:	all

C_SOURCES	= $(shell find . -type f -name '*.c')
H_SOURCES	= $(shell find . -type f -name '*.h')
OBJ			= ${C_SOURCES:.c=.o}

all: ${TEST_KNL_TARGET}

${TEST_KNL_TARGET}: ${OBJ}
	${LD} ${LDFLAGS} ${OBJ} -o $@

%.o: %.c
	${CC} ${CFLAGS} -c $< -o $@
