#include <iostream>
#include <array>
#include <cassert>
#include <chrono>
#include <random>

std::array<int, 15> vars;
int& AB = vars[0];
int& AC = vars[1];
int& AD = vars[2];
int& AE = vars[3];
int& AF = vars[4];
int& BC = vars[5];
int& BD = vars[6];
int& BE = vars[7];
int& BF = vars[8];
int& CD = vars[9];
int& CE = vars[10];
int& CF = vars[11];
int& DE = vars[12];
int& DF = vars[13];
int& EF = vars[14];

int solution()
{
  return BE + BF + EF + AC + AD + CD;
}

int start()
{
  return EF + CD + AB;
}

int intermediate(int i)
{
  switch (i)
  {
    case 0:
      return EF + AC + AD + CD;
    case 1:
      return AE + AF + EF + CD;
    case 2:
      return EF + BC + BD + CD;
    case 3:
      return BE + BF + EF + CD;
    case 4:
      return EF + AB + AC + AD + BC + BD + CD;
    case 5:
      return AB + AE + AF + BE + BF + EF + CD;
    case 6:
      return EF + AB + AC + BC;
    case 7:
      return CE + CF + EF + AB;
    case 8:
      return EF + AB + AD + BD;
    case 9:
      return DE + DF + EF + AB;
    case 10:
      return CD + CE + CF + DE + DF + EF + AB;
    case 11:
      return AB + AE + BE + CD;
    case 12:
      return CE + DE + CD + AB;
    case 13:
      return AB + AF + BF + CD;
    case 14:
      return CD + CF + DF + AB;
  }
  assert(false);
  return 0;
}

bool solved()
{
  int so = solution();
  int st = start();
  std::cout << "solution = " << so << "; start = " << st;
  if (solution() <= start())
    return false;
  for (int i = 0; i < 15; ++i)
  {
    int in = intermediate(i);
    std::cout << "; in(" << i << ") = " << in;
    if (start() <= in)
      return false;
  }
  std::cout << ": Solved!\n";
  return true;
}

int main()
{
  std::random_device rd;
  std::mt19937::result_type seed = rd() ^ (
          (std::mt19937::result_type)
          std::chrono::duration_cast<std::chrono::seconds>(
              std::chrono::system_clock::now().time_since_epoch()
              ).count() +
          (std::mt19937::result_type)
          std::chrono::duration_cast<std::chrono::microseconds>(
              std::chrono::high_resolution_clock::now().time_since_epoch()
              ).count() );
  std::cout << "seed = " << std::hex << seed << std::dec << std::endl;
  std::mt19937 generator(seed);
  std::uniform_int_distribution<int> distribution(-100, 100);

#if 0
  do
  {
    for (int i = 0; i != vars.size(); ++i)
      vars[i] = distribution(generator);
  }
  while (!solved());
#endif

  vars = { 30, -17, 44, -15, -87, -98, 14, -37, 58, 62, 3, -7, -70, -13, 21 };
  [[maybe_unused]] bool res = solved();
//  std::cout << "solved() = " << std::boolalpha << solved() << std::endl;

  for (int score : vars)
    std::cout << score << '\n';
}
