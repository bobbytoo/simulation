#include<map>
#include<fstream>
#include"stdafx.h"
#include"sgn_file_reader.h"

database zhutie(7200.0, 669.9, 37.2);
database shazi(1500.0, 1172.3, 1.63);
database kongqi(1.29, 1004.0, 0.023);

std::map<int, database> data;

double size;
double airtem;
double tem_step;

simulation::SgnFileReader reader(std::ifstream("D:\\huazhu\\result2.sgn", std::ios::in | std::ios::binary));