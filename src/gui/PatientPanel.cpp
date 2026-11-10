#include "gui/PatientPanel.h"
#include "model/GeneralPatient.h"
#include "model/EmergencyPatient.h"
#include "model/FollowUpPatient.h"
#include "utils/FileHandler.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QFileDialog>
#include <QHeaderView>
#include <QBrush>
#include <QColor>
#include <stdexcept>

namespace GUI {
    PatientPanel::PatientPanel(Controller::ClinicDatabase& database, QWidget *parent)
        : QWidget(parent), db(database) {
        setupLayout();
        setupConnections();
        refreshTable();
    }

    void PatientPanel::setupLayout() {
        QVBoxLayout* mainLayout = new QVBoxLayout(this);

        // Form Section
        QFormLayout* formLayout = new QFormLayout();
        idInput = new QLineEdit();
        idInput->setPlaceholderText("e.g., P001");
        idInput->setToolTip("Unique identifier for the patient");

        nameInput = new QLineEdit();
        nameInput->setPlaceholderText("Full Name");

        ageInput = new QLineEdit();
        ageInput->setPlaceholderText("0 - 120");

        contactInput = new QLineEdit();
        contactInput->setPlaceholderText("Phone or Email");
        
        typeSelector = new QComboBox();
        typeSelector->addItems({"General", "Emergency", "Follow-Up"});
        typeSelector->setToolTip("Select patient category (dynamically updates fields)");

        // Dynamic Component (Section 9b)
        extraFieldLabel = new QLabel("Notes / Details:");
        extraFieldInput = new QLineEdit();
        extraFieldInput->setPlaceholderText("Standard outpatient visit");

        formLayout->addRow("Patient ID:", idInput);
        formLayout->addRow("Name:", nameInput);
        formLayout->addRow("Age:", ageInput);
        formLayout->addRow("Contact:", contactInput);
        formLayout->addRow("Type:", typeSelector);
        formLayout->addRow(extraFieldLabel, extraFieldInput);
        mainLayout->addLayout(formLayout);

        // Buttons Section with Tooltips (Section 9g)
        QHBoxLayout* buttonLayout = new QHBoxLayout();
        addButton = new QPushButton("Add Patient");
        addButton->setToolTip("Register a new patient record");

        editButton = new QPushButton("Update Selected");
        editButton->setToolTip("Update Name, Age, and Contact of the selected patient");

        deleteButton = new QPushButton("Delete Selected");
        deleteButton->setToolTip("Remove the selected patient from the registry");

        undoButton = new QPushButton("Undo Last Action");
        undoButton->setToolTip("Reverse the last Add, Edit, or Delete operation (Stack)");

        saveButton = new QPushButton("Save (CSV)");
        saveButton->setToolTip("Export patient and appointment records to file (Ctrl+S)");

        loadButton = new QPushButton("Load (CSV)");
        loadButton->setToolTip("Import patient and appointment records from file (Ctrl+L)");

        buttonLayout->addWidget(addButton);
        buttonLayout->addWidget(editButton);
        buttonLayout->addWidget(deleteButton);
        buttonLayout->addWidget(undoButton);
        buttonLayout->addWidget(saveButton);
        buttonLayout->addWidget(loadButton);
        mainLayout->addLayout(buttonLayout);

        // Table Section
        patientTable = new QTableWidget(0, 6);
        patientTable->setHorizontalHeaderLabels({"ID", "Name", "Age", "Contact", "Category", "Priority Status"});
        patientTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        patientTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        mainLayout->addWidget(patientTable);
    }

    void PatientPanel::setupConnections() {
        connect(addButton, &QPushButton::clicked, this, &PatientPanel::onAddPatientClicked);
        connect(editButton, &QPushButton::clicked, this, &PatientPanel::onEditPatientClicked);
        connect(deleteButton, &QPushButton::clicked, this, &PatientPanel::onDeletePatientClicked);
        connect(undoButton, &QPushButton::clicked, this, &PatientPanel::onUndoClicked);
        connect(saveButton, &QPushButton::clicked, this, &PatientPanel::onSaveClicked);
        connect(loadButton, &QPushButton::clicked, this, &PatientPanel::onLoadClicked);
        connect(patientTable, &QTableWidget::itemSelectionChanged, this, &PatientPanel::onRowSelected);
        connect(typeSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &PatientPanel::onTypeChanged);
    }

    void PatientPanel::onTypeChanged(int index) {
        if (index == 0) {
            extraFieldLabel->setText("General Notes:");
            extraFieldInput->setPlaceholderText("Standard consultation");
        } else if (index == 1) {
            extraFieldLabel->setText("Severity Level (1-10):");
            extraFieldInput->setPlaceholderText("Enter integer 1 - 10 (Default: 5)");
        } else {
            extraFieldLabel->setText("Previous Doctor:");
            extraFieldInput->setPlaceholderText("e.g., Dr. Adeyemi");
        }
    }

    void PatientPanel::onRowSelected() {
        int row = patientTable->currentRow();
        if (row >= 0) {
            idInput->setText(patientTable->item(row, 0)->text());
            nameInput->setText(patientTable->item(row, 1)->text());
            ageInput->setText(patientTable->item(row, 2)->text());
            contactInput->setText(patientTable->item(row, 3)->text());
        }
    }

