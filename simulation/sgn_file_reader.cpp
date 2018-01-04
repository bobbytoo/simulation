#include"sgn_file_reader.h"
#include<iostream>
namespace simulation {
SgnFileReader::SgnFileReader(std::ifstream&& in) :in_(std::move(in)), grid_(nullptr),dx_(nullptr),dy_(nullptr),dz_(nullptr) {
  in_.read(reinterpret_cast<char*>(&filehead_), sizeof(filehead_));
  grid_ = new short[filehead_.nx*filehead_.ny*filehead_.nz];
  in_.read(reinterpret_cast<char*>(grid_), sizeof(short)*(filehead_.nx*filehead_.ny*filehead_.nz));
  dx_ = new float[filehead_.nx];
  in_.read(reinterpret_cast<char*>(dx_), sizeof(float)*filehead_.nx);
  dy_ = new float[filehead_.ny];
  in_.read(reinterpret_cast<char*>(dy_), sizeof(float)*filehead_.ny);
  dz_ = new float[filehead_.nz];
  in_.read(reinterpret_cast<char*>(dz_), sizeof(float)*filehead_.nz);
}

SgnFileReader::SgnFileReader(SgnFileReader&& rsgn) {
	in_ = std::move(rsgn.in_);
	filehead_ = rsgn.filehead_;
	dx_ = rsgn.dx_;
	dy_ = rsgn.dy_;
	dz_ = rsgn.dz_;
	rsgn.dx_ = nullptr;
	rsgn.dy_ = nullptr;
	rsgn.dz_ = nullptr;
}

SgnFileReader::~SgnFileReader() {
  in_.close();
  delete grid_;
}

void SgnFileReader::OutToTecplot(std::ofstream& out) const {
  out << "TITLE = \"TEST\"\n";
  out << "VARIABLES = \"X\",\"Y\",\"Z\",\"T\"\n";
  int nx = filehead_.nx;
  int ny = filehead_.ny;
  int nz = filehead_.nz;
  out << "ZONE I=" << nx << ",J=" << ny << ",K=" << nz << ",F=POINT\n";
  for (int k = 0; k < nz; ++k) {
    for (int j = 0; j < ny; ++j) {
      for (int i = 0; i < nx; ++i) {
        int64_t offset = k + nz*j + ny*nz*i;
        out << i << " " << j << " " << k << " " << grid_[offset] << std::endl;
      }
    }
  }
}
}//namespace simulation