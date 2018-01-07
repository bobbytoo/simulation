#include"tem_field.h"
#include"stdafx.h"
#include<iostream>

namespace simulation {
TemField::TemField() {
  nx = reader.GetNx();
  ny = reader.GetNy();
  nz = reader.GetNz();
  tem_field_ = new float[nx*ny*nz];
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
}

TemField::TemField(short i, short j, short k) :nx(i), ny(j), nz(k) {
  tem_field_ = new float[i*j*k];
  header.SN = 0;
  header.Time = 0;
  header.Tmax = 1000;
  header.Tmin = 20;
  header.TL = 0;
  header.TS = 0;
  header.ALLCPUTime = 0;
}

TemField::~TemField() {
  delete tem_field_;
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

void TemField::Calculate(const TemField& last) {
  for (int k = 0; k < nz; ++k) {
    for (int j = 0; j < ny; ++j) {
      for (int i = 0; i < nx; ++i) {
        tem_field_[GetNodeIndex(i, j, k)] = last.GetNextTem(i, j, k);
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
  double tmpj[6];
  double yz2[6];
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
    tmpj[x] = redaolv_j;
    if (!equal(T_i, T_j)) {
      if (equal(redaolv_i, redaolv_j)) {
        yinzi2 += (T_j - T_i) * redaolv_i / size;
      }
      else
        yinzi2 += (T_j - T_i) / (size / (2 * redaolv_i) + size / (2 * redaolv_j));
    }
    yz2[x] = yinzi2;
  }
  double T_next = T_i + yinzi1 * yinzi2;


  //  发热材料发热
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

SgnFileReader TemField::reader(std::ifstream("huazhu\\pn_test.sgn", std::ios::in | std::ios::binary));
}//namespace simulation
