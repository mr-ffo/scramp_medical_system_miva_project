#include "gui/MainWindow.h"
#include "gui/PatientPanel.h"
#include "gui/SearchSortPanel.h"
#include "gui/AppointmentPanel.h"
#include "gui/QueuePanel.h"
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QStatusBar>
#include <QMessageBox>
#include <QApplication>
#include <QKeySequence>

namespace GUI {
    MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
        setWindowTitle("SCAPMS - Smart Clinic Appointment & Patient Management System");
        resize(1080, 780); 

        centralWidget = new QWidget(this);
        mainLayout = new QVBoxLayout(centralWidget);
        setCentralWidget(centralWidget);

        tabWidget = new QTabWidget(this);
        mainLayout->addWidget(tabWidget);

        setupTabs();
        setupMenuAndShortcuts();
        setupTimer();
        updateDashboardStatus();
    }

    void MainWindow::setupTabs() {
        patientTab = new PatientPanel(clinicDb, this);
        tabWidget->addTab(patientTab, "1. Patient Records");

        appointmentTab = new AppointmentPanel(clinicDb, this);
        tabWidget->addTab(appointmentTab, "2. Appointments");

        queueTab = new QueuePanel(clinicDb, this);
        tabWidget->addTab(queueTab, "3. Waiting Queue");

        searchSortTab = new SearchSortPanel(clinicDb, this);
        tabWidget->addTab(searchSortTab, "4. Search, Sort & Reports");
    }

    void MainWindow::setupMenuAndShortcuts() {
        // Menu Bar & Keyboard Shortcuts (Sections 6b, 7.vii, 9f)
        QMenu* fileMenu = menuBar()->addMenu("&File");

        QAction* newAction = new QAction("&New Session", this);
        newAction->setShortcut(QKeySequence::New);
        connect(newAction, &QAction::triggered, this, &MainWindow::onNewDatabase);
        fileMenu->addAction(newAction);

        QAction* saveAction = new QAction("&Save Records...", this);
        saveAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_S));
        connect(saveAction, &QAction::triggered, patientTab, &PatientPanel::onSaveClicked);
        fileMenu->addAction(saveAction);

        QAction* loadAction = new QAction("&Load Records...", this);
        loadAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_L));
        connect(loadAction, &QAction::triggered, patientTab, &PatientPanel::onLoadClicked);
        fileMenu->addAction(loadAction);

        fileMenu->addSeparator();

        QAction* searchShortcutAction = new QAction("&Find Patient", this);
        searchShortcutAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_F));
        connect(searchShortcutAction, &QAction::triggered, this, &MainWindow::onJumpToSearch);
        fileMenu->addAction(searchShortcutAction);

        fileMenu->addSeparator();

        QAction* exitAction = new QAction("E&xit", this);
        exitAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Q));
        connect(exitAction, &QAction::triggered, qApp, &QApplication::quit);
        fileMenu->addAction(exitAction);
    }

    void MainWindow::setupTimer() {
        // Timer Event for periodic dashboard & queue alerts (Section 6d, 9d)
        dashboardTimer = new QTimer(this);
        connect(dashboardTimer, &QTimer::timeout, this, &MainWindow::updateDashboardStatus);
        dashboardTimer->start(3000); // Refreshes status bar every 3 seconds
    }

    void MainWindow::onNewDatabase() {
        clinicDb.clearAll();
        patientTab->refreshTable();
        appointmentTab->onRefreshClicked();
        updateDashboardStatus();
        QMessageBox::information(this, "New Session", "Clinic database cleared for a new session.");
    }

    void MainWindow::onJumpToSearch() {
        tabWidget->setCurrentWidget(searchSortTab);
        searchSortTab->focusSearchInput();
    }

    void MainWindow::updateDashboardStatus() {
        size_t totalPatients = clinicDb.getAllPatients().size();
        size_t waitingCount = clinicDb.getWaitingQueueSize();
        QString statusMsg = QString("Live Clinic Status  |  Registered Patients: %1  |  Waiting Queue: %2  |  Shortcuts: Ctrl+S (Save), Ctrl+L (Load), Ctrl+F (Search), Ctrl+Q (Exit)")
                            .arg(totalPatients)
                            .arg(waitingCount);
        statusBar()->showMessage(statusMsg);
    }
}