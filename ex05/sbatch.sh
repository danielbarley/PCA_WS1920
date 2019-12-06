#! /usr/bin/env bash
#SBATCH --partition=bench
#SBATCH -J heatgrid

wdir="PCA/matmul"
file="$wdir/time.out"

echo "" > $file
for n in 1 2 4 8 16 32
do
    for d in 10 100 500 1000 5000 10000
    do
        #| awk 'FNR<=2{print $3}'
        echo -en "$d $n " >> $file
        { srun time $wdir/matmul.out --dimension $d -n $n ; } 2>&1 | awk 'FNR<=2{print $3}' ORS=' ' | sed "s/elapsed/ /" >> $file
        echo -e "" >> $file
    done
done
