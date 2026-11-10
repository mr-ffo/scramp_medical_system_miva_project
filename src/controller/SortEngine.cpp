#include "controller/SortEngine.h"

namespace Controller {

    bool SortEngine::compare(const std::shared_ptr<Model::PatientRecord>& a, 
                             const std::shared_ptr<Model::PatientRecord>& b, 
                             SortCriteria criteria) {
        switch (criteria) {
            case SortCriteria::BY_NAME: return a->getName() < b->getName();
            case SortCriteria::BY_ID:   return a->getId() < b->getId();
            case SortCriteria::BY_AGE:  return a->getAge() < b->getAge();
            default:                    return a->getName() < b->getName();
        }
    }

    // Manual Implementation of Selection Sort (O(n^2))[cite: 163, 174]
    void SortEngine::selectionSort(std::vector<std::shared_ptr<Model::PatientRecord>>& patients, SortCriteria criteria) {
        int n = static_cast<int>(patients.size());
        for (int i = 0; i < n - 1; i++) {
            int minIndex = i;
            for (int j = i + 1; j < n; j++) {
                if (compare(patients[j], patients[minIndex], criteria)) {
                    minIndex = j;
                }
            }
            // Swap without using built-in library sorting
            if (minIndex != i) {
                auto temp = patients[i];
                patients[i] = patients[minIndex];
                patients[minIndex] = temp;
            }
        }
    }

    // Manual Implementation of Quick Sort (Average O(n log n))[cite: 163, 174]
    int SortEngine::partition(std::vector<std::shared_ptr<Model::PatientRecord>>& patients, int low, int high, SortCriteria criteria) {
        auto pivot = patients[high];
        int i = (low - 1);

        for (int j = low; j <= high - 1; j++) {
            if (compare(patients[j], pivot, criteria)) {
                i++;
                auto temp = patients[i];
                patients[i] = patients[j];
                patients[j] = temp;
            }
        }
        auto temp = patients[i + 1];
        patients[i + 1] = patients[high];
        patients[high] = temp;
        return (i + 1);
    }

    void SortEngine::quickSortRecursive(std::vector<std::shared_ptr<Model::PatientRecord>>& patients, int low, int high, SortCriteria criteria) {
        if (low < high) {
            int pi = partition(patients, low, high, criteria);
            quickSortRecursive(patients, low, pi - 1, criteria);
            quickSortRecursive(patients, pi + 1, high, criteria);
        }
    }

    void SortEngine::quickSort(std::vector<std::shared_ptr<Model::PatientRecord>>& patients, SortCriteria criteria) {
        if (!patients.empty()) {
            quickSortRecursive(patients, 0, static_cast<int>(patients.size()) - 1, criteria);
        }
    }
}