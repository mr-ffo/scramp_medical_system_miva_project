#include "gui/SearchSortPanel.h"
#include "controller/SortEngine.h"
#include "controller/SearchEngine.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QHeaderView>
#include <sstream>

namespace GUI {
    SearchSortPanel::SearchSortPanel(Controller::ClinicDatabase& database, QWidget *parent)
        : QWidget(parent), db(database) {
        setupLayout();
        setupConnections();
    }

    void SearchSortPanel::setupLayout() {
        QVBoxLayout* mainLayout = new QVBoxLayout(this);

        // --- SORTING SECTION ---
        QGroupBox* sortGroup = new QGroupBox("Manual Sorting Algorithms");
        QHBoxLayout* sortLayout = new QHBoxLayout(sortGroup);
        
        sortCriteriaSelector = new QComboBox();
        sortCriteriaSelector->addItems({"Sort by Name", "Sort by ID", "Sort by Age"});
        
        sortAlgorithmSelector = new QComboBox();
        sortAlgorithmSelector->addItems({"Selection Sort (O(n^2))", "Quick Sort (O(n log n))"});
        
        sortButton = new QPushButton("Execute Sort");
        
        sortLayout->addWidget(sortCriteriaSelector);
        sortLayout->addWidget(sortAlgorithmSelector);
        sortLayout->addWidget(sortButton);
        mainLayout->addWidget(sortGroup);

        // --- SEARCHING SECTION ---
        QGroupBox* searchGroup = new QGroupBox("Manual Searching Algorithms");
        QHBoxLayout* searchLayout = new QHBoxLayout(searchGroup);
        
        searchTypeSelector = new QComboBox();
        searchTypeSelector->addItems({
            "Linear Search (by ID)",
            "Binary Search (by ID)",
            "Recursive Search (by Name)",
            "Linear Filter (by Category)"
        });
        
        searchInput = new QLineEdit();
        searchInput->setPlaceholderText("Enter ID, Name, or Category (General / Emergency / Follow-Up)...");
        
        searchButton = new QPushButton("Execute Search");
        
        searchLayout->addWidget(searchTypeSelector);
        searchLayout->addWidget(searchInput);
        searchLayout->addWidget(searchButton);
        mainLayout->addWidget(searchGroup);

        // --- RESULTS TABLE ---
        resultTable = new QTableWidget(0, 6);
        resultTable->setHorizontalHeaderLabels({"ID", "Name", "Age", "Contact", "Category", "Priority"});
        resultTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        mainLayout->addWidget(resultTable);

        // --- CLINIC REPORTS SECTION ---
        QGroupBox* reportGroup = new QGroupBox("Clinic Analytics & Simple Reports");
        QVBoxLayout* reportLayout = new QVBoxLayout(reportGroup);

        generateReportButton = new QPushButton("Generate Clinic Summary Reports");
        generateReportButton->setToolTip("Generate Category Distribution, Pending Appointments, Frequent Patients, and Appointment Summary");

        reportOutput = new QTextEdit();
        reportOutput->setReadOnly(true);
        reportOutput->setPlaceholderText("Click 'Generate Clinic Summary Reports' to view the 4 analytical clinic reports...");

        reportLayout->addWidget(generateReportButton);
        reportLayout->addWidget(reportOutput);
        mainLayout->addWidget(reportGroup);
    }

    void SearchSortPanel::setupConnections() {
        connect(sortButton, &QPushButton::clicked, this, &SearchSortPanel::onSortClicked);
        connect(searchButton, &QPushButton::clicked, this, &SearchSortPanel::onSearchClicked);
        connect(generateReportButton, &QPushButton::clicked, this, &SearchSortPanel::onGenerateReportsClicked);
    }

    void SearchSortPanel::focusSearchInput() {
        searchInput->setFocus();
    }

