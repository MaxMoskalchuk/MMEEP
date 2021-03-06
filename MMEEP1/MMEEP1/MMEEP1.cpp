#include "stdafx.h"

#include <conio.h>
#include <windows.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstring>

#include "gnuplot_i.hpp"


using namespace std;
string s;
struct Point
{
	double price;
	double demand;
	double supply;
};
vector<Point> point(13);
class SolveSystem {
public:
	static const int count = 13;
	static vector<double> Gauss(vector<vector<double> > A, vector<double> &b)
	{
		int n = A.size();
		vector<double> x(n, 0);
		double det = 1.0;
		for (int i = 0; i < n; i++) A[i].push_back(b[i]);
		for (int k = 0; k < n; k++)
		{
			int next = k;
			for (int i = k + 1; i < n; i++)
				if (abs(A[i][k]) > abs(A[next][k]))next = i;
			if (next != k)
			{
				for (int i = k; i <= n; i++)swap(A[k][i], A[next][i]);
			}
			det *= A[k][k];
			for (int i = k + 1; i < n; i++)
			{
				double c = A[i][k] / A[k][k];
				for (int j = k; j <= n; j++)
				{
					A[i][j] = A[i][j] - c * A[k][j];
				}
			}
		}
		x[n - 1] = A[n - 1][n] / A[n - 1][n - 1];
		for (int i = n - 2; i >= 0; i--)
		{
			double sum = 0;
			for (int j = i + 1; j < n; j++)sum += A[i][j] * x[j];
			x[i] = (A[i][n] - sum) / A[i][i];
		}
		return x;
	}
	static vector<double> subtract(vector<double> x, vector<double> y)
	{
		vector<double> res(x.size());
		for (int i = 0; i<x.size(); ++i)
		{
			res[i] = x[i] - y[i];
		}
		return res;
	}
	static vector<double> calcF(vector<double> x, bool flag)
	{
		vector<double> v(3);
		v[0] = v[1] = v[2] = 0;
		for (int i = 0; i < count; i++)
		{
			double a;
			if (flag) a = point[i].supply; else a = point[i].demand;
			v[0] += x[0] + x[1] * exp(x[2] * point[i].price) - a;
			v[1] += (x[0] + x[1] * exp(x[2] * point[i].price) - a)* exp(x[2] * point[i].price);
			v[2] += (x[0] + x[1] * exp(x[2] * point[i].price) - a)*point[i].price*x[1] * exp(x[2] * point[i].price);
		}
		return v;
	}
	static vector<vector<double> > Jacobi(vector<double> x, bool flag)
	{
		vector<vector<double> > v(3, vector<double>(3));
		for (int i = 0; i < count; i++)
		{
			v[0][0] += 1;
			v[0][1] += exp(x[2] * point[i].price);
			v[0][2] += x[1] * point[i].price * exp(x[2] * point[i].price);

			double a;
			if (flag) a = point[i].supply; else a = point[i].demand;
			v[1][0] += exp(x[2] * point[i].price);
			v[1][1] += exp(2 * x[2] * point[i].price);
			v[1][2] += x[0] * point[i].price*exp(x[2] * point[i].price) +
				2 * x[1] * point[i].price * exp(2 * x[2] * point[i].price) - a * point[i].price*exp(x[2] * point[i].price);

			v[2][0] += point[i].price*x[1] * exp(x[2] * point[i].price);
			v[2][1] += x[0] * point[i].price*exp(x[2] * point[i].price) +
				2 * x[1] * point[i].price * exp(2 * x[2] * point[i].price) -
				a * point[i].price * exp(x[2] * point[i].price);
			v[2][2] += x[0] * point[i].price*point[i].price*x[1] * exp(x[2] * point[i].price) +
				2 * x[1] * x[1] * point[i].price*point[i].price*x[1] * exp(2 * x[2] * point[i].price) -
				a*x[1] * point[i].price*point[i].price*x[1] * exp(x[2] * point[i].price);
		}
		return v;
	}
	static vector<double> SolveWithNewton(double eps, vector<double> init_x, bool flag)
	{
		vector<double> x(3);
		x[0] = init_x[0]; x[1] = init_x[1]; x[2] = init_x[2];
		int step = 0;
		while (step<5000) {
			vector<vector<double> > A = Jacobi(x, flag);
			vector<double> b = calcF(x, flag);
			vector<double> z = Gauss(A, b);
			x = subtract(x, z);
			double norm = 0.0;
			for (int i = 0; i < z.size(); i++)norm = max(norm, abs(z[i]));
			step++;
			//if (norm < eps)break;
		} cout << fixed;
		cout.precision(5);
		cout << "Steps: " << step << endl;
		cout << "Solution:";
		for (int i = 0; i < x.size(); i++)cout << " " << x[i];
		cout << endl;
		return x;
	}
};

