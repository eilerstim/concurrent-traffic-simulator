#include <iostream>
#include <future>
#include <thread>
#include "TrafficLight.h"
#include "Generator.h"

/* Implementation of class "MessageQueue" */


template <typename T>
T MessageQueue<T>::receive()
{
    //perform queue modification under the lock
    std::unique_lock<std::mutex> uLock (_mutex);
    //pass unqiue lock to condition variable
    _condition.wait(uLock, [this] () {return !_queue.empty();});
    //remove first element from queue
    T msg = std::move(_queue.front());
    _queue.pop_front();
    return msg; // will not be copied due to RVO

}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{

    //perform queue modification under the lock
    std::lock_guard<std::mutex> uLock (_mutex);
    _queue.clear();
    //add element to queue
    _queue.emplace_back(msg);
    _condition.notify_one();
}


/* Implementation of class "TrafficLight" */


TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    while(true){
        TrafficLightPhase phase = _messageQueue.receive();
        if (phase == green) return;
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{



    double cycleDuration = RNG::dist(RNG::rng); // duration of a phase cycle in seconds

    // init stop watch
    auto lastUpdate = std::chrono::system_clock::now();
    long timeSinceLastUpdate {0};
    while (true)
    {
        // sleep at every iteration to reduce CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        // compute time difference to stop watch
        timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - lastUpdate).count();
        if (timeSinceLastUpdate >= cycleDuration) {
            _currentPhase = (_currentPhase == red) ? green : red;

            _messageQueue.send(std::move(_currentPhase));

            // reset stop watch for next cycle
            lastUpdate = std::chrono::system_clock::now();
            // re-roll cycle duration
            cycleDuration = RNG::dist(RNG::rng);
        }

    }
}