#ifndef APPOINTMENTPANEL_H
#define APPOINTMENTPANEL_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include "controller/ClinicDatabase.h"

namespace GUI {
    class AppointmentPanel : public QWidget {
        Q_OBJECT

    private:
        Controller::ClinicDatabase& db;

        QLineEdit* patientIdInput;
        QLineEdit* dateInput;
        QLineEdit* timeInput;
        QLineEdit* reasonInput;
        
        QPushButton* scheduleButton;
        QPushButton* cancelButton;
        QPushButton* refreshButton;

        QTableWidget* appointmentTable;

        void setupLayout();
        void setupConnections();

    public slots:
        void onScheduleClicked();
        void onCancelClicked();
        void onRefreshClicked();

    public:
        explicit AppointmentPanel(Controller::ClinicDatabase& database, QWidget *parent = nullptr);
    };
}

#endif