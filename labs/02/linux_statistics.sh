#!/bin/bash

if [ ! -d "linux/" ]; then
	echo "downloading linux source, this might take a while..."
	git clone https://github.com/torvalds/linux.git
fi

if [ -f "stats.csv" ]; then
	rm stats.csv
fi

if [ -d "C_FILES" ]; then
	rm -r C_FILES/
fi

if [ -d "H_FILES" ]; then
	rm -r H_FILES/
fi

echo directory,total,c files,h files,pl files,readme files,kconfig files,kbuild files,make files,other files >> stats.csv
ls -d linux/*/ | while read subdir
do
	find $subdir -type f | tail -n +2 > temp
	total=$(cat temp | wc -l)
	echo -e "$(echo $subdir | cut -d/ -f2) $total" >> stats.dat
	echo "$subdir = $total files"
	grep \\.c temp > temp2
	c_files=$(cat temp2 | wc -l)
	cat $(cat temp2) | grep "#include <linux/module.h>" | wc -l >> module_count
	grep \\.h temp > temp2
	h_files=$(cat temp2 | wc -l)
	pl_files=$(grep \\.pl temp |  wc -l)
	readme_files=$(grep README temp |  wc -l)
	kconfig_files=$(grep [Kk]config temp |  wc -l)
	kbuild_files=$(grep [Kk]build temp |  wc -l)
	make_files=$(grep [Mm]akefile temp | wc -l)
	other_files=$(expr $total - $c_files - $h_files - $pl_files - $readme_files - $kconfig_files - $kbuild_files - $make_files)
	echo $subdir,$total,$c_files,$h_files,$pl_files,$readme_files,$kconfig_files,$kbuild_files,$make_files,$other_files >> stats.csv
done
rm temp
rm temp2

./plot.py
rm stats.dat

TOTAL=$(awk -F, '{ sum += $2 } END { print sum }' stats.csv)
C=$(awk -F, '{ sum += $3 } END { print sum }' stats.csv)
H=$(awk -F, '{ sum += $4 } END { print sum }' stats.csv)
PL=$(awk -F, '{ sum += $5 } END { print sum }' stats.csv)
README=$(awk -F, '{ sum += $6 } END { print sum }' stats.csv)
KCONF=$(awk -F, '{ sum += $7 } END { print sum }' stats.csv)
KBUILD=$(awk -F, '{ sum += $8 } END { print sum }' stats.csv)
MAKEFILE=$(awk -F, '{ sum += $9 } END { print sum }' stats.csv)
OTHER=$(awk -F, '{ sum += $NF } END { print sum }' stats.csv)

echo -e "\nNumber of files: $TOTAL"
echo "Number of README files: $README"
echo "Number of Kconfig files: $KCONF"
echo "Number of Kbuild files: $KBUILD"
echo "Number of Makefiles files: $MAKEFILE"
echo "Number of .c files: $C"
echo "Number of .h files: $H"
echo "Number of .pl files: $PL"
echo "Number of others files: $OTHER"

echo -e "\nNumber of times the \"#include<linuz/module.h>\" appears = $(awk '{s+=$0} END {print s}' module_count)"
rm module_count

mkdir -v C_FILES H_FILES
cp -l $(find linux/ -type f -name "*.c") C_FILES/ 2> /dev/null
cp -l $(find linux/ -type f -name "*.h") H_FILES/ 2> /dev/null

echo -e "\nNumber of .c files: $(ls C_FILES | wc -l)"
echo -e "Number of .h files: $(ls H_FILES | wc -l)"
