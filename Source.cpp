#include <iostream>
#include <vector>
#include <algorithm>
#include <time.h>
#include <iterator>
#include <numeric>
#include <random>
#include <sstream>
#include <iomanip>

using namespace std;

int numberOfPoints = 100;

class point : public vector<double>
{
	friend ostream& operator<<(ostream& os, const point& p);
public:
	point() {}
	point(int dimension) : vector<double>(dimension) {}
	point operator+(const point& p) const
	{
		point wyn(size());
		transform(begin(), end(), p.begin(), wyn.begin(), plus<double>());
		return wyn;
	}
	point& operator-=(const point& p)
	{
		transform(begin(), end(), p.begin(), begin(), minus<double>());
		return *this;
	}
	point operator-(const point& p)
	{
		point wyn(size());
		transform(begin(), end(), p.begin(), wyn.begin(), minus<double>());
		return wyn;
	}
};

ostream& operator<<(ostream& os, const point& p)
{
	copy(p.begin(), p.end(), ostream_iterator<double>(cout, " "));
	return os;
}

double calculateCentr(double& p)
{
	return p = p / numberOfPoints;
}

template<int expectedValue, int deviation>
class numberGenerator
{
public:
	default_random_engine& random;
	numberGenerator(default_random_engine& random) : random(random) {}
	double operator()()
	{
		normal_distribution<double> normal(expectedValue, deviation);
		return normal(random);
	}
};

template<int expectedValue, int deviation>
class pointsGenerator
{
public:
	int n;
	default_random_engine& random;
	pointsGenerator(int n, default_random_engine& random) : n(n), random(random) {}
	point operator()() {
		point pkt(n);
		generate(pkt.begin(), pkt.end(), numberGenerator<expectedValue, deviation>(random));
		return pkt;
	}
};



double diffPow2(double x, double y)
{
	return pow((x - y), 2.0);
};

int main()
{
	int dimension;
	srand(time(NULL));
	static default_random_engine random(rand());

	cout << "Enter the number of dimensions: ";
	cin >> dimension;

	cout << endl << endl;

	vector<point> cloudA(numberOfPoints);

	cout << endl << numberOfPoints << " points from normal distribution N(0,10) were drawn to cloud A and " << numberOfPoints << " points from N(10,10) to cloud B\n";

	generate(cloudA.begin(), cloudA.end(), pointsGenerator<5, 10>(dimension, random));

	point centrcloudA(dimension);

	centrcloudA = accumulate(cloudA.begin(), cloudA.end(), centrcloudA);

	for_each(centrcloudA.begin(), centrcloudA.end(), calculateCentr);

	cout << "Cloud A centroid co-ordinates: \n";
	copy(centrcloudA.begin(), centrcloudA.end(), ostream_iterator<double>(cout, " "));
	cout << endl << endl;

	vector<point> cloudB(numberOfPoints);

	generate(cloudB.begin(), cloudB.end(), pointsGenerator<10, 10>(dimension, random));

	point centrcloudB(dimension);

	centrcloudB = accumulate(cloudB.begin(), cloudB.end(), centrcloudB);

	for_each(centrcloudB.begin(), centrcloudB.end(), calculateCentr);

	cout << "Cloud B centroid co-ordinates: \n";
	copy(centrcloudB.begin(), centrcloudB.end(), ostream_iterator<double>(cout, "  "));

	cout << "\n\nEuclidean distance between centroids: ";
	std::cout << sqrt(std::inner_product(centrcloudA.begin(), centrcloudA.end(), centrcloudB.begin(), 0,
		std::plus<double>(), diffPow2));
	cout << endl << endl;

	//Counting how many points are closer to other cloud
	int closerToA = count_if(cloudA.begin(), cloudA.end(), [&](point P)
		{
			double A = sqrt(std::inner_product(P.begin(), P.end(), centrcloudA.begin(), 0,
				std::plus<double>(), diffPow2));
			double B = sqrt(std::inner_product(P.begin(), P.end(), centrcloudB.begin(), 0,
				std::plus<double>(), diffPow2));
			if (A < B)
				return true;
			else
				return false;
		});
	int closerToB = count_if(cloudB.begin(), cloudB.end(), [&](point P)
		{
			double B = sqrt(std::inner_product(P.begin(), P.end(), centrcloudB.begin(), 0,
				std::plus<double>(), diffPow2));
			double A = sqrt(std::inner_product(P.begin(), P.end(), centrcloudA.begin(), 0,
				std::plus<double>(), diffPow2));
			if (B < A)
				return true;
			else
				return false;

		});


	cout << "CloudA's points which are closer to B centroid: " << numberOfPoints - closerToA;
	cout << "\nCloudB's points which are closer to A centroid: " << numberOfPoints - closerToB;
	return 0;
}