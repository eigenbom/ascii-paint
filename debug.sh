#!/bin/sh
export LD_LIBRARY_PATH=.:${LD_LIBRARY_PATH}
gdb ./ascii-paint
