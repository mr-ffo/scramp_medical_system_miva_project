#include "utils/FileHandler.h"
#include "model/GeneralPatient.h"
#include "model/EmergencyPatient.h"
#include "model/FollowUpPatient.h"
#include "model/Appointable.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace Utils {
    bool FileHandler::savePatientsToCSV(const std::vector<std::shared_ptr<Model::PatientRecord>>& patients, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) return false;

        for (const auto& patient : patients) {
            std::ostringstream apptStream;
            const auto& appts = patient->getAppointments();
            for (size_t i = 0; i < appts.size(); ++i) {
                apptStream << appts[i].getDate() << "~" << appts[i].getTime() << "~" << appts[i].getReason();
                if (i + 1 < appts.size()) apptStream << ";";
            }

            file << patient->getId() << ","
                 << patient->getName() << ","
                 << patient->getAge() << ","
                 << patient->getContactInfo() << ","
                 << patient->getPatientCategory() << ","
                 << apptStream.str() << "\n";
        }
        file.close();
        return true;
    }

    std::vector<std::shared_ptr<Model::PatientRecord>> FileHandler::loadPatientsFromCSV(const std::string& filename) {
        std::vector<std::shared_ptr<Model::PatientRecord>> loadedPatients;
        std::ifstream file(filename);
        
        if (!file.is_open()) {
            throw std::runtime_error("File loading failure: Could not open or find the specified file.");
        }

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            std::stringstream ss(line);
            std::string id, name, ageStr, contact, type, apptsSerialized;
            
            std::getline(ss, id, ',');
            std::getline(ss, name, ',');
            std::getline(ss, ageStr, ',');
            std::getline(ss, contact, ',');
            std::getline(ss, type, ',');
            std::getline(ss, apptsSerialized);

            if (id.empty() || name.empty()) continue;

            int age = 0;
            try { age = std::stoi(ageStr); } catch (...) { continue; }

            std::shared_ptr<Model::PatientRecord> pat;
            if (type == "General") {
                pat = std::make_shared<Model::GeneralPatient>(id, name, age, contact);
            } else if (type == "Emergency") {
                pat = std::make_shared<Model::EmergencyPatient>(id, name, age, contact, 5);
            } else {
                pat = std::make_shared<Model::FollowUpPatient>(id, name, age, contact, "Clinic Staff");
            }

            if (!apptsSerialized.empty()) {
                auto appointable = dynamic_cast<Model::Appointable*>(pat.get());
                if (appointable) {
                    std::stringstream apptSS(apptsSerialized);
                    std::string singleAppt;
                    while (std::getline(apptSS, singleAppt, ';')) {
                        std::stringstream fieldSS(singleAppt);
                        std::string d, t, r;
                        std::getline(fieldSS, d, '~');
                        std::getline(fieldSS, t, '~');
                        std::getline(fieldSS, r, '~');
                        if (!d.empty() && !t.empty()) {
                            appointable->scheduleAppointment(d, t, r);
                        }
                    }
                }
            }

            loadedPatients.push_back(pat);
        }
        file.close();
        return loadedPatients;
    }
}
