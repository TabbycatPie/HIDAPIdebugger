#include "hidapidebugger.h"
#include "ui_hidapidebugger.h"
#include "libusb.h"
#include <QDebug>
#include <QString>

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

int HIDAPIdebugger::getAllDevsNum(){


}


int findDevs(){
    // 初使化上下文
        int nRet = libusb_init(NULL);
        if (nRet < 0)
        {
            qDebug() << ("libusb_init(NULL) failed:[%s] \n", libusb_strerror(nRet));
            return -1;
        }

        qDebug() << ("libusb_init(NULL) ok \n");

        // 打开指定厂商的某类产品
        libusb_device_handle* pHandle = libusb_open_device_with_vid_pid(NULL, 0x5131, 0x2019);
        if (pHandle == NULL)
        {
            qDebug() << ("libusb_open_device_with_vid_pid(0x5131, 0x2019) failed \n");
            libusb_exit(NULL);
            return -1;
        }

        qDebug() << ("libusb_open_device_with_vid_pid(0x5131, 0x2019) ok \n");

        // 声明使用第1个接口
        nRet = libusb_claim_interface(pHandle, 0);
        if (nRet < 0)
        {
            qDebug() << "libusb_claim_interface(0) failed:" << libusb_strerror(nRet);
            libusb_close(pHandle);
            libusb_exit(NULL);
            return -1;
        }

        qDebug() << ("libusb_claim_interface(0) ok \n");

        // 向指定端点发送数据
        char sBuf[] = "1234567890";
        int nActualBytes = 0;
        nRet = libusb_bulk_transfer(pHandle, 0x01, (unsigned char *)sBuf, strlen(sBuf), &nActualBytes, 1000);
        if (nRet < 0)
        {
            qDebug() << "libusb_bulk_transfer(0x01) write failed:" << libusb_strerror(nRet);
            libusb_release_interface(pHandle, 0);
            libusb_close(pHandle);
            libusb_exit(NULL);
            return -1;
        }

        qDebug() << ("libusb_bulk_transfer(0x01) write size:[%d] \n", nActualBytes);

        // 从指定端点接收数据
        char sBuf2[128] = {0};
        nActualBytes = 0;
        nRet = libusb_bulk_transfer(pHandle, 0x81, (unsigned char *)sBuf2, sizeof(sBuf2), &nActualBytes, 1000);
        if (nRet < 0)
        {
            qDebug() << ("libusb_bulk_transfer(0x81) read failed:[%s] \n", libusb_strerror(nRet));
            libusb_release_interface(pHandle, 0);
            libusb_close(pHandle);
            libusb_exit(NULL);
            return -1;
        }

        qDebug() << ("libusb_bulk_transfer(0x81) read size:[%d] \n", nActualBytes);

        // 释放第1个接口
        libusb_release_interface(pHandle, 0);

        // 关闭设备
        libusb_close(pHandle);

        // 释放上下文
        libusb_exit(NULL);

        return 0;
}


void HIDAPIdebugger::HID_enum()
{

    findDevs();
}

