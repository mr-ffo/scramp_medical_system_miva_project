#ifndef PATIENTPANEL_H
#define PATIENTPANEL_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include "controller/ClinicDatabase.h"

namespace GUI {
    class PatientPanel : public QWidget {
        Q_OBJECT

    private:
        Controller::ClinicDatabase& db;

        QTableWidget* patientTable;
        QLineEdit* idInput;
        QLineEdit* nameInput;
        QLineEdit* ageInput;
        QLineEdit* contactInput;
        QComboBox* typeSelector;
        QLabel* extraFieldLabel;
        QLineEdit* extraFieldInput;

        QPushButton* addButton;
        QPushButton* editButton;
        QPushButton* deleteButton;
        QPushButton* undoButton;
        QPushButton* saveButton;
        QPushButton* loadButton;

        void setupLayout();
        void setupConnections();

    public slots:
        void refreshTable();
        void onAddPatientClicked();
        void onEditPatientClicked();
        void onDeletePatientClicked();
        void onUndoClicked();
        void onSaveClicked();
        void onLoadClicked();
        void onRowSelected();
        void onTypeChanged(int index);

    public:
        explicit PatientPanel(Controller::ClinicDatabase& database, QWidget *parent = nullptr);
    };
}

#endif