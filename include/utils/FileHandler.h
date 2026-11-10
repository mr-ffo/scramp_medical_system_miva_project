#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <string>
#include <vector>
#include <memory>
#include "model/PatientRecord.h"

namespace Utils {
    class FileHandler {
    public:
        static bool savePatientsToCSV(const std::vector<std::shared_ptr<Model::PatientRecord>>& patients, const std::string& filename);
        static std::vector<std::shared_ptr<Model::PatientRecord>> loadPatientsFromCSV(const std::string& filename);
    };
}

#endif
