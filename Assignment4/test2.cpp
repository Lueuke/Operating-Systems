#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <mutex>
#include <cmath>

// Mutexes for file writing
std::mutex even_mutex, odd_mutex, positive_mutex, negative_mutex, square_mutex, cube_mutex;

// Function to classify integers and write to appropriate files
void processIntegers(const std::vector<int>& numbers) {
    for (int num : numbers) {
        if (num % 2 == 0) {
            even_mutex.lock();
            std::ofstream even_file("even.out", std::ios_base::app);
            even_file << num << "\n";
            even_mutex.unlock();
        } else {
            odd_mutex.lock();
            std::ofstream odd_file("odd.out", std::ios_base::app);
            odd_file << num << "\n";
            odd_mutex.unlock();
        }

        if (num > 0) {
            positive_mutex.lock();
            std::ofstream positive_file("positive.out", std::ios_base::app);
            positive_file << num << "\n";
            positive_mutex.unlock();
        } else if (num < 0) {
            negative_mutex.lock();
            std::ofstream negative_file("negative.out", std::ios_base::app);
            negative_file << num << "\n";
            negative_mutex.unlock();
        }

        if (std::sqrt(num) == std::floor(std::sqrt(num))) {
            square_mutex.lock();
            std::ofstream square_file("square.out", std::ios_base::app);
            square_file << num << "\n";
            square_mutex.unlock();
        }

        if (std::cbrt(num) == std::floor(std::cbrt(num))) {
            cube_mutex.lock();
            std::ofstream cube_file("cube.out", std::ios_base::app);
            cube_file << num << "\n";
            cube_mutex.unlock();
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <path_to_source_file>\n";
        return 1;
    }

    std::ifstream input_file(argv[1]);
    if (!input_file) {
        std::cerr << "Error: Unable to open input file\n";
        return 1;
    }

    std::vector<int> numbers;
    int num;
    while (input_file >> num) {
        numbers.push_back(num);
    }

    // Dividing the numbers into two equal parts for processing by two threads
    size_t mid = numbers.size() / 2;
    std::vector<int> numbers1(numbers.begin(), numbers.begin() + mid);
    std::vector<int> numbers2(numbers.begin() + mid, numbers.end());

    // Create threads for processing
    std::thread t1(processIntegers, numbers1);
    std::thread t2(processIntegers, numbers2);

    // Join threads
    t1.join();
    t2.join();

    return 0;
}
