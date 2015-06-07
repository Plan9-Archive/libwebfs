</$objtype/mkfile

P=webfs

LIB=lib$P.$O.a
OFILES=$P.$O
HFILES=/sys/include/$P.h
CLEANFILES=$O.test

</sys/src/cmd/mksyslib

install:V:	$LIB
	cp $LIB /$objtype/lib/lib$P.a
	cp $P.h /sys/include/$P.h

uninstall:V:
	rm -f /$objtype/lib/lib$P.a /sys/include/$P.h

test:	$O.test
	./$O.test

$O.test:	test.$O
	$LD -o $target $prereq
