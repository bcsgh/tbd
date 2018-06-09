diff ${1}.cpp ${2%.*}.cpp -y && diff <(sort $1) <(sort $2) -y
exit $?