#! /usr/bin/env bash
#SBATCH --partition=bench
#SBATCH -J heatgrid
time mpirun -np 4 $1 3
# time mpirun -np 2 $1 3
# time mpirun -np 4 $1 3
# time mpirun -np 8 $1 3
