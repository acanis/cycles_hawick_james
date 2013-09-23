/*
 * Enumerating Circuits and Loops in Graphs with Self-Arcs and Multiple-Arcs
 * K.A. Hawick and H.A. James
 * Computer Science, Institute for Information and Mathematical Sciences,
 * Massey University, North Shore 102-904, Auckland, New Zealand
 * k.a.hawick@massey.ac.nz; heath.james@sapac.edu.au
 * Tel: +64 9 414 0800
 * Fax: +64 9 441 8181
 * Technical Report CSTN-013
 */

#include <vector>
#include <string>
#include <iostream>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
using namespace std;


int nVertices = 0;                  // number of vertices
int start = 0;                      // starting vertex index
vector< vector<int> > Ak; // integer array size n of lists
                                    // ie the arcs from the vertex
vector< vector<int> > B;  // integer array size n of lists
vector< bool > blocked;        // logical array indexed by vertex
int nCircuits = 0;                  // total number of circuits found;
vector< int > lengthHistogram; // histogram of circuit lengths
vector< vector<int> >     // adjacency table of occurrences of
    vertexPopularity;               // vertices in circuits of each length
vector< int > longestCircuit;  // the (first) longest circuit found
int lenLongest = 0;                 // its length
bool enumeration = true;            // explicitly enumerate circuits
vector< int > stack;           // stack of integers
static int stackTop = 0;            // the number of elements on the stack
                                    // also the index "to put the next one"

// return TRUE if value is NOT in the list
bool notInList (vector<int> &list, int val) {
    assert(list [0] < list.size());
    for (int i = 1; i <= list[0]; i++) {
        if (list[i] == val)
            return false;
    }
    return true;
}

// return TRUE if value is in the list
bool inList (vector<int> &list, int val) {
    assert(list[0] < list.size());
    for (int i = 1; i <= list[0]; i++) {
        if (list[i] == val)
            return true;
    }
    return false;
}

// empties a list by simply zeroing its size
bool emptyList (vector<int> &list) {
    assert(list[0] < list.size());
    list[0] = 0;
}

// adds on to the end (making extra space if needed)
void addToList (vector<int> &list, int val) {
    assert(list[0] < list.size());
    int newPos = list[0] + 1;
    list[newPos] = val;
    list[0] = newPos;
}

// removes all occurences of val in the list
int removeFromList (vector<int> &list, int val) {
    assert(list[0] < list.size());
    int nOccurrences = 0;
    for (int i = 1; i <= list[0]; i++) {
        if (list[i] == val) {
            nOccurrences++;
            for (int j = i; j<list[0]; j++) {
                list[j] = list[j+1];
            }
            --list[0]; // should be safe as list[0] is
                       // re-evaluated each time around the i-loop
            --i;
        }
    }
    return nOccurrences;
}

void stackPrint3d() {
    int i;
    printf("Cycle:\n");
    for (i = 0; i < stackTop-1; i++) {
        printf("%d ", stack[i]);
    }
    printf("%d\n", stack[i]);
}

int countAkArcs () { // return number of Arcs in graph
    int nArcs = 0;
    for (int i =0; i<nVertices; i ++) {
        nArcs += Ak[i][0]; // zero’th element gives nArcs for i
    }
    return nArcs;
}

void unblock (int u) {
    blocked [u] = false;
    for (int wPos = 1; wPos <= B[u][0]; wPos++) {
        // for each w in B[u]
        int w = B[u][wPos];
        wPos -= removeFromList(B[u], w);
        if (blocked[w])
            unblock(w);
    }
}

// initialise the stack to some size max
void stackInit(int max) {
    stack.resize(max);
    stackTop = 0;
}

// push an int onto the stack, extending if necessary
void stackPush (int val) {
    if (stackTop >= stack.size())
        stack.resize(stack.size() + 1);
    stack[stackTop++] = val;
}

int stackSize() {
    return stackTop;
}

int stackPop () {
    // pop an int off the stack
    assert(stackTop > 0);
    return stack[--stackTop];
}

void stackClear () {
    // clear the stack
    stackTop = 0;
}

