/*
 * Murashko Artem B20-02
 * Codeforces №114204208
 *
 */

#include <iostream>
#include <vector>
#include <map>
#include <stack>
#include <algorithm> // Used to reverse an Array Lists

using namespace std;

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

    void transpose() {
        // Find transposed matrix
        vector<vector<Edge<T, P>*>> transposedMatrix = adjacencyMatrix;
        for (int i = 0; i < adjacencyMatrix.size(); ++i)
            for (int j = 0; j < adjacencyMatrix.size(); ++j)
                transposedMatrix[i][j] = adjacencyMatrix[j][i];
        adjacencyMatrix = transposedMatrix;

        // Swap pointers 'to' and 'from' in every edge
        for (int i = 0; i < adjacencyMatrix.size(); ++i) {
            for (int j = 0; j < adjacencyMatrix.size(); ++j) {
                if (adjacencyMatrix[i][j] != nullptr) {
                    Edge<T, P>* edge = adjacencyMatrix[i][j];
                    Vertex<T>* from = adjacencyMatrix[i][j] -> from;
                    Vertex<T>* to = adjacencyMatrix[i][j] -> to;
                    edge -> from = to;
                    edge -> to = from;
                }
            }
        }
    }

    /*
     *  We will use a simple DFS with parents array to restore the path.
     *  The idea is to execute DFS from white vertices. And if we have an edge
     *  to gray vertex, then we have a path. I just save the vertex from and vertex to
     *  and restore a path using parents array.
     */

    void isAcyclic() {
        map <Vertex<T>*, int> color;
        map <Vertex<T>*, Vertex<T>*> parents;
        Vertex<T>* cycle_start;
        Vertex<T>* cycle_end;

        for (const auto& vertex: vertices) {
            if (DFS(vertex.second, color, parents, cycle_start, cycle_end)) {
                vector <Vertex<T>*> cycle;
                Vertex<T>* currentVertex = cycle_end;

                cycle.push_back(currentVertex);
                while (currentVertex != cycle_start) {
                    cycle.push_back(parents[currentVertex]);
                    currentVertex = parents[currentVertex];
                }

                reverse(cycle.begin(), cycle.end());

                P weight = adjacencyMatrix[cycle[0] -> index][cycle[1] -> index] -> weight;
                for (int i = 1; i < cycle.size() - 1; ++i)
                    weight += adjacencyMatrix[cycle[i] -> index][cycle[i + 1] -> index] -> weight;
                weight += adjacencyMatrix[cycle[cycle.size() - 1] -> index][cycle[0] -> index] -> weight;

                cout << weight << " ";
                for (auto i : cycle)
                    cout << i -> value << " ";
                cout << endl;
                return;
            }
        }
        cout << "ACYCLIC" << endl;
    }

    bool DFS(Vertex<T>* v, map <Vertex<T>*, int> &color, map <Vertex<T>*, Vertex<T>*> &parents, Vertex<T>* &cycle_start, Vertex<T>* &cycle_end) {
        color[v] = 1;
        vector<Edge<T, P>> edges = edgesFrom(v);
        for (int i = 0; i < edges.size(); ++i) {
            Vertex<T>* to = edges[i].to;

            if (color[to] == 0) {
                parents[to] = v;
                if (DFS(to, color, parents, cycle_start, cycle_end))
                    return true;
            } else if (color[to] == 1) {
                cycle_end = v;
                cycle_start = to;
                return true;
            }
        }
        color[v] = 2;
        return false;
    }
};

int main() {
    /*
     * 1. ADD_VERTEX <name> — add a vertex with a given name 1 ;
     * 2. REMOVE_VERTEX <name> — remove a vertex with a given name;
     * 3. ADD_EDGE <from_name> <to_name> <weight> — add an edge from
     * from_name to to_name with an integer weight weight;
     * 4. REMOVE_EDGE <from_name> <to_name> — remove an edge from from_name to to_name;
     * 5. HAS_EDGE <from_name> <to_name> — output TRUE if there is an edge
     * from from_name to to_name and FALSE otherwise;
     * 6. IS_ACYCLIC — output ACYCLIC if the graph is acylcic, otherwise output
     * <weight> <v1> <v2> ... <vN> where <weight> is the total weight of
     * the cycle and <v1> <v2> ... <vN> is a sequence of vertices constituting a cycle;
     * 7. TRANSPOSE — transpose the graph.
     */

    AdjacencyMatrixGraph <string, int> graph;
    string command;
    while (cin >> command) {
        if (command == "ADD_VERTEX") {
            string name;
            cin >> name;

            graph.addVertex(name);
        } else if (command == "REMOVE_VERTEX") {
            string name;
            cin >> name;

            graph.removeVertex(graph.findVertex(name));
        } else if (command == "ADD_EDGE") {
            string from_name, to_name;
            int weight;
            cin >> from_name >> to_name >> weight;

            graph.addEdge(graph.findVertex(from_name), graph.findVertex(to_name), weight);
        } else if (command == "REMOVE_EDGE") {
            string from_name, to_name;
            cin >> from_name >> to_name;

            graph.removeEdge(graph.findEdge(from_name, to_name));
        } else if (command == "HAS_EDGE") {
            string from_name, to_name;
            cin >> from_name >> to_name;

            cout << (graph.hasEdge(graph.findVertex(from_name), graph.findVertex(to_name)) ? "TRUE" : "FALSE") << endl;
        } else if (command == "IS_ACYCLIC") {
            graph.isAcyclic();
        } else if (command == "TRANSPOSE") {
            graph.transpose();
        }
    }

    return 0;
}
