# Multi-Threaded-Hash-Tree
Introduction
This experimental study intends to observe and analyze the performance of a multi-threaded file hashing algorithm, specifically its hashing time compared to various numbers of threads used. It should be noted that the algorithm is in the form of a C executable on CentOS/Linux.


Experiment
The experiment takes the form of a C program, namely htree.c. The C program is then *compiled, which creates an executable named htree (see footnote below for actual compiling line used). The program will be **executed several times with varying numbers of threads (1, 4, 
16, 32, 64, 128, and 256), in which then we will record the results and illustrate them in Figure 1. 



We will run our experiment on the cs3 machine, as it has 48 CPUs.

*The line that’s used for compiling htree.c into htree executable: gcc htree.c -o htree -Wall -Werror -phtread -std=gnu99
**Line used for executing htree executable: ./htree [FILEPATH] [NUMBER OF THREADS], in which [FILEPATH] is the file path to the file for hashing and [NUMBER OF THREADS] being the number of threads to be used by htree.

<img width="497" alt="image" src="https://github.com/asarisha/Multi-Threaded-Hash-Tree/assets/125152624/eb0e66cd-55af-4710-9784-859ca72ae18b">

Results

Figure 1. A graph of the data collected from executing the htree.c program with various thread count sizes up to around 250.


Analysis
As expected, the time taken to execute the program decreases as the number of threads executed increases. However, the relationship between the number of threads and speedup isn’t proportional: given that the machine has 48 CPUs, the execution time will only significantly decrease up to a certain point: we can observe that around 128 threads, the speed-up stagnates due to the fact that new threads creates aren’t executed by a unique CPU.

Conclusion
Given the results of this experimental study, it can be concluded that, given the necessary hardware requirements for multi-threading, the relationship between threads and execution time is best described as an inverse relationship.
