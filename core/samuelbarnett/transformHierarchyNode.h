#pragma once
#include <glm/ext/matrix_float4x4.hpp>

namespace samuelbarnett
{
	enum BoneNames
	{
		TORSO,
		HEAD,
		R_SHOULDER,
		R_ELBOW,
		R_WRIST,
		L_SHOULDER,
		L_ELBOW,
		L_WRIST,
		COUNT
	};

	struct Node
	{
		glm::mat4 localTransform;
		glm::mat4 globalTransform;
		unsigned int parentIndex;
	};


	struct NodeHierarchy
	{
		Node* nodes;
		unsigned int nodeCount;
	};
	NodeHierarchy CreateSkeleton();



	


}