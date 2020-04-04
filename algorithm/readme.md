#### Read Me of AAAI'18

To use our method proposed in AAAI'19, you should first execute "stable_lp.cpp" to get a feasible LP solution. Then execute "AAAI.cpp" to obtain the final result.


To complie the "stable_lp.cpp", you need first install the or_tools for C++ [link](https://developers.google.com/optimization/install/cpp/).

After compile "stable_lp.cpp" and get the executable program "stable_lp", you can run this command in terminal: "./stable_lp test.lp test.sol"

Finally, you can compile and execute "AAAI.cpp" as follows: "./AAAI19 test.sim test.lp test.sol" where "test.lp" and "test.sol" is the same file in the previous step.

Here are the formats of these three files: 

####### test.sim

It is the raw test data.

The first line contains three integers $n, m, E$, which means the number of workers, requests and edges in the bipartite graph.

Next $m$ lines, each line contains two floats $(x, y)$, which means the coordinate of the drivers.

Next $n$ lines, each line contains a integer $id$ and a float $val$, which means the task type and utility of requests.

Next $E$ lines, each line conatains two intergers $(u, v)$, which means there are an edge between worker $u$ and task $v$. 

####### test.lp

It is the input data of linear programming solver.

The first line contains six integers $n, m, E, grid\_len, num_x, num_y$, which means the number of workers, the number of requests, the number of edges in the bipartite graph, the length of grid index, the length and width of grid index respectively.

Next $m$ lines, each line contains two floats $(x, y)$, which means the coordinate of the drivers.

Next $n$ lines, each line contains a integer $id$ and a float $val$, which means the task type and utility of requests.

Next $E$ lines, each line conatains two intergers $(u, v)$, which means there are an edge between worker $u$ and task $v$. 

####### test.sol

It is the solution of linear programming, each line contains a float.

First $m$ floats means the value of $y_u$.

Next $n$ floats means the value of $y_v$.

The last $E$ floats means the value of $x_f$.

The meaning of $y_u, y_v, x_f$ can be found in the benchmark LP of our paper.
 