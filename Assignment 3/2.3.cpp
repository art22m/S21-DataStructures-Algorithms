/*
 * Murashko Artem B20-02
 * Codeforces №114205164
 *
 */

#include <iostream>
#include <vector>
#include <map>
#include <stack>
#include <algorithm> // Used to reverse an Array Lists

using namespace std;

const int INF = 1000000000;

template <typename T>
class Vertex {
public:
    T value; // Value (name) of a vertex
    int index; // Index in the Adjacency Matrix

    Vertex(T value, int index) {
        this -> value = value;
        this -> index = index;
    }
};

template <typename T, typename P>
class Edge {
public:
    Vertex<T> *from; // Pointer to the vertex where the edge comes from
    Vertex<T> *to; // Pointer to the vertex where the edge comes to
    P weight; // Weight of the edge

    Edge(Vertex<T> *from, Vertex<T> *to, P weight) {
        this -> from = from;
        this -> to = to;
        this -> weight = weight;
    }
};

template <typename T, typename P>
class GraphADT {
    /*
     * add a vertex with value value to the graph
     * and return reference to the created vertex object;
     */
    virtual Vertex<T>* addVertex(T value) = 0;

    /*
     * remove a vertex, provided a reference to a vertex object;
     */
    virtual void removeVertex(Vertex<T> *v) = 0;

    /*
     * add a (directed) edge between from and to vertices with
     * weight weight, return reference to the created edge object;
     */
    virtual Edge<T, P>* addEdge(Vertex<T> *from, Vertex<T> *to, P weight) = 0;

    /*
     * remove an edge, given a reference to an edge object;
     */
    virtual void removeEdge(Edge<T, P> *e) = 0;

    /*
     * return a collection or edge objects that are going from vertex v;
     */
    virtual vector<Edge<T, P>> edgesFrom(Vertex<T> *v) = 0;

    /*
     * return a collection or edge objects that are going into vertex v;
     */
    virtual vector<Edge<T, P>> edgesTo(Vertex<T> *v) = 0;

    /*
     * ﬁnd any vertex with the specified value;
     */
    virtual Vertex<T>* findVertex(T value) = 0;

    /*
     * ﬁnd any edge with specified values in the source and target vertices.
     */
    virtual Edge<T, P>* findEdge(T from_value, T to_value) = 0;

    /*
     * determine whether there exists a directed edge from v to u;
     */
    virtual bool hasEdge(Vertex<T> *v, Vertex<T> *u) = 0;
};

template <typename T, typename P>
class AdjacencyMatrixGraph : GraphADT<T, P> {
private:
    vector<vector<Edge<T, P>*>> adjacencyMatrix; // Our Adjacency Matrix (array of arrays) which stores pointers to edges
    map <T, Vertex<T>*> vertices; // Map where 'key' -> value (name) of vertex, 'value' -> pointer to this vertex
    stack <int> freeIndexes; // Stack which stores unoccupied indices in the matrix that may arise after deletion

    /* -- The idea of Map "Vertices" and Stack "FreeIndexes" --*/
    /*
     *  I decided to use a map to determine if there is a vertex in the graph with time complexity O(1).
     *  After deletion we will have empty row in our adjacency matrix. It's costly to reduce matrix size,
     *  transfer elements (Takes O (V ^ 2)). But we can make the whole row and column null pointers and add
     *  this free index to our stack (It will take (O(V)). In this regard, the next addition of a vertex will
     *  take O(1) instead of O(V^2).
     */

public:
    /*
     * If our stack is Empty (There are no free indexes in the Adjacency Matrix),
     * we should increase the size of our matrix and add vertex to the map.
     * Time complexity: O(V^2)
     *
     * If our stack is not Empty (There are free indexes in the Adjacency Matrix),
     * we just delete this index from stack and add vertex to the map.
     * Time complexity: O(1)
     *
     * Overall time complexity: O(V^2) worst case
     */
    Vertex<T>* addVertex(T value) {
        // If we already have a vertex with value 'value;
        if (vertices[value] != nullptr)
            return vertices[value];

        if (freeIndexes.empty()) {
            adjacencyMatrix.resize(adjacencyMatrix.size() + 1);
            for (int i = 0; i < adjacencyMatrix.size(); ++i)
                adjacencyMatrix[i].resize(adjacencyMatrix.size());

            auto *vertex = new Vertex<T> (value, adjacencyMatrix.size() - 1);
            vertices[value] = vertex;

            return vertex;
        } else {
            int index = freeIndexes.top();
            freeIndexes.pop();

            auto *vertex = new Vertex<T> (value, index);
            vertices[value] = vertex;

            return vertex;
        }
    }

