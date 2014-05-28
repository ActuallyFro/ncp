#!/bin/bash
clear
names_array=(`grep "remote \"" .git/config | awk '{print $2}' | tr -d "\"\|]"`)
branch="master"

echo -e "  ____                  __        ______  __       __"
echo -e " /\\  _ \\               /\\ \\      /\\  _  \\/\\ \\     /\\ \\"
echo -e " \\ \\ \\_\\ \\__  __    ___\\\\ \\ \\___  \\ \\ \\_\\ \\ \\ \\    \\ \\ \\"
echo -e "  \\ \\  __/\\ \\/\\ \\  /  __\\\\ \\  __ \\ \\ \\  __ \\ \\ \\    \\ \\ \\"
echo -e "   \\ \\ \\/\\ \\ \\_\\ \\/\\__,  \\\\ \\ \\ \\ \\ \\ \\ \\ \\ \\ \\ \\___ \\ \\ \\____"
echo -e "    \\ \\_\\ \\ \\____/\\/\\____/\\ \\_\\ \\_\\ \\ \\_\\ \\_\\ \\_____\\\\\\ \\_____\\"
echo -e "     \\/_/  \\/___/  \\/___/  \\/_/\\/_/  \\/_/\\/_/\\/_____/ \\/_____/"
echo -e ""

#echo "This is the array size: "${#names_array[*]}
adjusted_lines=${#names_array[*]}
adjusted_lines=$(( adjusted_lines - 1 ))
	for i in `seq 0 $adjusted_lines`;
	do
		echo "Pushing to Remote: ${names_array[$i]}"
		git push ${names_array[$i]} $branch
                #echo "names_array["$i"]: "${names_array[$i]}
		echo ""
        done
