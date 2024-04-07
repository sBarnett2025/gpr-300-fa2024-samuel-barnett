#include "transformHierarchyNode.h"

namespace samuelbarnett
{
	NodeHierarchy CreateSkeleton()
	{
		NodeHierarchy hierarchy;
		hierarchy.nodeCount = 8;
		hierarchy.nodes = new Node[hierarchy.nodeCount];

		// CONSTRUCT THE SKELETOR
		glm::vec3 pos;
		glm::quat rotate;
		glm::vec3 scale;
			
		// TORSO
		Node torso;
		pos = glm::vec3(1.0, 1.0, 1.0);
		rotate = glm::quat(1.0, 1.0, 1.0, 1.0);
		scale = glm::vec3(1.0, 1.0, 1.0);
		torso.SetupTransform(pos, rotate, scale);

		torso.parentIndex = -1;
		hierarchy.nodes[BoneNames::TORSO] = torso;

		// HEAD
		Node head;
		pos = glm::vec3(0.0, 5.0, 0.0);
		rotate = glm::quat(1.0, 1.0, 1.0, 1.0);
		scale = glm::vec3(1.0, 1.0, 1.0);
		head.SetupTransform(pos, rotate, scale);

		head.parentIndex = BoneNames::TORSO;
		hierarchy.nodes[BoneNames::HEAD] = head;

		// RIGHT SHOULDER
		Node rShoulder;
		pos = glm::vec3(2.0, 0.0, 0.0);
		rotate = glm::quat(1.0, 1.0, 1.0, 1.0);
		scale = glm::vec3(1.0, 1.0, 1.0);
		rShoulder.SetupTransform(pos, rotate, scale);

		rShoulder.parentIndex = BoneNames::TORSO;
		hierarchy.nodes[BoneNames::R_SHOULDER] = rShoulder;

		//  RIGHT ELBOW
		Node rElbow;
		pos = glm::vec3(0.0, -1.0, 0.0);
		rotate = glm::quat(1.0, 1.0, 1.0, 1.0);
		scale = glm::vec3(1.0, 1.0, 1.0);
		rShoulder.SetupTransform(pos, rotate, scale);

		rElbow.parentIndex = BoneNames::R_SHOULDER;
		hierarchy.nodes[BoneNames::R_ELBOW] = rElbow;

		// RIGHT WRIST
		Node rWrist;
		pos = glm::vec3(0.0, -1.0, 0.0);
		rotate = glm::quat(1.0, 1.0, 1.0, 1.0);
		scale = glm::vec3(1.0, 1.0, 1.0);
		rWrist.SetupTransform(pos, rotate, scale);

		rWrist.parentIndex = BoneNames::R_ELBOW;
		hierarchy.nodes[BoneNames::R_WRIST] = rWrist;

		// LEFT SHOULDER
		Node lShoulder;
		pos = glm::vec3(0.0, -2.0, 0.0);
		rotate = glm::quat(1.0, 1.0, 1.0, 1.0);
		scale = glm::vec3(1.0, 1.0, 1.0);
		lShoulder.SetupTransform(pos, rotate, scale);

		lShoulder.parentIndex = BoneNames::TORSO;
		hierarchy.nodes[BoneNames::L_SHOULDER] = lShoulder;

		// LEFT ELBOW
		Node lElbow;
		pos = glm::vec3(0.0, -1.0, 0.0);
		rotate = glm::quat(1.0, 1.0, 1.0, 1.0);
		scale = glm::vec3(1.0, 1.0, 1.0);
		lElbow.SetupTransform(pos, rotate, scale);

		lElbow.parentIndex = BoneNames::L_SHOULDER;
		hierarchy.nodes[BoneNames::L_ELBOW] = lElbow;

		// LEFT WRIST
		Node lWrist;
		pos = glm::vec3(0.0, -1.0, 0.0);
		rotate = glm::quat(1.0, 1.0, 1.0, 1.0);
		scale = glm::vec3(1.0, 1.0, 1.0);
		lWrist.SetupTransform(pos, rotate, scale);

		lWrist.parentIndex = BoneNames::L_ELBOW;
		hierarchy.nodes[BoneNames::L_WRIST] = lWrist;


		// return
		return hierarchy;
	}

	void SolveFK(NodeHierarchy& hierarchy)
	{
		for (int i = 0; i < hierarchy.nodeCount; i++)
		{
			Node node = hierarchy.nodes[i];

			if (node.parentIndex == -1)
			{
				node.globalTransform = node.localTransform;
			}
			else
			{
				node.globalTransform = hierarchy.nodes[node.parentIndex].globalTransform * node.localTransform;
			}
		}

	}



}
