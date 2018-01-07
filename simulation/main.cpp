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


void Init()
{
  //数据库初始化
  data.insert(std::pair<int, database>(0, zhutie));
  data.insert(std::pair<int, database>(2, shazi));
  data.insert(std::pair<int, database>(1, kongqi));
  data.insert(std::pair<int, database>(5, fare));

  //设置网格大小与空气温度
  size = 0.004;
  airtem = 20;
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
	fever_struct.d_q = fever_struct.d_mAl / (27 * 2) * fever_struct.H_Al \
									 + fever_struct.d_mSi / (28 * 3) * fever_struct.H_Si;
	fever_struct.T_i_fever = fever_struct.d_q / (data[5].midu * data[5].birerong * grid_v);
  std::cout << "fever_t : " << fever_struct.T_i_fever << std::endl;
}

int main() {
  ThreadPool pool(4);
  std::vector<std::future<void>> results;
  Init();
  simulation::TemField tem_last;
  double total_time = 0;
  simulation::TemField tem_next;
  for (int i = 1; i < 5000; ++i) {
    auto partone = std::bind(&simulation::TemField::CalculatePartOne, std::ref(tem_next), std::ref(tem_last));
    auto parttwo = std::bind(&simulation::TemField::CalculatePartTwo, std::ref(tem_next), std::ref(tem_last));
    auto partthree = std::bind(&simulation::TemField::CalculatePartThree, std::ref(tem_next), std::ref(tem_last));
    auto partfour = std::bind(&simulation::TemField::CalculatePartFour, std::ref(tem_next), std::ref(tem_last));
    results.emplace_back(pool.enqueue(partone));
    results.emplace_back(pool.enqueue(parttwo));
    results.emplace_back(pool.enqueue(partthree));
    results.emplace_back(pool.enqueue(partfour));

    for (auto& it : results) {
      it.get();
    }
    std::cout << "第" << i << "次完成！\n";
	tem_next.SetHeader(tem_last, tem_step);
    //准备新文件
	if (true) {
		char buf[10];
		sprintf_s(buf, "%d", i);
		std::string name = std::string("Tempart") + std::string(buf) + std::string(".lay");
		std::ofstream eachfile(name.c_str(), std::ios::out);
		tem_next.OutToTecplot(eachfile);
		eachfile.close();
	}
		tem_last.SwapTemField(tem_next);
		results.clear();
		total_time += tem_step;
    std::cout << "total_time:" << total_time << std::endl;
  }
	system("pause");
	return 0;
}