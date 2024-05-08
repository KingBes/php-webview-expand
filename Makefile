NAME			= webview
CPP             = g++
RM              = rm -f
CPP_FLAGS       = -Wall -c -I. $(shell pkg-config --cflags gtk+-3.0 webkit2gtk-4.0)-DWEBVIEW_STATIC -O3 -std=c++11

# 拓展so文件路径
LIBRARY_DIR		= /usr/local/lib/php/extensions/no-debug-non-zts-20220829
# ini文件路径
PHP_CONFIG_DIR	= /usr/local/etc/php/conf.d

LD              = g++
LD_FLAGS        = -Wall -Wextra -pedantic -shared -O3 
RESULT          = ${NAME}.so

PHPINIFILE		= docker-php-ext-${NAME}.ini

SOURCES			= $(wildcard *.cpp)
OBJECTS         = $(SOURCES:%.cpp=%.o)

all:	${OBJECTS} ${RESULT}

${RESULT}: ${OBJECTS}
		${LD} ${LD_FLAGS} -o $@ ${OBJECTS} -lphpcpp $(shell pkg-config --libs gtk+-3.0 webkit2gtk-4.0)

clean:
		${RM} *.obj *~* ${OBJECTS} ${RESULT}

${OBJECTS}: 
		${CPP} ${CPP_FLAGS} -fPIC -o $@ ${@:%.o=%.cpp}

install:
		cp -f ${RESULT} ${LIBRARY_DIR}/
		cp -f ${PHPINIFILE}	${PHP_CONFIG_DIR}/

uninstall:
		rm ${LIBRARY_DIR}/${RESULT}
		rm ${PHP_CONFIG_DIR}/${PHPINIFILE}
