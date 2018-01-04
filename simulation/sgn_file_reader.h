#pragma once
#include<fstream>
#include<utility>

/*sgn网格剖分文件读取类*/
namespace simulation {
class SgnFileReader {
 public:
  #pragma pack(1)
  struct HeadSgn {
    char version[10];
	  short flag;
	  short nx;
	  short ny;
	  short nz;
	  char space[238];
  };
  #pragma pack()
  explicit SgnFileReader(std::ifstream&& in);
  SgnFileReader(SgnFileReader&& rsgn);
  virtual ~SgnFileReader();
  void OutToTecplot(std::ofstream& out) const;
  inline
  short GetNx() const {
    return filehead_.nx;
  }
  inline
  short GetNy() const {
    return filehead_.ny;
  }
  inline
  short GetNz() const {
    return filehead_.nz;
  }
  short *grid_;

 private:
  std::ifstream in_;
  HeadSgn filehead_;
  float *dx_;
  float *dy_;
  float *dz_;
};
}//namespace simulation