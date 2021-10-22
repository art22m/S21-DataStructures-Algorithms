//
// Created by Artem Murashko B20-02
// 2.1 Sorted List ADT
//

#include <iostream>
#include <string>

using namespace std;

template <typename T>
class ListADT {
    virtual void push(T item) = 0; // add a new item to the List
    virtual T get(int index) = 0; // return the i-th element
    virtual int size() = 0; // return the size of the List
    virtual void print() = 0; // output all items of the List to console
};

template <typename T>
class List : ListADT<T> {
private:
    T* arr;
    int arraySize;
    int numberOfElements;
public:
    List() {
        arr = new T[1];
        arraySize = 1;
        numberOfElements = 0;
    }
    // Time complexity O(N) worst case
    void push(T item) {
        if (numberOfElements == arraySize) {
            T* tempArr = new T[arraySize * 2];

            for (int i = 0; i < numberOfElements; ++i)
                tempArr[i] = arr[i];

            delete [] arr;
            arraySize *= 2;
            arr = tempArr;
        }

        arr[numberOfElements] = item;
        numberOfElements++;
    }

    // Time complexity O(1) worst case
    T get(int index) {
        if (0 < index && index >= numberOfElements) {
            cout << "There are no elements with index = " << index << " in the array." << endl;
            cout << "Returning the first element." << endl;
            return arr[0];
        }
        return arr[index];
    }

    // Time complexity O(1) worst case
    int size() {
        return numberOfElements;
    }

    // Time complexity O(N) worst case
    void print() {
        for (int i = 0; i < numberOfElements; ++i)
            cout << arr[i] << " ";
        cout << endl;
    }
};

template <typename T>
class SortedList {
public:
    virtual void add(T item) = 0; // add a new item to the Sorted List
    virtual T least() = 0; // return the least element
    virtual T greatest() = 0; // return the greatest element
    virtual T get(int i) = 0; // return the i-th least element
    virtual int indexOf(T item) = 0; // return the index of an element (in a sorted sequence)
    virtual void remove(int i) = 0; // remove i-th least element from the Sorted List
    virtual List<T> searchRange(T from, T to) = 0; // find all items between from and to
    virtual int size() = 0; // return the size of the Sorted List
    virtual bool isEmpty() = 0; // return whether the Sorted List is empty
};

template <typename T>
class ArraySortedList : SortedList <T> {
private:
    T* array; // dynamic array, where we will store our data
    int arraySize; // size of the array
    int numberOfElements; // actual number of elements in the array
public:
    // Constructor
    ArraySortedList() {
        array = new T[1];
        arraySize = 1;
        numberOfElements = 0;
    }

    /*
    * Time complexity: O(N * K) worst case
    * N - actual number of elements in the array.
    * K - number of iterations for comparing the objects
    *
    * 1) If we won't resize the array:
    *   We will go through all the elements until we find an element larger than the item,
    *   then we swap elements from the end, till found this element. All in all, it takes linear
    *   time.
    *   Also, if we have type T = string, every comparison will take K iterations in the worst case,
    *   where K is the length of the strings: O(2K) = O(K).
    *   In the worst case, if we will compare all strings, i.e.
    *   go in the loop till the end, time complexity will be O(N * K)
    *
    *   Explanation: Assume we have all string with the same length K. Thus, to compare the
    *   whole string we need to compare each char, which will take K iterations.
    *   (abcd v abce : a = a, b = b, c = c, but d < e => abcd < abce)
    *
    * 2) If we wll resize the array:
    *   (1) create new array B with doubled size of A
    *   (2) store elements of A in B
    *   (3) delete A
    *   (4) reassign array B into A
    *   It will take linear time.
    *   After that do the steps of 1) (linear time)
    *   So, the time complexity is O(N + N) = O(N) worst case
    */
    void add(T item) override {
        // Resize part
        if (numberOfElements == arraySize) {
            T* tempArr = new T[arraySize * 2];

            for (int id = 0; id < arraySize; ++id)
                tempArr[id] = array[id];

            delete [] array;
            arraySize *= 2;
            array = tempArr;
        }

        // Add part
        if (numberOfElements == 0) {
            array[numberOfElements] = item;
        } else {
            bool added = false;
            for (int id = 0; id < numberOfElements; ++id) {
                if (item <= array[id]) {
                    added = true;
                    for (int p_id = numberOfElements; p_id > id; --p_id)
                        swap(array[p_id - 1], array[p_id]);
                    array[id] = item;
                    break;
                }
            }
            if (!added)
                array[numberOfElements] = item;
        }
        numberOfElements++;
    }

