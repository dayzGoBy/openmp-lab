#include <iostream>
#include <fstream>
#include <vector>
#include <string>
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

#pragma omp parallel num_threads(num_thr) default(shared)
    {
        std::vector<uint32_t> thread_private_ans(3, 0);
        double thread_private_dev = 0;
#pragma omp for schedule(dynamic)
        for (int th1 = 0; th1 < 254; th1++) {
            for (int th2 = th1 + 1; th2 < 255; th2++) {
                for (int th3 = th2 + 1; th3 < 256; th3++) {
                    uint32_t p1, p2, p3, p4;
                    double m1, m2, m3, m4;

                    p1 = prob[th1];
                    p2 = prob[th2] - prob[th1];
                    p3 = prob[th3] - prob[th2];
                    p4 = prob.back() - prob[th3];

                    m1 = (double) exp[th1] / prob[th1];
                    m2 = (double) (exp[th2] - exp[th1]) / (prob[th2] - prob[th1]);
                    m3 = (double) (exp[th3] - exp[th2]) / (prob[th3] - prob[th2]);
                    m4 = (double) (exp.back() - exp[th3]) / (prob.back() - prob[th3]);

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
        }
#pragma omp critical
        {
            if (thread_private_dev > deviation) {
                deviation = thread_private_dev;
                answer[0] = thread_private_ans[0];
                answer[1] = thread_private_ans[1];
                answer[2] = thread_private_ans[2];
            }
        }
    }
}

void calculate_threshold_no_omp(uint32_t n, std::vector<uint32_t> &exp, std::vector<uint32_t> &prob,
                                std::vector<uint32_t> &answer) {

    double deviation = 0;
    double avg = (double) exp.back() / n;

    for (int th1 = 0; th1 < 254; th1++) {
        for (int th2 = th1 + 1; th2 < 255; th2++) {
            for (int th3 = th2 + 1; th3 < 256; th3++) {
                uint32_t p1, p2, p3, p4;
                double m1, m2, m3, m4;

                p1 = prob[th1];
                p2 = prob[th2] - prob[th1];
                p3 = prob[th3] - prob[th2];
                p4 = prob.back() - prob[th3];

                m1 = (double) exp[th1] / prob[th1];
                m2 = (double) (exp[th2] - exp[th1]) / (prob[th2] - prob[th1]);
                m3 = (double) (exp[th3] - exp[th2]) / (prob[th3] - prob[th2]);
                m4 = (double) (exp.back() - exp[th3]) / (prob.back() - prob[th3]);

                double cur_deviation = p1 * (m1 - avg) * (m1 - avg) +
                                       p2 * (m2 - avg) * (m2 - avg) +
                                       p3 * (m3 - avg) * (m3 - avg) +
                                       p4 * (m4 - avg) * (m4 - avg);

                if (cur_deviation > deviation) {
                    deviation = cur_deviation;
                    answer[0] = th1;
                    answer[1] = th2;
                    answer[2] = th3;
                }
            }
        }
    }

}

double measure_time(uint32_t n, std::vector<uint32_t> &exp, std::vector<uint32_t> &prob,
                    std::vector<uint32_t> &answer, int num_thr) {
    double avg_time = 0;
    for (int i = 0; i < 16; ++i) {
        double start = omp_get_wtime();

        num_thr == -1 ? calculate_threshold_no_omp(n, exp, prob, answer)
                        : calculate_threshold(n, exp, prob, answer, num_thr);

        avg_time += (omp_get_wtime() - start);
    }
    return avg_time * 1000 / 16;
}

int main(int argc, char *argv[]) {
    std::ifstream in;
    in.open(argv[2]);

    if (!in.is_open()) {
        std::cout << "error while opening input file" << std::endl;
        exit(1);
    }

    std::ofstream out;
    out.open(argv[3]);

    if (!out.is_open()) {
        std::cout << "error while opening output file" << std::endl;
        exit(1);
    }

    std::string s;
    getline(in, s);

    uint32_t height, width;
    in >> width >> height;
    uint32_t n = height * width;

    getline(in, s);

    std::vector<std::vector<uint8_t>> image(width, std::vector<uint8_t>(height));
    std::vector<uint32_t> histogram(256, 0);
    std::vector<uint32_t> prob(256, 0);
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

    int num_thr = std::stoi(argv[1]);

    printf("Time (%i thread(s)): %g ms\n", num_thr,
           measure_time(n, exp, prob, answer, num_thr));


    std::cout << answer[0] << std::endl;
    std::cout << answer[1] << std::endl;
    std::cout << answer[2] << std::endl;

    out << "P5\n";
    out << width << " " << height << "\n";
    out << "255\n";

    for (int x = 0; x < height; x++) {
        for (int y = 0; y < width; y++) {
            char z = convert(answer, image[y][x]);
            out.write(&z, sizeof(char));
        }
    }
    out.close();
}