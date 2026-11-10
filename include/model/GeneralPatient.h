#ifndef GENERALPATIENT_H
#define GENERALPATIENT_H

#include "PatientRecord.h"
#include "Appointable.h"

namespace Model {
    // Inherits from both the abstract base class and the interface[cite: 158, 159]
    class GeneralPatient : public PatientRecord, public Appointable {
    public:
        GeneralPatient(std::string id, std::string n, int a, std::string contact);

        // Polymorphic methods from PatientRecord[cite: 159]
        std::string getPatientCategory() const override;
        std::string getPriorityLevel() const override;

        // Interface implementation from Appointable[cite: 159]
        bool scheduleAppointment(const std::string& date, const std::string& time, const std::string& reason) override;
        bool cancelAppointment(const std::string& appointmentId) override;
    };
}

#endif