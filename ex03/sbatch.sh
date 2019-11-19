#! /usr/bin/env bash
#SBATCH --partition=bench
#SBATCH -J heatgrid
time $1 0
time $1 1
time $1 2
time $1 3
time $1 4
