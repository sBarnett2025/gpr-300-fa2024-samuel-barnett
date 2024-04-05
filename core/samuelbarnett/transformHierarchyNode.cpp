#include "transformHierarchyNode.h"

namespace samuelbarnett
{
	NodeHierarchy CreateSkeleton()
	{
		NodeHierarchy hierarchy;
		hierarchy.nodeCount = 8;
		hierarchy.nodes = new Node[hierarchy.nodeCount];

		// CONSTRUCT THE SKELETOR

		// ROOT

		// TORSO
		Node torso;
		torso.parentIndex = 0;
		hierarchy.nodes[BoneNames::TORSO] = torso;

		// HEAD
		Node head;
		head.parentIndex = BoneNames::TORSO;
		hierarchy.nodes[BoneNames::HEAD] = head;

		// RIGHT SIDE
		Node rShoulder;
		rShoulder.parentIndex = BoneNames::TORSO;
		hierarchy.nodes[BoneNames::R_SHOULDER] = rShoulder;

		Node rElbow;
		rElbow.parentIndex = BoneNames::R_SHOULDER;
		hierarchy.nodes[BoneNames::R_ELBOW] = rElbow;

		Node rWrist;
		rWrist.parentIndex = BoneNames::R_ELBOW;
		hierarchy.nodes[BoneNames::R_WRIST] = rWrist;

		// LEFT SIDE
		Node lShoulder;
		lShoulder.parentIndex = BoneNames::TORSO;
		hierarchy.nodes[BoneNames::L_SHOULDER] = lShoulder;

		Node lElbow;
		lElbow.parentIndex = BoneNames::L_SHOULDER;
		hierarchy.nodes[BoneNames::L_ELBOW] = lElbow;

		Node lWrist;
		lWrist.parentIndex = BoneNames::L_ELBOW;
		hierarchy.nodes[BoneNames::L_WRIST] = lWrist;


		// return
		return hierarchy;
	}





}
