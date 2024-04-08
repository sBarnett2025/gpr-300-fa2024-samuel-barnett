#pragma once
#include <glm/ext/matrix_float4x4.hpp>
#include "../ew/transform.h"

namespace samuelbarnett
{
	enum BoneNames
	{
		TORSO = 0,
		HEAD = 1,
		R_SHOULDER = 2,
		R_ELBOW = 3,
		R_WRIST = 4,
		L_SHOULDER = 5,
		L_ELBOW = 6,
		L_WRIST = 7
	};

	struct Node
	{
		ew::Transform localTransform;
		glm::mat4 globalTransform;
		unsigned int parentIndex;

		void SetupTransform(glm::vec3 pos, glm::quat rotate, glm::vec3 scale)
		{
			//localTransform = glm::mat4(1.0f);
			//localTransform = glm::translate(localTransform, pos); // translate
			//localTransform *= glm::mat4_cast(rotate); // rotate
			//localTransform = glm::scale(localTransform, scale); // scale
		};
	};


	struct NodeHierarchy
	{
		Node* nodes;
		unsigned int nodeCount;

		void SolveFK();
	};

	NodeHierarchy CreateSkeleton();

	//void SolveFK(NodeHierarchy& hierarchy);


}