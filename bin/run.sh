#!/bin/bash

# USAGE: ./run_debug.sh <fasta_files_folder> <alignments_folder> [f(forward)|r(reverse)] <output_file_ids> <mincov> <maxcov> <minid> <maxid> <output_file_fasta>

path_fastas=$1
path_alignments=$2
output_file_ids=$4
counter=0
working_dir=$(pwd)

ls $path_fastas > temp___fastas
declare -A new_filenames
while read -r line; do
	name="TS"$counter
	new_filenames[$line]=$name
	((counter++))
done < temp___fastas 

if [ $3 = 'r' ]
then
	ls $path_alignments | grep '.r.' > temp___alignments
	extension=".r.align"
else
	ls $path_alignments | grep -v '.r.' | grep '.align'> temp___alignments
	extension=".align"
fi

while read line; do
		file_1=$(echo $line | cut -d '-' -f 1)".fasta"
		file_2=$(echo $line | cut -d '-' -f 2 | cut -d '.' -f 1)".fasta"
		new_name=${new_filenames[$file_1]}"-"${new_filenames[$file_2]}$extension
		cp $path_alignments$line ./$new_name
		./extract_alignments -in $new_name -out temp___out -tree $output_file_ids -mincov $5 -maxcov $6 -minid $7 -maxid $8
		rm $new_name
		cat temp___out >> $output_file_ids
		rm temp___out
done < temp___alignments
rm temp___*

for file in $path_fastas/*.fasta; do
	filename=$(echo $file | cut -d "/" -f 7)
	number=$(echo ${new_filenames[$filename]} | grep -o '[0-9]*')
	cat $output_file_ids | grep '>'$number'_' > ts$number
	if [ $3 = 'r' ]
	then
		./../../IMSAME-parallel_loading-AVLtree\ Original/bin/revComp $file $working_dir"/"$tempdir"/TS"$number
	else
		cp $file $working_dir"/"$tempdir"/TS"$number
	fi
	./obtain_fasta -inid ts$number -infasta TS$number -out temp___out 
	cat temp___out >> $9
	rm temp___out
	rm ts$number
	rm TS$number
done

echo ${new_filenames[@]}
