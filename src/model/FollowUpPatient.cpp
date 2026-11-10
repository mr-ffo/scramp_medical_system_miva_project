#include "model/FollowUpPatient.h"
#include <algorithm>

namespace Model {
    FollowUpPatient::FollowUpPatient(std::string id, std::string n, int a, std::string contact, std::string doctor)
        : PatientRecord(id, n, a, contact), previousDoctor(doctor) {}

    std::string FollowUpPatient::getPatientCategory() const { return "Follow-Up"; }
    std::string FollowUpPatient::getPriorityLevel() const { return "Scheduled"; } // Visually distinguished priority[cite: 167]

    bool FollowUpPatient::scheduleAppointment(const std::string& date, const std::string& time, const std::string& reason) {
        std::string apptId = "APT-FU-" + patientId + "-" + date;
        addAppointment(Appointment(apptId, date, time, reason));
        return true;
    }

    bool FollowUpPatient::cancelAppointment(const std::string& appointmentId) {
        auto it = std::remove_if(appointments.begin(), appointments.end(),
            [&](const Appointment& appt) { return appt.getId() == appointmentId; });
        if (it != appointments.end()) {
            appointments.erase(it, appointments.end());
            return true;
        }
        return false;
    }
}