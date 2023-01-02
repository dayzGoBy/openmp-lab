#include <iostream>
#include <random>
#include "omp.h"

int main(int argc, char *argv[]) {
    int r, n;
    std::cin >> r >> n;

    int hits = 0;
    int total = 0;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> un(-r, r);

    // we need to create separate generators for each thread
    omp_set_num_threads(1);
    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        double x = un(gen);
        double y = un(gen);
        #pragma omp atomic
        hits += (x * x + y * y <= r * r);
        #pragma omp atomic
        total++;
        //std::cout << hits << " " << total << "\n";
    }
    std::cout << (double) hits * 4 / total;
    return 0;
}