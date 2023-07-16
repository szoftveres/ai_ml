# AI, machine learning with neural network (Genetic algorithm)

Let's teach the little @ guy how to clear the obstacles, using neural network and machine learning, in ~250 lines of plain old ANSI C.

![game](game.png)

The 'lookahead' function returns an array of obstacles that the little @ guy sees, this is turned into 3 different actions during each step by a neural netowrk: move up, move down, or don't change position.
After a population is tested, the best performers (based on how far they advanced without bumping into an obstacle or hitting the wall) are selected to reproduce; also, some mutation is being introduced to each new generation, but plenty of non-mutated instances also remain, in order to adequately test out the winner genes of the previous generation.

To run, open a terminal and make sure its size is set to default 80 x 25
```
make clean ; make && ./aigame.bin
```
