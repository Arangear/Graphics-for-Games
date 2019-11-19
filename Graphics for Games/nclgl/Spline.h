#pragma once
#include "Vector3.h"
#include <vector>

class Spline
{
public:
	Spline(float nodeCount, bool rotation) : nodeCount(nodeCount), rotationType(rotation) {}

	void AddWaypoint(Vector3 point) { waypoints.push_back(point); }
	void ClearWaypoints() { waypoints.clear(); }
	void GenerateNodes();

	Vector3 Next();

private:
	std::vector<Vector3> waypoints;
	std::vector<Vector3> nodes;
	float nodeCount;
	float alpha = 0.5f;
	float iterator = 0;

	bool rotationType;

	float GetT(float t, Vector3 p0, Vector3 p1);
};