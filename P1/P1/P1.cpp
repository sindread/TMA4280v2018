// P1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "zeta0.h"
#include "mach0.h"
#include <iostream>
#include <limits>

using namespace std;


int main()
{
	zeta0 zeta;
	mach0 mach;
	
	cout.precision(std::numeric_limits<double>::digits10 + 2);
	//cout << fixed << zeta.calculatePi(3) << endl;
	cout << fixed << mach.calculatePi(3) << endl;

	/*cout << zeta.zUnitTest() << endl;
	cout << mach.mUnitTest() << endl;
	*/

	/*double* zetaVerify = zeta.zVerificationTest();
	for (int i = 0; i < 24; ++i)
	{
		cout << "k=" << i + 1 << ": " << *(zetaVerify + i) << endl;
	}

	double* machVerify = mach.mVerificationTest();
	for (int i = 0; i < 24; ++i)
	{
		cout << "k=" << i + 1 << ": " << *(machVerify + i) << endl;
	}*/


    return 0;
}

