#include<iostream>
#include<Eigen>
#include<cmath>
using namespace std;

int main() {
	float pi = acos(-1);
	float angle = pi/4;
	Eigen::Vector3f p(2.0f, 1.0f, 1);
	Eigen::Matrix3f A;
	Eigen::Matrix3f B;
	A << cos(angle), -sin(angle), 0,
		sin(angle), cos(angle), 0,
		0, 0, 1;
	B << 1, 0, 1,
		0, 1, 2,
		0, 0, 1;
	std::cout << B * A * p << std::endl;
	return 0;
}
