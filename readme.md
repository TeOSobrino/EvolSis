
# Genetic Algorithm using Island Method #

## sumary ##

This program is a genetic algorithm that aims to maximize a function.
It also enables parallelism for calcuting the objective function, in the file
``aux.h`` set the ``THREAD NUM`` parameter to the number of threads desired
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

$$p(f) = e^{\dfrac{-2\pi f}{T}}$$

where $f$ is the individual fitness, $T$ the temperature, a parameter that
increases the evolutive pressure (increases elitism).

## How to run ##

```bash
make all
make run
```

## How to use this in your own objective function ##

In file  ``include/aux.h``  insert the number of variables of your function,
then in file ``main.cpp`` include your function, and follow the pattern adding
``individual.fitness = calculated_val`` before returning.
