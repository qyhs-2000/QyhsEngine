// SPDX-FileCopyrightText: 2021 Jorrit Rouwe
// SPDX-License-Identifier: MIT

#include <Jolt/Jolt.h>

#include <Jolt/TriangleGrouper/TriangleGrouperClosestCentroid.h>
#include <Jolt/Geometry/MortonCode.h>

JPH_NAMESPACE_BEGIN

void TriangleGrouperClosestCentroid::Group(const VertexList &inVertices, const IndexedTriangleList &inTriangles, int inGroupSize, vector<uint> &outGroupedTriangleIndices)
{
	const uint triangle_count = (uint)inTriangles.size();
	const uint num_batches = (triangle_count + inGroupSize - 1) / inGroupSize;

	vector<Vec3> centroids;
	centroids.resize(triangle_count);

	outGroupedTriangleIndices.resize(triangle_count);

	for (uint t = 0; t < triangle_count; ++t)
	{
		// Store centroid
		centroids[t] = inTriangles[t].GetCentroid(inVertices);

		// initialize sort table
		outGroupedTriangleIndices[t] = t;
	}

	vector<uint>::iterator triangles_end = outGroupedTriangleIndices.end();

	// Sort per batch
	for (uint b = 0; b < num_batches - 1; ++b)
	{
		// Get iterators
		vector<uint>::iterator batch_begin = outGroupedTriangleIndices.begin() + b * inGroupSize;
		vector<uint>::iterator batch_end = batch_begin + inGroupSize;
		vector<uint>::iterator batch_begin_plus_1 = batch_begin + 1;
		vector<uint>::iterator batch_end_minus_1 = batch_end - 1;

		// Find triangle with centroid with lowest X coordinate
		vector<uint>::iterator lowest_iter = batch_begin;
		float lowest_val = centroids[*lowest_iter].GetX();
		for (vector<uint>::iterator other = batch_begin; other != triangles_end; ++other)
		{
			float val = centroids[*other].GetX();
			if (val < lowest_val)
			{
				lowest_iter = other;
				lowest_val = val;
			}
		}

		// Make this triangle the first in a new batch
		swap(*batch_begin, *lowest_iter);
		Vec3 first_centroid = centroids[*batch_begin];

		// Sort remaining triangles in batch on distance to first triangle
		sort(batch_begin_plus_1, batch_end, 
			[&first_centroid, &centroids](uint inLHS, uint inRHS)
			{ 
				return (centroids[inLHS] - first_centroid).LengthSq() < (centroids[inRHS] - first_centroid).LengthSq(); 
			});
			
		// Loop over remaining triangles
		float furthest_dist = (centroids[*batch_end_minus_1] - first_centroid).LengthSq();
		for (vector<uint>::iterator other = batch_end; other != triangles_end; ++other)
		{
			// Check if this triangle is closer than the furthest triangle in the batch
			float dist = (centroids[*other] - first_centroid).LengthSq();
			if (dist < furthest_dist)
			{
				// Replace furthest triangle
				uint other_val = *other;
				*other = *batch_end_minus_1;

				// Find first element that is bigger than this one and insert the current item before it
				vector<uint>::iterator upper = upper_bound(batch_begin_plus_1, batch_end, dist, 
					[&first_centroid, &centroids](float inLHS, uint inRHS)
					{
						return inLHS < (centroids[inRHS] - first_centroid).LengthSq(); 
					});
				copy_backward(upper, batch_end_minus_1, batch_end);
				*upper = other_val;

				// Calculate new furthest distance
				furthest_dist = (centroids[*batch_end_minus_1] - first_centroid).LengthSq();
			}
		}
	}
}

JPH_NAMESPACE_END
