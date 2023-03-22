NOW=$(date +%Y-%m-%d-%H-%M-%S)
CSV=out-$NOW.csv

NPOINTS="1000 10000 100000 1000000 10000000 100000000 1000000000"
NTHREADS="1 2 4 8 16"
NRUNS="5"

for P in $NPOINTS; do
  for T in $NTHREADS; do
     for R in $(seq 1 $NRUNS); do
        ./approxPi $@ $P $T
     done
   done
done | tee $CSV
