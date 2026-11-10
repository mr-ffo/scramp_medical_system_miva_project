#include "controller/SearchEngine.h"

namespace Controller {

    // Iterates through the list one by one until the target ID is found[cite: 163]
    int SearchEngine::linearSearchById(const std::vector<std::shared_ptr<Model::PatientRecord>>& patients, const std::string& targetId) {
        for (size_t i = 0; i < patients.size(); ++i) {
            if (patients[i]->getId() == targetId) {
                return static_cast<int>(i);
            }
        }
        return -1; // Not found
    }

    // Divides the search space in half repeatedly (requires sorted data)[cite: 163]
    int SearchEngine::binarySearchById(const std::vector<std::shared_ptr<Model::PatientRecord>>& sortedPatients, const std::string& targetId) {
        int left = 0;
        int right = static_cast<int>(sortedPatients.size()) - 1;

        while (left <= right) {
            int mid = left + (right - left) / 2;
            
            if (sortedPatients[mid]->getId() == targetId) {
                return mid;
            }
            
            if (sortedPatients[mid]->getId() < targetId) {
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }
        return -1; // Not found
    }

    // Calls itself incrementally until the target name matches or the list ends[cite: 163]
    int SearchEngine::recursiveSearchByName(const std::vector<std::shared_ptr<Model::PatientRecord>>& patients, const std::string& targetName, int currentIndex) {
        // Base Case 1: Reached the end of the vector without finding a match
        if (currentIndex >= static_cast<int>(patients.size())) {
            return -1; 
        }
        
        // Base Case 2: Found a match
        if (patients[currentIndex]->getName() == targetName) {
            return currentIndex;
        }
        
        // Recursive Step: Move to the next index
        return recursiveSearchByName(patients, targetName, currentIndex + 1);
    }
}