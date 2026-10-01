#include <stdio.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
using namespace std;

/*
Name: Christopher Bingham
Email: cbingha2@binghamton.edu
Assignment: Week 6 - Dijkstra's Algorithm
*/


//INFINITY Global Constant
const int INFIN = 999999;

int parentNode(int index) {
    /*
    Returns the index of the parent node of the node at the given index.
    Parameters:
       - index <int>: The index of the node for which to find the parent.
    Returns:
        - <int>: The index of the parent node.
    */
    return (index - 1) / 2;
}

int leftChildNode(int index) {
    /*
    Returns the index of the left child node of the node at the given index.
    Parameters:
       - index <int>: The index of the node for which to find the left child.
    Returns:
        - <int>: The index of the left child node.
    */
    return (2 * index) + 1;
}

int rightChildNode(int index) {
    /*
    Returns the index of the right child node of the node at the given index.
    Parameters:
       - index <int>: The index of the node for which to find the right child.
    Returns:
        - <int>: The index of the right child node.
    */
    return (2 * index) + 2;
}

class Edge {
    /*
    The edge of a given graph that connects vertices together.
    Parameters:
        vertex <int>: Vertex the edge leads to
        length <int>: Edge Length
        next vector<Edge> : Edges that connects to
    */
    public:
        int vertex;
        int length;
        vector<Edge> next;
};

class Vertex {
    /*
    The vertex of a given graph
    Parameters:
        distance <int>: Cost to add this vertex
        edges vector<Edge> : Other edges that this vertex connects to
        prior <int> : Prior vertex number
    */
    public:
        int distance;
        vector<Edge> edges;
        int prior = -1;

};

class HeapElement {
    /*
    Creates the heap element with vertex and distance
    Parameters:
        vertexNumber <int>: The vertex number
        distanceCost <int>: Distance to vertex
    */
    public:
        int vertexNumber;
        int distanceCost;
};

class Heap {
    /*
    Create the min Heap design used for us to get the minimum edges between our vertices
    Parameters:
        minHeap vector<HeapElement> : Min heap of all vertices
        index vector<int> : Track vertices index in heap
        heapSize <int>: Heap size
    */
    public:
        vector<HeapElement> minHeap;
        vector<int> index;
        int heapSize;
};

void heapSwap (Heap &heap, int index1, int index2) {
    /*
    Swap the two vertices within the heap & UPDATE THEIR INDEX!
    Parameters:
        heap <Heap>: Heap modified in place
        index1 <int>: Index of first value to swap
        index2 <int>: Index of second value to swap 
    Returns:
        None
    */
    HeapElement temp;
    temp = heap.minHeap[index1];
    heap.minHeap[index1] = heap.minHeap[index2];
    heap.minHeap[index2] = temp;

    heap.index[heap.minHeap[index1].vertexNumber] = index1;
    heap.index[heap.minHeap[index2].vertexNumber] = index2;

}

void heapPercolateUp (Heap &heap, int index) {
    /*
    Swap, if possible, the given vertex higher into the heap 
    Parameters:
        heap <Heap>: Heap modified in place
        index <int>: Index of vertex to percolate up
    Returns:
        None
    */
    while((index > 0) && (heap.minHeap[parentNode(index)].distanceCost > heap.minHeap[index].distanceCost)) {
        heapSwap(heap, index, parentNode(index));
        index = parentNode(index);
    }
}

void heapPercolateDown (Heap &heap, int index) {
    /*
    Swap, if possible, the vertex lower into the heap based on the largest child node
    Parameters:
        heap <Heap>: Heap modified in place
        index <int>: Index of vertex to percolate down 
    Returns:
        None
    */
    int small;
    while(index < heap.heapSize) {
        small = index;
        if((leftChildNode(index) < heap.heapSize) && (heap.minHeap[leftChildNode(index)].distanceCost < heap.minHeap[small].distanceCost)) {
            small = leftChildNode(index);
        }
        if((rightChildNode(index) < heap.heapSize) && (heap.minHeap[rightChildNode(index)].distanceCost < heap.minHeap[small].distanceCost)) {
            small = rightChildNode(index);
        }
        if (index == small) {
            return;
        }

        heapSwap(heap, index, small);
        index = small;
    }
}

int heapPop (Heap &heap, HeapElement &element) {
    /*
    Remove and record the top heap element.
    Parameters:
        heap <Heap>: Heap modified in place
        element <HeapElement>: Records Heap element 'popped'
    Returns:
        1 (Heap is not empty; Continue popping if necessary)
        0 (Heap is empty; No more elements to pop)
    */
    int index;
    int small;
    // HeapElement temp;

    if (heap.heapSize == 0){
        return 0;
    }

    element = heap.minHeap[0];                          // Record popped element
    heap.minHeap[0] = heap.minHeap[heap.heapSize - 1];  //Last element replaces top element
    heap.index[heap.minHeap[0].vertexNumber] = 0;       //Update index
    heap.heapSize--;                                    //Update smaller heap size

    heapPercolateDown(heap, 0);                         //Put element in proper heap placement

    return 1;
}

