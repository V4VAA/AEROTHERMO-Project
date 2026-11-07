#include "EquilFlow.h"
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;

int main(int argc, char** argv)
{
    vector<double> X1, X2, X3;
    
    EquilFlow air6("E6");

    EquilFlow air10("E10");

    X1 = air6.Composition(1, 3000.0, 10132.5);

    cout << "Part A" << endl;

    cout << "Mixture Enthalpy: " << air6.mix.h << endl;
	cout << "Mixture Specific Heat: " << air6.mix.Cp <<endl <<endl;

    double h_a = air6.mix.h;

    X2 = air6.Composition(2, 10132.5, h_a);

    cout << "Part B" << endl;
    cout << "Temperature: " << air6.mix.T << " K" << endl <<endl;

    X3 = air10.Composition(1, 6000.0, 25331.25);

    cout << "Part C" << endl;
    cout << "Mixture Enthalpy: " << air10.mix.h << " kJ/kg" << endl;
    cout << "Mixture Specific Heat: " << air10.mix.Cp << endl;

    for (int i = 0; i < air10.Spc.size(); ++i)
        cout << air10.Spc[i].name << " mole fraction: " << air10.Spc[i].X << endl;
    
    return 0;
}