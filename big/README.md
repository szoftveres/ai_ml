# Teaching a (deep) neural network through (massively parallel) genetic algorithm

This is a massively enhanced version of the [250 line C code AI project](https://github.com/szoftveres/ai_ml/small/)

#### Deep learning by:
 * Introducing multiple levels of hidden layers in the neural network

#### Parallelism by:
 * Multithreading on the local machine (pthreads)
 * The binary can also be started in **client**- or **server** mode
   * A client (the orchestrator) can connect to multiple servers through the network and can assign multiple jobs to each server.
   * The servers accept a large number of jobs (pre-buffering and unblocking the client) and use all their cores (my multithreading) to execute them
   * Out of order execution of jobs, i.e. the client can accept the results of fast jobs and continue before the results of previously assigned slow jobs come in. The servers don't hold results of fast jobs back.

As a result, the execution pretty much scales linearly with the number and speeds of CPU cores.

Normal, multithreaded mode:
```
make clean ; make && ./aigame.bin
```

Server mode:
```
make clean ; make && ./aigame.bin -s
```

Client mode:
```
make clean ; make && ./aigame.bin -c 127.0.0.1 -c 192.168.0.110
```
