#include <iostream>
#include "../OS12_COM_LIB/OS12.h"
#pragma comment(lib, "../x64/Debug/OS12_COM_LIB.lib")

int main()
{
	try
	{
		OS12HANDEL h1 = OS12::Init();
		OS12HANDEL h2 = OS12::Init();

		std::cout << "OS12::Adder::Add(h1, 8, 5) = " << OS12::Adder::Add(h1, 8, 5) << "\n";
		std::cout << "OS12::Adder::Add(h2, 8, 5) = " << OS12::Adder::Add(h2, 8, 5) << "\n";

		std::cout << "OS12::Adder::Sub(h1, 8, 5) = " << OS12::Adder::Sub(h1, 8, 5) << "\n";
		std::cout << "OS12::Adder::Sub(h2, 8, 5) = " << OS12::Adder::Sub(h2, 8, 5) << "\n";

		std::cout << "OS12::Multiplier::Mul(h1, 8, 5) = " << OS12::Multiplier::Mul(h1, 8, 5) << "\n";
		std::cout << "OS12::Multiplier::Mul(h2, 8, 5) = " << OS12::Multiplier::Mul(h2, 8, 5) << "\n";

		std::cout << "OS12::Multiplier::Div(h1, 8, 5) = " << OS12::Multiplier::Div(h1, 8, 5) << "\n";
		std::cout << "OS12::Multiplier::Div(h2, 8, 5) = " << OS12::Multiplier::Div(h2, 8, 5) << "\n";

		OS12::Dispose(h1);
		OS12::Dispose(h2);

	}
	catch (int e) { std::cout << "OS12: error = " << e << "\n"; }

	system("pause");

}


