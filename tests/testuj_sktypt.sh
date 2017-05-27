#!/bin/bash

# Testowany skrypt i testowany program
script=./chain_poly.sh
program=../release/calc_poly

params=( \
  '' \
  'calc_poly' \
  'calc_poly testy_skyptu/1 dummy' \
  './nie-ma-takiego-pliku testy_skyptu/1' \
  './testy/1/a testy_skyptu/1' \
  'calc_poly testy_skyptu/nie-ma-takiego-katalogu' \
)

if [ ! -f "$script" -o ! -x "$script" ]
then
  echo "Script $script is missing."
  exit 1
fi

if [ ! -f "$program" -o ! -x "$program" ]
then
  echo "Executable file $program is missing."
  exit 1
fi

(( pass = 0 ))
(( count = ${#params[@]} ))

for (( i = 0; i < count; ++i ))
do
  (( testnum = $i + 1))
  echo "TEST $testnum"
  p=`echo ${params[$i]} | sed "s@calc_poly@$program@g"`
  echo "$script $p"
  timeout 2 $script $p
  retcode=$?
  if (( $retcode == 1 ))
  then
    (( pass++ ))
    echo "TEST $testnum PASS"
    echo
  else
    echo "TEST $testnum FAIL (return code $retcode)"
    echo
  fi
done

for testname in testy_skryptu/*
do
  (( count++ ))
  echo "TEST $testname"
  $script $program $testname > $testname/tmp
  diff $testname/wynik $testname/tmp &>/dev/null
  if (( $? == 0 ))
  then
    (( pass++ ))
    echo "TEST $testname PASS"
    echo
  else
    echo "TEST $testname FAIL"
    echo
  fi
  rm -f $testname/tmp &>/dev/null
done

echo "$pass/$count"
