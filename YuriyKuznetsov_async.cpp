/*
необходимо написать алгоритм accumulate только с возможностью паралельного
вычисления, т.е. на разных потоках. пример есть массив скажем 500Mb и у вас в
системе есть возможность паралельного исчеселния на 8 потоках. Т.е. на реальном
паралелизме. вам нужно коректно разбить этот массив на такие части чтобы
максимально загрузить эти 8 потоков. результаты сравнить для последовательного
вычесления, это скажем если у вас просто банальный цикл который проходит по
контейнеру элемент за элементом, и паралельного вычесление с разным количеством
тредов.
 */
#include <array>
#include <chrono>
#include <future>
#include <iostream>
#include <iterator>
#include <numeric>
#include <vector>

int main() {
  constexpr size_t V_SIZE{500000000 / sizeof(int)};
  constexpr int START_VALUE{0};
  std::vector<int> v{};
  if (V_SIZE >= v.max_size()) {
    std::cout << "Can't allocate memory for vector\n";
  }
  v.reserve(V_SIZE);
  std::cout << v.size() << '\n';
  std::cout << v.capacity() << '\n';
  std::cout << v.max_size() << '\n';

  char value{0};
  for (size_t index = 0; index < V_SIZE; ++index) {
    v.push_back(value);
    if (value == 9) {
      value = 0;
      continue;
    }
    ++value;
  }
  std::cout << v.size() << '\n';

  std::chrono::time_point<std::chrono::steady_clock> start =
      std::chrono::steady_clock::now();
  auto sum = std::accumulate(v.cbegin(), v.cend(), START_VALUE);
  std::chrono::time_point<std::chrono::steady_clock> end =
      std::chrono::steady_clock::now();
  std::cout << "milliseconds: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                     start)
                   .count()
            << '\n';
  std::cout << "sum: " << sum << '\n';

  sum = 0;
  start = std::chrono::steady_clock::now();
  constexpr size_t MAX_THREAD_COUNT{8};
  constexpr size_t ONE_THREAD_EL_COUNT{V_SIZE / MAX_THREAD_COUNT};
  auto it_beg{v.begin()};
  auto it_curEnd{v.begin() + ONE_THREAD_EL_COUNT};
  using pair_boolFuture = std::pair<bool, std::future<int>>;
  std::array<pair_boolFuture, MAX_THREAD_COUNT> arrFuture{};
  for (size_t thrIndex = 0; thrIndex < MAX_THREAD_COUNT; ++thrIndex) {
    arrFuture[thrIndex].second = std::async(
        std::launch::async,
        [](std::vector<int>::iterator in_begin,
           std::vector<int>::iterator in_end) {
          return std::accumulate(in_begin, in_end, START_VALUE);
        },
        it_beg, it_curEnd);
    if (thrIndex < MAX_THREAD_COUNT - 1) {
      std::advance(it_beg, ONE_THREAD_EL_COUNT);
      std::advance(it_curEnd, ONE_THREAD_EL_COUNT);
    }
  }
  size_t threadCount{};
  while (threadCount < MAX_THREAD_COUNT) {
    for (size_t thrIndex = 0; thrIndex < MAX_THREAD_COUNT; ++thrIndex) {
      if (arrFuture.at(thrIndex).first == false) {
        std::future_status curStatus{
            arrFuture[thrIndex].second.wait_for(std::chrono::microseconds(1))};
        switch (curStatus) {
        case std::future_status::deferred:
          std::cout << "Future of thread " << thrIndex + 1
                    << " is deferred. \n";
          sum += arrFuture[thrIndex].second.get();
          arrFuture[thrIndex].first = true;
          ++threadCount;
          break;
        case std::future_status::timeout:
          std::cout << "Future of thread " << thrIndex + 1
                    << " is timed out. \n";
          break;
        case std::future_status::ready:
          sum += arrFuture[thrIndex].second.get();
          arrFuture[thrIndex].first = true;
          ++threadCount;
          break;
        default:
          std::cout << "Undefined error while waiting for a future \n";
          return -1;
        }
      }
    }
  }
  end = std::chrono::steady_clock::now();
  std::cout << "milliseconds: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                     start)
                   .count()
            << '\n';
  std::cout << "sum: " << sum << '\n';

  return 0;
}