void heapInsert (Heap &heap, int vertex, int distance) {
    /*
    Insert an element into the heap
    Parameters:
        heap <Heap>: Heap element is inserted into
        vertex <int>: Vertex number
        distance <int>: Distance to vertex
    Returns:
        None
    */
    int index;
    // HeapElement temp;

    //Add vertex and distance at end of heap
    heap.minHeap[heap.heapSize].vertexNumber = vertex;
    heap.minHeap[heap.heapSize].distanceCost = distance;
    heap.index[vertex] = heap.heapSize;
    index = heap.heapSize;
    heap.heapSize++;                                    //Update heap size

    heapPercolateUp(heap, index);                       //Percolate element into place

}

void heapDecreaseKey (Heap &heap, int vertex, int distance) {
    /*
    Update element heap position when improved distance is found
    Parameters:
        heap <Heap>: Heap modified in place
        vertex <int> : Vertex number
        distance <int> : Distance to vertex
    Return:
        None
    */
    int vertexIndex = heap.index[vertex];

    // if (heap.minHeap[vertexIndex].vertexNumber != vertex) {
    //     exit(1);
    // }
    heap.minHeap[vertexIndex].distanceCost = distance;  //Update vertex distance
    heapPercolateUp(heap, vertexIndex);                 //Percolate element into place
}

void heapShow (Heap &heap) {
    /*
    Display the heap show proper changes in values and heap removal
    Parameters:
        heap <Heap>: the heap used to display the information
    Returns:
        None
    */
    cout << "Heap : " << heap.heapSize << " elements\n";
    for (int i = 0; i < heap.heapSize; ++i) {
        cout << "[v " << heap.minHeap[i].vertexNumber << " dist " << heap.minHeap[i].distanceCost << " (idx " << heap.index[heap.minHeap[i].vertexNumber] << " i " << i << ")]";
    }
    cout << "\n";

}

int readFile(ifstream &inputFile, int argc, char *argv[]) {
    /*
    Reads the input file and builds the array to be sorted.
    Parameters:
       - inputFile <ifstream>: The input file stream from which to read the integers.
       - argc <int>: The number of command line arguments provided.
    Returns:
        - 0: Successful execution
        - 1: Error in opening the input file
    */
    if (argc != 2) {
        string fileName;
        cout << "Enter File Name:" << endl;
        cin >> fileName;
        inputFile.open(fileName);
        if (!inputFile) {
            cout << "Cannot Open File: " << fileName << endl;
            cout << "CommandLine Usage: ./primAlgo <input_file>" << endl;
            return 1;
        }
    } else {
        inputFile.open(argv[1]);
        if (!inputFile) {
            cout << "Cannot Open File: " << argv[1] << endl;
            cout << "CommandLine Usage: ./primAlgo <input_file>" << endl;
            return 1;
        }
    }
    return 0;
}

void initData(ifstream &file, int &numVertices, int &numEdges, vector<Vertex> &vertices, vector<Edge> &edges, Heap &heap) {
    /*
    Initialize some of the data used for algorithm
    Parameters:
        file <ifstream>: File to read data from
        numVertices <int>: Number of vertices
        numEdges <int>: Number of edges
        vertices <vector<Vrtex>>: Vector of vertices to add
        edges <vector<Edge>>: Vector of edges to add
        heap <Heap>: Min heap to help build shortest paths
    Returns:
        None
    */
    file >> numVertices;
    file >> numEdges;

    //Initialize some of the data
    vertices.resize(numVertices);
    edges.resize(2*numEdges);   //For undirected graph (to & from vertices)

    heap.heapSize = 0;
    heap.minHeap.resize(numVertices);
    heap.index.resize(numVertices);
}

void addVertices(vector<Vertex> &vertices, Heap &heap, int numVertices) {
    /*
    Create and add vertices into the heap
    Parameters:
        vertices vector<Vertex> : Vector of vertices to be filled
        heap <Heap>: Heap vertices inserted into
        numVertices <int>: number of vertices to create
    Returns:
        None 
    */
    for (int i = 0; i < numVertices; i++) {
        // cout << "STUCK";
        vertices[i].distance = INFIN;
        vertices[i].edges.resize(0);
        heapInsert(heap, i, INFIN);
    }
}

void addEdgesUndirected(ifstream &file, vector<Edge> &edges, vector<Vertex> &vertices, int numEdges) {
    /*
    Create and add all edges for their respective connecting vertices
    Parameters:
        file <ifstream>: Read the values in from the file
        edges <vector<Edge>>: Vector created edges are added
        vertices <vector<Vertex>>: Vector of vertices to add edges to
        numEdges <int> : Number of edges to add
    Returns:
        None
    */
    
    int eI;
    int v1;
    int v2;
    int length;

    for(int i = 0; i < numEdges; ++i) {
        file >> v1;
        file >> v2;
        file >> length;
        edges[eI].length = length;
        edges[eI].vertex = v2;
        edges[eI].next.insert(edges[eI].next.end(), vertices[v1].edges.begin(), vertices[v1].edges.end());
        vertices[v1].edges.push_back(edges[eI]);
        ++eI;
        edges[eI].length = length;
        edges[eI].vertex = v1;
        edges[eI].next.insert(edges[eI].next.end(), vertices[v2].edges.begin(), vertices[v2].edges.end());
        vertices[v2].edges.push_back(edges[eI]);
        ++eI;
    }

}

