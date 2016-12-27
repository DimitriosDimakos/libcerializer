AUTOMAKE_OPTIONS = foreign #include-deps
where_PRIMARY =  _LTLIBRARIES 

lib_LTLIBRARIES=libcerializer.la

libcerializer_la_SOURCES= \
       cerializer.c \
       dynmessage.c \
       dynmessage_cerializer.c \
       hashmap.c \
       log.c \
       slinkedlist.c \
       stdlib_util.c \
       string_util.c \
       cerializer.h \
       dynmessage.h \
       dynmessage_cerializer.h \
       hashmap.h \
       log.h \
       slinkedlist.h \
       stdlib_util.h \
       string_util.h

libcerializer_la_HEADERS= \
       cerializer.h \
       dynmessage.h \
       dynmessage_cerializer.h

libcerializer_ladir=$(includedir)

libcerializer_la_LDFLAGS=@LDFLAGS@ -version-info 1:1:0

include_HEADERS= \
       cerializer.h \
       dynmessage.h \
       dynmessage_cerializer.h

