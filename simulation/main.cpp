/*ini文件读取测试*/
#include<iostream>
#include<fstream>
#include<map>
#include<vector>
#include<future>
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

  //设置网格大小与空气温度
  size = 0.006;
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
}

int main() {
  ThreadPool pool(4);
  std::vector<std::future<void>> results;
  std::ofstream out("D:\\result.txt", std::ios::out | std::ios::app);
  Init();
  simulation::TemField tem_last(reader);
  tem_last.OutToTecplot(out,reader);
  out.close();
  simulation::TemField tem_next(reader.GetNx(), reader.GetNy(), reader.GetNz());
  for (int i = 1; i < 100; ++i) {
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
    //tem_next.OutToTecplotZoo(out, reader);
    //准备新文件
    char buf[10];
    sprintf_s(buf, "%d", i);
    std::string name = std::string("D:/t") + std::string(buf) + std::string(".lay");
    std::ofstream eachfile(name.c_str(), std::ios::out);

    tem_next.Calculate(tem_last);
    tem_next.OutToTecplot(eachfile,reader);
    tem_last.SwapTemField(tem_next);
    results.clear();
    eachfile.close();
  }
	system("pause");
	return 0;
}