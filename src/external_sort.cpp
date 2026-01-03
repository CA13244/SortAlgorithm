#include "external_sort.h"
#include "data_generator.h"
#include "file_utils.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <queue>
#include <vector>
#include <memory>
#include <sys/stat.h>
#include <cstdlib>
#include <ctime>

#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

using namespace std;

// initialize static member variable
size_t ExternalSort::memoryLimit = 100 * 1024 * 1024; // default 100MB

// create temporary directory
bool createTempDirectory(const string& dir) {
#ifdef _WIN32
    return _mkdir(dir.c_str()) == 0;
#else
    return mkdir(dir.c_str(), 0777) == 0;
#endif
}

// read a chunk of data
template<typename T>
vector<T> ExternalSort::readChunk(const string& filename, size_t chunkSize) {
    ifstream inFile(filename, ios::binary);
    if (!inFile) {
        throw runtime_error("Cannot open file: " + filename);
    }

    // move to end to get file size
    inFile.seekg(0, ios::end);
    size_t fileSize = inFile.tellg();
    inFile.seekg(0, ios::beg);

    // calculate number of elements to read
    size_t elementSize = sizeof(T);
    size_t maxElements = chunkSize / elementSize;
    size_t elementsToRead = min(maxElements, fileSize / elementSize);

    vector<T> data(elementsToRead);
    inFile.read(reinterpret_cast<char*>(data.data()), elementsToRead * elementSize);
    inFile.close();

    return data;
}

// write a chunk of data
template<typename T>
void ExternalSort::writeChunk(const string& filename, const vector<T>& data) {
    ofstream outFile(filename, ios::binary);
    if (!outFile) {
        throw runtime_error("Cannot open file: " + filename);
    }

    outFile.write(reinterpret_cast<const char*>(data.data()),
                 data.size() * sizeof(T));
    outFile.close();
}

// create initial sorted runs
template<typename T>
int ExternalSort::createInitialRuns(const string& inputFile,
                                  const string& tempDir,
                                  bool (*compare)(const T&, const T&)) {

    ifstream inFile(inputFile, ios::binary | ios::ate);
    if (!inFile) {
        throw runtime_error("Cannot open input file: " + inputFile);
    }

    // get file size
    size_t fileSize = inFile.tellg();
    inFile.seekg(0, ios::beg);

    // calculate number of elements per run
    size_t elementSize = sizeof(T);
    size_t elementsPerRun = memoryLimit / elementSize;

    vector<T> buffer(elementsPerRun);
    int runCount = 0;

    while (inFile) {
        // read a batch of data
        inFile.read(reinterpret_cast<char*>(buffer.data()),
                   elementsPerRun * elementSize);
        size_t bytesRead = inFile.gcount();
        size_t elementsRead = bytesRead / elementSize;

        if (elementsRead == 0) break;

        // resize buffer
        buffer.resize(elementsRead);

        // sort this batch
        if (compare) {
            sort(buffer.begin(), buffer.end(), compare);
        } else {
            sort(buffer.begin(), buffer.end());
        }

        // write to temporary file
        string runFile = tempDir + "/run_" + to_string(runCount++) + ".dat";
        writeChunk<T>(runFile, buffer);

        cout << "Created run " << runCount << " (" << elementsRead << " elements)" << endl;
    }

    inFile.close();
    return runCount;
}

// multi-way merge
template<typename T>
void ExternalSort::mergeRuns(const vector<string>& runFiles,
                           const string& outputFile,
                           bool (*compare)(const T&, const T&)) {

    int numRuns = runFiles.size();
    if (numRuns == 0) return;

    // open all run files
    vector<ifstream> runStreams(numRuns);
    vector<bool> hasMore(numRuns, true);
    vector<T> currentValues(numRuns);

    for (int i = 0; i < numRuns; i++) {
        runStreams[i].open(runFiles[i], ios::binary);
        if (!runStreams[i]) {
            throw runtime_error("Cannot open run file: " + runFiles[i]);
        }

        // read first value from each run
        if (runStreams[i].read(reinterpret_cast<char*>(&currentValues[i]), sizeof(T))) {
            hasMore[i] = true;
        } else {
            hasMore[i] = false;
        }
    }

    // use min-heap for multi-way merge
    using Element = pair<T, int>;
    auto heapCompare = [compare](const Element& a, const Element& b) {
        if (compare) {
            return compare(b.first, a.first); // min-heap
        } else {
            return b.first < a.first;
        }
    };

    priority_queue<Element, vector<Element>, decltype(heapCompare)> minHeap(heapCompare);

    for (int i = 0; i < numRuns; i++) {
        if (hasMore[i]) {
            minHeap.push({currentValues[i], i});
        }
    }

    // open output file
    ofstream outFileStream(outputFile, ios::binary);
    if (!outFileStream) {
        throw runtime_error("Cannot open output file: " + outputFile);
    }

    int outputCount = 0;
    const int BATCH_SIZE = 100000;
    vector<T> outputBuffer;
    outputBuffer.reserve(BATCH_SIZE);

    while (!minHeap.empty()) {
        auto [value, runIndex] = minHeap.top();
        minHeap.pop();

        // add to output buffer
        outputBuffer.push_back(value);
        outputCount++;

        // if buffer is full, write to file
        if (outputBuffer.size() >= BATCH_SIZE) {
            outFileStream.write(reinterpret_cast<const char*>(outputBuffer.data()),
                              outputBuffer.size() * sizeof(T));
            outputBuffer.clear();
        }

        // read next value from the same run
        if (runStreams[runIndex].read(reinterpret_cast<char*>(&currentValues[runIndex]), sizeof(T))) {
            minHeap.push({currentValues[runIndex], runIndex});
        } else {
            hasMore[runIndex] = false;
        }
    }

    // write remaining buffer contents
    if (!outputBuffer.empty()) {
        outFileStream.write(reinterpret_cast<const char*>(outputBuffer.data()),
                          outputBuffer.size() * sizeof(T));
    }

    // close all files
    for (auto& stream : runStreams) {
        stream.close();
    }
    outFileStream.close();

    cout << "Merge completed, total " << outputCount << " elements output" << endl;
}

