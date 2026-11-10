#include "model/EmergencyPatient.h"

namespace Model {
    EmergencyPatient::EmergencyPatient(std::string id, std::string n, int a, std::string contact, int severity)
        : PatientRecord(id, n, a, contact), severityLevel(severity) {}

    std::string EmergencyPatient::getPatientCategory() const { return "Emergency"; }
    std::string EmergencyPatient::getPriorityLevel() const { return "High Priority"; } // Visually distinguished priority[cite: 167]
}