
.include <src.opts.mk>

BINDIR=		/etc/c/rc.d
FILESGROUPS=	FILES


FILES=	dhcpd
FILES+= ntpd
FILES+= cron
FILES+= hostapd
FILES+= pf

.for fg in ${FILESGROUPS}
${fg}MODE?=	${BINMODE}
.endfor

.include <bsd.prog.mk>

