#include "transformHierarchyNode.h"

namespace samuelbarnett
{
	NodeHierarchy CreateSkeleton()
	{
		NodeHierarchy hierarchy;
		hierarchy.nodeCount = 8;
		hierarchy.nodes = new Node[hierarchy.nodeCount];

		// CONSTRUCT THE SKELETOR



		// TORSO
		Node torso;
		torso.localTransform.position = glm::vec3(5.0, 5.0, 5.0);
		//torso.localTransform.rotation = glm::quat(1.0, 1.0, 1.0, 1.0);
		torso.localTransform.scale = glm::vec3(2.0, 2.0, 2.0);

		torso.parentIndex = -1;
		hierarchy.nodes[BoneNames::TORSO] = torso;

		// HEAD
		Node head;
		head.localTransform.position = glm::vec3(0.0, 2.0, 0.0);
		//head.localTransform.rotation = glm::quat(1.0, 1.0, 1.0, 1.0);
		head.localTransform.scale = glm::vec3(0.5, 0.5, 0.5);

		head.parentIndex = BoneNames::TORSO;
		hierarchy.nodes[BoneNames::HEAD] = head;

		// RIGHT SHOULDER
		Node rShoulder;
		rShoulder.localTransform.position = glm::vec3(-2.0, 0.0, 0.0);
		rShoulder.localTransform.rotation = glm::quat(1, 0, 1, 0);
		rShoulder.localTransform.scale = glm::vec3(0.75, 0.75, 0.75);

		rShoulder.parentIndex = BoneNames::TORSO;
		hierarchy.nodes[BoneNames::R_SHOULDER] = rShoulder;

		//  RIGHT ELBOW
		Node rElbow;
		rElbow.localTransform.position = glm::vec3(0.0, -1.0, 0.0);
		//rElbow.localTransform.rotation = glm::quat(1.0, 1.0, 1.0, 1.0);
		//rElbow.localTransform.scale = glm::vec3(1.0, 1.0, 1.0);

		rElbow.parentIndex = BoneNames::R_SHOULDER;
		hierarchy.nodes[BoneNames::R_ELBOW] = rElbow;

		// RIGHT WRIST
		Node rWrist;
		rWrist.localTransform.position = glm::vec3(0.0, -1.0, 0.0);
		//rWrist.localTransform.rotation = glm::quat(1.0, 1.0, 1.0, 1.0);
		//rWrist.localTransform.scale = glm::vec3(1.0, 1.0, 1.0);

		rWrist.parentIndex = BoneNames::R_ELBOW;
		hierarchy.nodes[BoneNames::R_WRIST] = rWrist;

		// LEFT SHOULDER
		Node lShoulder;
		lShoulder.localTransform.position = glm::vec3(2.0, 0.0, 0.0);
		//lShoulder.localTransform.rotation = glm::quat(1.0, 1.0, 1.0, 1.0);
		lShoulder.localTransform.scale = glm::vec3(0.75, 0.75, 0.75);

		lShoulder.parentIndex = BoneNames::TORSO;
		hierarchy.nodes[BoneNames::L_SHOULDER] = lShoulder;

		// LEFT ELBOW
		Node lElbow;
		lElbow.localTransform.position = glm::vec3(0.0, -1.0, 0.0);
		//lElbow.localTransform.rotation = glm::quat(1.0, 1.0, 1.0, 1.0);
		//lElbow.localTransform.scale = glm::vec3(1.0, 1.0, 1.0);

		lElbow.parentIndex = BoneNames::L_SHOULDER;
		hierarchy.nodes[BoneNames::L_ELBOW] = lElbow;

		// LEFT WRIST
		Node lWrist;
		lWrist.localTransform.position = glm::vec3(0.0, -1.0, 0.0);
		//lWrist.localTransform.rotation = glm::quat(1.0, 1.0, 1.0, 1.0);
		//lWrist.localTransform.scale = glm::vec3(1.0, 1.0, 1.0);

		lWrist.parentIndex = BoneNames::L_ELBOW;
		hierarchy.nodes[BoneNames::L_WRIST] = lWrist;

		// return
		return hierarchy;
	}

	void NodeHierarchy::SolveFK()
	{
		for (int i = 0; i < nodeCount; i++)
		{
			if (nodes[i].parentIndex == -1)
			{
				nodes[i].globalTransform = nodes[i].localTransform.modelMatrix();
			}
			else
			{
				nodes[i].globalTransform = nodes[nodes[i].parentIndex].globalTransform * nodes[i].localTransform.modelMatrix();
			}
		}

	}



}
