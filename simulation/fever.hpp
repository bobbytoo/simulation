#pragma once
#include<map>
namespace fever {
  struct fever_t {
    double fever_start_time;
    double v_Al;
    double v_Si;
    double perm_Al;
    double perm_Si;
    double H_Al;
    double H_Si;
    double m_Al;
    double m_Si;
    double d_mAl;
    double d_mSi;
    double d_q;
    double T_i_fever;
  };

  extern fever_t fever_struct;
  extern std::map<int64_t, float> grid_m_Al;
  extern std::map<int64_t, float> grid_m_Si;

  extern short ni;
  extern short nj;
  extern short nk;
  extern short* sgn_file;
  extern float* tem_field;
  extern short fever_num;
  extern float density;
  extern float sh;
  extern float size;
  extern float step;

  //该函数用来导入我所需要的数据
  void LoadData(short* sgn,short nx, short ny, short nz, short num, double sleeve_density,
    double sleeve_sh, float grid_size, double time_step);

  int64_t GetNodeIndex(short i, short j, short k);

  void FeverInit();

  void setFeverGrid(double m_Al, double m_Si);

  void eachStep(float* t_field,float this_time);

  void transferAfterFever(float* t_field);

  float GetFeverT(int64_t index, float this_time);

  float GetAlRate();

  float GetSiRate();
}