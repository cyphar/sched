#!/usr/bin/env make
# sched: a simple scheduler written in C.
# Copyright (C) 2015 Aleksa Sarai
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

SRC := $(wildcard *.c) test/main.c
DEP := $(wildcard *.h) $(SRC)
BIN := sched

CFLAGS  ?= -ansi -I.
LDFLAGS ?=
WARN    := -Wall -Wextra

.PHONY: clean

sched: $(DEP)
	$(CC) $(CFLAGS) -o $(BIN) $(SRC) $(LDFLAGS) $(WARN)

clean:
	rm $(BIN)
