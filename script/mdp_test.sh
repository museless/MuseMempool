#! /bin/sh
# Just for Unix/Linux
# Program: use for dump mass.out callgrind.out
# 2015/09/23 Muse

cd /Code/C/Mempool/script

rm -f ../log/*
mkdir ../log

valgrind --tool=callgrind ../mdpool
callgrind_annotate --inclusive=yes callgrind.out.* > ../log/call_inc
callgrind_annotate --tree=both callgrind.out.* > ../log/call_tree
rm -f callgrind.out.*

valgrind --tool=massif ../mdpool
ms_print massif.out.* > ../log/massif
rm -f massif.out.* 
