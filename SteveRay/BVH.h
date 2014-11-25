#pragma once
#include "AABBox.h"
#include "SurfaceElement.h"
#include <stack>
#include "Utils.h"

/// <summary>
/// 
/// </summary>
class BVH
{
public:
	BVH();
	~BVH();

	void create(std::vector<AABBox>& boxes, const float sceneSize, const Point3& sceneCenter);
	bool compareBoxMorton(AABBox a, AABBox b);

	bool intersectRay(const Ray& ray, float& distance, SurfaceElement& surfel) const;

private:

	/// <summary>
	/// Expands a 10-bit integer into 30 bits
	/// by inserting 2 zeros after each bit.
	/// Taken from http://devblogs.nvidia.com/parallelforall/thinking-parallel-part-iii-tree-construction-gpu/
	/// Written by Tero Karras
	/// </summary>
	/// <param name="v">The expanded int.</param>
	/// <returns></returns>
	unsigned int expandBits(unsigned int v)
	{
		v = (v * 0x00010001u) & 0xFF0000FFu;
		v = (v * 0x00000101u) & 0x0F00F00Fu;
		v = (v * 0x00000011u) & 0xC30C30C3u;
		v = (v * 0x00000005u) & 0x49249249u;
		return v;
	}

	//TODO: Update doc.  Use inverse scene size
	/// <summary>
	/// Calculates a 30-bit Morton code for the
	/// given 3D point located within the unit cube [0,1]. 
	/// Taken from http://devblogs.nvidia.com/parallelforall/thinking-parallel-part-iii-tree-construction-gpu/
	/// Written by Tero Karras
	/// </summary>
	/// <param name="vec">The vector.</param>
	/// <returns>The morton code</returns>
	unsigned int morton3D(const Vector3& vec, const float sceneSize, const Point3& sceneCenter)
	{
		float x = (vec.x() - sceneCenter.x()) / sceneSize + 0.5;
		float y = (vec.y() - sceneCenter.y()) / sceneSize + 0.5;
		float z = (vec.z() - sceneCenter.z()) / sceneSize + 0.5;

		x = std::min(std::max(x * 1024.0f, 0.0f), 1023.0f);
		y = std::min(std::max(y * 1024.0f, 0.0f), 1023.0f);
		z = std::min(std::max(z * 1024.0f, 0.0f), 1023.0f);
		unsigned int xx = expandBits(static_cast<uint32_t>(x));
		unsigned int yy = expandBits(static_cast<uint32_t>(y));
		unsigned int zz = expandBits(static_cast<uint32_t>(z));
		return xx * 4 + yy * 2 + zz;
	}


	/// <summary>
	/// Returns the count of the leading zeros in the binary representation of an int
	/// </summary>
	/// <param name="value">The sount of leading zeros.</param>
	/// <returns></returns>
	uint32_t countLeadingZeros(uint32_t value)
	{
		unsigned long leading_zero = 0;

		if (_BitScanReverse(&leading_zero, value))
		{
			return 31 - leading_zero;
		}
		// return 32 since results are undefined
		return 32;
	}

	class Node
	{
	public:
		Node(Node* leftChild, Node* rightChild) :
			leftChild(leftChild), rightChild(rightChild), isLeaf(false), box(nullptr)
		{
		};

		Node(AABBox *box) :
			box(box), isLeaf(true)
		{
		};

		Node() : 
			isLeaf(false), box(nullptr)
		{
			
		}

		~Node()
		{
			delete box;
		};

		bool isLeaf;
		AABBox *box;
		Node* leftChild;
		Node* rightChild;

//		Node* parent;
//		int m;
//		int rs;
//		int re;
//		int s;

	
		/// <summary>
		/// Builds the bounding box.
		/// </summary>
		/// <returns>True if box could be built, false if not enough information exists to build the bounding box</returns>
		bool buildBoundingBox()
		{
			//box is already built for leaves
			if (isLeaf)
			{
				return true;
			}
			//can't build a box if the children do not have boxes
			if (leftChild->box == nullptr || rightChild->box == nullptr)
			{
				return false;
			}

			//get all 4 corners
			const Point3& uL = (leftChild->box->upperCorner());
			const Point3& lL = (leftChild->box->lowerCorner());
			const Point3& uR = (rightChild->box->upperCorner());
			const Point3& lR = (rightChild->box->lowerCorner());

			//build the new corners
			Point3 *upper = new Point3(fmax(uL.x(), uR.x()),
				fmax(uL.y(), uR.y()),
				fmax(uL.z(), uR.z()));

			Point3 *lower = new Point3(fmin(lL.x(), lR.x()),
				fmin(lL.y(), lR.y()),
				fmin(lL.z(), lR.z()));

			box = new AABBox(*upper, *lower, nullptr);

			return true;
		}
	};


	/// <summary>
	/// Encapsulates data needed to build the BVH on a user-defined stack
	/// </summary>
	struct constructData
	{
		constructData(Node* node, int first, int last) :
			node(node), first(first), last(last)
		{

		}

		constructData()
		{
			
		}

		Node *node;
		int first;
		int last;
	};

	void generateHierarchyFullParallel(std::vector<AABBox>& boxes);
	void generateHierarchyStackLinear(std::vector<AABBox>& boxes);
	void generateHierarchyRecursive(std::vector<AABBox>& boxes, Node* node, int first, int last, int i, ProgressBar& progressBar);

	void buildBoundingBox();


	int findSplit(std::vector<AABBox> boxes, int first, int last);

	int commonPrefix(std::vector<AABBox>& boxes, int firstIndex, int secondIndex);



	Node* m_leafNodes;
	Node* m_branchNodes;

	static int sign(int x)
	{
		if (x > 0) return 1;
		if (x < 0) return -1;
		return 0;
	}
};