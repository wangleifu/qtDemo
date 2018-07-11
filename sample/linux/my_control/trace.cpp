#include "trace.hpp"
using namespace std;
Trace::Trace()
{
}
Trace::~Trace()
{
}

bool
Trace::print_menu()
{
	db_helper.select_menu_and_print();
	return true;
}
bool
Trace::push(HUST::FlightData data)
{
	flight_data.push_back(data);
	return true;
}
bool
Trace::save(std::string trace_nick_name, std::string describe)
{
	db_helper.insert_menu(trace_nick_name, describe, flight_data);
	return true;
}
bool
Trace::get(std::string trace_nick_name, vector<HUST::FlightData> &vec_data)
{
	// 查询数据库，对每一行数据封装进fd， 然后装进 flight_data 返回。
	db_helper.select_menu(trace_nick_name, vec_data);
	return true;
}
bool
Trace::clear()
{
    std::cout << "clear flight data" << std::endl;
    vector<HUST::FlightData>().swap(flight_data);
}
string
Trace::get_trace()
{
    return db_helper.get_traces();
}
