#include"fever.hpp"
#include<map>
#include<iostream>

namespace fever {
  fever_t fever_struct;
  std::map<int64_t, float> grid_m_Al;
  std::map<int64_t, float> grid_m_Si;

  short ni;
  short nj;
  short nk;
  short* sgn_file;
  float* tem_field;
  short fever_num;
  float density;
  float sh;
  float size;
  float step;

  //该函数用来导入我所需要的数据
  void fever::LoadData(short* sgn,short nx, short ny, short nz, short num, double sleeve_density,
    double sleeve_sh, float grid_size, double time_step) {
    sgn_file = sgn;
    ni = nx;
    nj = ny;
    nk = nz;
    fever_num = num;
    density = sleeve_density;
    sh = sleeve_sh;
    size = grid_size;
    step = time_step;
    std::cout << nx << "\n" << ny << "\n" << nz << "\n";
    std::cout << fever_num << std::endl;
    std::cout << density << std::endl;
    std::cout << sh << std::endl;
    std::cout << size << std::endl;
    std::cout << step << std::endl;
    FeverInit();
  }

  int64_t GetNodeIndex(short i, short j, short k) {
    if (i >= ni || j >= nj || k >= nk || i < 0 || j < 0 || k < 0)
      return -1;
    return k + nk * j + nj * nk*i;
  }

  void FeverInit() {
    double grid_v = size * size * size;
    //设置发热参数
    fever_struct.fever_start_time = 18; //s
    fever_struct.perm_Al = 0.17;
    fever_struct.perm_Si = 0.l;
    fever_struct.v_Al = 6.8;
    fever_struct.v_Si = 4;
    fever_struct.H_Al = 838000;
    fever_struct.H_Si = 1378510;
    fever_struct.m_Al = density * grid_v * fever_struct.perm_Al;
    fever_struct.m_Si = density * grid_v * fever_struct.perm_Si;
    setFeverGrid(fever_struct.m_Al, fever_struct.m_Si);
  }

  void setFeverGrid(double m_Al, double m_Si) {
    for (short k = 0; k < nk; ++k) {
      for (short j = 0; j < nj; ++j) {
        for (short i = 0; i < ni; ++i) {
          int64_t index = GetNodeIndex(i, j, k);
          if (sgn_file[index] == fever_num) {
            grid_m_Al.insert(std::pair<int64_t, double>(index, fever_struct.m_Al));
            grid_m_Si.insert(std::pair<int64_t, double>(index, fever_struct.m_Si));
          }
        }
      }
    }
  }

  void eachStep(float* t_field,float this_time) {
    tem_field = t_field;
    for (short k = 0; k < nk; ++k) {
      for (short j = 0; j < nj; ++j) {
        for (short i = 0; i < ni; ++i) {
          int64_t index = GetNodeIndex(i, j, k);
          if (sgn_file[index] == fever_num) {
            tem_field[index] += GetFeverT(index, this_time);
          }
        }
      }
    }
  }

  float GetFeverT(int64_t index, float this_time) {
    if (this_time < fever_struct.fever_start_time)
      return 0;
    float grid_v = size * size * size;
    float al_rate = GetAlRate();
    float si_rate = GetSiRate();
    float d_mAl = 0;
    if (grid_m_Al[index] > 0) {
      d_mAl = al_rate * step * grid_v;
      grid_m_Al[index] -= d_mAl;
    }
    float d_mSi = 0;
    if (grid_m_Si[index] > 0) {
      d_mSi = si_rate * step * grid_v;
      grid_m_Si[index] -= d_mSi;
    }
    float d_q = d_mAl * 1000 / (27 * 2) * fever_struct.H_Al \
      + d_mSi * 1000 / (28 * 3) * fever_struct.H_Si;
    if (grid_m_Al[index] <= 0 && grid_m_Al[index] <= 0) {
      return 0.0;
    }
    return d_q / (density * sh * grid_v);
  }

  float GetAlRate() {
    return 6.8;
  }

  float GetSiRate() {
    return 4.0;
  }
}