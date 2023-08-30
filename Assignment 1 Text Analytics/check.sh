#!/bin/sh

NWORDS=100 # change this to test different values of nWords

echo "Compiling..."
make || exit 1
echo

if [ ! -d "out" ]
then
	echo "Creating out/ directory"
	mkdir "out"
fi

echo "Clearing old observed output files..."
rm -f out/*.$NWORDS.out
echo

for text in data/*.txt
do
	num=$(basename "$text" .txt)

	echo "$text"

	# expected output
	exp="out/$num.$NWORDS.exp"
	if [ ! -f "$exp" ] || [ "$(wc -l < "$exp")" -ne $NWORDS ]
	then
		echo "Generating expected output since it doesn't exist yet..."
		sed -e '1,/\*\*\* START OF/ d' -e '/\*\*\* END OF/,$ d' < "$text" | tr 'A-Z' 'a-z' | tr -cs "a-z0-9\'-" "\n" | grep '..' | sed -f stop.sed | ./stem | sort | uniq -c | sort -k1nr -k2 | head -n $NWORDS | sed -e 's/ *//' > "$exp"
	fi

	# observed output
	out="out/$num.$NWORDS.out"
	./tw $NWORDS "$text" > "$out"
	exit_status=$?

	if diff "$exp" "$out" > /dev/null
	then
		if [ "$exit_status" -ne 0 ]
		then
			echo "Failed - your program gave the correct output, but exited with status $exit_status"
			echo "A program that runs normally (with no errors) should exit with status 0"
		else
			echo "Passed"
		fi
	else
		echo "Failed - check differences between $exp and $out"
	fi

	echo
done

