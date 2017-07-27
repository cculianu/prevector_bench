#include "prevector.h"
#include <vector>
#include <iostream>
#include <sstream>
#include <functional>
#include <list>
#include <thread>
#include <atomic>

double getTime();

typedef std::function<std::vector<int> *(void)> VectorSwapNextFunc;

static double vector_test(int n, int m, unsigned reserve, const VectorSwapNextFunc & func = VectorSwapNextFunc())
{
    double t0 = getTime();
    for (int i = 0; i < n; ++i) {
        std::vector<int> v;
        if (reserve) v.reserve(reserve);
        for (int j = 0; j < m; ++j)
            v.push_back(j|i);
        std::vector<int> *swap = 0;
        if (func && (swap=func())) swap->swap(v);
    }
    return getTime()-t0;
}

template <int rsv>
using PrevectorSwapNextFunc = std::function<prevector<rsv,int> *(void)>;

template <int rsv> double prevector_test(int n, int m, const PrevectorSwapNextFunc<rsv> & func = PrevectorSwapNextFunc<rsv>())
{
    double t0 = getTime();
    for (int i = 0; i < n; ++i) {
        prevector<rsv, int> v;
        for (int j = 0; j < m; ++j)
            v.push_back(j|i);
        prevector<rsv, int> *swap = 0;
        if (func && (swap=func())) swap->swap(v);
    }
    return getTime()-t0;
}

struct Log : public std::ostringstream
{
    Log() : std::ostringstream() {}
    Log(const std::string &s) : std::ostringstream() { (*this) << s; }
    ~Log() {
        std::string s(str());
        flush();
        mut.lock();
        std::cout << s;
        std::cout.flush();
        mut.unlock();
    }

private:
    static std::mutex mut;
};

/*static*/ std::mutex Log::mut;

void test1()
{
    static const int n = 1000000, m = 1000;
    Log() << __func__ << ": comparing vector versus prevector for " << m << " push_backs, " << n << " times..." << std::endl;
    double tp = prevector_test<m>(n,m);
    double tv = vector_test(n,m,m);
    double tvnr = vector_test(n,m,0);

    Log() << __func__ << ": RESULT - vector=" << tv << "s, vector_no_reserve=" << tvnr << "s, prevector=" << tp << "s" << std::endl;
}


void test1b()
{
    static const int n = 10000000, m = 100, rsv=12;
    Log() << __func__ << ": comparing vector versus prevector for " << m << " push_backs, " << n << " times, with reserve size=" << rsv << "..." << std::endl;
    double tp = prevector_test<rsv>(n,m);
    double tv = vector_test(n,m,rsv);
    double tvnr = vector_test(n,m,0);

    Log() << __func__ << ": RESULT - vector=" << tv << "s, vector_no_reserve=" << tvnr << "s, prevector=" << tp << "s" << std::endl;
}

void test2()
{
    static const int n = 1000000, m = 5000, rsv = 100;
    Log() << __func__ << ": comparing vector versus prevector for " << m << " push_backs, " << n << " times, with reserve size=" << rsv << "..." << std::endl;
    double tp = prevector_test<rsv>(n,m);
    double tv = vector_test(n,m,rsv);
    double tvnr = vector_test(n,m,0);

    Log() << __func__ << ": RESULT - vector=" << tv << "s, vector_no_reserve=" << tvnr << "s, prevector=" << tp << "s" << std::endl;
}

void test2b()
{
    static const int n = 50000000, m = 102, rsv = 97;
    Log() << __func__ << ": comparing vector versus prevector for " << m << " push_backs, " << n << " times, with reserve size=" << rsv << "..." << std::endl;
    double tp = prevector_test<rsv>(n,m);
    double tv = vector_test(n,m,rsv);
    double tvnr = vector_test(n,m,0);

    Log() << __func__ << ": RESULT - vector=" << tv << "s, vector_no_reserve=" << tvnr << "s, prevector=" << tp << "s" << std::endl;
}


void test3()
{
    static const int n = 100000, m = 50000, rsv = 48;
    Log() << __func__ << ": comparing vector versus prevector for " << m << " push_backs, " << n << " times, with reserve size=" << rsv << "..." << std::endl;
    double tp = prevector_test<rsv>(n,m);
    double tv = vector_test(n,m,rsv);
    double tvnr = vector_test(n,m,0);

    Log() << __func__ << ": RESULT - vector=" << tv << "s, vector_no_reserve=" << tvnr << "s, prevector=" << tp << "s" << std::endl;
}

void test4()
{
    static const int n = 100000, m = 75000, rsv = 48, swapsz = 100;
    Log() << __func__ << ": comparing vector versus prevector for " << m << " push_backs, " << n << " times, with reserve size=" << rsv << ", with swapping="<<swapsz<<"..." << std::endl;
    std::vector<prevector<rsv,int> > swaps; swaps.resize(swapsz);
    int ct = 0;
    auto lambdapv = [&]()->prevector<rsv,int>*{
        return &swaps[ct++%swaps.size()];
    };
    double tp = prevector_test<rsv>(n,m,lambdapv);
    std::vector<std::vector<int> > swaps2; swaps2.resize(swapsz);
    ct = 0;
    auto lambda = [&]()->std::vector<int> *{
            return &swaps2[ct++%swaps2.size()];
    };
    double tv = vector_test(n,m,rsv,lambda);
    ct = 0;
    double tvnr = vector_test(n,m,0,lambda);

    Log() << __func__ << ": RESULT - vector=" << tv << "s, vector_no_reserve=" << tvnr << "s, prevector=" << tp << "s" << std::endl;
}

