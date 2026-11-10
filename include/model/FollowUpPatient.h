#ifndef FOLLOWUPPATIENT_H
#define FOLLOWUPPATIENT_H

#include "PatientRecord.h"
#include "Appointable.h"

namespace Model {
    class FollowUpPatient : public PatientRecord, public Appointable {
    private:
        std::string previousDoctor; // Specific attribute for follow-ups[cite: 159]

    public:
        FollowUpPatient(std::string id, std::string n, int a, std::string contact, std::string doctor);

        std::string getPatientCategory() const override;
        std::string getPriorityLevel() const override;
        std::string getPreviousDoctor() const { return previousDoctor; }

        bool scheduleAppointment(const std::string& date, const std::string& time, const std::string& reason) override;
        bool cancelAppointment(const std::string& appointmentId) override;
    };
}

#endif