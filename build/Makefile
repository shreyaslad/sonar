default: all

.PHONY:	all				\
		limine			\
		bddisasm		\
		lai				\
		clean			\

LAI_C_SOURCES	= ${wildcard ${LAI_DIR}/*.c}
LAI_O_SOURCES	= ${LAI_C_SOURCES:.c=.o}

all:
	if [ ! -d '${LIMINE_DIR}' ] ; then	\
	make limine;	\
	fi	\

	if [ ! -d '${BDDISASM_DIR}' ] ; then	\
	make bddisasm;	\
	fi	\
	
	if [ ! -d '${LAI_COMPILE_DIR}' ] ; then	\
	make lai;	\
	fi	\

limine:
	git clone ${LIMINE_REPO} ${LIMINE_DIR} --branch=${LIMINE_BRANCH}
	make -C ${LIMINE_DIR} limine-install

bddisasm:
	git clone ${BDDISASM_REPO} ${BDDISASM_DIR} --branch=${BDDISASM_BRANCH}
	make -C ${BDDISASM_DIR} bddisasm

lai:
	git clone ${LAI_REPO} ${LAI_COMPILE_DIR} --branch=${LAI_BRANCH}
	mkdir -p ${LAI_HEADER_DIR}
	cp -r ${LAI_COMPILE_DIR}/include/acpispec ${LAI_COMPILE_DIR}/include/lai ${LAI_HEADER_DIR}

clean:
	rm -rf ${DEPS_DIR}
