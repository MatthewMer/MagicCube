// MagicCube.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <future>
#include <functional>
#include <queue>

#include "ArgParser.h"
#include "Threading.h"

using std::cout;
using std::vector;
using std::queue;
using std::iota;
using std::next_permutation;

struct result {
    bool is_magic = false;
    std::vector<size_t> data;
};

using result_t = result;

void update_progress(size_t const& total, size_t const& done) {
    double progress = (double)done / total;

    int barWidth = 70;

    std::cout << "[";
    int pos = barWidth * progress;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << " %\r";
    std::cout.flush();
}

bool check_dim(vector<size_t> const& data, args const& r, size_t const& dim, std::vector<size_t> vec) {
    if (dim == 0) { return true; }

    auto offsets = std::vector<size_t>(r.k);
    auto dim_ = dim - 1;

    for (size_t i = 0; i < r.k; ++i) {
        offsets[i] = static_cast<size_t>(pow(r.n, i)) * vec[i];
    }
    auto offset = static_cast<size_t>(pow(r.n, dim_));

    for (size_t i = 0; i < r.n; ++i) {
        vec[dim_] = i;
        if (!check_dim(data, r, dim_, vec)) { return false; }
    }

    auto magic = true;
    for (size_t j = 0; j < offset; ++j) {       //
        size_t sum = 0;
        for (size_t i = 0; i < r.n; ++i) {
            offsets[dim_] = offset * i + j;
            size_t index = 0;
            for (size_t j = 0; j < r.k; ++j) {
                index += offsets[j];
            }
            sum += data[index];
            //cout << data[index] << " + ";
        }
        //cout << " = " << sum << "\n";
        magic = magic && sum == r.M;
        if (!magic) { break; }
    }
    return magic;
}

int main(int argc, char* argv[])
{
    auto r = args{};
    if (parse(argc, argv, r)) {
        size_t set_size = static_cast<size_t>(pow(r.n, r.k));
        r.M = (r.n * (set_size + 1)) / 2;

        auto data = vector<size_t>(set_size);
        iota(data.begin(), data.end(), 1);

        Threading::ThreadPool<result_t> pool(32);

        cout << "Dimension k =\t" << r.k << "\n";
        cout << "Order n =\t" << r.n << "\n";
        cout << "M =\t\t" << r.M << "\n";
        cout << "Set =\t\t[ ";
        for (auto const& n : data) {
            cout << n << " ";
        }
        cout << "]\n";
        cout << "Thread count =\t" << pool.ThreadCount() << "\n";

        std::mutex mut;
        std::condition_variable cv;

        std::mutex mut_cout;

        std::unique_lock<std::mutex> lock(mut);

        std::thread t1 = std::thread([&]() {
            std::unique_lock lock(mut);

            do {
                pool.Enqueue(
                    [=]() -> result_t {
                    auto vec = std::vector<size_t>(r.k, 0);
                    auto res = result_t{};
                    res.is_magic = check_dim(data, r, r.k, vec);

                    if (res.is_magic) {
                        res.data = data;
                    }
                    return res;
                    },
                    [&](result_t& res) {
                        if (res.is_magic) {
                            std::unique_lock<std::mutex> lock(mut_cout);
                            cout << "[ ";
                            for (size_t i = 0; i < res.data.size(); ++i) {
                                cout << res.data[i] << " ";
                            }
                            cout << "]\n";
                        }
                    }
                );
            } while (next_permutation(data.begin(), data.end()));

            cv.notify_one();
            });

        cv.wait(lock);
        if (t1.joinable()) {
            t1.join();
        }

        pool.Wait();
        pool.Stop();
        return 0;
    }
}
