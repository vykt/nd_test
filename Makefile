
#
# Unfinished template
#


.RECIPEPREFIX:=>

CC=gcc
CFLAGS=-ggdb -Wall -fPIC
INCLUDE=

SRC_DIR=$(shell pwd)/src
BUILD_DIR=$(shell pwd)/build
DOC_DIR=$(shell pwd)/doc


#[set build options]
ifeq ($(build),debug)
	CFLAGS += -O0
else
	CFLAGS += -O2
endif


test:
> $(MAKE) -C ${SRC_DIR} tgt CC='${CC}' BUILD_DIR='${BUILD_DIR}'

clean:
> $(MAKE) -C ${SRC_DIR} clean_all CC='${CC}' BUILD_DIR='${BUILD_DIR}'
