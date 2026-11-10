#ifndef EMERGENCYPATIENT_H
#define EMERGENCYPATIENT_H

#include "PatientRecord.h"

namespace Model {
    class EmergencyPatient : public PatientRecord {
    private:
        int severityLevel; // Specific attribute for emergencies[cite: 159]

    public:
        EmergencyPatient(std::string id, std::string n, int a, std::string contact, int severity);

        std::string getPatientCategory() const override;
        std::string getPriorityLevel() const override;
        
        int getSeverityLevel() const { return severityLevel; }
    };
}

#endif