//
// Created by Artem Murashko on 13.02.2021
// 2.3 Accounting for a cafe
// Submission: 108053120
//

#include <iostream>
#include <string>
#include <iomanip>

using namespace std;


/*
 * Solution Description:
 *
 * I have a hash map of Lists, since i handle collision by Separate Chaining.
 * Each element of list is a pair of pair and a Set, i.e.
 * each element has following form: pair < pair<string, double> , Set<T> >,
 * call it tuple.
 *
 * When we input a line in the form: date + time + receipt_ID + cost + item_title
 * i take only date, receipt_ID and cost.
 *
 * Every iteration i use method put, in the following form:
 * pair <pair<date, cost>, Set.insert(receipt_ID) >
 *
 * Adhere the following rules:
 * 1) Receipt ID inserts into the Set, since we need to find a number of unique receipts ID
 * 2) Use method add for sum up all prices, to get total cost
 * I have added a description to each method, so it will not be difficult to understand points 1 and 2
 *
 * At the end, it only remains to display all tuples in the required form.
 */

const int szP = 50000; // the maximum number of entries in the input

//Set is an associative container that contains a set of unique objects of type T
template <typename T>
class SetADT {
    virtual void insert(T item) = 0; // add item of type T into the set
    virtual int size() = 0; // return actual number of elements of the set
};

template <typename T>
class Set : SetADT<T> {
private:
    T* arrayList; // dynamic array which stores unique objects of type T
    int arraySize; // size of the array
    int numberOfElements; // actual number of elements
public:
    // constructor
    Set() {
        arrayList = new T[1];
        arraySize = 1;
        numberOfElements = 0;
    }

    /*
     * Time complexity: O(N * K) worst case
     * N - actual number of elements in the array.
     * K - number of iterations for comparing the objects
     *
     * Actual number of elements equal to the size of the set,
     * then we apply double strategy, which increase the size
     * of the array.
     *
     * When we insert the object into the set,
     * we go throw all existing objects O(N), and if
     * the number is not unique, then we do
     * nothing.
     *
     * Since we compare each iteration, in the worst case
     * it will take O(K), where K - number of iterations for
     * comparing the objects.
     *
     * All in all, method insert takes O(N*K)
     */
    void insert(T item) override {
        if (numberOfElements == arraySize) {
            T* tempArr = new T[arraySize * 2];

            for (int i = 0; i < numberOfElements; ++i)
                tempArr[i] = arrayList[i];

            delete [] arrayList;
            arraySize *= 2;
            arrayList = tempArr;
        }

        bool ifInSet = false; // boolean variable, which indicates if the element unique
        for (int i = 0; i < numberOfElements; ++i) {
            if (arrayList[i] == item) {
                ifInSet = true;
                break;
            }
        }

        if (!ifInSet) {
            arrayList[numberOfElements] = item;
            numberOfElements++;
        }
    }

    /*
     * Time complexity: O(1) worst case
     * Requires constant time to return the variable.
     */
    int size() override {
        return numberOfElements;
    }
};

template <typename T>
class ListADT {
    virtual void push(T item) = 0; // add a new item to the List
    virtual T get(int index) = 0; // return the i-th element
    virtual void add(int ind, double term) = 0; // add object term to the total price
    virtual void insertInSet(int ind, string ID) = 0; // insert i-th element into the string
    virtual int getSetSizeFromList(int ind) = 0; // return size of the list
    virtual int size() = 0; // return the size of the List
};

template <typename T>
class List : ListADT<T> {
private:
    T* arrayList; // dynamic array which stores objects of type T
    int arraySize; // size of the array
    int numberOfElements; // actual number of elements in the array
public:
    // constructor
    List() {
        arrayList = new T[1];
        arraySize = 1;
        numberOfElements = 0;
    }

    /*
     * Time complexity: O(N) worst case
     *
     * As in task 2.1, doubling takes linear time.
     * Due to that, O(N) in the worst case.
     */
    void push(T item) override {
        if (numberOfElements == arraySize) {
            T* tempArr = new T[arraySize * 2];

            for (int i = 0; i < numberOfElements; ++i)
                tempArr[i] = arrayList[i];

            delete [] arrayList;
            arraySize *= 2;
            arrayList = tempArr;
        }

        arrayList[numberOfElements] = item;
        numberOfElements++;
    }

    /*
     * Time complexity: O(1) worst case
     *
     * Just return element from array.
     */
    T get(int index) override {
        if (index < 0 || index >= numberOfElements) {
            cout << "There are no elements with index = " << index << " in the array." << endl;
            cout << "Returning the first element." << endl;
            return arrayList[0];
        }
        return arrayList[index];
    }

