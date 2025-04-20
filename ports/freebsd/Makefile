# $FreeBSD$

PORTNAME=       ctv
PORTVERSION=    1.1.0
CATEGORIES=     textproc
USE_GITHUB=    yes
GH_ACCOUNT=    q5sys
GH_PROJECT=    ctv
GH_TAGNAME=    1.1.0

MAINTAINER=     jt@obs-sec.com
COMMENT=        Command-line Text Viewer - a simple interactive text-based file browser

LICENSE=        MIT
LICENSE_FILE=   ${WRKSRC}/LICENSE

LIB_DEPENDS=    libncurses.so:devel/ncurses

USES=           cmake compiler:c++17-lang

PLIST_FILES=    bin/ctv

.include <bsd.port.mk>
