#include <iostream>
#include <fstream>
#include <vector>
#include <omp.h>
#include <chrono>

using namespace std;
using namespace chrono;

// Recursive quick sort function that sorts elements in a specified range of an array
void quickSort(vector<int>& arr, int left, int right, int depth = 0) {
    // Base case: if the array has one or no elements, it's already sorted
    if (left >= right) return;

    // Pivot selection: middle element
    int pivot = arr[(left + right) / 2];
    int i = left, j = right;

    // Partitioning the array
    while (i <= j) {
        // Move left pointer until an element greater than pivot is found
        while (arr[i] < pivot) i++;
        // Move right pointer until an element less than pivot is found
        while (arr[j] > pivot) j--;
        // Swap elements and move pointers
        if (i <= j) {
            swap(arr[i], arr[j]);
            i++;
            j--;
        }
    }

    // Parallel recursion using OpenMP tasks
    if (depth < omp_get_max_threads()) {
        // Create a new OpenMP task for the left partition
        #pragma omp task
        quickSort(arr, left, j, depth + 1);

        // Create a new OpenMP task for the right partition
        #pragma omp task
        quickSort(arr, i, right, depth + 1);

        // Wait for all tasks created in this scope to complete
        #pragma omp taskwait
    } else {
        // If maximum depth reached, perform the sorting sequentially
        quickSort(arr, left, j, depth + 1);
        quickSort(arr, i, right, depth + 1);
    }
}

int main(int argc, char* argv[]) {
    // Check for correct number of arguments
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <number of threads> <input file name>\n";
        return 1;
    }

    // Extract number of threads from arguments
    int numThreads = atoi(argv[1]);
    // Extract input file name from arguments
    string inputFile = argv[2];

    // Open input file
    ifstream inFile(inputFile);
    if (!inFile) {
        cerr << "Error: Unable to open input file: " << inputFile << endl;
        return 1;
    }

    vector<int> data;
    int value;
    // Read input file data into a vector
    while (inFile >> value) {
        data.push_back(value);
    }
    inFile.close();

    // Measure the start time
    auto start = high_resolution_clock::now();

    // Parallel region with OpenMP
    #pragma omp parallel num_threads(numThreads)
    {
        // Execute quick sort within a single thread context, without waiting for other threads
        #pragma omp single nowait
        quickSort(data, 0, data.size() - 1);
    }

    // Measure the end time
    auto stop = high_resolution_clock::now();
    // Calculate the duration
    auto duration = duration_cast<milliseconds>(stop - start);
    // Output the execution time
    cout << "Time taken: " << duration.count() << " milliseconds" << endl;

    // Open output file
    of
