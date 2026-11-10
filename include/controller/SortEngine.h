#ifndef SORTENGINE_H
#define SORTENGINE_H

#include <vector>
#include <memory>
#include "model/PatientRecord.h"

namespace Controller {
    // Defines what field we are sorting by[cite: 163]
    enum class SortCriteria { BY_NAME, BY_ID, BY_AGE };

    class SortEngine {
    private:
        // Helper to compare two records based on the selected criteria
        static bool compare(const std::shared_ptr<Model::PatientRecord>& a, 
                            const std::shared_ptr<Model::PatientRecord>& b, 
                            SortCriteria criteria);

        // Helpers for the Quick Sort algorithm
        static int partition(std::vector<std::shared_ptr<Model::PatientRecord>>& patients, int low, int high, SortCriteria criteria);
        static void quickSortRecursive(std::vector<std::shared_ptr<Model::PatientRecord>>& patients, int low, int high, SortCriteria criteria);

    public:
        // Algorithm 1: Selection Sort[cite: 163]
        static void selectionSort(std::vector<std::shared_ptr<Model::PatientRecord>>& patients, SortCriteria criteria);

        // Algorithm 2: Quick Sort[cite: 163]
        static void quickSort(std::vector<std::shared_ptr<Model::PatientRecord>>& patients, SortCriteria criteria);
    };
}

#endif