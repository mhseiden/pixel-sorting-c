#!/bin/bash
# Create an Animated GIF using Max Seiden's Pixelsort
##
usage() {
	cat 1>&2 <<EOF
Usage:
	./make_gif.sh input <k frames> CMD out.gif

E.g.
	./make_gif.sh input.jpg 128 'SORT COLS DESC BY AVG WITH DARK \${IDX} RUNS THEN SORT ROWS DESC BY AVG WITH DARK \${IDX} RUNS' out.gif

The ${IDX} variable is replaced dynamically with sed during template generation.

Requires imagemagick.
EOF
}

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
INP=$1
SZ=$2
COMMAND=$3
OUT=$4

if [ "$#" -ne "4" ];
then
	usage
	exit 1
fi


TMPDIR=$(mktemp -d -t sort)
TEMPLATE() {
	NUMITERS=$1
	CMDIN=$2
	i=$3
	if [ $NUMITERS -gt "1" ]; then
		for ITER in `seq $(($NUMITERS - 1))`;
		do
			echo -n $(echo $CMDIN | sed -e "s/\${IDX}/$i/g" -e "s/\$IDX/$i/g")
			echo -n " THEN "
		done;
	fi;
	echo -n $(echo $CMDIN | sed -e "s/\${IDX}/$i/g" -e "s/\$IDX/$i/g")
}
for J in `seq $SZ`;
do
	${DIR}/../bin/pixelsort ${INP} ${TMPDIR}/${J}.jpg "$(TEMPLATE 1 "$COMMAND" ${J})";
done

convert -delay 1 $(for f in `ls ${TMPDIR} | sort -n`; do echo ${TMPDIR}/${f}; done) -loop 0 ${OUT}
rm -rf ${TMPDIR}
