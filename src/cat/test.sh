#!/bin/bash

COUNTER=0
SUCCESS=0
FAILED=0

flags=(
    "b"
    "e"
    "n"
    "s"
    "t"
    "v"
)

tests=(
"FLAGS test_files/test_case_cat.txt"
"FLAGS test_files/test_case_cat.txt test_files/test_1_cat.txt"
)

manual=(
"-s test_files/test_1_cat.txt"
"-b -e -n -s -t -v test_files/test_1_cat.txt"
"-b test_files/test_1_cat.txt nofile.txt"
"-t test_files/test_3_cat.txt"
"-n test_files/test_2_cat.txt"
"no_file.txt"
"-n -b test_files/test_1_cat.txt"
"-s -n -e test_files/test_4_cat.txt"
"test_files/test_1_cat.txt -n"
"-n test_files/test_1_cat.txt"
"-n test_files/test_1_cat.txt test_files/test_2_cat.txt"
"-v test_files/test_5_cat.txt"
"-- test_files/test_5_cat.txt"
)

gnu=(
"-T test_files/test_1_cat.txt"
"-E test_files/test_1_cat.txt"
"-vT test_files/test_3_cat.txt"
"--number test_files/test_2_cat.txt"
"--squeeze-blank test_files/test_1_cat.txt"
"--number-nonblank test_files/test_4_cat.txt"
"test_files/test_1_cat.txt --number --number"
"-bnvste test_files/test_6_cat.txt"
)

run_test() {
PARAM=$(echo "$@" | sed "s/FLAGS/$var/")
let "COUNTER++"
if diff  <(cat $PARAM) <(./s21_cat $PARAM) -s
then
    let "SUCCESS++"
    echo "$COUNTER - SUCCESS $PARAM"
else
    let "FAIL++"
    echo "$COUNTER - FAIL $PARAM\n"
fi
}

for i in "${manual[@]}"
do
    run_test "$i"
done

for i in "${gnu[@]}"
do
    run_test "$i"
done

for var1 in "${flags[@]}"
do
    for i in "${tests[@]}"
    do
        var="-$var1"
        run_test "$i"
    done
done
printf "\n"
echo "======================="
echo "2 PARAMETERS"
echo "======================="
printf "\n"

for var1 in "${flags[@]}"
do
    for var2 in "${flags[@]}"
    do
        if [ $var1 != $var2 ]
        then
            for i in "${tests[@]}"
            do
                var="-$var1 -$var2"
                run_test "$i"
            done
        fi
    done
done
printf "\n"
echo "======================="
echo "3 PARAMETERS"
echo "======================="
printf "\n"
for var1 in "${flags[@]}"
do
    for var2 in "${flags[@]}"
    do
        for var3 in "${flags[@]}"
        do
            if [ $var1 != $var2 ] && [ $var2 != $var3 ] && [ $var1 != $var3 ]
            then
                for i in "${tests[@]}"
                do
                    var="-$var1 -$var2 -$var3"
                    run_test "$i"
                done
            fi
        done
    done
done
printf "\n"
echo "======================="
echo "4 PARAMETERS"
echo "======================="
printf "\n"
for var1 in "${flags[@]}"
do
    for var2 in "${flags[@]}"
    do
        for var3 in "${flags[@]}"
        do
            for var4 in "${flags[@]}"
            do
                if [ $var1 != $var2 ] && [ $var2 != $var3 ] \
                && [ $var1 != $var3 ] && [ $var1 != $var4 ] \
                && [ $var2 != $var4 ] && [ $var3 != $var4 ]
                then
                    for i in "${tests[@]}"
                    do
                        var="-$var1 -$var2 -$var3 -$var4"
                        run_test "$i"
                    done
                fi
            done
        done
    done
done


printf "\n"
echo ========================
echo "RESULTS:"
echo "------------------------"
echo "FAILED:     $FAILED"
echo "SUCCESSFUL: $SUCCESS"
echo "ALL:        $COUNTER"
echo "------------------------"
