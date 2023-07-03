#!/bin/bash

# 单个测试用例测试
function dotestcase()
{
    CFILELIST=`ls function/$1*.c`

    # 要查找的C文件名
    CFILE=""

    n=0
    for cfilename in $CFILELIST
    do
        CFILE=$cfilename
        n=`expr $n + 1`
    done

    if [ $n -ne 1 ]; then
        return
    fi

    # 去掉C文件后缀，保留文件名
    TESTNAME=${CFILE%%.*}

    INFILE="${TESTNAME}.in"
    OUTFILE="${TESTNAME}.out"
    IRFILE="${TESTNAME}.ir"
    RESULTFILE="${TESTNAME}.result"

    ASTFILE="gen_asts/ast_$1.png"
    CMDNAME="./minic -a -o ${ASTFILE} ${CFILE}"
    echo ${CMDNAME}
    OUTSTR=`eval ${CMDNAME}`
}

TestCaseNum_From=0
TestCaseNum_To=105

rm -r gen_asts
mkdir gen_asts

for ((i = $TestCaseNum_From; i <= $TestCaseNum_To; i ++))
do
    TestCaseNo=`printf '%03d' $i`
    dotestcase $TestCaseNo
done
rm -r ./llvm-*
