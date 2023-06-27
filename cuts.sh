#!/bin/bash

FIRSTGATES="firstGates.txt"
SECONDGATES="secondGates.txt"

MATRIX="Ge2-RelAng.cmat"
#MATRIX="RelAng6790.cmat"

MATRIX2="RelAngMixedFull.cmat"

CHANELS="1" # number of channels for the spectra

INDICES="2 3" # indices for the banana

FIRSTE="930"
SECONDE="1473"

FOUND1G="0"
FOUND2G="0"
while IFS= read -r line
do
    CHECKG=${line%%/*}
    if [ "$FIRSTE" = "$CHECKG" ]; then
        FOUND1G="1"
        line=${line#"$CHECKG"}
        line=${line#"/"}
        GATE1=${line%%/*}
        line=${line#"$GATE1"}
        line=${line#"/"}
        BG1L=${line%%/*}
        line=${line#"$BG1L"}
        line=${line#"/"}
        BG1R=${line%%/*}
        line=${line#"$BG1R"}
        line=${line#"/"}
        FACTORS=${line%%/*}
        line=${line#"$FACTORS"}
        line=${line#"/"}
        GATEN1=${line%%/*}
    fi
done <"$FIRSTGATES"

if [ "$FOUND1G" = "0" ]; then
    echo "Could not find your first gate, something is wrong, the program will execute poorly!"
fi

while IFS= read -r line
do
    CHECKG=${line%%/*}
    if [ "$SECONDE" = "$CHECKG" ]; then
        FOUND2G="1"
        line=${line#"$CHECKG"}
        line=${line#"/"}
        GATE2=${line%%/*}
        line=${line#"$GATE2"}
        line=${line#"/"}
        BG2L=${line%%/*}
        line=${line#"$BG2L"}
        line=${line#"/"}
        BG2R=${line%%/*}
        line=${line#"$BG2R"}
        line=${line#"/"}
        FACTOR2=${line%%/*}
        line=${line#"$FACTOR2"}
        line=${line#"/"}
        GATEN2=${line%%/*}
    fi
done <"$SECONDGATES"

if [ "$FOUND2G" = "0" ]; then
    echo "Could not find your second gate, something is wrong, the program will execute poorly!"
fi

#GATE1="1343, 1347"
#BG1L="1354, 1363"
#BG1R="1313, 1317"
#FACTORS="100, -33.333, -33.333"
#GATEN1="1343, 1347"

#A1=${GATE1%,*}
#A2=${GATE1##*,}
#FIRSTE=$(($A1/2+$A2/2))

#GATE2="1676, 1683"
#BG2L="1694, 1701"
#BG2R="1646, 1655"
#FACTOR2="-0.44444"
#GATEN2="1677, 1682"

#A3=${GATE2%,*}
#A4=${GATE2##*,}
#SECONDE=$(($A3/2+$A4/2))

FACTOR=${FACTORS##*,}

FORMAT="|a:1"

rm -r ${FIRSTE}-${SECONDE}

mkdir ${FIRSTE}-${SECONDE}

rm output*

rm Result*

cd ${FIRSTE}-${SECONDE}
mkdir Errors
cd ../

echo "Running cmat"

cmat -l << echo
o ${MATRIX}
m2d
$INDICES

$GATE1

Y
${FIRSTE}-${SECONDE}/${FIRSTE}-${SECONDE}G
m2d
$INDICES

$BG1L

Y
${FIRSTE}-${SECONDE}/${FIRSTE}-${SECONDE}L
m2d
$INDICES

$BG1R

Y
${FIRSTE}-${SECONDE}/${FIRSTE}-${SECONDE}R
ADD
3
${FIRSTE}-${SECONDE}/${FIRSTE}-${SECONDE}G
${FIRSTE}-${SECONDE}/${FIRSTE}-${SECONDE}L
${FIRSTE}-${SECONDE}/${FIRSTE}-${SECONDE}R
${FIRSTE}-${SECONDE}/${FIRSTE}-${SECONDE}
$FACTORS
0.01
o ${FIRSTE}-${SECONDE}/${FIRSTE}-${SECONDE}
GATE
2

$GATE2

Y
${FIRSTE}-${SECONDE}/${FIRSTE}-${SECONDE}G${FORMAT}
GATE
2

$BG2L

Y
${FIRSTE}-${SECONDE}/${FIRSTE}-${SECONDE}L${FORMAT}
GATE
2

$BG2R

Y
${FIRSTE}-${SECONDE}/${FIRSTE}-${SECONDE}R${FORMAT}
q
echo

sadd -l << echo
i ${FIRSTE}-${SECONDE}/${FIRSTE}-${SECONDE}G${FORMAT}
a ${FIRSTE}-${SECONDE}/${FIRSTE}-${SECONDE}L${FORMAT} $FACTOR2
a ${FIRSTE}-${SECONDE}/${FIRSTE}-${SECONDE}R${FORMAT} $FACTOR2
o ${FIRSTE}-${SECONDE}/input${FORMAT}
q
echo

cmat -l << echo
o ${MATRIX2}
m2d
$INDICES

$GATEN1

Y
${FIRSTE}-${SECONDE}/${FIRSTE}-${SECONDE}NG
c
o ${FIRSTE}-${SECONDE}/${FIRSTE}-${SECONDE}NG
GATE
2

$GATEN2

Y
${FIRSTE}-${SECONDE}/input2${FORMAT}
q
echo

cd ${FIRSTE}-${SECONDE}

cmat -l << echo
o ${FIRSTE}-${SECONDE}L
GATE
2

$BG2L

Y
Errors/g11.txt${FORMAT}
GATE
2

$GATE2

Y
Errors/g12.txt${FORMAT}
GATE
2

$BG2R

Y
Errors/g13.txt${FORMAT}
c
o ${FIRSTE}-${SECONDE}G
GATE
2

$BG2L

Y
Errors/g21.txt${FORMAT}
GATE
2

$GATE2

Y
Errors/g22.txt${FORMAT}
GATE
2

$BG2R

Y
Errors/g23.txt${FORMAT}
c
o ${FIRSTE}-${SECONDE}R
GATE
2

$BG2L

Y
Errors/g31.txt${FORMAT}
GATE
2

$GATE2

Y
Errors/g32.txt${FORMAT}
GATE
2

$BG2R

Y
Errors/g33.txt${FORMAT}
c
o ${FIRSTE}-${SECONDE}NG
GATE
2

$GATEN2

Y
Errors/n22.txt${FORMAT}
q
echo

cd ../

./a.out $FACTOR $FACTOR2 ${FIRSTE}-${SECONDE}

cp output ${FIRSTE}-${SECONDE}/

root -l fitcor.c
