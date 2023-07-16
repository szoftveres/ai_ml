# ai_ml

AI, machine learning with neural network

Let's teach the little @ guy to clear the obstacles, using neural network and machine learning, in ~250 lines of plain old ANSI C.
![game](game.png)

The 'lookahead' function returns an array of obstacles that the little @ guy sees, this is turned into 3 different actions during each step by a neural netowrk: up, down, or don't change.
After a population is tested, the best performers (based on how far they advanced without bumping into an obstacle or hitting the wall) are selected to reproduce; also, some mutation is being introduced to each new generation, but plenty of non-mutated instances are also preserved, in order to test out the waters with the genes of the previous generations.
