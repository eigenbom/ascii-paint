#!/bin/sh
clear
export LD_LIBRARY_PATH=.:${LD_LIBRARY_PATH}
scons -Q && ./ascii-paint
