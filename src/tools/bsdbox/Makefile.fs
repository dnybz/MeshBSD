#
# Toolbox on filesystem
#

###################################################################
# Programs from stock /bin
#

###################################################################
# Programs from standard /sbin
#
CRUNCH_PROGS_sbin+=	fsck_ffs mdmfs mdconfig mount_msdosfs newfs
CRUNCH_ALIAS_mdmfs=	mount_mfs

##################################################################
# Programs from stock /usr/bin
#

##################################################################
# Programs from stock /usr/sbin
#

##################################################################
# Library dependencies
#
CRUNCH_LIBS+= -lbsdxml
CRUNCH_LIBS+= -lgeom
CRUNCH_LIBS+= -lkiconv
CRUNCH_LIBS+= -lsbuf
CRUNCH_LIBS+= -lufs

