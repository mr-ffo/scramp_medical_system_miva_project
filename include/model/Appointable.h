#ifndef APPOINTABLE_H
#define APPOINTABLE_H

#include <string>

namespace Model {
    // Pure interface: No member variables here to prevent C++ ambiguity
    class Appointable {
    public:
        virtual ~Appointable() = default;
        
        virtual bool scheduleAppointment(const std::string& date, const std::string& time, const std::string& reason) = 0;
        virtual bool cancelAppointment(const std::string& appointmentId) = 0;
    };
}

#endif