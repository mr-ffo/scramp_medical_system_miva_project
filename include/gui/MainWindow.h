#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QTimer>
#include "controller/ClinicDatabase.h"

namespace GUI {
    class PatientPanel;
    class AppointmentPanel;
    class QueuePanel;
    class SearchSortPanel;

    class MainWindow : public QMainWindow {
        Q_OBJECT

    private:
        Controller::ClinicDatabase clinicDb;

        QWidget* centralWidget;
        QVBoxLayout* mainLayout;
        QTabWidget* tabWidget;
        QTimer* dashboardTimer;

        PatientPanel* patientTab;
        AppointmentPanel* appointmentTab;
        QueuePanel* queueTab;
        SearchSortPanel* searchSortTab;

        void setupTabs();
        void setupMenuAndShortcuts();
        void setupTimer();

    private slots:
        void onNewDatabase();
        void onJumpToSearch();
        void updateDashboardStatus();

    public:
        explicit MainWindow(QWidget *parent = nullptr);
        ~MainWindow() override = default;
    };
}

#endif