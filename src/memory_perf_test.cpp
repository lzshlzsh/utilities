/**
 * @file memory_perf_test.cpp
 * @brief
 * @version 1.0
 * @date 04/22/2016 10:43:08 AM
 * @author sammieliu,sammieliu@tencent.com 
 * @copyright Copyright 1998 - 2016 Tencent. All Rights Reserved.
 */
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <stdint.h>

#include <iostream>

using namespace std;

enum {
    MEM_SIZE = (1 << 30),
    TEST_ITERATION = 100,
};


/**
 * @brief 
 * @class TimeProfile
 */
class TimeProfile
{
public:
    /**
     * @brief
     */
    TimeProfile () {
        reset();
    }
    ~TimeProfile() {
        double cst;
        cst = cost();
        cout << "cost: " << cst 
            << ",\tperf: " << static_cast<double>(MEM_SIZE) * TEST_ITERATION * 1e6 / cst / (1 << 30)
            << "G/s" << endl;
    }
    /**
     * @brief 
     */
    double cost() const {
        struct timeval t;
        double now;

        gettimeofday(&t, NULL);
        now = t.tv_sec * 1e6 + t.tv_usec;
        return now - start_;
    }
    /**
     * @brief 
     */
    void reset() {
        struct timeval t;
        gettimeofday(&t, NULL);
        start_ = t.tv_sec * 1e6 + t.tv_usec;
    }
protected:
private:
    double start_;
};


/**
 * @brief 
 */
static void read_perf(void *addr)
{
    uint64_t t = 0;
    TimeProfile profile;

    __asm__ __volatile__ (
        "xorq %%rbx, %%rbx\n"
        "3:cmpq %3, %%rbx\n"
        "jg 4f\n"
        "xorq %%rcx, %%rcx\n"
        "movq %1, %%r11\n"
        "1:cmpq %0, %%rcx\n"
        "jg 2f\n"
        "movq (%%r11), %2\n"
        "addq $8, %%rcx\n"
        "addq $8, %%r11\n"
        "jmp 1b\n"
        "2:incq %%rbx\n"
        "jmp 3b\n"
        "4:\n"
        :
        :"i"(MEM_SIZE - 1), "r"(addr), "r"(t), "i"(TEST_ITERATION - 1)
        :"rcx", "rbx", "r11"
        );
}

/**
 * @brief 
 */
static void write_perf(void *addr)
{
    uint64_t t = 0;
    TimeProfile profile;

    __asm__ __volatile__ (
        "xorq %%rbx, %%rbx\n"
        "3:cmpq %3, %%rbx\n"
        "jg 4f\n"
        "xorq %%rcx, %%rcx\n"
        "movq %1, %%r11\n"
        "1:cmpq %0, %%rcx\n"
        "jg 2f\n"
        "movq %2, (%%r11)\n"
        "addq $8, %%rcx\n"
        "addq $8, %%r11\n"
        "jmp 1b\n"
        "2:incq %%rbx\n"
        "jmp 3b\n"
        "4:\n"
        :
        :"i"(MEM_SIZE - 1), "r"(addr), "r"(t), "i"(TEST_ITERATION - 1)
        :"rcx", "rbx", "r11"
        );
}

/**
 * @brief 
 */
int main()
{
    void *addr;

    addr = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (reinterpret_cast<void *>(-1) == addr) {
        cout << "mmap failed: " << strerror(errno) << endl;
        return -1;
    }
    read_perf(addr);
    write_perf(addr);
    munmap(addr, MEM_SIZE);
    return 0;
}

