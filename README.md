# COP4520 - Assignment 2

## Problem 1

This was written in C and compiled on gcc 10.2.1 (Debian 10.2.1-6).
After cloning the repository, compile and run using these commands:

    gcc question1.c -o question1 -lpthread
    ./question1 <n>

...where `n` is the number of guests/threads to spawn.
The output will be printed to stdout.

### Description

The Minotaur's Birthday Party is a one-to-one copy of the P Prisoner's Problem from the slides, and thus has the same solution. One guest (thread) is assigned to be the "leader". Whenever this leader is summoned by the Minotaur (the master thread, or a producer in a sense), they check to see if the cupcake is eaten. If it is eaten, they increment their "eaten" count by one and ask for another cupcake. Once this eaten count is equal to n-1 (where n is the number of guests), the leader is free to eat the cupcake themself and then declare victory. Meanwhile, the regular guests' job is different. If the guest sees that the cupcake is uneaten and hasn't had one yet, they will eat it. If the guest sees that the cupcake is uneaten and *has* eaten it, they will leave it for another guest. They will not request another cupcake if they see that it is eaten.

This translates very well into code with some caveats. The guest threads will spin until their ID matches the one the Minotaur thread chooses. Once chosen, they will enter the labyrinth and perform their corresponding actions, setting a flag that tells the Minotaur that they have entered. Meanwhile, the Minotaur spins until the guest has said they are done performing actions in the labyrinth.

I found that spinning is fairly expensive as opposed to the "queue" method suggested in Problem 2. Prescribing a very tiny sleep period of 1 microsecond per "spin" is actually less computationally expensive than just letting the threads constantly checking if it's their turn.

Needless to say, there is admittedly more to be desired in terms of *actual* efficiency. The *theoretical* average runtime is O(n^2) (where n = the number of threads), since the leader needs to visit n times and, given ideal randomness, will be chosen once every n summons. The program generally follows this suggested runtime, though 100 guests takes about 0.5 seconds on my machine, which is rather long for such a small amount. Compiler optimization flags like -O2 did not yield any improvements.

## Problem 2

This was also written in C and compiled on the same compiler.
Compile and run using these commands:

    gcc question2.c -o question2 -lpthread
    ./question2 <n> <verbose>

...where `n` is the number of guests/threads to spawn and `verbose` is either 0 or 1 (if 1, the program will print out every guest that has visited the vase).

### Description

This problem is much simpler than problem 1. I chose to implement option 3. All guests get summoned and are allowed to enqueue to see the vase. When it is their turn, they will look at the vase then notify the next person in line that it's their turn.

In practice, enqueueing happens so fast that guests/threads generally enqueue themselves in sequential order. You can see this behavior yourself with the verbose mode enabled.

Option 1 is a naive method of letting all threads access a resource. Any implementation would not be starvation-free because the same thread could repeatedly look at the vase, and all other threads would be forced to spin and cause contention.

Option 2 is only slightly better, though is still not starvation-free. The thread could still repeatedly check the vase after setting the sign to "AVAILABLE" then quickly setting it back to "BUSY" & reentering.

Option 3 is the only option that is starvation-free. By letting threads enqueue, we ensure that every thread is guaranteed to be able to visit the vase by saving a spot for themselves in line.

## Sources

I actually didn't consult any outside resources this time, just the lecture discussion on the P Prisoner's Problem for problem 1. I have prior experience with pthreads.
