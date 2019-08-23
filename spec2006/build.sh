#!/bin/bash
ulimit -s unlimited
ulimit -c unlimited

#. shrc
source shrc
relocate



sed -e"s/COPTIMIZE       =       -flto -Ofast/COPTIMIZE       =       -flto -O2/" config/gcc483-n32.cfg > config/new.cfg
date
runspec.build -D -c new.cfg -i train -n 3 462 -T base
date

sed -e"s/COPTIMIZE       =       -flto -Ofast/COPTIMIZE       =       -flto -O3/" config/gcc483-n32.cfg > config/new.cfg
date
runspec.build -D -c new.cfg -i train -n 3 462 -T base
date

sed -e"s/COPTIMIZE       =       -flto -Ofast/COPTIMIZE       =       -flto -Ofast/" config/gcc483-n32.cfg > config/new.cfg
date
runspec.build -D -c new.cfg -i train -n 3 462 -T base
date

sed -e"s/COPTIMIZE       =       -flto -Ofast/COPTIMIZE       =       -flto -Ofast -funroll-loops/" config/gcc483-n32.cfg > config/new.cfg
date
runspec.build -D -c new.cfg -i train -n 3 462 -T base
date

sed -e"s/COPTIMIZE       =       -flto -Ofast/COPTIMIZE       =       -flto -Ofast -funroll-all-loops/" config/gcc483-n32.cfg > config/new.cfg
date
runspec.build -D -c new.cfg -i train -n 3 462 -T base
date

sed -e"s/COPTIMIZE       =       -flto -Ofast/COPTIMIZE       =       -flto -Ofast -funroll-all-loops/" config/gcc483-n32.cfg > config/new.cfg
date
runspec.build -D -c new.cfg -i train -n 3 462 -T base
date

sed -e"s/COPTIMIZE       =       -flto -Ofast/COPTIMIZE       =       -flto -Ofast -funroll-all-loops -funsafe-loop-optimizations/" config/gcc483-n32.cfg > config/new.cfg
date
runspec.build -D -c new.cfg -i train -n 3 462 -T base
date

sed -e"s/COPTIMIZE       =       -flto -Ofast/COPTIMIZE       =       -flto -Ofast -funroll-all-loops -funsafe-loop-optimizations/" config/gcc483-n32.cfg > config/new.cfg
date
runspec.build -D -c new.cfg -i train -n 3 462 -T base
date

sed -e"s/COPTIMIZE       =       -flto -Ofast/COPTIMIZE       =       -flto -Ofast -falign-jumps=16 -falign-loops=16 -falign-functions=16/" config/gcc483-n32.cfg > config/new.cfg
date
runspec.build -D -c new.cfg -i train -n 3 462 -T base
date

sed -e"s/COPTIMIZE       =       -flto -Ofast/COPTIMIZE       =       -flto -Ofast -falign-jumps=16 -falign-loops=16 -falign-functions=16 -fgcse-sm -fgcse-las -finline-functions -finline-limit=1000/" config/gcc483-n32.cfg > config/new.cfg
date
runspec.build -D -c new.cfg -i train -n 3 462 -T base
date

sed -e"s/COPTIMIZE       =       -flto -Ofast/COPTIMIZE       =       -flto -Ofast -funroll-loops -falign-jumps=16 -falign-loops=16 -falign-functions=16 -fgcse-sm -fgcse-las -finline-functions -finline-limit=1000/" config/gcc483-n32.cfg > config/new.cfg
date
runspec.build -D -c new.cfg -i train -n 3 462 -T base
date


