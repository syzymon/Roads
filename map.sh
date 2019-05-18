#!/bin/bash
INPUT=$1

bad_route_number () {
    if [[ ! $1 =~ ^[0-9]+$ ]] || [[ $1 -lt 1 ]] || [[ $1 -gt 999 ]]
    then
        return 0
    else
        return 1
    fi
}

if [[ ! -f ${INPUT} ]]
then
    echo -e "Wrong filename"
    exit 1
fi

for i in "${@:2}"; do
    if bad_route_number ${i};
    then
        echo -e "Incorrect argument $i"
        exit 1
    fi
done

for i in "${@:2}"; do
    result=$(awk 'BEGIN{FS=";"} $1=="'${i}'" {
        total = 0;
        for (var = 3; var <= NF; var+=3){
            total +=  $var
        }
        print total
    }' ${INPUT})
    if [[ ! -z "$result" ]]
    then
        echo "${i};${result}"
    fi
done

