#!/bin/bash

APPFIX=""
WINOS=0

# OS类别
OSKind=`uname -s`
    
# 机器位数
OS3264=`uname -m`

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

    # CMDNAME="./minic${APPFIX} -i -o ${IRFILE} ${CFILE}"
    CMDNAME="./cmake_minic${APPFIX} -i -o ${IRFILE} ${CFILE}"
    OUTSTR=`eval ${CMDNAME} 2>&1`
    echo "${OUTSTR}" > ${RESULTFILE}

    if [ ! -f ${IRFILE} ]; then
        echo "${IRFILE} not generated"
        return
    fi

    # Linux发布者
    if [[ $OSKind == "Linux" ]]; then
        LinuxDisID=`lsb_release -i -s`
        export LD_LIBRARY_PATH=$(pwd)/IRCompiler/${OSKind}-${OS3264}/${LinuxDisID}:${LD_LIBRARY_PATH}
        CMDNAME="./IRCompiler/${OSKind}-${OS3264}/${LinuxDisID}/IRCompiler${APPFIX} -R ${IRFILE}"
    else
        CMDNAME="./IRCompiler/${OSKind}-${OS3264}/IRCompiler${APPFIX} -R ${IRFILE}"
    fi

    if [ -f ${INFILE} ]; then
        CMDNAME="${CMDNAME} < ${INFILE}"
    fi

    OUTSTR=`eval ${CMDNAME} 2>&1`
    RUNRESULT=$?

    if [ -n "${OUTSTR}" ]; then
        OUTSTR="${OUTSTR}\n"
    fi

    if [ ${WINOS} -eq 1 ]; then
        echo -e "${OUTSTR}${RUNRESULT}" > ${RESULTFILE}
    else
        echo -e "${OUTSTR}${RUNRESULT}" > ${RESULTFILE}
    fi

    diff --strip-trailing-cr "${RESULTFILE}" "${OUTFILE}" > /dev/null 2>&1

    if [ $? -ne 0 ]; then
        echo "${CFILE} NG"
        TestCaseNumNG=$((TestCaseNumNG+1))
    else
        echo "${CFILE} OK"
        rm -f ${RESULTFILE}
        rm -f ${IRFILE}
        TestCaseNumOK=$((TestCaseNumOK+1))
    fi
}

TestCaseNum_From=0
TestCaseNum_To=105
TestCaseNumNG=0
TestCaseNumOK=0

if [ $# -eq 1 ]; then
    TestCaseNum_From=$1
    TestCaseNum_To=$1
fi

if [ $# -ge 2 ]; then
    TestCaseNum_From=$1
    TestCaseNum_To=$2
fi

PlatformOS=`uname -a`

if [[ ${PlatformOS} =~ "MINGW" ]] || [[ ${PlatformOS} =~ "MSYS" ]]; then
    APPFIX=".exe"
    OSKind="MINGW"
    WINOS=1
fi

for ((i = $TestCaseNum_From; i <= $TestCaseNum_To; i ++))
do
    TestCaseNo=`printf '%03d' $i`
    dotestcase $TestCaseNo
done

echo "Number of tests successfully passed: ${TestCaseNumOK}"
echo "Number of failed tests: ${TestCaseNumNG}"
# rm -r llvm-*s