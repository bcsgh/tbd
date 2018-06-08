diff ${1}.cpp ${2%.*}.cpp && diff $*
exit $?