    /*
     * Push index of deleted vertex to the stack and delete
     * this vertex from map. Also we need to delete all edges
     * which connected this vertex from adjacency matrix (just make
     * the row and the column null pointers).
     *
     * Time complexity: O(V) worst case
     */
    void removeVertex(Vertex<T> *v) {
        if (v == nullptr)
            return;

        int index = v -> index;
        freeIndexes.push(v -> index);
        vertices.erase(v -> value);

        // Make the column and the row zeroes where the vertex was
        for (int i = 0; i < adjacencyMatrix.size(); ++i)
            adjacencyMatrix[index][i] = nullptr;
        for (int i = 0; i < adjacencyMatrix.size(); ++i)
            adjacencyMatrix[i][index] = nullptr;
    }

    /*
     * Just add the pointer to the edge in the adjacency matrix.
     * Constant time, since access to the array takes constant time.
     *
     * Time complexity: O(1) worst case
     */
    Edge<T, P>* addEdge(Vertex<T> *from, Vertex<T> *to, P weight) {
        if (from == nullptr || to == nullptr)
            return nullptr;

        Edge<T, P> *edge = new Edge<T, P>(from, to, weight);
        int indexRow = from -> index;
        int indexColumn = to -> index;
        adjacencyMatrix[indexRow][indexColumn] = edge;

        return edge;
    }

    /*
     * Just delete the pointer to the edge in the adjacency matrix.
     * Constant time, since access to the array takes constant time.
     *
     * Time complexity: O(1) worst case
     */
    void removeEdge(Edge<T, P> *e) {
        if (e == nullptr)
            return;

        int indexRow = e -> from -> index;
        int indexColumn = e -> to -> index;
        adjacencyMatrix[indexRow][indexColumn] = nullptr;
    }

    /*
     * We need to go through all the pointers in the row
     * of the vertex. If we find not a null pointer,
     * add this edge to the array.
     *
     * Time complexity: O(V) worst case
     */
    vector<Edge<T, P>> edgesFrom(Vertex<T> *v) {
        vector<Edge<T, P>> edges;
        if (v == nullptr)
            return edges;

        int indexRow = v -> index;
        for (int i = 0; i < adjacencyMatrix.size(); ++i)
            if (adjacencyMatrix[indexRow][i] != nullptr)
                edges.push_back(*adjacencyMatrix[indexRow][i]);

        return edges;
    }

    /*
     * We need to go through all the pointers in the column
     * of the vertex. If we find not a null pointer,
     * add this edge to the array.
     *
     * Time complexity: O(V) worst case
     */
    vector<Edge<T, P>> edgesTo(Vertex<T> *v) {
        vector<Edge<T, P>> edges;
        if (v == nullptr)
            return edges;

        int indexColumn = v -> index;
        for (int i = 0; i < adjacencyMatrix.size(); ++i)
            if (adjacencyMatrix[i][indexColumn] != nullptr)
                edges.push_back(*adjacencyMatrix[i][indexColumn]);

        return edges;
    }

    /*
     * Return a pointer from our map. Constant time, since access to
     * the map takes constant time.
     *
     * Time complexity: O(1) worst case
     */
    Vertex<T>* findVertex(T value) {
        return vertices[value];
    }

    /*
     * We need to find vertices with values
     * 'from_value' and 'to_value'. After that, we will get
     * the indexes of this vertices in the adjacency matrix.
     * It remains only to return the reference from the matrix,
     * knowing the indices. Constant time, since findVertex(T value)
     * takes constant time and access to the matrix takes constant time.
     *
     * Time complexity: O(1)
     */
    Edge<T, P>* findEdge(T from_value, T to_value) {
        auto vertexFrom = findVertex(from_value);
        auto vertexTo = findVertex(to_value);

        if (vertexFrom == nullptr || vertexTo == nullptr)
            return nullptr;

        int indexRow = vertexFrom -> index;
        int indexColumn = vertexTo -> index;

        return adjacencyMatrix[indexRow][indexColumn];
    }

