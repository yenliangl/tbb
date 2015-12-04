#include "tbb/tbb.h"
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>

void Foo(const int a)
{
  std::cout << "==========> a=" << a << " at thread " << syscall(SYS_gettid) << std::endl;
  sleep(2);
}

// parallel_for adapter;
class ApplyFoo {
private:
  const int* m_data;

public:
  void operator()(const tbb::blocked_range<size_t>& range) const {
    const int* d = m_data;

    for (size_t i = range.begin(); i != range.end(); ++i) {
      Foo(d[i]);
    }
  }

  ApplyFoo(int a[])
    : m_data(a) {
  }
};

int main()
{
  int a[20] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
  tbb::parallel_for(tbb::blocked_range<size_t>(0, 20),
                    ApplyFoo(a));
  return 0;
}
