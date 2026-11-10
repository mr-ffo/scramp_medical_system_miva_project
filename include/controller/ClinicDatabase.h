#ifndef CLINICDATABASE_H
#define CLINICDATABASE_H

#include <vector>
#include <queue>
#include <stack>
#include <memory>
#include <string>
#include <exception>
#include "model/PatientRecord.h"

namespace Controller {

    // Custom Exception Class for Duplicate Patient IDs (Section 10)
    class DuplicatePatientException : public std::exception {
    private:
        std::string message;
    public:
        explicit DuplicatePatientException(const std::string& id)
            : message("Error: Patient ID '" + id + "' already exists in the system.") {}
        const char* what() const noexcept override {
            return message.c_str();
        }
    };

    enum class AdminActionType { ADD_PATIENT, DELETE_PATIENT, MODIFY_PATIENT };

    struct AdminAction {
        AdminActionType type;
        std::shared_ptr<Model::PatientRecord> patientSnapshot;
        int originalIndex;
        std::string prevName;
        int prevAge = 0;
        std::string prevContact;
    };

    class ClinicDatabase {
    private:
        std::vector<std::shared_ptr<Model::PatientRecord>> patientRegistry;
        std::queue<std::string> waitingQueue; 
        std::stack<AdminAction> undoStack;

        static constexpr size_t CACHE_SIZE = 10;
        std::shared_ptr<Model::PatientRecord> recentCache[CACHE_SIZE];
        size_t cacheCount = 0;

    public:
        ClinicDatabase() = default;
        
        void addPatient(std::shared_ptr<Model::PatientRecord> patient);
        bool editPatient(const std::string& id, const std::string& newName, int newAge, const std::string& newContact, bool recordUndo = true);
        bool deletePatient(const std::string& id, bool recordUndo = true);
        void undoLastOperation();
        void clearAll();

        void enqueuePatient(const std::string& id);
        std::string dequeueNextPatient();
        size_t getWaitingQueueSize() const { return waitingQueue.size(); }

        void updateCache(std::shared_ptr<Model::PatientRecord> patient);
        std::vector<std::shared_ptr<Model::PatientRecord>> getRecentCacheList() const;
        
        std::vector<std::shared_ptr<Model::PatientRecord>>& getAllPatients() { return patientRegistry; }
    };
}

#endif