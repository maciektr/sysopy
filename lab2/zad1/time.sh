#! /bin/bash

program=main.out

records=(1000 10000);
sizes=(1 4 512 1024 4096 8192);

generate_data () {
    for n in "${records[@]}"
    do
        for size in "${sizes[@]}"
        do
            echo "Generating" $n "records of size" $size
            ./$program generate dane_$n\_$size.txt $n $size
        done
    done
}

# generate_data

measure () {
    for n in "${records[@]}"
    do
        for size in "${sizes[@]}"
        do
            echo "Measuring" $n "records of size" $size
            data=dane_$n\_$size.txt
            echo "Copying by lib"
            (time ./$program copy $data dane.txt $n $size lib) 2> >(tail -n 2)
            echo "Sorting by lib"
            (time ./$program sort dane.txt $n $size lib) 2> >(tail -n 2)
            rm dane.txt
            echo "Copying by sys"
            (time ./$program copy $data dane.txt $n $size sys) 2> >(tail -n 2)
            echo "Sorting by sys"
            (time ./$program sort dane.txt $n $size sys) 2> >(tail -n 2)
            rm dane.txt
        done
    done
}

measure

remove () {
    for n in "${records[@]}"
    do
        for size in "${sizes[@]}"
        do
            rm dane_$n\_$size.txt 
        done
    done
}

# remove