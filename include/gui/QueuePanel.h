#ifndef QUEUEPANEL_H
#define QUEUEPANEL_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include "controller/ClinicDatabase.h"

namespace GUI {
    class QueuePanel : public QWidget {
        Q_OBJECT

    private:
        Controller::ClinicDatabase& db;

        // GUI Components
        QLineEdit* patientIdInput;
        QPushButton* enqueueButton;
        QPushButton* dequeueButton;
        QListWidget* queueDisplay; // Perfect for a 1D list of waiting IDs

        void setupLayout();
        void setupConnections();

    private slots:
        void onEnqueueClicked();
        void onDequeueClicked();

    public:
        explicit QueuePanel(Controller::ClinicDatabase& database, QWidget *parent = nullptr);
    };
}

#endif