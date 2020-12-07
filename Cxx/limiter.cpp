#include <iostream>
#include <vector>
#include <array>
#include <functional>
#include <limits>

/*
cells:
|----|----|----|
| 0  | 1  | 2  |
|----|----|----|
| 5  | 4  | 3  |
|----|----|----|
| 6  | 7  | 8  |
|----|----|----|
*/

int main()
{
  std::array<std::array<int, 2>, 12> face_cell_list { {
      //0      1      2      3      4      5      6      7      8      9      10      11
      {0,1}, {1,2}, {2,3}, {3,4}, {1,4}, {4,5}, {5,0}, {5,6}, {6,7}, {4,7}, {7, 8}, {8, 3} } };

  std::array<int, 9> phi_cell { { 5, 1, 9,  6, 3, 2,  4, 7, 8 } };

  std::array<int, 9> phi_lb_cell;
  std::array<int, 9> phi_ub_cell;

  for (int it = 0; it != phi_cell.size(); ++it)
  {
    phi_lb_cell[it] = std::numeric_limits<int>::max();
    phi_ub_cell[it] = std::numeric_limits<int>::min();
  }

  std::array<int, 9> branch_lb;
  std::array<int, 9> branch_ub;

  for (int it = 0; it != phi_cell.size(); ++it)
  {
    branch_lb[it] = std::numeric_limits<int>::max();
    branch_ub[it] = std::numeric_limits<int>::max();
  }


  for (int it = 0; it != face_cell_list.size(); ++it)
  {
    auto c0 = face_cell_list[it][0];
    auto c1 = face_cell_list[it][1];

    auto phi_min = std::min(phi_cell[c0], phi_cell[c1]);
    auto phi_max = std::max(phi_cell[c0], phi_cell[c1]);

    // phi_lb_cell[c0] = std::min(phi_lb_cell[c0], phi_min);
    // phi_ub_cell[c0] = std::max(phi_ub_cell[c0], phi_max);

    // phi_lb_cell[c1] = std::min(phi_lb_cell[c1], phi_min);
    // phi_ub_cell[c1] = std::max(phi_ub_cell[c1], phi_max);

    if (phi_min < phi_lb_cell[c0])
    {
      phi_lb_cell[c0] = phi_min;
      branch_lb[c0] = it;
    }

    if (phi_max > phi_ub_cell[c0])
    {
      phi_ub_cell[c0] = phi_max;
      branch_ub[c0] = it;
    }

    if (phi_min < phi_lb_cell[c1])
    {
      phi_lb_cell[c1] = phi_min;
      branch_lb[c1] = it;
    }

    if (phi_max > phi_ub_cell[c1])
    {
      phi_ub_cell[c1] = phi_max;
      branch_ub[c1] = it;
    }
  }

  std::cout << "standard" << std::endl;
  for (int it = 0; it != phi_cell.size(); ++it)
  {
    std::cout << phi_cell[it] << ": " << phi_lb_cell[it] << " " << phi_ub_cell[it] << std::endl;
  }


  for (int it = 0; it != phi_cell.size(); ++it)
  {
    phi_lb_cell[it] = 0;
    phi_ub_cell[it] = 0;
  }


  for (int it = 0; it != face_cell_list.size(); ++it)
  {
    auto c0 = face_cell_list[it][0];
    auto c1 = face_cell_list[it][1];
    auto phi_min = std::min(phi_cell[c0], phi_cell[c1]);
    auto phi_max = std::max(phi_cell[c0], phi_cell[c1]);

    if (branch_lb[c0] == it)
    {
      phi_lb_cell[c0] += phi_min;
    }

    if (branch_ub[c0] == it)
    {
      phi_ub_cell[c0] += phi_max;
    }

    if (branch_lb[c1] == it)
    {
      phi_lb_cell[c1] += phi_min;
    }

    if (branch_ub[c1] == it)
    {
      phi_ub_cell[c1] += phi_max;
    }
  }


  std::cout << "recorded" << std::endl;
  for (int it = 0; it != phi_cell.size(); ++it)
  {
    std::cout << phi_cell[it] << ": " << phi_lb_cell[it] << " " << phi_ub_cell[it] << std::endl;
  }
}