    /*
     * Time complexity: O(1) worst case
     * Since we have a Sorted List, the minimum element is
     * the first element of the array. Accessing the array takes constant time.
     */
    T least() override {
        if (numberOfElements == 0) {
            cout << "There are no elements in the array." << endl;
            return array[numberOfElements];
        } else
            return array[0];
    }

    /*
     * Time complexity: O(1) worst case
     * Since we have a Sorted List, the maximum element is
     * the last actual element of the array. Accessing the array takes constant time.
     */
    T greatest() override {
        if (numberOfElements == 0) {
            cout << "There are no elements in the array." << endl;
            return array[numberOfElements];
        } else
            return array[numberOfElements - 1];
    }

    /*
     * Time complexity: O(1) worst case
     * Since we know the id of the element and accessing the array takes constant time.
     */
    T get(int i) override {
        if (i < 0 || i > numberOfElements) {
            cout << "There are no " << i << "th element in the array." << endl;
            cout << "Returning the least element." << endl;
            return array[0];
        }
        return array[i - 1];
    }

    /*
     * Time complexity: O(N * K) worst case
     * N - actual number of elements in the array.
     * K - number of iterations for comparing the objects.
     *
     * In the worst case, if item will be equal to the maximum element or it does not exist
     * we will go through all the elements of the array, which will take linear time O(N).
     *
     *
     * Also, if we have type T = string, every comparison will take K iterations in the worst case,
     * where K is the length of the strings O(K).
     *
     * Explanation: Assume we have all string with the same length K. Thus, to compare the
     * whole string we need to compare each char, which will take K iterations.
     * ("abcd" v "abce" : a = a, b = b, c = c, but d < e => abcd < abce)
     */
    int indexOf(T item) override {
        int index = -1;

        for (int id = 0; id < numberOfElements; ++id) {
            if (array[id] == item) {
                index = id;
                break;
            }
        }

        if (index == -1) {
            cout << item << " is not in the ArraySortedList." << endl;
            cout << "Returning id of the least element." << endl;
            return 0;
        }
        return index;
    }

    /*
     * Time complexity: O(N) worst case
     * N - actual number of elements in the array.
     *
     * In the worst case we need to remove first element of the array.
     * Due this reason, we need to swap all elements till the end.
     * Swap method takes constant time. Thus, it will take linear time.
     */
    void remove(int i) override {
        if (i <= 0 || i > numberOfElements) {
            cout << "There are no " << i << "th element in the array." << endl;
            return;
        }

        for (int id = i - 1; id < numberOfElements - 1; ++id)
            swap(array[id], array[id + 1]);

        numberOfElements--;
    }

    /*
     * Time complexity: O(N * K) worst case
     * N - actual number of elements in the array.
     * K - number of iterations for comparing the objects.
     *
     * In all cases we go throw all elements of array.
     * In the worst case, we should add into the List all the elements
     * of array, i.e. execute push() method N times. Worst case of push
     * is O(N) (double strategy), but array won't double every added element, due
     * to that amortized cost of push is O(1). So, the time complexity of the
     * searchRange method is linear.
     *
     * Also, if we have type T = string, every comparison will take K iterations in the worst case,
     * where K is the length of the strings: O(2K) = O(K).
     *
     * Explanation: Assume we have all string with the same length K. Thus, to compare the
     * whole string we need to compare each char, which will take K iterations.
     * (abcd v abce : a = a, b = b, c = c, but d < e => abcd < abce)
     */
    List<T> searchRange(T from, T to) override {
        List<T> lst;

        for (int id = 0; id < numberOfElements; ++id) {
            if (from <= array[id] && array[id] <= to) {
                lst.push(array[id]);
            }
        }

        return lst;
    }

    /*
     * Time complexity: O(1) worst case
     * Just return value of variable. Constant time.
     */
    int size() override {
        return numberOfElements;
    }

    /*
     * Time complexity: O(1) worst case
     * Just return true/false, by simple comparison. Constant time.
     */
    bool isEmpty() override {
        return numberOfElements == 0;
    }
};

int main() {

    return 0;
}