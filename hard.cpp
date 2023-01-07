#include <iostream>
#include <fstream>
#include <vector>
#include "omp.h"

char convert(std::vector<uint32_t> &a, uint8_t pixel) {
    if (pixel <= a[0]) {
        return 0;
    } else if (pixel <= a[1]) {
        return 84;
    } else if (pixel <= a[2]) {
        return -86;
    } else {
        return -1;
    }
}

void calculate_threshold(uint32_t n, std::vector<uint32_t> &exp, std::vector<uint32_t> &prob,
                         std::vector<uint32_t> &answer, int num_thr) {

    double deviation = 0;
    double avg = (double) exp.back() / n;

#pragma omp parallel num_threads(num_thr) default(none) shared(exp, prob, deviation, n, avg, answer)
    {
#pragma omp for schedule(static)
        for (int th1 = 0; th1 < 253; th1++) {
            std::vector<uint32_t> thread_private_ans(3, 0);
            double thread_private_dev = 0;
            for (int th2 = th1 + 1; th2 < 254; th2++) {
                for (int th3 = th2 + 1; th3 < 255; th3++) {
                    double p1, p2, p3, p4;
                    double m1, m2, m3, m4;

                    p2 = (double) (prob[th2] - prob[th1]) / n;
                    p3 = (double) (prob[th3] - prob[th2]) / n;
                    p4 = (double) (prob.back() - prob[th3]) / n;
                    p1 = (double) prob[th1] / n;

                    m2 = (double) (exp[th2] - exp[th1]) / (prob[th2] - prob[th1]);
                    m3 = (double) (exp[th3] - exp[th2]) / (prob[th3] - prob[th2]);
                    m4 = (double) (exp.back() - exp[th3]) / (prob.back() - prob[th3]);
                    m1 = (double) exp[th1] / prob[th1];

                    double cur_deviation = p1 * (m1 - avg) * (m1 - avg) +
                                           p2 * (m2 - avg) * (m2 - avg) +
                                           p3 * (m3 - avg) * (m3 - avg) +
                                           p4 * (m4 - avg) * (m4 - avg);

                    if (cur_deviation > thread_private_dev) {
                        thread_private_dev = cur_deviation;
                        thread_private_ans[0] = th1;
                        thread_private_ans[1] = th2;
                        thread_private_ans[2] = th3;
                    }
                }
            }
#pragma omp critical
            if (thread_private_dev > deviation) {
                deviation = thread_private_dev;
                answer[0] = thread_private_ans[0];
                answer[1] = thread_private_ans[1];
                answer[2] = thread_private_ans[2];
            }
        }
    }
}

double measure_time(uint32_t n, std::vector<uint32_t> &exp, std::vector<uint32_t> &prob,
                    std::vector<uint32_t> &answer, int num_thr) {
    double avg_time = 0;
    for (int i = 0; i < 16; ++i) {
        double start = omp_get_wtime();

        calculate_threshold(n, exp, prob, answer, num_thr);

        avg_time += (omp_get_wtime() - start);
    }
    return avg_time / 16;
}

int main(int argc, char *argv[]) {
    std::ifstream in;
    in.open(R"(C:\Users\User\CLionProjects\itmo-comp-arch22-lab4-dayzGoBy\test_data\in.pgm)");

    std::string s;
    getline(in, s);

    uint32_t height, width;
    in >> width >> height;
    uint32_t n = height * width;

    getline(in, s);

    std::vector<std::vector<uint8_t>> image(width, std::vector<uint8_t>(height));
    std::vector<uint32_t> histogram(256, 0);
    std::vector<uint32_t> prob(256, 0); // prob[t] is the probability function (multiplied by img size)
    std::vector<uint32_t> exp(256, 0);

    for (int x = 0; x < height; ++x) {
        for (int y = 0; y < width; ++y) {
            uint8_t pixel;
            in.read((char *) &pixel, sizeof(uint8_t));
            image[y][x] = pixel;
            histogram[pixel]++;
        }
    }
    in.close();

    prob[0] = histogram[0];
    for (int t = 1; t < 256; ++t) {
        prob[t] = prob[t - 1] + histogram[t];
        exp[t] = t * histogram[t] + exp[t - 1];
    }

    std::vector<uint32_t> answer(3, UINT32_MAX);

    /*for(int num_thr = 1; num_thr < 12; num_thr++) {
        std::cout << measure_time(n, exp, prob, answer, num_thr) << std::endl;
    }*/
    calculate_threshold(n, exp, prob, answer, 12);

    std::cout << answer[0] << std::endl;
    std::cout << answer[1] << std::endl;
    std::cout << answer[2] << std::endl;

    std::ofstream out;
    out.open(R"(C:\Users\User\CLionProjects\itmo-comp-arch22-lab4-dayzGoBy\res.pgm)");

    out << "P5\n";
    out << width << " " << height << "\n";
    out << "255\n";

    for (int x = 0; x < height; x++) {
        for (int y = 0; y < width; y++) {
            char z = convert(answer, image[y][x]);
            out.write((char *) &z, sizeof(char));
        }
    }
    out.close();
}