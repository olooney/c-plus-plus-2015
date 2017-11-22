#include <fstream>
#include "daemonize.h"
#include <thread>
#include <chrono>

void worker() {
    std::ofstream lout("worker.log");
    lout << "worker started" << std::endl;
    for ( int i=0; i<10; i++ ) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        lout << "worked for " << i+1 << " seconds" << std::endl;
    }
    lout << "worker is done." << std::endl;
    lout.close();
}

int main() {
    daemonize(worker, "/home/olooney/");
}
