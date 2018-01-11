#include"tem_field.h"
#include"stdafx.h"
#include"threadpool.h"
#include"fever.hpp"
#include<vector>
#include<future>
#include<iostream>

namespace simulation {
TemField::TemField(float step) {
  nx = reader.GetNx();
  ny = reader.GetNy();
  nz = reader.GetNz();
  tem_field_ = new float[nx*ny*nz];
  my_field = tem_field_;
  for (int k = 0; k < nz; ++k) {
    for (int j = 0; j < ny; ++j) {
      for (int i = 0; i < nx; ++i) {
        uint32_t index = k + nz*j + ny*nz*i;
        if (reader.grid_[index] == 0)
          tem_field_[index] = 1000.0;
        else
          tem_field_[index] = 20.0;
      }
    }
  }
  header.SN = 0;
  header.Time = 0;
  header.Tmax = 1000;
  header.Tmin = 20;
  header.TL = 0;
  header.TS = 0;
  header.ALLCPUTime = 0;
  tem_step = step;
}

TemField::TemField(short i, short j, short k,float step) :nx(i), ny(j), nz(k),tem_step(step) {
  tem_field_ = new float[i*j*k];
  my_field = tem_field_;
  header.SN = 0;
  header.Time = 0;
  header.Tmax = 1000;
  header.Tmin = 20;
  header.TL = 0;
  header.TS = 0;
  header.ALLCPUTime = 0;
}

TemField::TemField(float* t_field, short x, short y, short z,float step):tem_field_(t_field), nx(x), ny(y), nz(z),tem_step(step){
	my_field = nullptr;
}

TemField::~TemField() {
  delete my_field;
}

uint32_t TemField::GetNodeIndex(short i, short j, short k) const {
  if (i >= nx || j >= ny || k >= nz || i < 0 || j < 0 || k < 0)
    return -1;
  return k + nz*j + ny*nz*i;
}

void TemField::SwapTemField(TemField& rhr) {
  float *tmp = tem_field_;
  tem_field_ = rhr.tem_field_;
  rhr.tem_field_ = tmp;
  float t = header.Time;
  header.Time = rhr.header.Time;
  rhr.header.Time = t;
}

void TemField::Calculate(const TemField& last,short index) {
  for (int k = 0; k < nz; ++k) {
    for (int j = 0; j < ny; ++j) {
      for (int i = 0; i < nx; ++i) {
		  if (reader.grid_[GetNodeIndex(i, j, k)] == index) {
			  tem_field_[GetNodeIndex(i, j, k)] = last.GetNextTem(i, j, k);
		  }
      }
    }
  }
  SetHeader(last,tem_step);
}

void TemField::CalculatePartOne(const TemField& last) {
  for (int k = 0; k < nz; ++k) {
    for (int j = 0; j < ny / 2; ++j) {
      for (int i = 0; i < nx / 2; ++i) {
        tem_field_[GetNodeIndex(i, j, k)] = last.GetNextTem(i, j, k);
      }
    }
  }
}

void TemField::CalculatePartTwo(const TemField& last) {
  for (int k = 0; k < nz; ++k) {
    for (int j = 0; j < ny / 2; ++j) {
      for (int i = nx / 2; i < nx; ++i) {
        tem_field_[GetNodeIndex(i, j, k)] = last.GetNextTem(i, j, k);
      }
    }
  }
}

void TemField::CalculatePartThree(const TemField& last) {
  for (int k = 0; k < nz; ++k) {
    for (int j = ny / 2; j < ny; ++j) {
      for (int i = 0; i < nx / 2; ++i) {
        tem_field_[GetNodeIndex(i, j, k)] = last.GetNextTem(i, j, k);
      }
    }
  }
}

void TemField::CalculatePartFour(const TemField& last) {
  for (int k = 0; k < nz; ++k) {
    for (int j = ny / 2; j < ny; ++j) {
      for (int i = nx / 2; i < nx; ++i) {
        tem_field_[GetNodeIndex(i, j, k)] = last.GetNextTem(i, j, k);
      }
    }
  }
}

double TemField::GetNextTem(short i, short j, short k) const {
  uint32_t current_node_index = GetNodeIndex(i, j, k);
  if (reader.grid_[current_node_index] == 1)
    return tem_field_[current_node_index];

  database current_node_data = GetNodeData(reader,current_node_index);
  double T_i = tem_field_[current_node_index];
  double redaolv_i = current_node_data.redaolv;
  double yinzi1 = tem_step / (current_node_data.midu * current_node_data.birerong * size);
  double yinzi2 = 0;
  uint32_t round_index[6] = {
    GetNodeIndex(i - 1,j,k),
    GetNodeIndex(i + 1,j,k),
    GetNodeIndex(i,j - 1,k),
    GetNodeIndex(i,j + 1,k),
    GetNodeIndex(i,j,k - 1),
    GetNodeIndex(i,j,k + 1)
  };
  //热传导
  double tmpi = redaolv_i;
  for (int x = 0; x < 6; ++x) {
    double T_j;
    double redaolv_j;
    if (round_index[x] == -1) {
      T_j = 20.0;
      redaolv_j = data[1].redaolv;
    }
    else {
      T_j = tem_field_[round_index[x]];
      redaolv_j = GetNodeData(reader,round_index[x]).redaolv;
    }
    if (!equal(T_i, T_j)) {
      if (equal(redaolv_i, redaolv_j)) {
        yinzi2 += (T_j - T_i) * redaolv_i / size;
      }
      else
        yinzi2 += (T_j - T_i) / (size / (2 * redaolv_i) + size / (2 * redaolv_j));
    }
  }
  double T_next = T_i + yinzi1 * yinzi2;

  return T_next;
}

double TemField::GetFeverTem() const {
	if (header.Time >= fever_struct.fever_start_time && header.Time <= fever_struct.fever_end_time) {
		return fever_struct.T_i_fever;
	}
	return 0.0;
}

void TemField::SetHeader(const TemField& last,double time_step) {
  header.SN = last.header.SN;
  header.Time = last.header.Time + time_step;
  header.TL = last.header.TL;
  header.TS = last.header.TS;
  header.Tmax = 0.0;
  header.Tmin = 0.0;
}

void TemField::OutToTecplot(std::ofstream& out) const {
  out << "TITLE = \"TEST\"\n";
  out << "VARIABLES = \"X\",\"Y\",\"Z\",\"T\",\"D\"\n";
  out << "ZONE I=" << nx << ",J=" << ny << ",K=" << nz << ",F=POINT\n";
  for (int k = 0; k < nz; ++k) {
    for (int j = 0; j < ny; ++j) {
      for (int i = 0; i < nx; ++i) {
        uint32_t index = k + nz*j + ny*nz*i;
        out << i << " " << j << " " << k << " " << tem_field_[index] <<" " << reader.grid_[index] << std::endl;
      }
    }
  }
}

void TemField::OutToTecplotZoo(std::ofstream& out) const {
  out << "ZONE I=" << nx << ",J=" << ny << ",K=" << nz << ",F=POINT\n";
  for (int k = 0; k < nz; ++k) {
    for (int j = 0; j < ny; ++j) {
      for (int i = 0; i < nx; ++i) {
        uint32_t index = k + nz*j + ny*nz*i;
        out << i << " " << j << " " << k << " " << tem_field_[index] << " " << reader.grid_[index] << std::endl;
      }
    }
  }
}

void TemField::TemSimulation(uint32_t times) {
	ThreadPool pool(4);
	std::vector<std::future<void>> results;
	std::ofstream file("d:\\totalfile.txt", std::ios::out);
	OutToTecplot(file);
	TemField tem_next(tem_step);
	for (uint32_t i = 1; i < times; ++i) {
		auto partone = std::bind(&TemField::CalculatePartOne, std::ref(tem_next), std::ref(*this));
		auto parttwo = std::bind(&TemField::CalculatePartTwo, std::ref(tem_next), std::ref(*this));
		auto partthree = std::bind(&TemField::CalculatePartThree, std::ref(tem_next), std::ref(*this));
		auto partfour = std::bind(&TemField::CalculatePartFour, std::ref(tem_next), std::ref(*this));
		results.emplace_back(pool.enqueue(partone));
		results.emplace_back(pool.enqueue(parttwo));
		results.emplace_back(pool.enqueue(partthree));
		results.emplace_back(pool.enqueue(partfour));

		for (auto& it : results) {
			it.get();
		}

		tem_next.SetHeader(*this, tem_step);
		fever::eachStep(tem_next.tem_field_, tem_next.header.Time);

		std::cout << "第" << i << "次计算结束！\n";
    if (i > 200 && i < 1500 && i % 50 == 0) {
      tem_next.OutToTecplotZoo(file);
    }
		if (false) {
			char buf[10];
			sprintf_s(buf, "%d", i);
			std::string name = std::string("Tempart") + std::string(buf) + std::string(".lay");
			std::ofstream eachfile(name.c_str(), std::ios::out);
			tem_next.OutToTecplot(eachfile);
			eachfile.close();
		}

		this->SwapTemField(tem_next);
		results.clear();
	}
	file.close();
}

void TemField::TemSimulation_F(uint32_t times,short index) {
	TemField tem_next(tem_step);
	for (uint32_t i = 0; i < times; ++i) {
		tem_next.Calculate(*this, index);
		this->SwapTemField(tem_next);
	}
}
SgnFileReader TemField::reader(std::ifstream("huazhu\\pn_test.sgn", std::ios::in | std::ios::binary));
}//namespace simulation
