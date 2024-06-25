#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <mutex>

const double PI = 3.14159265358979323846;

std::mutex mtx; 

class WorkerBee {
public:
    WorkerBee(double v) : speed(v), returning(false) {
        x = initialX;
        y = initialY;
        targetX = 0;
        targetY = 0;
    }

    void update(double time) {
        double dx = targetX - x;
        double dy = targetY - y;
        double distance = sqrt(dx * dx + dy * dy);
        if (distance < speed * time) {
            x = targetX;
            y = targetY;
            returning = !returning;
            if (returning) {
                targetX = initialX;
                targetY = initialY;
            } else {
                targetX = 0;
                targetY = 0;
            }
        } else {
            x += speed * time * (dx / distance);
            y += speed * time * (dy / distance);
        }
    }

    void printPosition() const {
        std::lock_guard<std::mutex> guard(mtx);
        std::cout << "Worker Bee Position: (" << x << ", " << y << ")\n";
    }

private:
    double x, y;
    double speed;
    bool returning;
    double initialX = 100;  
    double initialY = 100;  
    double targetX, targetY;
};

class DroneBee {
public:
    DroneBee(double v, int n) : speed(v), changeDirectionTime(n) {
        x = 0;
        y = 0;
        direction = randomDirection();
        lastDirectionChange = 0;
    }

    void update(double time) {
        if (time - lastDirectionChange >= changeDirectionTime) {
            direction = randomDirection();
            lastDirectionChange = time;
        }

        x += speed * cos(direction) * time;
        y += speed * sin(direction) * time;
    }

    void printPosition() const {
        std::lock_guard<std::mutex> guard(mtx);
        std::cout << "Drone Bee Position: (" << x << ", " << y << ")\n";
    }

private:
    double x, y;
    double speed;
    int changeDirectionTime;
    double direction;
    double lastDirectionChange;

    double randomDirection() {
        return ((double)rand() / RAND_MAX) * 2 * PI;
    }
};

void workerBeeThread(WorkerBee& bee, double timeStep, double totalTime) {
    for (double time = 0; time <= totalTime; time += timeStep) {
        bee.update(timeStep);
        bee.printPosition();
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(timeStep * 1000)));
    }
}

void droneBeeThread(DroneBee& bee, double timeStep, double totalTime) {
    for (double time = 0; time <= totalTime; time += timeStep) {
        bee.update(timeStep);
        bee.printPosition();
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(timeStep * 1000)));
    }
}

int main() {
    srand(time(0));

    double workerBeeSpeed = 1.0;
    WorkerBee workerBee(workerBeeSpeed);

    double droneBeeSpeed = 1.0;
    int directionChangeInterval = 5;
    DroneBee droneBee(droneBeeSpeed, directionChangeInterval);

    double timeStep = 1.0;
    double totalTime = 20.0;

    std::thread workerThread(workerBeeThread, std::ref(workerBee), timeStep, totalTime);
    std::thread droneThread(droneBeeThread, std::ref(droneBee), timeStep, totalTime);

    workerThread.join();
    droneThread.join();

    return 0;
}
