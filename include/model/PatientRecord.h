#ifndef PATIENTRECORD_H
#define PATIENTRECORD_H

#include <string>
#include <vector>
#include "Appointment.h"

namespace Model {
    class PatientRecord {
    protected:
        // Common information required by the document
        std::string patientId;
        std::string name;
        int age;
        std::string contactInfo;
        
        // Composition: Associated with one or more appointments
        std::vector<Appointment> appointments;

    public:
        PatientRecord(std::string id, std::string n, int a, std::string contact)
            : patientId(id), name(n), age(a), contactInfo(contact) {}

        virtual ~PatientRecord() = default;

        // Pure virtual functions making this an abstract class
        virtual std::string getPatientCategory() const = 0;
        virtual std::string getPriorityLevel() const = 0;

        // Getters
        std::string getId() const { return patientId; }
        std::string getName() const { return name; }
        int getAge() const { return age; }
        std::string getContactInfo() const { return contactInfo; }
        const std::vector<Appointment>& getAppointments() const { return appointments; }

        // Setters
        void setName(const std::string& n) { name = n; }
        void setAge(int a) { age = a; }
        void setContactInfo(const std::string& contact) { contactInfo = contact; }
        void addAppointment(const Appointment& appt) { appointments.push_back(appt); }
    };
}

#endif