    void SearchSortPanel::displayPatients(const std::vector<std::shared_ptr<Model::PatientRecord>>& patients) {
        resultTable->setRowCount(0);
        for (size_t i = 0; i < patients.size(); ++i) {
            resultTable->insertRow(i);
            resultTable->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(patients[i]->getId())));
            resultTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(patients[i]->getName())));
            resultTable->setItem(i, 2, new QTableWidgetItem(QString::number(patients[i]->getAge())));
            resultTable->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(patients[i]->getContactInfo())));
            resultTable->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(patients[i]->getPatientCategory())));
            resultTable->setItem(i, 5, new QTableWidgetItem(QString::fromStdString(patients[i]->getPriorityLevel())));
        }
    }

    void SearchSortPanel::onSortClicked() {
        auto patientsToSort = db.getAllPatients(); 
        
        Controller::SortCriteria criteria;
        if (sortCriteriaSelector->currentIndex() == 0) criteria = Controller::SortCriteria::BY_NAME;
        else if (sortCriteriaSelector->currentIndex() == 1) criteria = Controller::SortCriteria::BY_ID;
        else criteria = Controller::SortCriteria::BY_AGE;

        if (sortAlgorithmSelector->currentIndex() == 0) {
            Controller::SortEngine::selectionSort(patientsToSort, criteria);
        } else {
            Controller::SortEngine::quickSort(patientsToSort, criteria);
        }

        displayPatients(patientsToSort);
    }

    void SearchSortPanel::onSearchClicked() {
        auto patients = db.getAllPatients();
        std::string query = searchInput->text().trimmed().toStdString();
        
        if (query.empty()) {
            displayPatients(patients);
            return;
        }

        int index = -1;
        std::vector<std::shared_ptr<Model::PatientRecord>> resultList;
        int searchType = searchTypeSelector->currentIndex();

        if (searchType == 0) {
            index = Controller::SearchEngine::linearSearchById(patients, query);
            if (index != -1) {
                db.updateCache(patients[index]);
                resultList.push_back(patients[index]);
            }
        } 
        else if (searchType == 1) {
            auto sortedForBinary = patients;
            Controller::SortEngine::quickSort(sortedForBinary, Controller::SortCriteria::BY_ID);
            index = Controller::SearchEngine::binarySearchById(sortedForBinary, query);
            if (index != -1) {
                db.updateCache(sortedForBinary[index]);
                resultList.push_back(sortedForBinary[index]);
            }
        } 
        else if (searchType == 2) {
            index = Controller::SearchEngine::recursiveSearchByName(patients, query);
            if (index != -1) {
                db.updateCache(patients[index]);
                resultList.push_back(patients[index]);
            }
        }
        else if (searchType == 3) {
            // Manual Linear Search by Category
            for (const auto& p : patients) {
                if (p->getPatientCategory() == query) {
                    resultList.push_back(p);
                }
            }
        }

        if (resultList.empty()) {
            QMessageBox::information(this, "Not Found", "No patient matching that query was found in the system.");
        }
        
        displayPatients(resultList);
    }

    void SearchSortPanel::onGenerateReportsClicked() {
        const auto& patients = db.getAllPatients();
        int generalCount = 0, emergencyCount = 0, followUpCount = 0;
        int totalAppointments = 0;

        std::ostringstream oss;
        oss << "====================================================\n";
        oss << "   SCAPMS CLINIC ANALYTICS & SUMMARY REPORTS\n";
        oss << "====================================================\n\n";

        // Counting Categories & Appointments
        for (const auto& p : patients) {
            std::string cat = p->getPatientCategory();
            if (cat == "General") generalCount++;
            else if (cat == "Emergency") emergencyCount++;
            else if (cat == "Follow-Up") followUpCount++;

            totalAppointments += static_cast<int>(p->getAppointments().size());
        }

        // Report 1: Patient Category Distribution
        oss << "1. PATIENT CATEGORY DISTRIBUTION\n";
        oss << "----------------------------------------------------\n";
        oss << "   General Patients   : " << generalCount << "\n";
        oss << "   Emergency Patients : " << emergencyCount << "\n";
        oss << "   Follow-Up Patients : " << followUpCount << "\n";
        oss << "   Total Registered   : " << patients.size() << "\n\n";

        // Report 2: Patients with Pending Appointments
        oss << "2. PATIENTS WITH PENDING APPOINTMENTS\n";
        oss << "----------------------------------------------------\n";
        bool hasPending = false;
        for (const auto& p : patients) {
            if (!p->getAppointments().empty()) {
                hasPending = true;
                oss << "   - [" << p->getId() << "] " << p->getName()
                    << " (" << p->getAppointments().size() << " scheduled)\n";
            }
        }
        if (!hasPending) oss << "   No patients currently have pending appointments.\n";
        oss << "\n";

        // Report 3: Most Frequently Attended / Recently Accessed Patients (Array Cache & Booking Count)
        oss << "3. MOST FREQUENTLY ATTENDED & RECENTLY ACCESSED PATIENTS\n";
        oss << "----------------------------------------------------\n";
        auto cacheList = db.getRecentCacheList();
        if (cacheList.empty()) {
            oss << "   No patient records in the Recent Cache [10] yet.\n";
        } else {
            for (size_t i = 0; i < cacheList.size(); ++i) {
                oss << "   Cache Slot [" << i << "]: " << cacheList[i]->getName()
                    << " (ID: " << cacheList[i]->getId()
                    << ", Appointments: " << cacheList[i]->getAppointments().size() << ")\n";
            }
        }
        oss << "\n";

        // Report 4: Daily Appointment & Queue Summary
        oss << "4. DAILY APPOINTMENT & QUEUE SUMMARY\n";
        oss << "----------------------------------------------------\n";
        oss << "   Total Active Appointments : " << totalAppointments << "\n";
        oss << "   Patients in Waiting Queue : " << db.getWaitingQueueSize() << "\n";

        reportOutput->setPlainText(QString::fromStdString(oss.str()));
    }
}