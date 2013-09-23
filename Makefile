all:
	g++ circuits_hawick.cpp -o circuits_hawick

clean:
	rm -f circuits_hawick test1.ps test2.ps test3.ps

test1:
	printf "0 1\n1 0\n0 2\n2 3\n3 4\n4 0\n" | ./circuits_hawick 5

test2:
	printf "0 1\n0 2\n1 0\n1 3\n2 0\n3 0\n3 1\n3 2" | ./circuits_hawick 4

test3:
	printf "0 2\n0 10\n0 14\n1 5\n1 8\n2 7\n2 9\n3 3\n3 4\n3 6\n4 5\n4 13\n 4 15\n6 13\n8 0\n8 4\n8 8\n9 9\n10 7\n10 11\n11 6\n12 1\n12 1\n12 2\n12 10\n12 12\n 12 14\n13 3\n13 12\n13 15\n14 11\n15 0" | ./circuits_hawick 16

dot: test1.ps test2.ps test3.ps

%.ps: %.dot
	dot -Tps -Gsize='7.5,10' -Gratio=auto -Gcenter=true $< > $@


