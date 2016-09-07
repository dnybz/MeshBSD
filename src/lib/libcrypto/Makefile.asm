# $FreeBSD: head/secure/lib/libcrypto/Makefile.asm 299480 2016-05-11 20:06:23Z jkim $
# Use this to help generate the asm *.S files after an import.  It is not
# perfect by any means, but does what is needed.
# Do a 'make -f Makefile.asm all' and it will generate *.S.  Move them
# to the arch subdir, and correct any exposed paths and $ FreeBSD $ tags.

.include "Makefile.inc"

.include <bsd.prog.mk>
