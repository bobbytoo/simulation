#pragma once
#include<map>

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
extern std::map<int64_t,float> grid_m_Al;
extern std::map<int64_t,float> grid_m_Si;

extern short ni;
extern short nj;
extern short nk;
extern short* sgn_file;
extern float* tem_field;
extern short fever_num;
extern double density;
extern double sh;
extern short size;
extern double step;

//该函数用来导入我所需要的数据
void LoadData(short* sgn,float* t_field,short nx,short ny,short nz,short num,double sleeve_density,
		      double sleeve_sh,short grid_size,double time_step) {
	sgn_file = sgn;
	tem_field = t_field;
	ni = nx;
	nj = ny;
	nk = nz;
	fever_num = num;
	density = sleeve_density;
	sh = sleeve_sh;
	size = grid_size;
	step = time_step;
}

int64_t GetNodeIndex(short i, short j, short k) {
	if (i >= ni || j >= nj || k >= nk || i < 0 || j < 0 || k < 0)
		return -1;
	return k + nk * j + nj * nk*i;
}

void FeverInit() {
	double grid_v = size * size * size;
	//设置发热参数
	fever_struct.fever_start_time = 18; //s
	fever_struct.perm_Al = 0.17;
	fever_struct.perm_Si = 0.l;
	fever_struct.v_Al = 6.8;
	fever_struct.v_Si = 4;
	fever_struct.H_Al = 838000;
	fever_struct.H_Si = 1378510;
	fever_struct.m_Al = density * grid_v * fever_struct.perm_Al;
	fever_struct.m_Si = density * grid_v * fever_struct.perm_Si;
	setFeverGrid(fever_struct.m_Al, fever_struct.d_mSi);
	fever_struct.d_mAl = fever_struct.v_Al * step * grid_v;
	fever_struct.d_mSi = fever_struct.v_Si * step * grid_v;
	fever_struct.d_q = fever_struct.d_mAl / (27 * 2) * fever_struct.H_Al \
		+ fever_struct.d_mSi / (28 * 3) * fever_struct.H_Si;
	fever_struct.T_i_fever = fever_struct.d_q * 1000 / (density * sh * grid_v);
}

void setFeverGrid(double m_Al,double m_Si) {
	for (short k = 0; k < nk; ++k) {
		for (short j = 0; j < nj; ++j) {
			for (short i = 0; i < ni; ++i) {
				int64_t index = GetNodeIndex(i, j, k);
				if (sgn_file[index] == fever_num) {
					grid_m_Al.insert(std::pair<int64_t, double>(index, fever_struct.m_Al));
					grid_m_Si.insert(std::pair<int64_t, double>(index, fever_struct.m_Si));
				}
			}
		}
	}
}

void eachStep() {
	for (short k = 0; k < nk; ++k) {
		for (short j = 0; j < nj; ++j) {
			for (short i = 0; i < ni; ++i) {
				int64_t index = GetNodeIndex(i, j, k);
				if (sgn_file[index] == fever_num) {
					tem_field[index] += GetFeverT(index);
				}
			}
		}
	}
}

float GetFeverT(int64_t index) {

}
