#include "gui/QueuePanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QString>

namespace GUI {
    QueuePanel::QueuePanel(Controller::ClinicDatabase& database, QWidget *parent)
        : QWidget(parent), db(database) {
        setupLayout();
        setupConnections();
    }

    void QueuePanel::setupLayout() {
        QVBoxLayout* mainLayout = new QVBoxLayout(this);

        // Input Section
        QHBoxLayout* inputLayout = new QHBoxLayout();
        patientIdInput = new QLineEdit();
        patientIdInput->setPlaceholderText("Enter Patient ID to join Waiting Queue");
        enqueueButton = new QPushButton("Add to Queue (Enqueue)");
        
        inputLayout->addWidget(patientIdInput);
        inputLayout->addWidget(enqueueButton);
        mainLayout->addLayout(inputLayout);

        // Action Section
        dequeueButton = new QPushButton("Call Next Patient (Dequeue)");
        dequeueButton->setStyleSheet("background-color: #2e8b57; color: white; font-weight: bold; padding: 10px;");
        mainLayout->addWidget(dequeueButton);

        // Display Section
        queueDisplay = new QListWidget();
        mainLayout->addWidget(queueDisplay);
    }

    void QueuePanel::setupConnections() {
        connect(enqueueButton, &QPushButton::clicked, this, &QueuePanel::onEnqueueClicked);
        connect(dequeueButton, &QPushButton::clicked, this, &QueuePanel::onDequeueClicked);
    }

    void QueuePanel::onEnqueueClicked() {
        std::string id = patientIdInput->text().toStdString();
        
        if (id.empty()) {
            QMessageBox::warning(this, "Input Error", "Please enter a Patient ID.");
            return;
        }

        // Validate the patient actually exists in the database
        bool patientExists = false;
        for (const auto& patient : db.getAllPatients()) {
            if (patient->getId() == id) {
                patientExists = true;
                break;
            }
        }

        if (!patientExists) {
            QMessageBox::critical(this, "Error", "Patient ID not found in the registry.");
            return;
        }

        // Add to backend queue
        db.enqueuePatient(id);
        
        // Add to visual list
        queueDisplay->addItem(QString("Patient ID: ") + QString::fromStdString(id) + " - Waiting");
        patientIdInput->clear();
    }

    void QueuePanel::onDequeueClicked() {
        // Pop from backend queue
        std::string nextId = db.dequeueNextPatient();
        
        if (nextId.empty()) {
            QMessageBox::information(this, "Queue Empty", "There are no patients waiting.");
            return;
        }

        // Show doctor alert
        QMessageBox::information(this, "Doctor Ready", 
            QString("Calling Patient ID: %1 to the examination room.").arg(QString::fromStdString(nextId)));

        // Remove from the top of the visual list to keep it synced (FIFO logic)
        if (queueDisplay->count() > 0) {
            delete queueDisplay->takeItem(0);
        }
    }
}