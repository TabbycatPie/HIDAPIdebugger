#include "hidapidebugger.h"
#include "ui_hidapidebugger.h"

HIDAPIdebugger::HIDAPIdebugger(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::HIDAPIdebugger)
{
    ui->setupUi(this);
}

HIDAPIdebugger::~HIDAPIdebugger()
{
    delete ui;
}