void printLengthNPath(vector<Vertex> &vertices, Heap &heap, int startVertex, int endVertex) {
    /*
    Print out the length from <StartVertex> to <EndVertex> and its path there
    Parameters:
        vertices vector<Vertex>: Vector of vertices
        heap <Heap>: Heap to find vertices
        startVertex: Start of the path
        endVertex: End of the path
    Returns:
        None
    */
    cout << vertices[endVertex].distance << endl;
    while(vertices[endVertex].prior != -1){
        int temp = vertices[endVertex].prior;
        // cout << "First Edge: " << endVertex << " | Second Edge: " << temp << "| Next Prior: " << vertices[temp].prior << endl;
        cout << endVertex << " " << temp << endl;
        endVertex = temp; 
    }
}

void dijkstraAlgo(Heap &heap, vector<Vertex> &v, HeapElement &element) {
    /*
    Perform Dijkstra's and find shortest path from 0 to V-1 by default; can change to be choosen values
    Parameters:
        heap <Heap>: Heap used to find shortest distances
        v <vector<Vertex>: Vector of vertices
        element <HeapElement> : Gets element from the heap pop
    Returns:
        None
    */
       //Perform Dijkstra's and get the shortest paths!

    //Testing if the algo is working from any vertex start
    bool chooseStart = false;    //Change to true to individually choose start and end nodes
    int randNumStart;
    if (chooseStart) {
        cout << "Enter a starting index from 0->" << v.size()-1 << endl;
        cin >> randNumStart;
        v[randNumStart].distance = 0;
        heapDecreaseKey(heap, randNumStart, 0);
    } else{
        v[0].distance = 0;
        heapDecreaseKey(heap, 0, 0);
    }

    int v1;
    int v2; 
    vector<int> visited(v.size(), 0);
    // int totalCost = 0;
    int max = -1;

    // heapShow(heap);

    while (heapPop(heap, element)) {
        // totalCost += element.distanceCost;

        // v[element.vertexNumber].distance = 0;

        v1 = element.vertexNumber;

        if (visited[v1]){
            // Already visited this vertex. Continue to next one
            continue;
        }
        visited[v1] = 1;

        for(Edge outEdge : v[v1].edges) {
             v2 = outEdge.vertex;

            if (v[v1].distance + outEdge.length < v[v2].distance) {

                v[v2].distance = v[v1].distance + outEdge.length;
                v[v2].prior = v1;
                heapDecreaseKey(heap, v2, v[v1].distance + outEdge.length);
            }
        }
        // heapShow(heap);
    }

    // cout << totalCost << "\n";
    // for(int i = 0; i < v.size(); ++i){
    //     if (v[i].distance > max){
    //         max = v[i].distance;
    //         cout << "Distance to vertex " << i << " is " << v[i].distance << endl;
    //     }
    // }
    // cout << "Max Distance: " << max  << endl;

    if(chooseStart){
        int getVal;
        cout << "Which Distance to Find? (" << randNumStart << "-> X) | " << "X = 0 ->" << v.size()-1 << ")"  << endl;
        cin >> getVal;
        printLengthNPath(v, heap, randNumStart, getVal);
    } else {
        printLengthNPath(v, heap, 0, v.size()-1);
    }

    // int num = v.size()-1;
    // int test;
    // cout << "Length: " << v[num].distance;
    // while(v[num].prior != -1){
    //     int temp = heap.minHeap[heap.index[num]].vertexNumber;
    //     cout << "First Edge: " << num << " | Second Edge: " << temp << "| Next Prior: " << v[temp].prior << endl;
    //     num = v[temp].prior;
    //     cin >> test; 
    // }
}

int main(int argc, char *argv[]) {
    /*
    Main function to execute the dijkstra's algorithm.
    CommandLine Usage:
        - ./primAlgo <input_file>
    Command line arguments:
        - argv[1]: The name of the input file containing the integers to be sorted.
        - If no command line argument is provided, the user will be prompted to enter the file name.
    Returns:
        - 0: Successful execution
        - 1: Error in opening the input file
    */

    //Read the input file and build the array to be sorted
    ifstream inputFile;

    if (readFile(inputFile, argc, argv)) {
        return 1;
    }

    //Setting up values
    vector<Vertex> v;
    vector<Edge> e;
    int numV;
    int numE;
    Heap heap;
    // int i;
    // int eI;
    vector<int> s;
    int v1;
    int v2;
    int length;
    HeapElement element;
    // vector<Edge> outEdge;
    // int outEdge;
    int max;

    //Initialize the data
    initData(inputFile, numV, numE, v, e, heap);

    //Add vertices
    addVertices(v, heap, numV);

    // heapShow(heap);

    //Add edges (undirected; to & from both vertices) 
    addEdgesUndirected(inputFile, e, v, numE);

    //Perform Dijkstra's and get the shortest paths!
    dijkstraAlgo(heap, v, element);

    return 0;

}