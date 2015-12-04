#include "tbb/tbb.h"
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>

// parallel_reduce adapter;
class SumFoo {
public:
  SumFoo(int* a)
    : m_data(a),
      m_sum(0) {
  }
  SumFoo(SumFoo& r, tbb::split)
    : m_data(r.m_data),
      m_sum(0) {
  }
  void operator()(const tbb::blocked_range<size_t>& r) {
    const int* d = m_data;
    int sum = m_sum;

    for (size_t i = r.begin(); i != r.end(); ++i) {
      sum += foo(d[i]);
    }

    m_sum = sum;
  }

  // Join the result with the other half of iteration space
  void join(const SumFoo& r) {
    m_sum += r.m_sum;
  }

  long sum() const { return m_sum; }

private:
  int foo(const int a) const
  {
    std::cout << "==========> a=" << a << " at thread " << syscall(SYS_gettid) << std::endl;
    sleep(2);
    return a;
  }

private:
  const int* m_data;
  long       m_sum;

};

#include <vector>
int main()
{
  std::vector<int> a(100);
  for (int i = 0; i < 100; ++i) {
    a[i] = i+1;
  }

  SumFoo sf(&a[0]);
  tbb::parallel_reduce(tbb::blocked_range<size_t>(0, 100), sf);

  std::cerr << "The sum = " << sf.sum() << std::endl;
  return 0;
}
