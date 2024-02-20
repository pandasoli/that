#!/bin/bash

TYPE="\e[0;30m"
KEY="\e[0;90m"
IDENT="\e[0;3m"
RESET="\e[0m"
NUM="\e[0;2m"
STR="\e[0;2;3m"
TAB="  "

echo -e "\e[0;1m1	A"
echo -e "\e[0m0	A"
echo -e "${IDENT}3	A	identifier"
echo -e "${STR}2;3	A	string"
echo -e "${NUM}2	A	number"
echo -e "${KEY}90	A	keyword"
echo -e "${TYPE}30	A	type"
echo -en "$RESET"

echo -e "
\`\`\`th
${KEY}import ${STR}stdio

${IDENT}a ${TYPE}int${RESET} = ${NUM}12

${KEY}fn ${IDENT}print${RESET} {
$TAB${IDENT}println${RESET}(${STR}\"Printing {a}\"${RESET})
}

${IDENT}print${RESET}()
$RESET\`\`\`"
