#pragma once
#include<map>
#include"sgn_file_reader.h"

struct database
{
  double midu;
  double birerong;
  double redaolv;
  database() :midu(0.0), birerong(0.0), redaolv(0.0) {}
  database(double m, double b, double r) :midu(m), birerong(b), redaolv(r) {}
};

extern database zhutie;
extern database shazi;
extern database kongqi;

extern std::map<int, database> data;

extern double size;
extern double airtem;
extern double tem_step;

extern simulation::SgnFileReader reader;