#include "model/GeneralPatient.h"
#include <algorithm>

namespace Model {
    GeneralPatient::GeneralPatient(std::string id, std::string n, int a, std::string contact)
        : PatientRecord(id, n, a, contact) {}

    std::string GeneralPatient::getPatientCategory() const { return "General"; }
    std::string GeneralPatient::getPriorityLevel() const { return "Normal"; } // Visually distinguished priority[cite: 167]

    bool GeneralPatient::scheduleAppointment(const std::string& date, const std::string& time, const std::string& reason) {
        std::string apptId = "APT-" + patientId + "-" + date;
        addAppointment(Appointment(apptId, date, time, reason));
        return true;
    }

    bool GeneralPatient::cancelAppointment(const std::string& appointmentId) {
        auto it = std::remove_if(appointments.begin(), appointments.end(),
            [&](const Appointment& appt) { return appt.getId() == appointmentId; });
        if (it != appointments.end()) {
            appointments.erase(it, appointments.end());
            return true;
        }
        return false;
    }
}