bool circuit(int v) { // based on Johnson ’s logical procedure CIRCUIT
    bool f = false;
    stackPush(v);
    blocked[v] = true;
    for (int wPos = 1; wPos <= Ak[v][0]; wPos++) { // for each w in list Ak[v]:
        int w = Ak[v][wPos];
        if (w < start) continue; // ignore relevant parts of Ak
        if (w == start) { // we have a circuit,
            if (enumeration) {
                stackPrint3d(); // print out the stack to record the circuit
            }
            assert (stackTop <= nVertices);
            ++lengthHistogram[stackTop]; // add this circuit ’s length to the length histogram
            nCircuits++; // and increment count of circuits found
            if (stackTop > lenLongest) { // keep a copy of the longest circuit found
                lenLongest = stackTop;
                longestCircuit = stack;
            }
            for (int i = 0; i < stackTop; i ++) // increment [circuit-length][vertex] for all vertices in this circuit
                 ++vertexPopularity[stackTop][stack[i]];
            f = true;
        } else if (!blocked[w]) {
            if (circuit(w)) f = true;
        }
    }
    if (f) {
        unblock (v);
    } else {
        for (int wPos = 1; wPos <= Ak[v][0]; wPos++) { // for each w in list Ak[v]:
            int w = Ak[v][wPos];
            if (w < start) continue;  // ignore relevant parts of Ak
            if (notInList(B[w], v)) addToList(B[w], v);
        }
    }
    v = stackPop();
    return f;
}


void setupGlobals(int argc, char* argv[]) {  // presupposes nVertices is set up
    nVertices = atoi(argv[1]);
    printf("num_vertices = %d\n", nVertices);
    Ak.resize(nVertices); // Ak[i][0] is the number of members, Ak[i][1]..Ak[i][n] ARE the members, i>0
    B.resize(nVertices);  // B[i][0] is the number of members, B[i][1]..B[i][n] ARE the members , i>0
    blocked.resize(nVertices); // we use blocked [0]..blocked[n-1], i> = 0

    for (int i = 0; i < nVertices; i++) {
        Ak[i].resize(nVertices);
        B[i].resize(nVertices);
        blocked[i] = false;
    }

    int v1, v2;
    while (cin >> v1 && cin >> v2) {
        printf("%d -> %d\n", v1, v2);
        addToList(Ak[v1], v2);
    }

    lengthHistogram.resize(nVertices+1); // will use as [1]...[n] to histogram circuits by length
                                          // [0] for zero length circuits, which are impossible
    for (int len = 0; len < lengthHistogram.size(); len++) // initialise histogram bins to empty
        lengthHistogram[len] = 0;
    stackInit(nVertices);
    vertexPopularity.resize(nVertices+1); // max elementary circuit length is exactly nVertices
    for (int len = 0; len <= nVertices; len++) {
        vertexPopularity[len].resize(nVertices);
        for (int j = 0; j < nVertices; j++) {
            vertexPopularity[len][j] = 0;
        }
    }
}

/*
 * to replicate the result from figure 10 in the paper, run the program as
 * follows:
 *
* echo "0 2\n0 10\n0 14\n1 5\n1 8\n2 7\n2 9\n3 3\n3 4\n3 6\n4 5\n4 13\n\
* 4 15\n6 13\n8 0\n8 4\n8 8\n9 9\n10 7\n10 11\n11 6\n12 1\n12 1\n12 2\n12 10\n12 12\n\
* 12 14\n13 3\n13 12\n13 15\n14 11\n15 0" | ./circuits_hawick 16
 */
int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("usage: echo \"v1 v2\nv1 v3\n...\" | %s num_vertices\n", argv[0]);
        return 1;
    }
    setupGlobals(argc, argv);
    stackClear();
    start = 0;
    bool verbose = false;
    while (start < nVertices) {
        if (verbose && enumeration) printf("Starting s = %d\n", start);
        for (int i = 0; i < nVertices; i++) { // for all i in Vk
            blocked[i] = false;
            emptyList(B[i]);
        }
        circuit(start);
        start = start + 1;
    }
    return 0;
}