    /*
     * Time complexity: O(1) worst case
     *
     * Summation of two doubles takes constant time.
     */
    void add(int ind, double term) override {
        if (0 < ind || ind >= numberOfElements) {
            cout << "Index = " << ind << " is out of range" << endl;
            return;
        }
        arrayList[ind].first.second += term;
    }

    /*
     * Time complexity: O(N * K) worst case
     * N - actual number of elements in the array.
     * K - number of iterations for comparing the objects
     *
     * *watch insert method in the class Set
     */
    void insertInSet(int ind, string ID) override {
        if (0 < ind || ind >= numberOfElements) {
            cout << "Index = " << ind << " is out of range" << endl;
            return;
        }
        arrayList[ind].second.insert(ID);
    }

    /*
     * Time complexity: O(1) worst case
     *
     *  *watch size method in the class Set
     */
    int getSetSizeFromList(int ind) override {
        if (0 < ind || ind >= numberOfElements) {
            cout << "Index = " << ind << " is out of range" << endl;
            return 0;
        }
        return arrayList[ind].second.size();
    }

    /*
     * Time complexity: O(1) worst case
     *
     * Requires constant time to return the variable.
     */
    int size() override {
        return numberOfElements;
    }
};

template <typename T1, typename T2>
class MapADT {
    virtual void put (T1 key, T2 value, string ID) = 0; // add a new tuple to the HashMap
    virtual T2 getValue (T1 key) = 0; // return the value of the object T1 key of the HashMap
    virtual int getHashID (T1 item) = 0; // hashing and compressing the object
};

template <typename T1, typename T2>
class HashTable : MapADT<T1, T2> {
private:
    /*
     * An array of Lists, since i handle collision using Separate Chaining
     */
    List<pair<pair<T1, T2>, Set<string>>> * table; // We have an array of Lists, since

    int getHashID (T1 key) override {
        size_t hashedKey = hash<T1>()(key); // getting the hash of the object key
        return hashedKey % szP; // compressing the hash value
    }
public:
    // constructor
    HashTable() {
        table = new List<pair<pair<T1, T2>, Set<string>>>  [szP];
    }

    /*
     * Check if a tuple <key, value, Set<T> exists.
     * If yes:
     *    1) use list's method add
     *    2) insert in set string ID
     * If no:
     *    1) adding the tuple into the HashMap
     *    2) insert in set string ID
     */
    void put (T1 key, T2 value, string ID) override {
        int index = getHashID(key);

        bool flag = false;

        for (int i = 0; i < table[index].size(); ++i) {
            if (table[index].get(i).first.first == key) {
                table[index].add(i, value);
                table[index].insertInSet(i, ID);
                flag = true;
                break;
            }
        }
        if (!flag) {
            table[index].push(make_pair(make_pair(key, value), Set<string>()));
            table[index].insertInSet(table[index].size() - 1, ID);
        }
    }

    /*
     * get id of object key using getHashID method
     * Checking, if the tuple with key exists and
     * return the value.
     */
    T2 getValue (T1 key) override {
        int index = getHashID(key);

        for (int i = 0; i < table[index].size(); ++i) {
            if (table[index].get(i).first.first == key) {
                return table[index].get(i).first.second;
            }
        }

        cout << "There are no tuple with object " << key << endl;
        cout << "Method returned 0";
        return 0;
    }

    /*
     * Output all the tuples in the format <key, value, Set<T>.size()>
     * I go through the entire array of lists, looking for all existing tuples
     */
    void print () {
        for (int i = 0; i < szP; ++i) {
            if (table[i].size() != 0) {
                for (int j = 0; j < table[i].size(); ++j) {
                    cout << fixed << setprecision(2);
                    cout << table[i].get(j).first.first << " $" << table[i].get(j).first.second << " " << table[i].getSetSizeFromList(j) << endl;
                }
            }
        }

    }
};

int main() {
    int K; // number of input lines
    cin >> K;

    string date, time, ID, cost, title;
    double doubleCost;

    HashTable<string, double> mp;

    while (K--) {
        cin >> date >> time >> ID >> cost;
        cin.ignore();
        getline(cin, title);

        cost.erase(cost.begin()); // delete dollar sign
        doubleCost = stod(cost); // cast string to double

        mp.put(date, doubleCost, ID);
    }

    mp.print(); // output the answer
    return 0;
}