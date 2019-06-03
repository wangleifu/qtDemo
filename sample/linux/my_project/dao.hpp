#ifndef DAO_HPP
#define DAO_HPP
class DAO
{
public:
	DAO();
	~DAO();

public:
	void save();
	void delet();
	void update();
	void select();

private:
	Model model;
};

#endif