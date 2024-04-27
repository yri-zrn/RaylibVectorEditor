#include "RaylibApplication.hpp"

int main(int, char**) {
    cw::RaylibApplication* app = new cw::RaylibApplication();
    app->Run();
    return 0;
}