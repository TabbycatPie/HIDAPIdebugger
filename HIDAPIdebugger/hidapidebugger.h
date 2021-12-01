#ifndef HIDAPIDEBUGGER_H
#define HIDAPIDEBUGGER_H

#include <QMainWindow>
#include "libusb.h"

QT_BEGIN_NAMESPACE
namespace Ui { class HIDAPIdebugger; }
QT_END_NAMESPACE

class HIDAPIdebugger : public QMainWindow
{
    Q_OBJECT

public:
    HIDAPIdebugger(QWidget *parent = nullptr);
    ~HIDAPIdebugger();

    int getAllDevsNum();
    int getDevsNum();
    libusb_device** getDevsList();
private:
    Ui::HIDAPIdebugger *ui;
    void HID_enum();

};
#endif // HIDAPIDEBUGGER_H
