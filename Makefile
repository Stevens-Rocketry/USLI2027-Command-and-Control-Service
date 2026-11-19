# By George K Oliynyk @ 2026

PROG=	ccservice
OBJS=	ccservice.o notify.o service.o test.o
CFLAGS+=	-Wall -Werror -Wextra -Wformat=2 -Wjump-misses-init -Wlogical-op -Wpedantic -Wshadow -g
LDLIBS= 	-lsystemd
VPATH=		src:src/states

all: ${PROG}

-include .depend

depend:
	${CC} -M ${CFLAGS} src/*.c src/*/*.c > .depend

${PROG}: ${OBJS}
	@echo $@ depends on $?
	${CC} ${LDFLAGS} ${OBJS} -o ${PROG} ${LDLIBS}

clean:
	rm -f ${PROG} ${OBJS}