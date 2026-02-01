#include <linux/uinput.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <vector>

class DeviceManager{
    public:
    DeviceManager(std::vector<int> keyboardButtons = {});
    ~DeviceManager();
    void pressKey(int key);
    void moveMouse(int dx, int dy);

    private:
    void emit(int fd, int type, int code, int val);
    void setupDevice(int& fd, const char* name, uinput_setup& usetup);
    void setupMouse();
    void setupKeyboard(const std::vector<int> keys);
    void cleanKeyboard();
    void cleanMouse();
    
    uinput_setup usetupMouse, usetupKeyboard;
    int fdMouse, fdKeyboard;
};