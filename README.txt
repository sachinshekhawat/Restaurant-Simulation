This is a project demonstrating the proper use of shared data structures between multiple child processes occuring from the same "parent" process with the help of semaphore mutexes. Each entity (i.e. the doorman, the waiter, each customer etc) is represented as a process and depending on their status (active/sleeping) they are executing different tasks having always as a goal to be synchronized and concurrent as well as to avoid race conditions and deadlocks.

-> To compile run `make`

-> Run:

Command line options:
 * -n number of groups to come
 * -l name of configuration file (conf.txt)
 * -d time after which statistics will be printed

Command to run:

    ./restaurant -n customers -l configfile 
