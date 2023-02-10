//
//  main.cpp
//  Multi1
//
//  Created by Serap Taş on 10.02.2023.
//

/*
#include <iostream>
#include <fstream>
#include <thread>

void workerFunction(int N, std::string filename)
{
    int sum = 0;
    for (int i = 1; i <= N; ++i)
    {
        sum += i;
    }

    std::ofstream file(filename); // create and write info to file
    file << "Sum of the first " << N << " positive integers: " << sum << std::endl;
    file.close();

    std::cout << "Result written to file " << filename << " in thread id " << std::this_thread::get_id() << std::endl;
}




void workerFunction2(int N, std::string filename)
{
    int sum = 0;
    for (int i = 1; i <= N; ++i)
    {
        sum += i;
    }

    std::ofstream file(filename); // create and write info to file
    file << "Sum of the first " << N << " positive integers: " << sum << std::endl;
    file.close();

    std::cout << "Result written to file " << filename << " in thread id " << std::this_thread::get_id() << std::endl;
}

int main()
{
    int N = 100;
    std::string filename = "result.txt";

    std::thread worker(workerFunction, N, filename);

    std::cout << "Worker thread launched from main with id " << worker.get_id() << std::endl;
    
    int N2 = 10;
    std::thread worker2(workerFunction2, N2, filename);

    std::cout << "Worker thread launched from main with id " << worker2.get_id() << std::endl;


    // The join method is called in the main thread to wait for the worker thread to complete before the main thread terminates.
    worker.join();
    worker2.join();
    std::cout << "Worker thread joined." << std::endl;

    return 0;
}
*/



#include <iostream>
#include <fstream>
#include <thread>

void sum_of_numbers(int start, int end, std::string filename) {
    int sum = 0;
    for (int i = start; i <= end; i++) {
        sum += i;
    }
    
    std::ofstream outfile;
    outfile.open(filename, std::ios::app);
    outfile << "Sum of numbers from " << start << " to " << end << ": " << sum << std::endl;
    outfile.close();
    
    std::cout << "Sum of numbers from " << start << " to " << end << ": " << sum << std::endl;
}


int main() {
    std::thread first (sum_of_numbers, 1, 50, "sum_of_numbers.txt");
    std::thread second (sum_of_numbers, 51, 100, "sum_of_numbers.txt");
    
    std::cout << "Running both threads..." << std::endl;
    
    
    // The join method is called in the main thread to wait for the worker thread to complete before the main thread terminates.
    first.join();
    second.join();
    
    std::cout << "Both threads have finished." << std::endl;
    
    return 0;
}

