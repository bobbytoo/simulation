#pragma once
#include<fstream>
#include<functional>
#include"sgn_file_reader.h"
#include"stdafx.h"

namespace simulation {
inline
bool equal(double a, double b) {
  return (a - b > -0.0001 && a - b < 0.0001);
}

class TemField {
 public:
  #pragma pack(1)
  struct HeadTem {
    short SN;
    float Time;
    float Tmax;
    float Tmin;
    float TL;
    float TS;
    float ALLCPUTime;
    char ss[230];
  };
  #pragma pack()
  explicit TemField(float step);
  TemField(short i, short j, short k, float step);
  TemField(float *t_field,short x,short y,short z,float step);
  virtual ~TemField();
  uint32_t GetNodeIndex(short i, short j, short k) const;
  void SwapTemField(TemField& rhr);
  void Calculate(const TemField& last,short index);
  double GetNextTem(short i, short j, short k) const;
  void SetHeader(const TemField& last,double time_step);
  void OutToTecplot(std::ofstream& out) const;
  void OutToTecplotZoo(std::ofstream& out) const;
  inline
  database GetNodeData(const SgnFileReader& reader,uint32_t index) const {
    if (index == -1)
      return data[1];
    else if (reader.grid_[index] == 100)
      return data[2];
 
    return data[reader.grid_[index]];
  }
  void CalculatePartOne(const TemField& last);
  void CalculatePartTwo(const TemField& last);
  void CalculatePartThree(const TemField& last);
  void CalculatePartFour(const TemField& last);
  double GetFeverTem() const;
  void TemSimulation(uint32_t times);
  void TemSimulation_F(uint32_t times,short index);
  inline
  void dropOtherField() {
	tem_field_ = nullptr;
  }

public:
  static SgnFileReader reader;
  HeadTem header;
  float *tem_field_;
  float *my_field;
  short nx;
  short ny;
  short nz;
  float tem_step;
};
}