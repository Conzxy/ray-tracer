#include <random>
#include <iostream>

int main()
{
  std::random_device rd;
  std::cout << "true random number: " << rd() << '\n';

  std::default_random_engine dre;
  std::cout << "pesudo random number: " << dre() << '\n';
}
