# New ports collection makefile for:   creep
# Date created:        3 July 2011
# Whom:                Robert Crossfield
#
# $FreeBSD$
#

PORTNAME=      drcreep
DISTVERSION=   374
#PORTVERSION=   1
CATEGORIES=    games
MASTER_SITES=  SF/creep/

MAINTAINER=    segra@strobs.com
COMMENT=       The Castles of Dr. Creep

DATADIR=	${PREFIX}/share/${PORTNAME}
#MAN1=          drcreep.1
#MANCOMPRESSED= yes

USE_GMAKE=      yes
USE_SDL=        sdl

WRKSRC=		${WRKDIR}/head

PLIST_FILES= 	bin/${PORTNAME}

do-build:
	cd ${WRKSRC} &&	${CXX} -DFREEBSD -o ${PORTNAME} ${CXXFLAGS} \
				`${SDL_CONFIG} --cflags --libs` -I./src src/*.cpp src/graphics/*.cpp src/graphics/scale/*.cpp src/castle/*.cpp src/castle/objects/*.cpp src/vic-ii/*.cpp src/resid-0.16/*.cpp src/sound/*.cpp -lcompat

do-install:
	${INSTALL_PROGRAM} ${WRKSRC}/${PORTNAME} ${PREFIX}/bin
	${MKDIR} ${DATADIR}
	${MKDIR} ${DATADIR}/data
	${MKDIR} ${DATADIR}/data/castle
	${MKDIR} ${DATADIR}/data/castles
	${MKDIR} ${DATADIR}/data/save

	${INSTALL_DATA} ${WRKSRC}/run/data/char.rom ${DATADIR}/data

	@echo '------------------------------------------------------'
	@echo ''
	@echo 'Please copy your Castles of Dr. Creep disk image (D64) to ${DATADIR}/data'
	@echo ''
	@echo ''

.include <bsd.port.mk>
