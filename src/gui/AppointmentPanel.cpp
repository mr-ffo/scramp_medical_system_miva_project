#include "gui/AppointmentPanel.h"
#include "model/Appointable.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QHeaderView>
#include <QMessageBox>

namespace GUI {
    AppointmentPanel::AppointmentPanel(Controller::ClinicDatabase& database, QWidget *parent)
        : QWidget(parent), db(database) {
        setupLayout();
        setupConnections();
        onRefreshClicked();
    }

    void AppointmentPanel::setupLayout() {
        QVBoxLayout* mainLayout = new QVBoxLayout(this);

        QFormLayout* formLayout = new QFormLayout();
        patientIdInput = new QLineEdit();
        patientIdInput->setPlaceholderText("Enter Patient ID (e.g., P001)");
        
        dateInput = new QLineEdit();
        dateInput->setPlaceholderText("YYYY-MM-DD");
        
        timeInput = new QLineEdit();
        timeInput->setPlaceholderText("HH:MM");
        
        reasonInput = new QLineEdit();
        reasonInput->setPlaceholderText("Reason for consultation");

        formLayout->addRow("Patient ID:", patientIdInput);
        formLayout->addRow("Date:", dateInput);
        formLayout->addRow("Time:", timeInput);
        formLayout->addRow("Reason:", reasonInput);
        mainLayout->addLayout(formLayout);

        QHBoxLayout* buttonLayout = new QHBoxLayout();
        scheduleButton = new QPushButton("Schedule Appointment");
        scheduleButton->setToolTip("Schedule an appointment for General or Follow-Up patients");

        cancelButton = new QPushButton("Cancel Selected Appointment");
        cancelButton->setToolTip("Cancel the highlighted appointment in the table");

        refreshButton = new QPushButton("Refresh List");

        buttonLayout->addWidget(scheduleButton);
        buttonLayout->addWidget(cancelButton);
        buttonLayout->addWidget(refreshButton);
        mainLayout->addLayout(buttonLayout);

        appointmentTable = new QTableWidget(0, 6);
        appointmentTable->setHorizontalHeaderLabels({"Appt ID", "Patient ID", "Patient Name", "Date", "Time", "Reason"});
        appointmentTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        appointmentTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        mainLayout->addWidget(appointmentTable);
    }

    void AppointmentPanel::setupConnections() {
        connect(scheduleButton, &QPushButton::clicked, this, &AppointmentPanel::onScheduleClicked);
        connect(cancelButton, &QPushButton::clicked, this, &AppointmentPanel::onCancelClicked);
        connect(refreshButton, &QPushButton::clicked, this, &AppointmentPanel::onRefreshClicked);
    }

    void AppointmentPanel::onScheduleClicked() {
        std::string targetId = patientIdInput->text().trimmed().toStdString();
        std::string date = dateInput->text().trimmed().toStdString();
        std::string time = timeInput->text().trimmed().toStdString();
        std::string reason = reasonInput->text().trimmed().toStdString();

        if (targetId.empty() || date.empty() || time.empty()) {
            QMessageBox::warning(this, "Input Error", "Please fill in Patient ID, Date, and Time.");
            return;
        }

        auto& patients = db.getAllPatients();
        bool patientFound = false;

        for (auto& patient : patients) {
            if (patient->getId() == targetId) {
                patientFound = true;
                auto appointablePatient = dynamic_cast<Model::Appointable*>(patient.get());
                
                if (appointablePatient) {
                    appointablePatient->scheduleAppointment(date, time, reason);
                    db.updateCache(patient);
                    QMessageBox::information(this, "Success", "Appointment scheduled successfully.");
                    patientIdInput->clear(); dateInput->clear(); timeInput->clear(); reasonInput->clear();
                } else {
                    QMessageBox::critical(this, "Polymorphic Restriction", 
                        "Emergency patients cannot schedule standard appointments. They require immediate triage.");
                }
                break;
            }
        }

        if (!patientFound) {
            QMessageBox::warning(this, "Not Found", "Patient ID not found in the registry.");
        }

        onRefreshClicked();
    }

    void AppointmentPanel::onCancelClicked() {
        int currentRow = appointmentTable->currentRow();
        if (currentRow < 0) {
            QMessageBox::warning(this, "Selection Error", "Please select an appointment row to cancel.");
            return;
        }

        std::string apptId = appointmentTable->item(currentRow, 0)->text().toStdString();
        std::string patId = appointmentTable->item(currentRow, 1)->text().toStdString();

        for (auto& patient : db.getAllPatients()) {
            if (patient->getId() == patId) {
                auto appointablePatient = dynamic_cast<Model::Appointable*>(patient.get());
                if (appointablePatient && appointablePatient->cancelAppointment(apptId)) {
                    QMessageBox::information(this, "Cancelled", "Appointment cancelled successfully.");
                    onRefreshClicked();
                    return;
                }
            }
        }
        QMessageBox::warning(this, "Error", "Could not cancel the selected appointment.");
    }

    void AppointmentPanel::onRefreshClicked() {
        appointmentTable->setRowCount(0);
        auto& patients = db.getAllPatients();
        
        int row = 0;
        for (const auto& patient : patients) {
            auto appointablePatient = dynamic_cast<Model::Appointable*>(patient.get());
            if (appointablePatient) {
                for (const auto& appt : patient->getAppointments()) {
                    appointmentTable->insertRow(row);
                    appointmentTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(appt.getId())));
                    appointmentTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(patient->getId())));
                    appointmentTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(patient->getName())));
                    appointmentTable->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(appt.getDate())));
                    appointmentTable->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(appt.getTime())));
                    appointmentTable->setItem(row, 5, new QTableWidgetItem(QString::fromStdString(appt.getReason())));
                    row++;
                }
            }
        }
    }
}