void test5()
{
    static const int n = 100000, m = 75000, rsv = 48, swapsz = 100;
    Log() << __func__ << ": vector first -- comparing vector versus prevector for " << m << " push_backs, " << n << " times, with reserve size=" << rsv << ", with swapping="<<swapsz<<"..." << std::endl;
    int ct = 0;
    std::vector<std::vector<int> > swaps2; swaps2.resize(swapsz);
    auto lambda = [&]()->std::vector<int> *{
            return &swaps2[ct++%swaps2.size()];
    };
    ct = 0;
    double tv = vector_test(n,m,rsv,lambda);
    ct = 0;
    double tvnr = vector_test(n,m,0,lambda);

    std::vector<prevector<rsv,int> > swaps; swaps.resize(swapsz);
    auto lambdapv = [&]()->prevector<rsv,int>*{
        return &swaps[ct++%swaps.size()];
    };
    ct = 0;
    double tp = prevector_test<rsv>(n,m,lambdapv);

    Log() << __func__ << ": RESULT - vector=" << tv << "s, vector_no_reserve=" << tvnr << "s, prevector=" << tp << "s" << std::endl;
}

void test6()
{
    static const int n = 10000000, m = 100, rsv=12, swapsz=250000;
    Log() << __func__ << ": comparing vector versus prevector for " << m << " push_backs, " << n << " times, with reserve size=" << rsv << ", with swapping="<<swapsz<<"..." << std::endl;
    int ct = 0;
    std::vector<std::vector<int> > swaps2; swaps2.resize(swapsz);
    auto lambda = [&]()->std::vector<int> *{
            return &swaps2[ct++%swaps2.size()];
    };
    ct = 0;
    double tv = vector_test(n,m,rsv,lambda);
    ct = 0;
    double tvnr = vector_test(n,m,0,lambda);

    std::vector<prevector<rsv,int> > swaps; swaps.resize(swapsz);
    auto lambdapv = [&]()->prevector<rsv,int>*{
        return &swaps[ct++%swaps.size()];
    };
    ct = 0;
    double tp = prevector_test<rsv>(n,m,lambdapv);

    Log() << __func__ << ": RESULT - vector=" << tv << "s, vector_no_reserve=" << tvnr << "s, prevector=" << tp << "s" << std::endl;
}


int main(void)
{
    int nthreads = std::thread::hardware_concurrency();
    if (!nthreads) nthreads = 1;
    if (nthreads > 4) nthreads = 4;
    typedef std::function<void(void)> TestFunc;
    std::list<std::thread> threads;
    std::vector<TestFunc> funcs = { test1, test1b, test2, test2b, test3, test4, test5, test6 };
    std::atomic<int> fctr(0);

    if (nthreads > 1)
        Log() << "Running " << funcs.size() << " tests in " << nthreads << " concurrent threads" << std::endl;
    else
        Log() << "Running " << funcs.size() << " tests" << std::endl;

    for (int i = 0; i < nthreads; ++i) {
        threads.emplace_back([i,&funcs,nthreads,&fctr]{
            //Log() << "Thread " << i << " started " << std::endl;
            int fidx;
            while ((fidx = fctr++) < (int)funcs.size())
                funcs[fidx]();
            //Log() << "Thread " << i << " ended " << std::endl;
        });
    }
    for (auto & thread : threads) {
        thread.join();
    }
    return 0;
}

/* ---------------------- PLATFORM STUFF ------------------------*/

#if defined(OSX)
#include <mach/mach_time.h>
#include <stdint.h>
double getTime()
{
        double t = static_cast<double>(mach_absolute_time());
        struct mach_timebase_info info;
        mach_timebase_info(&info);
        return t * (1e-9 * static_cast<double>(info.numer) / static_cast<double>(info.denom) );
}
#elif defined(WINDOWS)
#include <windows.h>
double getTime()
{
    static __int64 freq = 0;
    static __int64 t0 = 0;
    __int64 ct;

    if (!freq) {
        QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
    }
    QueryPerformanceCounter((LARGE_INTEGER *)&ct);   // reads the current time (in system units)
    if (!t0) {
        t0 = ct;
    }
    return double(ct-t0)/double(freq);
}
#elif defined(LINUX)
#include <time.h>
double getTime()
{
        static double t0 = -9999.;
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        double t = double(ts.tv_sec) + double(ts.tv_nsec)/1e9;
        if (t0 < 0.) t0 = t;
        return t-t0;
}
#else
#  error Unknown platform! Define one of OSX, WINDOWS, or LINUX on compile command-line!
#endif
