#include <iostream>
#include <pthread.h>
#include <cstdlib>

using namespace std;

/**
 * Global configurations
 */
const int MAX_THREADS = 10000;
const int DEFAULT_ITERATIONS = 3;

long long int counter = 0;
int iterations = DEFAULT_ITERATIONS;
pthread_t threads[MAX_THREADS];

/**
 * Ackermann function - A classic example of a highly recursive function.
 * Used here to simulate heavy computational load.
 */
int ackermann(int m, int n) {
    if (m == 0) {
        return n + 1;
    } else if (m > 0 && n == 0) {
        return ackermann(m - 1, 1);
    } else if (m > 0 && n > 0) {
        return ackermann(m - 1, ackermann(m, n - 1));
    }
    return 0; // Should not reach here
}

/**
 * Wrapper function for the computation task.
 * This function is called either sequentially or via threads.
 */
void* computeTask(void*) {
    int m = 4, n = 1;
    int result = ackermann(m, n);
    cout << "Ackermann(" << m << ", " << n << ") = " << result << endl;
    return NULL;
}

void printUsage(const char* programName) {
    cout << "Usage: " << programName << " <mode>" << endl;
    cout << "Modes:" << endl;
    cout << "  1: Typical Program Execution (Sequential)" << endl;
    cout << "  2: Threaded Program Execution (Parallel)" << endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    int mode = atoi(argv[1]);

    if (mode == 1) {
        cout << "\nStarting Typical (Sequential) Program Execution Speed Test..." << endl;
        for (int i = 0; i < iterations; i++) {
            computeTask(NULL);
        }
    } else if (mode == 2) {
        cout << "\nStarting Threaded (Parallel) Program Execution Speed Test..." << endl;
        for (int i = 0; i < iterations; i++) {
            if (pthread_create(&threads[i], NULL, computeTask, NULL) != 0) {
                cerr << "Error creating thread " << i << endl;
                return 1;
            }
        }

        for (int i = 0; i < iterations; i++) {
            pthread_join(threads[i], NULL);
        }
    } else {
        cerr << "\nError: Incorrect mode selected." << endl;
        printUsage(argv[0]);
        return 1;
    }

    return 0;
}
