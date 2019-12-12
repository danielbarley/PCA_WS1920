#! /usr/bin/env bash
#SBATCH --partition=bench
#SBATCH -J heatgrid

proj="matmul3"

wdir="PCA"
file="$wdir/$proj/time.out"

echo "size threads scheduling t_compute t_wall" > $file
for O in static dynamic guided
do
    for n in 1 2 4 8 16 32
    do
        for d in 10 100 500 1000 5000 10000
        do
            #| awk 'FNR<=2{print $3}'
            echo -en "$d $n $O " >> $file
            { srun time $wdir/$proj/$proj\_$O.out --dimension $d -n $n ; } 2>&1 | awk 'FNR<=2{print $3}' ORS=' ' | sed "s/elapsed/ /" >> $file
            echo -e "" >> $file
        done
    done
done
