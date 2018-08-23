# Optimal-Static-BST
Implementation of an optimal static binary search tree in C
Emily Davis
Summer 2018

My program bst is an implementation of an optimal static binary search tree.
It takes user input of keys (integers only), probabilities of searching those keys, and
probabilities of searching between those keys.  It then uses dynamic
programming to find the optimal structure of the tree.  Finally, it outputs
the generated W-table and  OPT-table along with the expected search time and
a visual of the resulting tree (rotated 90-degrees for printing purposes).

bst can be compiled into an executable by running:
                gcc -o bst bst.c

bst can either be used by interacting via the command line, or by running the
included script "myscript" and piping an input file with cat to it as follows:

                cat input.txt | ./myscript

I have included two sample input files, input.txt and input2.txt.
