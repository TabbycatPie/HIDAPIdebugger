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
    connect(ui->btn_test,&QPushButton::clicked,this,[=]{
        HID_test();
    });
}

HIDAPIdebugger::~HIDAPIdebugger()
{
    delete ui;
}


int test(){
    // 初使化上下文
        int nRet = libusb_init(NULL);
        if (nRet < 0)
        {
            qDebug() << "libusb_init(NULL) failed:[%s] \n" << libusb_strerror(nRet);
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

        // 声明使用第3个接口
        nRet = libusb_claim_interface(pHandle, 2);
        if (nRet < 0)
        {
            qDebug() << "libusb_claim_interface(3) failed:" << libusb_strerror(nRet);
            libusb_close(pHandle);
            libusb_exit(NULL);
            return -1;
        }

        qDebug() << ("libusb_claim_interface(0) ok \n");

        // 向指定端点发送数据
        char sBuf[] = "1234567890";
        int nActualBytes =0;
        nRet = libusb_bulk_transfer(pHandle, 0x00, (unsigned char *)sBuf, strlen(sBuf), &nActualBytes, 1000);
        if (nRet < 0)
        {
            qDebug() << "libusb_bulk_transfer(0x03) write failed:" << libusb_strerror(nRet) << nRet;
            libusb_release_interface(pHandle, 0);
            libusb_close(pHandle);
            libusb_exit(NULL);
            return -1;
        }

        qDebug() << ("libusb_bulk_transfer(0x01) write size:[%d] \n", nActualBytes);

        // 从指定端点接收数据
        char sBuf2[128] = {0};
        nActualBytes = 0;
        nRet = libusb_bulk_transfer(pHandle, 0x00, (unsigned char *)sBuf2, sizeof(sBuf2), &nActualBytes, 1000);
        if (nRet < 0)
        {
            qDebug() << "libusb_bulk_transfer(0x81) read failed:[%s] \n" << libusb_strerror(nRet);
            libusb_release_interface(pHandle, 0);
            libusb_close(pHandle);
            libusb_exit(NULL);
            return -1;
        }

        qDebug() << "libusb_bulk_transfer(0x81) read size:[%d] \n" << nActualBytes;

        // 释放第1个接口
        libusb_release_interface(pHandle, 0);

        // 关闭设备
        libusb_close(pHandle);

        // 释放上下文
        libusb_exit(NULL);

        return 0;
}

static int device_satus(libusb_device_handle *hd)
{

    int interface = 0;
    unsigned char byte;
    libusb_control_transfer(hd, LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE,
            LIBUSB_REQUEST_CLEAR_FEATURE,
            0,
            interface,
            &byte, 1, 5000);

    qDebug() << "status:" << byte;
/**
 * byte
 * normal:0x18
 * other :0x10
 */
    return 0;
}


QString getDevInfoString(libusb_device* dev){
    libusb_device_descriptor dev_desc;
    int r = libusb_get_device_descriptor(dev,&dev_desc);
    if(r < 0)
        return "";
    else{
        QString desc_str = "";
        desc_str +=  "Port num            :" + QString().asprintf("%d",libusb_get_port_number(dev));
        desc_str += "\nSpeed              :" + QString().asprintf("%d",libusb_get_device_speed(dev));
        desc_str += "\nAddress            :" + QString().asprintf("%d",libusb_get_device_address(dev));
        desc_str += "\nBus num            :" + QString().asprintf("%d",libusb_get_bus_number(dev));
        desc_str +="\nVID                 :" + QString().asprintf("0x%x",dev_desc.idVendor);
        desc_str +="\nPID                 :" + QString().asprintf("0x%x",dev_desc.idProduct);
        desc_str +="\nManufacturer        :" + QString().asprintf("%d",dev_desc.iManufacturer);
        desc_str +="\nSerialNumber        :" + QString().asprintf("%d",dev_desc.iSerialNumber);
        desc_str +="\nMaxPacketSize       :" + QString().asprintf("%d",dev_desc.bMaxPacketSize0);
        desc_str +="\nNumConfigurations   :" + QString().asprintf("%d",dev_desc.bNumConfigurations);
        //get device config list
        QString config_str = "";
        struct libusb_config_descriptor *dev_config_list;
        int ret = libusb_get_config_descriptor(dev,1,&dev_config_list);
        if(ret >= 0){
            config_str += "\n    NumInterfaces     :"+QString::number(dev_config_list->bNumInterfaces);
            config_str += "\n    Max power         :"+QString::number(dev_config_list->MaxPower);
        }
        desc_str += config_str;
        return desc_str + "\n";
    }
}

int findDevs(){
    libusb_device **devs; //pointer to pointer of device, used to retrieve a list of devices
    libusb_device_handle *dev_handle; //a device handle
    libusb_context *ctx = NULL; //a libusb session
    int r = 0; //for return values
    ssize_t cnt; //holding number of devices in list
    r = libusb_init(&ctx); //initialize the library for the session we just declared
    if(r < 0) {
        perror("Init Error\n"); //there was an error
        return 1;
    }
    libusb_set_debug(ctx, LIBUSB_LOG_LEVEL_INFO); //set verbosity level to 3, as suggested in the documentation

    cnt = libusb_get_device_list(ctx, &devs); //get the list of devices
    if(cnt < 0) {
        perror("Get Device Error\n"); //there was an error
        return 1;
    }
    qDebug() << cnt <<" Devices in list.\n";

    dev_handle = libusb_open_device_with_vid_pid(ctx, 0x5131, 0x2019); //these are vendorID and productID I found for my usb device
    if(dev_handle == NULL)
        perror("Cannot open device\n");
    else
        qDebug() <<("Device Opened\n");
    libusb_free_device_list(devs, 1); //free the list, unref the devices in it

    if(libusb_kernel_driver_active(dev_handle, 0) == 1) { //find out if kernel driver is attached
        qDebug() <<("Kernel Driver Active\n");
        if(libusb_detach_kernel_driver(dev_handle, 0) == 0) //detach it
            qDebug() <<("Kernel Driver Detached!\n");
    }
    r = libusb_claim_interface(dev_handle, 0); //claim interface 0 (the first) of device (mine had jsut 1)
    if(r < 0) {
        perror("Cannot Claim Interface\n");
        return 1;
    }
    qDebug() <<("Claimed Interface\n");

    device_satus(dev_handle);

    libusb_close(dev_handle); //close the device we opened
    libusb_exit(ctx); //needs to be called to end the


}

void HIDAPIdebugger::HID_test(){
    test();
}


void HIDAPIdebugger::HID_enum()
{
    libusb_device **devs_list;
    libusb_context *ctx = NULL; //a libusb session
    QString descs = "";

    //init libusb
    int r = libusb_init(&ctx); //initialize the library for the session we just declared
    if(r < 0) {
        qDebug() << "libusb_init(&ctx) Error:" + QString().number(r);
        return;
    }
    qDebug() << "libusb_init(&ctx) Success.";

    //get device list
    int dev_num = libusb_get_device_list(ctx,&devs_list);
    if(dev_num < 0){
        qDebug() << "libusb_get_device_list(ctx,&devs_list) Error:" + QString().number(dev_num);
        return;
    }

    //print devices
    for(int d = 0;d<dev_num;d++){
        descs += getDevInfoString(devs_list[d]);
    }


    //show on textview
    ui->tv_log->setText(ui->tv_log->toPlainText()+descs);

    libusb_device_handle *dev_handle; //a device handle
    dev_handle = libusb_open_device_with_vid_pid(ctx, 0x5131, 0x2019); //these are vendorID and productID I found for my usb device
    if(dev_handle == NULL)
        qDebug() <<("Cannot open device\n");
    else
        qDebug() <<("Device Opened\n");
    libusb_free_device_list(devs_list, 1); //free the list, unref the devices in it
    r = libusb_claim_interface(dev_handle, 3);
    if(r < 0) {
        qDebug() <<("Cannot Claim Interface\n");
        return;
    }
    qDebug() <<("Claimed Interface\n");

}