// sort integer file
void ExternalSort::sortIntegerFile(const string& inputFile, const string& outputFile) {
    cout << "Starting integer external sort: " << inputFile << " -> " << outputFile << endl;

    // create temporary directory
    string tempDir = "temp_external_int_" + to_string(time(nullptr));
    if (!createTempDirectory(tempDir)) {
        throw runtime_error("Cannot create temporary directory: " + tempDir);
    }

    vector<string> runFiles;
    int runCount = 0;

    try {
        // Phase 1: create initial runs
        cout << "Phase 1: Creating initial runs..." << endl;
        runCount = createInitialRuns<int64_t>(inputFile, tempDir, compareInt);

        // collect run file list
        for (int i = 0; i < runCount; i++) {
            runFiles.push_back(tempDir + "/run_" + to_string(i) + ".dat");
        }

        // Phase 2: multi-way merge
        cout << "Phase 2: Multi-way merge (" << runCount << " runs)..." << endl;
        mergeRuns<int64_t>(runFiles, outputFile, compareInt);

        cout << "Integer external sort completed: " << outputFile << endl;

    } catch (const exception& e) {
        // cleanup temporary files
        for (const auto& file : runFiles) {
            remove(file.c_str());
        }
        rmdir(tempDir.c_str());
        throw;
    }

    // cleanup temporary files
    for (int i = 0; i < runCount; i++) {
        string runFile = tempDir + "/run_" + to_string(i) + ".dat";
        remove(runFile.c_str());
    }
    rmdir(tempDir.c_str());
}

// sort double file
void ExternalSort::sortDoubleFile(const string& inputFile, const string& outputFile) {
    cout << "Starting double external sort: " << inputFile << " -> " << outputFile << endl;

    // create temporary directory
    string tempDir = "temp_external_double_" + to_string(time(nullptr));
    if (!createTempDirectory(tempDir)) {
        throw runtime_error("Cannot create temporary directory: " + tempDir);
    }

    vector<string> runFiles;
    int runCount = 0;

    try {
        // Phase 1: create initial runs
        cout << "Phase 1: Creating initial runs..." << endl;
        runCount = createInitialRuns<double>(inputFile, tempDir, compareDouble);

        // collect run file list
        for (int i = 0; i < runCount; i++) {
            runFiles.push_back(tempDir + "/run_" + to_string(i) + ".dat");
        }

        // Phase 2: multi-way merge
        cout << "Phase 2: Multi-way merge (" << runCount << " runs)..." << endl;
        mergeRuns<double>(runFiles, outputFile, compareDouble);

        cout << "Double external sort completed: " << outputFile << endl;

    } catch (const exception& e) {
        // cleanup temporary files
        for (const auto& file : runFiles) {
            remove(file.c_str());
        }
        rmdir(tempDir.c_str());
        throw;
    }

    // cleanup temporary files
    for (int i = 0; i < runCount; i++) {
        string runFile = tempDir + "/run_" + to_string(i) + ".dat";
        remove(runFile.c_str());
    }
    rmdir(tempDir.c_str());
}

// sort string file
void ExternalSort::sortStringFile(const string& inputFile, const string& outputFile) {
    cout << "Starting string external sort: " << inputFile << " -> " << outputFile << endl;

    // For strings, we use a simplified approach: read into memory and sort
    // For large data, more complex processing would be needed
    try {
        // read data
        auto data = DataGenerator::readStringData(inputFile);

        // sort
        sort(data.begin(), data.end(), compareString);

        // write to output file
        ofstream outFile(outputFile);
        if (!outFile) {
            throw runtime_error("Cannot open output file: " + outputFile);
        }

        for (const auto& str : data) {
            outFile << str << "\n";
        }
        outFile.close();

        cout << "String external sort completed: " << outputFile << endl;

    } catch (const exception& e) {
        cerr << "String external sort failed: " << e.what() << endl;
        throw;
    }
}
