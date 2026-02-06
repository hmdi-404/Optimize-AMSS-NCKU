# The optimization for performance computing AMSS-NCKU in ASC2026

## Command for compile file
```bash
mpic++ -O3 -ffast-math -march=native TwoPunctureABE.cpp TwoPunctures.cpp -o TwoPunctureABE
```

## Command for run
```bash
mpirun -np 1 --map-by ppr:1:socket:PE="<number of threads in your computer>" ./TwoPunctureABE
```
### or 
```bash
./TwoPunctureABE
```

Warning: My code not perfect but faster than default version
