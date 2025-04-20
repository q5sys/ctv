# $FreeBSD$

PORTNAME=	ctv
PORTVERSION=	1.0.0
CATEGORIES=	textproc

MAINTAINER=	ports@FreeBSD.org
COMMENT=	Command-line Text Viewer - a simple interactive text-based file browser

LICENSE=	MIT
LICENSE_FILE=	${WRKSRC}/LICENSE

LIB_DEPENDS=	libncurses.so:devel/ncurses

USES=		cmake compiler:c++17-lang
USE_GITHUB=	yes
GH_ACCOUNT=	q5sys

PLIST_FILES=	bin/ctv

.include <bsd.port.mk>
