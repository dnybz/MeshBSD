# $FreeBSD: head/usr.sbin/bsnmpd/modules/Makefile 266936 2014-06-01 03:14:03Z loos $

.include <src.opts.mk>

.PATH: ${.CURDIR}/../../../tools/bsnmp/snmpd

SUBDIR=	snmp_bridge 
#	snmp_hast 
SUBDIR+= 	snmp_hostres 
SUBDIR+= 	snmp_lm75
SUBDIR+= 	snmp_mibII 
SUBDIR+= 	snmp_target 
SUBDIR+= 	snmp_usm 
SUBDIR+= 	snmp_vacm 
SUBDIR+= 	snmp_wlan

.if ${MK_PF} != "no"
SUBDIR+= 	snmp_pf
.endif

.if ${MK_NETGRAPH_SUPPORT} != "no"
SUBDIR+=snmp_netgraph
.endif

INCS=	snmpmod.h
INCSDIR= ${INCLUDEDIR}/bsnmp

.include <bsd.prog.mk>
