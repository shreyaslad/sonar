default: all

.PHONY:	all				\
		limine			\
		clean			\

all:
	if [ ! -d '${LIMINE_DIR}' ] ; then	\
	make limine;	\
	fi	\

limine:
	git clone ${LIMINE_REPO} ${LIMINE_DIR} --branch=${LIMINE_BRANCH}-binary
	make -C $(LIMINE_DIR)

clean:
	rm -rf ${DEPS_DIR}
