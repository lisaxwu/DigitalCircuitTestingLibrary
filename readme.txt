to compile:
g++ cnode.cpp cgate.cpp circuit.cpp sim.cpp -o sim

to run:
./sim s27.txt s27test.txt
./sim s298f_2.txt s298f_2test.txt
./sim s344f_2.txt s344f_2test.txt
./sim s349f_2.txt s349f_2test.txt

s27.txt is where the circuit is defined.

s27test.txt is the test input file.

to generate test set for a stuck-at-0/1 fault on some node, input the node number(fault-net), and the fault-value which is 0/1.

example:

./sim s27.txt s27test.txt 6 1