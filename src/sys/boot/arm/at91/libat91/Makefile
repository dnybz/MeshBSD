#  $FreeBSD: releng/11.0/sys/boot/arm/at91/libat91/Makefile 265420 2014-05-06 04:22:01Z imp $

.include <src.opts.mk>
.include "${.CURDIR}/../Makefile.inc"

SOC?=at91rm9200

LIB=		at91
INTERNALLIB=
SRCS=${SOC}_lowlevel.c delay.c eeprom.c emac.c emac_init.c getc.c \
	putchar.c printf.c reset.c spi_flash.c xmodem.c \
	sd-card.c strcvt.c strlen.c strcmp.c memcpy.c strcpy.c \
	memset.c memcmp.c
SRCS+=ashldi3.c divsi3.S
MAN=

.if ${MK_TAG_LIST} != "no"
CFLAGS += -I${.CURDIR}/.. -DSUPPORT_TAG_LIST
SRCS+=tag_list.c
.endif

.include <bsd.lib.mk>
