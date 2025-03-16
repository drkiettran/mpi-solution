# Notes
Developing an MPI program to multiply two matrices.
Given matrix $A(n \times m)$ and matrix $B(m \times l)$. 
$A \times B = C(n \times l)$.

## Partitioning
Each row in matrix $C$ can be computed independently from other rows in matrix $C$. 
Thus, we can assign $X$ number of rows of Matrix $A$ to a process in a process group.
Each process can compute the $X$ number of rows in matrix $C$.

Given we have P processes for n rows.
$X = P / n$. If $R = P \mod{n} \gt 0$, $R$ rows can be spread across the first $R$ processes in the process group. 

## Communication
- Each process must know matrix $B$, the values of $m, l$
- Each process requires $X$ or $X+1$ (if $P \mod{n} > 0$) rows.