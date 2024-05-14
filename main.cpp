#include <iostream>
#include <vector>
#include <math.h>
#include <random>
#include <thread>
#include <chrono>

double points_inside = 0;

double func(double x)
{
    return abs(sin(x) * x);
}

void count_points(double a, double b, double lower_bound, double upper_bound, int points_amount_local )
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
        if ((y > 0 && val > 0 && y < val) || (y < 0 && val < 0 && y > val))
        {
            local_points++;
        }
    }
    points_inside += local_points;
}

double monte_carlo(double a, double b, int thread_amount, int points_amount)
{
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
    return (upper_bound - lower_bound) * (b - a) * (points_inside / points_amount / thread_amount);
}
int main(){
    int threads, points;
    double a, b;
    std::cout << "Enter desirable thread amount: \n";
    std::cin >> threads;
    std::cout << "Enter the amount of points: \n";
    std::cin >> points;
    std::cout << "Enter function bounds" << '\n';
    std::cin >> a >> b;
    auto start = std::chrono::high_resolution_clock::now();
    std::cout << "answer: " << monte_carlo(a, b, threads, points) << '\n';
    std::cout << "time taken: "  << (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start)).count()/1000.0 << '\n';
    return 0;
}