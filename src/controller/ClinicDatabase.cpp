#include "controller/ClinicDatabase.h"
#include <stdexcept>

namespace Controller {
    void ClinicDatabase::addPatient(std::shared_ptr<Model::PatientRecord> patient) {
        // Check for duplicate Patient ID and throw Custom Exception
        for (const auto& existing : patientRegistry) {
            if (existing->getId() == patient->getId()) {
                throw DuplicatePatientException(patient->getId());
            }
        }

        patientRegistry.push_back(patient);
        
        // Save operation to stack for undo capability
        undoStack.push({AdminActionType::ADD_PATIENT, patient, static_cast<int>(patientRegistry.size() - 1), "", 0, ""});
        
        updateCache(patient);
    }

    bool ClinicDatabase::editPatient(const std::string& id, const std::string& newName, int newAge, const std::string& newContact, bool recordUndo) {
        for (size_t i = 0; i < patientRegistry.size(); ++i) {
            if (patientRegistry[i]->getId() == id) {
                if (recordUndo) {
                    undoStack.push({
                        AdminActionType::MODIFY_PATIENT,
                        patientRegistry[i],
                        static_cast<int>(i),
                        patientRegistry[i]->getName(),
                        patientRegistry[i]->getAge(),
                        patientRegistry[i]->getContactInfo()
                    });
                }
                patientRegistry[i]->setName(newName);
                patientRegistry[i]->setAge(newAge);
                patientRegistry[i]->setContactInfo(newContact);
                updateCache(patientRegistry[i]);
                return true;
            }
        }
        return false;
    }

    bool ClinicDatabase::deletePatient(const std::string& id, bool recordUndo) {
        for (auto it = patientRegistry.begin(); it != patientRegistry.end(); ++it) {
            if ((*it)->getId() == id) {
                if (recordUndo) {
                    int index = std::distance(patientRegistry.begin(), it);
                    undoStack.push({AdminActionType::DELETE_PATIENT, *it, index, "", 0, ""});
                }
                
                patientRegistry.erase(it);
                return true;
            }
        }
        return false;
    }

    void ClinicDatabase::undoLastOperation() {
        if (undoStack.empty()) {
            throw std::runtime_error("No administrative operations left to undo.");
        }

        AdminAction lastAction = undoStack.top();
        undoStack.pop();

        if (lastAction.type == AdminActionType::ADD_PATIENT) {
            deletePatient(lastAction.patientSnapshot->getId(), false);
        } 
        else if (lastAction.type == AdminActionType::DELETE_PATIENT) {
            if (lastAction.originalIndex >= 0 && lastAction.originalIndex <= static_cast<int>(patientRegistry.size())) {
                patientRegistry.insert(patientRegistry.begin() + lastAction.originalIndex, lastAction.patientSnapshot);
            } else {
                patientRegistry.push_back(lastAction.patientSnapshot);
            }
        }
        else if (lastAction.type == AdminActionType::MODIFY_PATIENT) {
            editPatient(lastAction.patientSnapshot->getId(), lastAction.prevName, lastAction.prevAge, lastAction.prevContact, false);
        }
    }

    void ClinicDatabase::clearAll() {
        patientRegistry.clear();
        while (!waitingQueue.empty()) waitingQueue.pop();
        while (!undoStack.empty()) undoStack.pop();
        for (size_t i = 0; i < CACHE_SIZE; ++i) recentCache[i] = nullptr;
        cacheCount = 0;
    }

    void ClinicDatabase::enqueuePatient(const std::string& id) {
        waitingQueue.push(id);
    }

    std::string ClinicDatabase::dequeueNextPatient() {
        if (waitingQueue.empty()) return "";
        std::string nextPatient = waitingQueue.front();
        waitingQueue.pop();
        return nextPatient;
    }

    void ClinicDatabase::updateCache(std::shared_ptr<Model::PatientRecord> patient) {
        for (size_t i = CACHE_SIZE - 1; i > 0; --i) {
            recentCache[i] = recentCache[i - 1];
        }
        recentCache[0] = patient;
        if (cacheCount < CACHE_SIZE) cacheCount++;
    }

    std::vector<std::shared_ptr<Model::PatientRecord>> ClinicDatabase::getRecentCacheList() const {
        std::vector<std::shared_ptr<Model::PatientRecord>> list;
        for (size_t i = 0; i < cacheCount; ++i) {
            if (recentCache[i] != nullptr) {
                list.push_back(recentCache[i]);
            }
        }
        return list;
    }
}