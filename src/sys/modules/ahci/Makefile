# $FreeBSD: head/sys/modules/ahci/Makefile 292001 2015-12-08 20:05:27Z andrew $

.PATH: ${.CURDIR}/../../dev/ahci

KMOD=	ahci
SRCS=	ahci.c ahci_pci.c ahciem.c ahci.h device_if.h bus_if.h pci_if.h opt_cam.h

.include <bsd.kmod.mk>
