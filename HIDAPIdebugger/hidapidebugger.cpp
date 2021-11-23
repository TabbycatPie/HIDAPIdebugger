#include "hidapidebugger.h"
#include "ui_hidapidebugger.h"
#include "hidapi.h"

HIDAPIdebugger::HIDAPIdebugger(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::HIDAPIdebugger)
{
    ui->setupUi(this);
    connect(ui->btn_enum,&QPushButton::clicked,this,[=]{
        HID_enum();
    });
}

HIDAPIdebugger::~HIDAPIdebugger()
{
    delete ui;
}

QString getDinfoString(hid_device_info* dinfo){
    QString temp = "";
    temp +="Device info:\n";
    temp +="Path:" + QString(dinfo->path) + "\n";
    temp +="Product id:" + QString::number(dinfo->product_id)+"\n";
    temp +="Vendor  id:" + QString::number(dinfo->vendor_id) + "\n";
    temp +="Manufacturer string:" + QString::fromWCharArray(dinfo->manufacturer_string) + "\n";
    temp +="Product      string:" + QString::fromWCharArray(dinfo->product_string) + "\n";
    temp +="Interface number:" + QString::number(dinfo->interface_number) + "\n";
    temp +="Serial    number:" + QString::fromWCharArray(dinfo->serial_number) + "\n";
    temp +="Usage     number:" + QString::number(dinfo->usage) + "\n";
    temp +="Usagepage number:" + QString::number(dinfo->usage_page) + "\n\n";
    if(dinfo->next == nullptr){
        temp += "\n";
    }
    return temp;
}
void HIDAPIdebugger::HID_enum()
{
    hid_device_info* d_info = hid_enumerate(0x5131,0x2019);
    QString log = "";
    while(d_info != nullptr){
        log += getDinfoString(d_info);
        d_info = d_info->next;
    }
    ui->tv_log->setText(log);
}

