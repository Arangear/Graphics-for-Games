//Author:			Daniel Cieslowski
//Last Modified:	26/11/2019
//Student No:		190562751
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

	Vector3 Next(int i = 1);

	float GetNodeCount() { return nodeCount; }

private:
	std::vector<Vector3> waypoints;
	std::vector<Vector3> nodes;
	float nodeCount;
	float alpha = 0.5f;
	float iterator = 0;

	bool rotationType;

	float GetT(float t, Vector3 p0, Vector3 p1);
};