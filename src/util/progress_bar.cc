#include "progress_bar.hh"

#include <iostream>
#include <cstring>

using namespace std;

namespace util {

void update_progress_bar(char symbol, int percentage)
{
  char progress_bar[103];
  progress_bar[0] = '[';
  memset(progress_bar+1, symbol, percentage);
  memset(progress_bar+percentage+1, ' ', 100-percentage);
  progress_bar[101] = ']';
  progress_bar[102] = '\0';
  std::cout << "\r (" << percentage << "%): " << progress_bar;
  std::flush(std::cout);
}

}
