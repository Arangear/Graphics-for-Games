//Author:			Daniel Cieslowski
//Last Modified:	25/11/2019
//Student No:		190562751
#include "Spline.h"

void Spline::GenerateNodes()
{
	nodes.clear();

	std::vector<Vector3> waypointsLoop(waypoints);
	if (!rotationType)
	{
		waypointsLoop.push_back(waypoints[0]);
		waypointsLoop.push_back(waypoints[1]);
		waypointsLoop.push_back(waypoints[2]);
	}
	else
	{
		waypointsLoop.push_back(Vector3(waypoints[0].x, waypoints[0].y + 360.0f, waypoints[0].z));
		waypointsLoop.push_back(Vector3(waypoints[1].x, waypoints[1].y + 360.0f, waypoints[1].z));
		waypointsLoop.push_back(Vector3(waypoints[2].x, waypoints[2].y + 360.0f, waypoints[2].z));
	}

	for (int i = 0; i < waypointsLoop.size() - 3; i++)
	{
		Vector3 p0 = waypointsLoop[i];
		Vector3 p1 = waypointsLoop[i + 1];
		Vector3 p2 = waypointsLoop[i + 2];
		Vector3 p3 = waypointsLoop[i + 3];

		float t0 = 0.0f;
		float t1 = GetT(t0, p0, p1);
		float t2 = GetT(t1, p1, p2);
		float t3 = GetT(t2, p2, p3);

		for (float t = t1; t < t2; t += ((t2 - t1) / (nodeCount / (float)waypoints.size())))
		{
			Vector3 a1 = p0 * (t1 - t) / (t1 - t0) + p1 * (t - t0) / (t1 - t0);
			Vector3 a2 = p1 * (t2 - t) / (t2 - t1) + p2 * (t - t1) / (t2 - t1);
			Vector3 a3 = p2 * (t3 - t) / (t3 - t2) + p3 * (t - t2) / (t3 - t2);

			Vector3 b1 = a1 * (t2 - t) / (t2 - t0) + a2 * (t - t0) / (t2 - t0);
			Vector3 b2 = a2 * (t3 - t) / (t3 - t1) + a3 * (t - t1) / (t3 - t1);

			Vector3 c = b1 * (t2 - t) / (t2 - t1) + b2 * (t - t1) / (t2 - t1);

			nodes.push_back(c);
		}
	}
}

Vector3 Spline::Next(int i)
{
	iterator+=i;
	while (iterator >= nodes.size())
	{
		iterator -= nodes.size();
	}
	Vector3 result = nodes[iterator];
	return result;
}

float Spline::GetT(float t, Vector3 p0, Vector3 p1)
{
	float a = powf((p1.x - p0.x), 2.0f) + powf((p1.y - p0.y), 2.0f) + powf((p1.z - p0.z), 2.0f);
	float b = powf(a, 0.5f);
	float c = powf(b, alpha);

	return (c + t);
}