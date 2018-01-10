#pragma once
#include<map>
#include"sgn_file_reader.h"

struct database {
  double midu;
  double birerong;
  double redaolv;
  database() :midu(0.0), birerong(0.0), redaolv(0.0) {}
  database(double m, double b, double r) :midu(m), birerong(b), redaolv(r) {}
};

struct fever_t {
	double fever_start_time;
	double fever_end_time;
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
extern database zhutie;
extern database shazi;
extern database kongqi;
extern database fare;

extern std::map<int, database> data;

extern float size;

