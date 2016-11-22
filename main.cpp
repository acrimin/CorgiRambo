// Brian Malloy        Game Construction
#include "manager.h"


int main(int, char*[]) {
    srand(time(NULL));
    try {
        bool repeat = true;
        while (repeat) {
            Manager manager;
            repeat = manager.play();
        }
    }
    catch (const string& msg) {
        std::cout << msg << std::endl;
    }
    catch (...) {
        std::cout << "Oops, someone threw an exception!" << std::endl;
    }
    return 0;
}
