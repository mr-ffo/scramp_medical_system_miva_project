#ifndef APPOINTMENT_H
#define APPOINTMENT_H

#include <string>

namespace Model {
    class Appointment {
    private:
        std::string appointmentId;
        std::string date;
        std::string time;
        std::string reason;

    public:
        Appointment(std::string id, std::string d, std::string t, std::string r)
            : appointmentId(id), date(d), time(t), reason(r) {}

        std::string getId() const { return appointmentId; }
        std::string getDate() const { return date; }
        std::string getTime() const { return time; }
        std::string getReason() const { return reason; }
    };
}

#endif