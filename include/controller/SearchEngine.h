#ifndef SEARCHENGINE_H
#define SEARCHENGINE_H

#include <vector>
#include <memory>
#include <string>
#include "model/PatientRecord.h"

namespace Controller {
    class SearchEngine {
    public:
        // 4a. Linear Search by Patient ID
        static int linearSearchById(const std::vector<std::shared_ptr<Model::PatientRecord>>& patients, const std::string& targetId);

        // 4b. Binary Search (assumes the vector is already sorted by ID)[cite: 163]
        static int binarySearchById(const std::vector<std::shared_ptr<Model::PatientRecord>>& sortedPatients, const std::string& targetId);

        // 4c. Recursive Search by Patient name[cite: 163]
        static int recursiveSearchByName(const std::vector<std::shared_ptr<Model::PatientRecord>>& patients, const std::string& targetName, int currentIndex = 0);
    };
}

#endif