#pragma once
#include<fstream>
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
  explicit TemField();
  TemField(short i, short j, short k);
  virtual ~TemField();
  uint32_t GetNodeIndex(short i, short j, short k) const;
  void SwapTemField(TemField& rhr);
  void Calculate(const TemField& last);
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

 private:
  static SgnFileReader reader;
  HeadTem header;
  double *tem_field_;
  short nx;
  short ny;
  short nz;
};
}