const int count = 12;
pair<double, double> eq_points(vector<double> supply, vector<double> demand, double p)
{

	double lx = -1000, rx = 1000;
	while (abs(rx - lx) >  1e-10) {
		double midx = 0.5 * (lx + rx);
		double y1 = supply[0] + supply[1] * exp((lx-p)*supply[2]);
		double y2 = demand[0] + demand[1] * exp(lx*demand[2]);
		double midy1 = supply[0] + supply[1] * exp((midx-p)*supply[2]);
		double midy2 = demand[0] + demand[1] * exp(midx*demand[2]);

		if ((y1 - y2 - 1e-10 > 0 && midy1 - midy2 - 1e-10 > 0) || (y1 - y2 + 1e-10 < 0 && midy1 - midy2 + 1e-10 < 0)) {
			lx = midx;
		}
		else {
			rx = midx;
		}
	}
	return make_pair(lx, supply[0] + supply[1] * exp((lx-p)*supply[2]));
}
void wait_for_key() {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
	cout << endl << "Press ENTER to continue..." << endl;
	FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
	_getch();
#elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
	cout << endl << "Press ENTER to continue..." << endl;
	cin.clear();
	cin.ignore(cin.rdbuf()->in_avail());
	cin.get();
#endif
	return;
}
int main()
{
	freopen("input.txt", "r", stdin);
	int i, j;
	for (i = 0; i < 13; i++)
	{
		cin >> point[i].price >> point[i].demand >> point[i].supply;
	}
	vector<double> supply = SolveSystem::SolveWithNewton(0.0001, { 940, -941, -0.00019 }, true);


	vector<double> demand = SolveSystem::SolveWithNewton(0.0001, { 1, 200, -0.7 }, false);

	/*
	for (i = 0; i < 13; i++)
	{
	cout << point[i].supply << " == " << supply[0] + supply[1] * exp(point[i].price*supply[2]) << endl;
	}
	for (i = 0; i < 13; i++)
	{
	cout << point[i].demand << " == " << demand[0] +demand[1] * exp(point[i].price*demand[2]) << endl;
	}*/
	pair<double, double> eq = eq_points(supply, demand, 0.);
	cout << "Equilibrium point" << endl;
	cout << eq.first << " " << eq.second << endl;
	double a, b, c, d;
	a = (supply[0] + supply[1] * exp((eq.first + 0.001)*supply[2])) - (supply[0] + supply[1] * exp((eq.first)*supply[2]));
	b = a*eq.first / (0.001 * (supply[0] + supply[1] * exp((eq.first)*supply[2])));

	c = (demand[0] + demand[1] * exp((eq.first + 0.001)*demand[2])) - (demand[0] + demand[1] * exp((eq.first)*demand[2]));
	d = c*eq.first / (0.001 * (demand[0] + demand[1] * exp((eq.first)*demand[2])));


	cout << "Supply elasticity at the equilibrium: " << b << endl;
	cout << "Demand elasticity at the equilibrium: " << d << endl;
	cout << (abs(d) > abs(b) ? "Stable" : "Unstable") << endl;

	double avg_supply, avg_demand, avg_price, sum1 = 0, sum2 = 0, sum3 = 0;

	double min_y = 100000, min_x = 100000, max_y = -10000, max_x = -10000;
	for (i = 0; i < 13; i++)
	{
		sum1 += point[i].demand;
		sum2 += point[i].supply;
		sum3 += point[i].price;
		min_y = min(min_y, point[i].price);
		max_y = max(max_y, point[i].price);
		min_x = min(min_x, min(point[i].supply, point[i].demand));
		max_x = max(max_x, max(point[i].supply, point[i].demand));

	}
	avg_demand = sum1 / 13.;
	avg_supply = sum2 / 13.;
	avg_price = sum3 / 13.;

	double aa = point[12].supply - point[0].supply;
	double bb = point[12].price - point[0].price;
	double el_s = (aa / bb) / (avg_supply / avg_price);

	double cc = point[12].demand - point[0].demand;
	double dd = point[12].price - point[0].price;
	double el_d = (cc / dd) / (avg_demand / avg_price);

	cout << "Supply: " << el_s << " - ";
	if (abs(el_s) < 1.) cout << "inelastic" << endl; else cout << "elastic" << endl;
	cout << "Demand: " << el_d << " - ";
	if (abs(el_d) < 1.) cout << "inelastic" << endl; else cout << "elastic" << endl;

	vector<pair<double, double>> v_eq_p;
	for (double p = 0.25; p <2.1; p +=0.25 )
	{
		v_eq_p.push_back(eq_points(supply, demand, p));
		cout << v_eq_p.back().first << " " << v_eq_p.back().second << endl;
	}





	Gnuplot plot;
	plot.set_title("MMEEP");
	plot.set_legend("outside");
	plot.set_xlabel("Q");
	plot.set_ylabel("P");
	plot.set_xrange(0, max_x + 5);
	plot.set_yrange(0, max_y + 1);
	plot.set_grid();

	plot.cmd("set style line 1 lc rgb '#3fd815' pt 7 ps 1.5 lt 1 lw 2\n");
	plot.cmd("set style line 2 lc rgb '#1534c1' pt 7 ps 1.5 lt 1 lw 2\n");
	plot.cmd("set style line 3 lc rgb '#cccc14' pt 7 ps 1.5 lt 1 lw 2\n");
	plot.cmd("set style line 4 lc rgb '#d30610' pt 7 ps 1.5 lt 1 lw 2\n");
	plot.cmd("set style line 5 lc rgb '#d006e2' pt 7 ps 1.5 lt 1 lw 2\n");
	plot.cmd("set style line 6 lc rgb '#0afffa' pt 7 ps 1.5 lt 1 lw 2\n");
	plot.cmd("set style line 7 lc rgb '#ff6600' pt 7 ps 1.5 lt 1 lw 2\n");
	plot.cmd("set style line 8 lc rgb '#000000' pt 7 ps 1.5 lt 1 lw 2\n");


	plot.cmd("f(x)=(log((x-" + to_string(supply[0]) + ")/" + to_string(supply[1]) +
		"))/" + to_string(supply[2]) + "\n");
	///plot.cmd("plot f(x)\n");
	plot.cmd("g(x)=(log((x-" + to_string(demand[0]) + ")/" + to_string(demand[1]) +
		"))/" + to_string(demand[2]) + "\n");
	plot.cmd("plot f(x) ls 1 title \"Supply\"\n");
	plot.cmd("replot g(x) ls 2 title \"Demand\"\n");


	ofstream ofs("supply.txt");
	for (i = 0; i < 13; i++) {
		ofs << point[i].supply << "   " << point[i].price << endl;
	}
	ofs.close();

	ofs.open("demand.txt");
	for (i = 0; i < 13; i++) {
		ofs << point[i].demand << "   " << point[i].price << endl;
	}
	ofs.close();


	plot.cmd("replot 'C:/Users/Asus/Source/Repos/MMEEP/MMEEP1/MMEEP1/supply.txt' w p ls 1 title 'Supply points'\n");
	plot.cmd("replot 'C:/Users/Asus/Source/Repos/MMEEP/MMEEP1/MMEEP1/demand.txt' w p ls 2 title 'Demand points'\n");

	plot.cmd("eqx=" + to_string(eq.second) + "\n");
	plot.cmd("eqy=" + to_string(eq.first) + "\n");
	plot.cmd("replot '+' using (eqx):(eqy) title \"Equilibrium\"  w p ls 7\n");

	i = 0;
	ofs.open("point.txt");

	for (double p = 0.25; p < 2.1; p += 0.25)
	{
		plot.cmd("s" + to_string(i + 1) + "(x)="+to_string(p)+" + (log((x-" + to_string(supply[0]) + ")/" + to_string(supply[1]) +
			"))/" + to_string(supply[2]) + "\n");
		plot.cmd("replot s" + to_string(i + 1) + "(x) ls 6 title \"Supply with tax " + to_string(p) + "$\"");
		ofs << v_eq_p[i].second << "   " << v_eq_p[i].first << endl;
		i++;
	}
	ofs.close();
	plot.cmd("replot 'C:/Users/Asus/Source/Repos/MMEEP/MMEEP1/MMEEP1/point.txt' w p ls 8 title 'Equilibrium points with tax'\n");

	wait_for_key();

	return 0;
}