    /*
     * Since we have references to the vertices, we can get
     * the indexes. Then we need to check, if our Adjacency Matrix
     * contains not a null pointer by these indices.
     *
     * Time complexity: O(1)
     */
    bool hasEdge(Vertex<T> *v, Vertex<T> *u) {
        if (v == nullptr || u == nullptr)
            return false;

        int indexRow = v -> index;
        int indexColumn = u -> index;

        return adjacencyMatrix[indexRow][indexColumn] != nullptr;
    }
};

/*
 * The idea is not to add those edges, bandwidth of which is greater than the minimum.
 * After that we can apply common dijkstra algorithm.
 */
template <typename T, typename P>
void dijkstra(AdjacencyMatrixGraph<T, P> graph, int n, int start, int end) {
    vector <int> distances; // Array List of distances
    distances.resize(n + 1);
    vector <int> parents; // Array List of parents
    parents.resize(n + 1);

    for (int i = 1; i < n + 1; ++i)
        distances[i] = INF;
    distances[start] = 0;

    /*
     * I know, that it is inefficient to sort the array every time and still remove from it every iteration,
     * but the program has passed all the tests on the codeforces.
     *
     * P.S. it would be more efficient to search for the minimum element every time (linear time)
     * and remove it (In case we would like to solve the problem without using a priority queue).
     */
    vector <pair<int, int>> q; // Pseudo priority queue to extract vertex with min distance at every iteration
    q.push_back(make_pair(0, start));

    while (!q.empty()) {
        sort(q.begin(), q.end()); // Sort Array List to extract the min

        pair <int, int> u = q[0]; // Extracted vertex with it's distance
        q.erase(q.begin()); // Delete this pair (vertex + distance) from Array List

        if (u.first > distances[u.second])
            continue;

        vector<Edge<T, P>> edges = graph.edgesFrom(graph.findVertex(u.second)); // Find all edges from u, to find all adjacent vertices

        for (auto i : edges) { // Go through every edge (every adjacent vertex)
            // Relaxation
            if(distances[u.second] + i.weight.first < distances[i.to -> value]) {
                distances[i.to -> value] = distances[u.second] + i.weight.first;
                q.push_back(make_pair(distances[i.to -> value], i.to -> value));
                parents[i.to -> value] = u.second; // Add parent of u
            }
        }
    }

    if (distances[end] == INF) { // If the distance to vertex 'end' equal to INF => path does not exist
        cout << "IMPOSSIBLE";
        return;
    }

    vector <int> path; // Array List which will contain the path from 'start' to 'end'
    int bandwidth = graph.findEdge(parents[end], end) -> weight.second; // Bandwidth of the path

    // Go through parents list while we won't come to vertex 'start', starting at vertex 'end'
    for (int v = end; v != start; v = parents[v]) {
        if (v != start) // Also update bandwidth on the path
            bandwidth = min(bandwidth, graph.findEdge(parents[v], v)->weight.second);

        path.push_back(v);
    }

    path.push_back(start);
    reverse(path.begin(), path.end());

    cout << path.size() << " " << distances[end] << " " << bandwidth << endl;
    for (auto vertex: path)
        cout << vertex << " ";
}

int main() {
    // The weight of edge will be pair. First element of pair will be length, second - bandwidth
    AdjacencyMatrixGraph <int, pair<int, int>> graph;
    int n, m, b;
    cin >> n >> m;

    // Initialize graph with vertices
    for (int i = 1; i <= n; ++i)
        graph.addVertex(i);

    // Add edges to the graph
    int ik, jk, lk, bk;
    vector<vector<int>> input;
    for (int i = 0; i < m; ++i) {
        cin >> ik >> jk >> lk >> bk;
        input.push_back({ik, jk, lk, bk});
    }

    int start, end;
    cin >> start >> end >> b;

    for (auto i : input) {
        if (i[3] >= b) // add to the graph only those vertices whose bandwidth is greater than or equal to the minimum
            graph.addEdge(graph.findVertex(i[0]), graph.findVertex(i[1]), make_pair(i[2], i[3]));
    }
    dijkstra(graph, n, start, end);
    return 0;
}