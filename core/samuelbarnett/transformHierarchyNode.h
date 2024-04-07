#pragma once
#include <glm/ext/matrix_float4x4.hpp>
#include "../ew/transform.h"

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

		void SetupTransform(glm::vec3 pos, glm::quat rotate, glm::vec3 scale)
		{
			localTransform = glm::mat4(1.0f);
			localTransform = glm::translate(localTransform, pos);
			localTransform *= glm::mat4_cast(rotate);
			localTransform = glm::scale(localTransform, scale);
		};
	};


	struct NodeHierarchy
	{
		Node* nodes;
		unsigned int nodeCount;
	};

	NodeHierarchy CreateSkeleton();

	void SolveFK(NodeHierarchy& hierarchy);


}