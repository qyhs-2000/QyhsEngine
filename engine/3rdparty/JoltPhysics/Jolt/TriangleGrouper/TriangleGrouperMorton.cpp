// SPDX-FileCopyrightText: 2021 Jorrit Rouwe
// SPDX-License-Identifier: MIT

#include <Jolt/Jolt.h>

#include <Jolt/TriangleGrouper/TriangleGrouperMorton.h>
#include <Jolt/Geometry/MortonCode.h>

JPH_NAMESPACE_BEGIN

void TriangleGrouperMorton::Group(const VertexList &inVertices, const IndexedTriangleList &inTriangles, int inGroupSize, vector<uint> &outGroupedTriangleIndices)
{
	const uint triangle_count = (uint)inTriangles.size();

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

	// Get bounding box of all centroids
	AABox centroid_bounds;
	for (uint t = 0; t < triangle_count; ++t)
		centroid_bounds.Encapsulate(centroids[t]);

	// Make sure box is not degenerate
	centroid_bounds.EnsureMinimalEdgeLength(1.0e-5f);

	// Calculate morton code for each centroid
	vector<uint32> morton_codes;
	morton_codes.resize(triangle_count);
	for (uint t = 0; t < triangle_count; ++t)
		morton_codes[t] = MortonCode::sGetMortonCode(centroids[t], centroid_bounds);

	// Sort triangles based on morton code
	sort(outGroupedTriangleIndices.begin(), outGroupedTriangleIndices.end(), [&morton_codes](uint inLHS, uint inRHS) { return morton_codes[inLHS] < morton_codes[inRHS]; });
}

JPH_NAMESPACE_END
