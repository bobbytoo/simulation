/*ini文件读取测试*/
#include<iostream>
#include<fstream>
#include<map>
#include<vector>
#include<future>
#include<cstdio>
#include"tem_field.h"
#include"sgn_file_reader.h"
#include"stdafx.h"
#include"threadpool.h"
#include"fever.hpp"
#include"tem_field.h"

static float tem_step;

void Init()
{
  //数据库初始化
  data.insert(std::pair<int, database>(0, zhutie));
  data.insert(std::pair<int, database>(2, shazi));
  data.insert(std::pair<int, database>(1, kongqi));
  data.insert(std::pair<int, database>(5, fare));

  //设置网格大小与空气温度
  size = 0.004;
  tem_step = 10;

  //设置时间步长
  std::map<int, database>::iterator it = data.begin();
  double tmp = (*it).second.midu*(*it).second.birerong;
  for (std::map<int, database>::iterator it = data.begin(); it != data.end(); ++it)
  {
    if ((*it).first == 1) {
        continue;
    }
    double current = (*it).second.midu *(*it).second.birerong;
    if (tmp > current) {
      tmp = current;
    }
  }
  tem_step = tmp * size * size / (6 * 37.2);
  std::cout << "time step : ";
  std::cout << tem_step << std::endl;

	double grid_v = size * size * size;
	//设置发热参数
	fever_struct.fever_start_time = 18; //s
	fever_struct.fever_end_time = 38; //s
	fever_struct.perm_Al = 0.17;
	fever_struct.perm_Si = 0.l;
	fever_struct.v_Al = 6.8;
	fever_struct.v_Si = 4;
	fever_struct.H_Al = 838000;
	fever_struct.H_Si = 1378510;
	fever_struct.m_Al = data[5].midu * grid_v * fever_struct.perm_Al;
	fever_struct.m_Si = data[5].midu * grid_v * fever_struct.perm_Si;
	fever_struct.d_mAl = fever_struct.v_Al * tem_step * grid_v;
	fever_struct.d_mSi = fever_struct.v_Si * tem_step * grid_v;
	fever_struct.d_q = fever_struct.d_mAl * 1000 / (27 * 2) * fever_struct.H_Al \
									 + fever_struct.d_mSi * 1000 / (28 * 3) * fever_struct.H_Si;
	fever_struct.T_i_fever = fever_struct.d_q * 1000/ (data[5].midu * data[5].birerong * grid_v);
}

int main() {

  Init();
  
  fever::LoadData(simulation::TemField::reader.grid_,
    simulation::TemField::reader.GetNx(),
    simulation::TemField::reader.GetNy(),
    simulation::TemField::reader.GetNz(),
    5,
    data[5].midu,
    data[5].birerong,
    size,
    tem_step);

  simulation::TemField tem(tem_step);
  tem.TemSimulation(2001);

  system("pause");
  return 0;
}