    void PatientPanel::onAddPatientClicked() {
        try {
            std::string id = idInput->text().trimmed().toStdString();
            std::string name = nameInput->text().trimmed().toStdString();
            std::string contact = contactInput->text().trimmed().toStdString();
            
            if (id.empty() || name.empty()) {
                throw std::invalid_argument("Patient ID and Name cannot be empty.");
            }

            bool ok;
            int age = ageInput->text().toInt(&ok);
            if (!ok || age < 0 || age > 120) {
                throw std::out_of_range("Error: Patient age must be between 0 and 120.");
            }

            std::shared_ptr<Model::PatientRecord> newPatient;
            std::string type = typeSelector->currentText().toStdString();

            if (type == "General") {
                newPatient = std::make_shared<Model::GeneralPatient>(id, name, age, contact);
            } else if (type == "Emergency") {
                int severity = 5;
                if (!extraFieldInput->text().isEmpty()) {
                    bool sevOk;
                    int s = extraFieldInput->text().toInt(&sevOk);
                    if (sevOk && s >= 1 && s <= 10) severity = s;
                }
                newPatient = std::make_shared<Model::EmergencyPatient>(id, name, age, contact, severity);
            } else {
                std::string doc = extraFieldInput->text().trimmed().toStdString();
                if (doc.empty()) doc = "Clinic Staff";
                newPatient = std::make_shared<Model::FollowUpPatient>(id, name, age, contact, doc);
            }

            db.addPatient(newPatient);
            refreshTable();
            
            idInput->clear(); nameInput->clear(); ageInput->clear(); contactInput->clear(); extraFieldInput->clear();

        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Validation / Input Error", e.what());
        }
    }

    void PatientPanel::onEditPatientClicked() {
        try {
            std::string id = idInput->text().trimmed().toStdString();
            std::string name = nameInput->text().trimmed().toStdString();
            std::string contact = contactInput->text().trimmed().toStdString();

            if (id.empty() || name.empty()) {
                throw std::invalid_argument("Select a patient and ensure ID and Name are not empty.");
            }

            bool ok;
            int age = ageInput->text().toInt(&ok);
            if (!ok || age < 0 || age > 120) {
                throw std::out_of_range("Error: Patient age must be between 0 and 120.");
            }

            if (db.editPatient(id, name, age, contact)) {
                refreshTable();
                QMessageBox::information(this, "Updated", "Patient information updated successfully.");
            } else {
                QMessageBox::warning(this, "Not Found", "No patient with that ID exists to update.");
            }
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Input Error", e.what());
        }
    }

    void PatientPanel::onDeletePatientClicked() {
        int currentRow = patientTable->currentRow();
        if (currentRow >= 0) {
            std::string id = patientTable->item(currentRow, 0)->text().toStdString();
            db.deletePatient(id);
            refreshTable();
        } else {
            QMessageBox::warning(this, "Selection Error", "Please select a patient row to delete.");
        }
    }

    void PatientPanel::onUndoClicked() {
        try {
            db.undoLastOperation();
            refreshTable();
        } catch (const std::exception& e) {
            QMessageBox::information(this, "Undo Stack", e.what());
        }
    }

    void PatientPanel::onSaveClicked() {
        QString fileName = QFileDialog::getSaveFileName(this, "Save Patients", "", "CSV Files (*.csv);;Text Files (*.txt)");
        if (!fileName.isEmpty()) {
            if (Utils::FileHandler::savePatientsToCSV(db.getAllPatients(), fileName.toStdString())) {
                QMessageBox::information(this, "Success", "Patient and appointment records saved successfully.");
            } else {
                QMessageBox::critical(this, "Error", "Failed to save the file.");
            }
        }
    }

    void PatientPanel::onLoadClicked() {
        QString fileName = QFileDialog::getOpenFileName(this, "Load Patients", "", "CSV Files (*.csv);;Text Files (*.txt)");
        if (!fileName.isEmpty()) {
            try {
                auto loadedPatients = Utils::FileHandler::loadPatientsFromCSV(fileName.toStdString());
                for (const auto& p : loadedPatients) {
                    try {
                        db.addPatient(p);
                    } catch (const Controller::DuplicatePatientException&) {
                        // Skip duplicate IDs gracefully when merging loaded files
                    }
                }
                refreshTable();
                QMessageBox::information(this, "Success", "Records loaded successfully.");
            } catch (const std::exception& e) {
                QMessageBox::critical(this, "File Error", e.what()); 
            }
        }
    }

    void PatientPanel::refreshTable() {
        patientTable->setRowCount(0);
        const auto& patients = db.getAllPatients();
        for (size_t i = 0; i < patients.size(); ++i) {
            patientTable->insertRow(i);
            patientTable->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(patients[i]->getId())));
            patientTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(patients[i]->getName())));
            patientTable->setItem(i, 2, new QTableWidgetItem(QString::number(patients[i]->getAge())));
            patientTable->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(patients[i]->getContactInfo())));
            patientTable->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(patients[i]->getPatientCategory())));
            
            QTableWidgetItem* priorityItem = new QTableWidgetItem(QString::fromStdString(patients[i]->getPriorityLevel()));
            // Custom Table Presentation (Section 9a)
            std::string cat = patients[i]->getPatientCategory();
            if (cat == "Emergency") {
                priorityItem->setForeground(QBrush(QColor(220, 20, 60))); // Crimson red for High Priority
            } else if (cat == "Follow-Up") {
                priorityItem->setForeground(QBrush(QColor(30, 144, 255))); // Dodger blue for Scheduled
            } else {
                priorityItem->setForeground(QBrush(QColor(46, 139, 87))); // Sea green for Normal
            }
            patientTable->setItem(i, 5, priorityItem);
        }
    }
}