#include "virtdevice.hpp"

void DeviceManager::emit(int fd, int type, int code, int val)
{
    input_event ie;

    ie.type = type;
    ie.code = code;
    ie.value = val;
    /* timestamp values below are ignored */
    ie.time.tv_sec = 0;
    ie.time.tv_usec = 0;

    write(fd, &ie, sizeof(ie));
}

void DeviceManager::setupDevice(int& fd, const char* name, uinput_setup& usetup){
    memset(&usetup, 0, sizeof(usetup));

    usetup.id.bustype = BUS_USB;
    usetup.id.vendor = 0x1234;  /* sample vendor */
    usetup.id.product = 0x5678; /* sample product */
    strcpy(usetup.name, name);

    ioctl(fd, UI_DEV_SETUP, &usetup);
    ioctl(fd, UI_DEV_CREATE);
}

void DeviceManager::setupMouse(){
    fdMouse = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    
    ioctl(fdMouse, UI_SET_EVBIT, EV_KEY);
    ioctl(fdMouse, UI_SET_KEYBIT, BTN_LEFT);

    ioctl(fdMouse, UI_SET_EVBIT, EV_REL);
    ioctl(fdMouse, UI_SET_RELBIT, REL_X);
    ioctl(fdMouse, UI_SET_RELBIT, REL_Y);

    setupDevice(fdMouse, "Virtual mouse", usetupMouse);

    sleep(1);
}
void DeviceManager::setupKeyboard(const std::vector<int> keys){
    fdKeyboard = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    
    ioctl(fdKeyboard, UI_SET_EVBIT, EV_KEY);

    for(int key : keys){
        ioctl(fdKeyboard, UI_SET_KEYBIT, key);
    }

    setupDevice(fdKeyboard, "Virtual keyboard", usetupKeyboard);

    sleep(1);
}
void DeviceManager::cleanKeyboard(){
    sleep(1);

    ioctl(fdKeyboard, UI_DEV_DESTROY);
    close(fdKeyboard);
}
void DeviceManager::cleanMouse(){
    sleep(1);

    ioctl(fdMouse, UI_DEV_DESTROY);
    close(fdMouse);
}
void DeviceManager::pressKey(int key){
    emit(fdKeyboard, EV_KEY, key, 1);
    emit(fdKeyboard, EV_SYN, SYN_REPORT, 0);
    emit(fdKeyboard, EV_KEY, key, 0);
    emit(fdKeyboard, EV_SYN, SYN_REPORT, 0);
}
void DeviceManager::moveMouse(int dx, int dy){
    emit(fdMouse, EV_REL, REL_X, dx);
    emit(fdMouse, EV_REL, REL_Y, dy);
    emit(fdMouse, EV_SYN, SYN_REPORT, 0);
}

DeviceManager::DeviceManager(std::vector<int> keyboardButtons){
    setupKeyboard(keyboardButtons);
    setupMouse();
}

DeviceManager::~DeviceManager(){
    cleanKeyboard();
    cleanMouse();
}

// int main()
// {
//     DeviceManager manager({
//         KEY_A,
//         KEY_B,
//         KEY_C
//     });

//     for(int i = 0; i < 50; ++i){
//         manager.moveMouse(5, 5);
//         usleep(15000);
//     }

//     manager.pressKey(KEY_A);
//     manager.pressKey(KEY_B);
//     manager.pressKey(KEY_C);
// }