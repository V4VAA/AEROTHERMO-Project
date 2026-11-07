#pragma once
#include <vector>
#include <string>
#include <iostream>

using namespace std;

/*namespace EquilFlow
{

	vector<double> Composition(int calcopt, double in1, double in2);
	vector<double> NormalShock(double altitude, double Mach);
	vector<double> ObliqueShock(double altitude, double Mach, double theta, double Beta_override = 0);
	vector<double> AeroCoeffs(double eps, double delta, double rn, double rb, double AoA);
	vector<double> StagFayRiddell(double T, double P, double U, double rnose, int wall);

	//const double* const Composition(int calcopt, double in1, double in2);
}*/

	class EquilFlow
	{
	public:
		EquilFlow();
		EquilFlow(string in);
		~EquilFlow();

		int GasModel;
		double Pinf,Tinf,rho_inf,Uinf,ainf;
		double CN, CA, CD, CL, LOD, CPmax;
		vector<double> freestream;
		vector<double> Composition(int calcopt, double in1, double in2);
		vector<double> NormalShock(double altitude, double Mach);
		vector<double> NormalShock(double pressure, double temperature, double U1);
		vector<double> ObliqueShock(double altitude, double Mach, double theta, double Beta_override = 0);
		vector<double> ObliqueShock(double pressure, double temperature, double U1, double theta, double Beta_override = 0);
		vector<double> AeroSphereCone(double eps, double delta, double rn, double rb, double AoA);
		vector<double> StagFayRiddell(double T, double P, double U, double rnose, int wall);
		void AeroEllipsoid(double eps, double delta, double AoA);
		void FreestreamTable(ifstream &fin);
		struct species
		{
			species()
			{
				name = "default";
				CPi = 0;
				MWi = 0;
				X = 0;
			
			}
			species(const species& RHS)
			{
				name = RHS.name;
				CPi = RHS.CPi;
				MWi = RHS.MWi;
				X = RHS.X;
			}
			~species()
			{ 	}
			string name;
			double CPi;//species specific heat in J/mol-K
			double MWi;//species molecular weight
			double X;//mole fraction


		};

		struct mixture
		{
			mixture()
			{
				T=0;
				P=0;
				Cp=0;
				h=0;
				rho=0;
				MW=0;
				gamma=0;
				mu=0;
				Dim=0;
				kf=0;
				Pr=0;
				Le=0;
				U2=0;
				Beta=0;

			}
			~mixture() {}
			double T;
			double P;
			double Cp;
			double h;
			double rho;
			double MW;
			double gamma;
			double mu;
			double Dim;
			double kf;
			double Pr;
			double Le;
			double U2;
			double Beta;

		};

		vector<species> Spc;
		mixture mix;
	};
