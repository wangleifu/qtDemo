#ifndef TRACE_H
#define TRACE_H
#include <vector>
#include <string>
#include "hust.hpp"
#include "DBHelper.hpp"
using namespace std;

class Trace
{
public:
	Trace();
	~Trace();

public:
	bool print_menu();
	bool push(HUST::FlightData data);              // 把数据 fd 存入向量 flight_data
public:
    bool clear();
    string get_trace();
	bool save(std::string trace_nick_name, std::string describe);                                   // 把数据从向量 flight_data 存入数据库
	bool get(std::string trace_nick_name,
	         vector<HUST::FlightData> &vec_data);  // 从数据库读取数据到向量 flight_data

private:
	vector<HUST::FlightData> flight_data;
	DBHelper                 db_helper;
};
#endif
