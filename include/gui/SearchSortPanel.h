#ifndef SEARCHSORTPANEL_H
#define SEARCHSORTPANEL_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QTextEdit>
#include <vector>
#include <memory>
#include "controller/ClinicDatabase.h"

namespace GUI {
    class SearchSortPanel : public QWidget {
        Q_OBJECT

    private:
        Controller::ClinicDatabase& db;
        
        QComboBox* sortCriteriaSelector;
        QComboBox* sortAlgorithmSelector;
        QPushButton* sortButton;
        
        QComboBox* searchTypeSelector;
        QLineEdit* searchInput;
        QPushButton* searchButton;

        QTableWidget* resultTable;

        // Simple Reports Generator (Mandatory Feature)
        QPushButton* generateReportButton;
        QTextEdit* reportOutput;

        void setupLayout();
        void setupConnections();
        void displayPatients(const std::vector<std::shared_ptr<Model::PatientRecord>>& patients);

    public slots:
        void onSortClicked();
        void onSearchClicked();
        void onGenerateReportsClicked();
        void focusSearchInput();

    public:
        explicit SearchSortPanel(Controller::ClinicDatabase& database, QWidget *parent = nullptr);
    };
}

#endif