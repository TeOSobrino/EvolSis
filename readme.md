
# Genetic Algorithm using Island Method #

## What is a genetic algorithm? ##

A genetic algorithm is a program that aims to find the global maxima or minima
of a function, by using randomness and inspiration in nature.

A genetic algorithm starts with an initial population of random individuals,
each individual is a solution to the problem, therefore they all have a function
value associated with them, this value is named fitness, individuals are sorted
based on their fitnes, then they pass through selection and crossover.

Selection will determine if a new individual is fit enough to enter the new
population, doing so by killing one of the individuals of the old population,
after this process new and old population are merged, to ensure that
the number of individuals is constant.

Crossover is a technique to produce offspring using two individuals, this
can be achieved by the central point crossover, in which two individuals
exchange their genes and generate offspring, or an average of all parent's genes
after crossover mutation happens, in one of the offspring's gene, in order to
ensure that the population can explore the solution space.

Then the algorithm returns to selection step, repeating until a satisfactory
answer is found (or time requirements are met).

<p align="center">
   <img src=https://github.com/TeOSobrino/EvolSis/blob/main/imgs/ag.jpg?raw=true/>
</p>

# Island Method #

## Motivation ##

Genetic Algorithms have multiple crossover and selection techniques, some
converge faster, but can be stuck at local maxima or minima, other converge slower
and can take much time to find an accetable solution, for that reason the island
method implements parallelism, running several difernt genetic algorithms, each
with it's own initial population, crossover and selection techniques, and exchanging
their best individuals from time to time, making for a greater genetic diversity
and aiming for a better final solution.

## summary ##

### Theory ###

This program is a genetic algorithm that aims to maximize a function.
It also enables parallelism for calcuting the objective function, in the file
`` aux.h `` set the `` THREAD NUM `` parameter to the number of threads desired
(if the function being analyzed is very expensive, this can help, but if the
function is simple, the overhead will delay the total execution time).

This program uses the island method, running 4 populations (islands) in
parallel (one thread for each island), each with unique traits, selection and
crossover techniques, at a determined period of time migration between the
islands happens (the islands will exchange the best individuals)

A schematic model of the algorithm:

![alt text](https://github.com/TeOSobrino/EvolSis/blob/main/imgs/Parallel-genetic-algorithm-with-island-model.png?raw=true)

This allows some form of redundancy in finding solutions, and exploits the
difference between selection techniques and how fast they converge, e.g. elitist
crossover (all individuals mate with the best) or entropy-boltzmann, where each
individual gets a probability assigned by the exponential distribution:

$$p(f) = e^{\tfrac{-2\pi f}{T}}$$

where $f$ is the individual fitness, $T$ the temperature, a parameter that
increases the evolutive pressure (increases elitism).

### Practice ###

In pratice, most parameters (number of individuals,
generations, islands, threads, etc)
and are `` #defines `` that can be modified in file `` include/aux.h ``.

#### To run your own function ####

The `` GENE_NUM `` parameter represents the number of variables.
In file `` main.cpp `` include your objective function, and follow the pattern adding
`` individual.fitness = calculated_val `` before returning.

## Techniques used ##

### Variable mutation rate ###

If the best individual in the population is the same for a significant amount of
generations, the mutation rate will first be decreased (searching for local optima)
the increased (searching for global optima), seeking to leave the current location
as it may be a saddle point.

### Predation by sinthesys ###

In each generation the worst individuals are replaced by the population mean,
excluding thoose individuals.

### Genocide ###

In a determined period genocide will happen, eliminating all individuals and
leaving only the best solution behind, effectivelly resetting the population.

## How to Run ##

### Requirements ###

The boost library is used for IPC (inter-process comunication), and mutexes to
ensure that each thread will not override other thread individuals.

for Ubuntu:

``` bash
sudo apt-get install -y g++ make 
sudo apt install -y libboost-all-dev
```

for compilation and running:

```bash
make all
make run
```
