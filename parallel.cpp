// Parallel.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

// System Headers.
#include <iostream>
#include <chrono>
#include <thread>
#include <functional>
#include <cmath>
#include <mutex>

// Application Headers.
#include "thread_pool.h"



// Mutex for the showNumber() function.
std::mutex mutexShowNumber_;



// Pause the thread for a short time period.
void pause()
{
    // True to sleep, false for actual calculations.
    const bool SLEEP = true;

    // Pause for a little while.
    if (SLEEP)
    {
        // Sleep for 1 second.
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    else
    {
        // Do actual work that hurts the processor.
        for (int i = 0; i < 10000000; i++)
        {
            double result = sqrt((double)i);
            result = pow(result, 0.78);
            result = pow(result, 1.32);
            result = pow(result, 0.57);
        }
    }
}



// Write the number to the console and pause for a little while.
void showNumber
(
    int number
)
{
    // Wait / Work for a while.
    pause();

    // Wait for all the other showNumber() functions to finish.
    std::unique_lock <std::mutex> lock(mutexShowNumber_);

    // Write the number to console.
    std::cout << number << " ";
    std::cout.flush();

    // Release the lock.
    lock.unlock();

    // Wait / Work for a while.
    pause();

    // Wait for all the other showNumber() functions to finish.
    lock.lock();

    // Write the number to console.
    std::cout << number << " ";
    std::cout.flush();
}



// Compare parallel loops and serial loops.
int main()
{
    // The number loops for the test.
    const int NUM_LOOPS = 30;

    std::cout << "Hello from Parallel.\n";

    // Call the function n times in serial.
    for (int i = 0; i < NUM_LOOPS; i++)
    {
        showNumber(i);
    }
    std::cout << std::endl;

    // Call the function n times on a new thread each time.
    std::thread* myThreads[NUM_LOOPS];
    for (int i = 0; i < NUM_LOOPS; i++)
    {
        myThreads[i] = new std::thread(showNumber, i);
    }

    // Wait for each thread to finish.
    for (int i = 0; i < NUM_LOOPS; i++)
    {
        // Wait for this thread to finish.
        myThreads[i]->join();
    }
    std::cout << std::endl;

    // Call the function n times with a thread pool objecct.
    ThreadPool threadPool(4);
    for (int i = 0; i < NUM_LOOPS; i++)
    {
        threadPool.addTask(std::bind(showNumber, i));
    }
    threadPool.waitAllFinish();
    threadPool.waitAllFinish();
    std::cout << std::endl;

    // Lets go again with the thread pool.
    threadPool.stopMultithreading();
    for (int i = 0; i < NUM_LOOPS; i++)
    {
        threadPool.addTask(std::bind(showNumber, i));
    }
    threadPool.waitAllFinish();
    threadPool.startMultithreading();
    std::cout << std::endl;

    // Lets go again with the thread pool.
    for (int i = 0; i < NUM_LOOPS; i++)
    {
        threadPool.addTask(std::bind(showNumber, i));
    }
    threadPool.waitAllFinish();
    std::cout << std::endl;

    std::cout << "Goodbye." << std::endl;
}
