#!/usr/bin/env bash

n=1000
x=2147483648
repeats=5

printf "" > out.txt

# gcc main.c -o main0 -O0 && printf -- "-O0 " >> out.txt && ./main0 $n $x >> out.txt
# gcc main.c -o main1 -O1 && printf -- "-O1 " >> out.txt && ./main1 $n $x >> out.txt
# gcc main.c -o main2 -O2 && printf -- "-O2 " >> out.txt && ./main2 $n $x >> out.txt
# gcc main.c -o main3 -O3 && printf -- "-O3 " >> out.txt && ./main3 $n $x >> out.txt

printf "" > tmp.txt

gcc main2.c -o main20 -O0 && for (( i=1; i <= $repeats; i++ )); do { time ./main20 $n $x; } 2>> tmp.txt; done;
gcc main2.c -o main21 -O1 && for (( i=1; i <= $repeats; i++ )); do { time ./main21 $n $x; } 2>> tmp.txt; done;
gcc main2.c -o main22 -O2 && for (( i=1; i <= $repeats; i++ )); do { time ./main22 $n $x; } 2>> tmp.txt; done;
gcc main2.c -o main23 -O3 && for (( i=1; i <= $repeats; i++ )); do { time ./main23 $n $x; } 2>> tmp.txt; done;
python3 tmp.py 5 >> out.txt
