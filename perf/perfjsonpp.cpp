#include "jsonpp.hpp"
#include <string>
#include <chrono>
#include <iostream>
#include <fstream>

template <class Clock, class TimeUnit, class F, class... Args>
    double measure(F&& func, Args&&... args)
{
    auto start = Clock::now();
    func(std::forward<Args>(args)...);
    auto end = Clock::now();
    return static_cast<double>(std::chrono::duration_cast<TimeUnit>(end - start).count());
}

template <class Clock, class TimeUnit, class F, class... Args>
double repeat(size_t reps, F&& func, Args&&... args)
{
    double total = 0;
    for (size_t i = 0; i < reps; ++i) {
        total += measure<Clock, TimeUnit>(std::forward<F>(func), std::forward<Args>(args)...);
    }
    return total;
}

size_t retrieveFileSize(const std::string& name)
{
    std::ifstream file(name);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open " + name);
    }

    file.seekg(0, SEEK_END);
    auto size = file.tellg();
    return static_cast<size_t>(size);
}

template <class T>
double KB(T bytes)
{
    return bytes / (double)1000;
}

template <class T>
double MB(T bytes)
{
    return bytes / (double)1000000;
}

std::string readFile(const std::string& name)
{
    auto size = retrieveFileSize(name);
    
    std::ifstream file(name);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open " + name);
    }

    std::cout << name << " is " << KB(size) << " KB " << MB(size) << " MB big!\n";

    std::string text, line;
    while (std::getline(file, line)) {
        text += (line + '\n');
    }
    return text;
}

void report(const std::string& name)
{
    std::cout << "Opening " << name << "\n";
    auto text = readFile(name);
    constexpr size_t reps = 10;
    auto totalTime = repeat<std::chrono::steady_clock, std::chrono::microseconds>(reps, json::parse, text);
    std::cout << "After parsing " << reps << " times, it took an average of " << totalTime / reps << " us " << (totalTime / 1000) / reps << " ms to parse " << name << ".\n";
}

int main()
{
    auto perfFiles = { "../tiny.json", "../small.json", "../medium.json", "../large.json" };
    for (auto f : perfFiles) {
        try {
            report(f);
        } catch (const std::exception& e) {
            std::cout << "EXCEPTION: " << e.what() << std::endl;
        }
    }
}