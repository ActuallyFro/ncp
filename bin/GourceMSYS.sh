#!/bin/bash.exe
names_array=(save this)
names_array=(`git branch | tr " " '\n' | sort  | cat --squeeze-blank | grep -v "*"`)
adjusted_lines=${#names_array[*]}
adjusted_lines=$(( adjusted_lines - 1 ))

for i in `seq 0 $adjusted_lines`;
do
	echo "Looking at Repo["$i"]: "${names_array[$i]}
	gource --git-branch ${names_array[$i]} --output-custom-log "${names_array[$i]}.glog" .
done

cat *.glog | sort -n > Combined.glogc

gource Combined.glogc \
	 -s 0.7 \
	 -i 0 \
	 -1280x720 \
	 --title "Network Copy" \
	 --highlight-users \
	 --highlight-dirs \
	 --file-extensions \
	 --hide mouse,filenames \
	 --key \
	
rm *.glog*
