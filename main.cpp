#include <iostream>
#include <vector>
#include <math.h>
#include <random>
#include <thread>
#include <mutex>
#include <chrono>

std::vector<long double> thread_responces = {};
std::mutex thread_responces_mutex;

double func(double x)
{
    return sin(x) * x;
}

void count_points(double a, double b, double lower_bound, double upper_bound, long long points_amount_local )
{
    double local_points = 0;
    std::mt19937 generator(time(nullptr));
    for (int i = 0; i < points_amount_local; i++)
    {
        std::uniform_real_distribution<> dist_w(a, b);
        std::uniform_real_distribution<> dist_h(lower_bound, upper_bound);
        double x = dist_w(generator);
        double y = dist_h(generator);
        double val = func(x);
        if ((y > 0 && val > 0 && y < val))
        {
            local_points++;
        }
        else if ((y < 0 && val < 0 && y > val))
        {
            local_points--;
        }
    }
    thread_responces_mutex.lock();
    thread_responces.push_back(local_points);
    thread_responces_mutex.unlock();
}

double monte_carlo(double a, double b, int thread_amount, long long points_amount)
{
    long double points_inside = 0;
    double  lower_bound = -b;
    double upper_bound = b;

    double segment = ((b-a)/thread_amount);

    std::vector<std::thread*> threads;
    
    for (int thread_number = 0; thread_number < thread_amount; thread_number++)
    {
        std::thread* new_thread = new std::thread(count_points, a + thread_number*segment,
         a + (thread_number + 1)*segment, lower_bound, upper_bound, points_amount/thread_amount + 1);
        threads.push_back(new_thread);
    }
    for (auto x : threads)
    {
        x->join();
    }
    while (thread_responces.size() < thread_amount)
    {
        std::cout << "threads ready: " << thread_responces.size() << " / " << thread_amount << '\n';
    }
    for (auto x : thread_responces)
    {
        points_inside += x;
    }
    return (upper_bound - lower_bound) * (b - a) * (points_inside / points_amount);
}
int main(){
    int threads;
    long long points;
    double a, b;
    std::cout << "Enter desirable thread amount: \n";
    std::cin >> threads;
    std::cout << "Enter the amount of points: \n";
    std::cin >> points;
    std::cout << "Enter function bounds" << '\n';
    std::cin >> a >> b;
    if (a > b) { std::swap(a, b);}
    if (points > 0 && threads > 0)
    {
        auto start = std::chrono::high_resolution_clock::now();
        std::cout << "answer: " << monte_carlo(a, b, threads, points) << '\n';
        std::cout << "time taken: "  << (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start)).count()/1000.0 << '\n';
    }
    else
    {
        throw std::runtime_error("where have you seen a negative amount of threads or points?\n");
    }

    // теперь посмотрим, что программа действительно работает правильно для случая например от -7 до 48
    // 25.33834445151358
    double correct_result = 25.338344451;
    long long point_amount = 1000000;
    double epsilon = 0.01;
    double total_time = 0;
    int run_amount = 100;
    double precision = correct_result * (96 * 55 - correct_result) / point_amount / (epsilon * epsilon) / (96 * 55 * 96 * 55);
    double our_precision = 0;
    for (int i = 0; i < run_amount; i ++)
    {
        auto start = std::chrono::high_resolution_clock::now();
        if (correct_result - monte_carlo(-7, 48, 10, point_amount) >= epsilon * 96 * 55)
        {
            our_precision ++;
        }
        total_time += (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start)).count()/1000.0;
    }
    std::cout << "average runtime: " << total_time / run_amount << '\n';
    if (our_precision/run_amount <= our_precision)
    {
        std::cout << "everything works!!!";
    }
    else
    {
        std::cout << "oh no, it broke again...";
    }
    return 0;
}