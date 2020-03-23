#!/usr/bin/env bash

doxygen Doxyfile
cd doc/latex
make
xdg-open ./refman.pdf