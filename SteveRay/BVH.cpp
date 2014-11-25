#include "stdafx.h"
#include "BVH.h"
#include "Utils.h"


BVH::BVH()
{
}


BVH::~BVH()
{
	delete[] m_leafNodes;
	delete[] m_branchNodes;
}

struct myclass
{
	bool operator()(AABBox a, AABBox b)
	{
		return (a.mortonCentroid() < b.mortonCentroid());
	}
} myobject;

void BVH::create(std::vector<AABBox>& boxes, const float sceneSize, const Point3& sceneCenter)
{
	m_leafNodes = new Node[boxes.size()];
	m_branchNodes = new Node[boxes.size() - 1];


	//TODO: give bounds for morton so all trans are not unit cube
	//first, assign morten codes
#if !_DEBUG
#pragma omp parallel for
#endif
	for (int i = 0; i < boxes.size(); i++)
	{
		boxes[i].setMortonCentroid(morton3D(boxes[i].centroid(), sceneSize, sceneCenter));
		m_leafNodes[i].isLeaf = true;
		m_leafNodes[i].box = new AABBox(boxes[i]);
	}

	//TODO: parallel sort?
	//then sort
	std::sort(boxes.begin(), boxes.end(), myobject);


	int i = 0;
	ProgressBar progressBar;

	//create heirarchy
//	generateHierarchyFullParallel(boxes);
			generateHierarchyStackLinear(boxes);
	//    	generateHierarchyRecursive(boxes, &m_branchNodes[0], 0, boxes.size() - 1, i, progressBar);

	//	progressBar.end();

	buildBoundingBox();
}


bool BVH::compareBoxMorton(AABBox a, AABBox b)
{
	return a.mortonCentroid() < b.mortonCentroid();
}


/// <summary>
/// Check for ray intersection iteratively, using a stack of Nodes
/// </summary>
/// <param name="ray">The ray.</param>
/// <param name="distance">The distance.</param>
/// <param name="surfel">The surfel.</param>
/// <returns></returns>
bool BVH::intersectRay(Ray const& ray, float& distance, SurfaceElement& surfel) const
{
	bool result = false;
	std::stack<Node*> nodeStack;
	nodeStack.push(&m_branchNodes[0]);

	do
	{
		Node* node = nodeStack.top();
		nodeStack.pop();

		if (node->box->intersectRay(ray, distance))
		{
			//if a leaf, check for intersection with the primitive the node holds
			if (node->isLeaf)
			{
				//make sure intersection is calculated before OR
				result = node->box->primitive()->intersectRay(ray, distance, surfel) || result;
			}
			//otherwise, check the children
			else
			{
				nodeStack.push(node->leftChild);
				nodeStack.push(node->rightChild);
			}
		}
	}
	while (!nodeStack.empty());

	return result;
}

void BVH::generateHierarchyRecursive(std::vector<AABBox>& boxes, Node* node, int first, int last, int i, ProgressBar& progressBar)
{
	// Determine where to split the range.
	int split = findSplit(boxes, first, last);

	Node* rightChild;
	if (split == first)
	{
		rightChild = &m_leafNodes[split];
	}
	else
	{
		rightChild = &m_branchNodes[split];

		//push next node
		generateHierarchyRecursive(boxes, rightChild, first, split, i, progressBar);
	}

	Node* leftChild;
	if (split + 1 == last)
	{
		leftChild = &m_leafNodes[split + 1];
	}
	else
	{
		leftChild = &m_branchNodes[split + 1];

		//push next node
		generateHierarchyRecursive(boxes, leftChild, split + 1, last, i, progressBar);
	}

	//	m_branchNodes[i].m = max;
//	node->re = last;
//	node->rs = first;
//	node->s = split;

	node->rightChild = rightChild;
	node->leftChild = leftChild;
//	leftChild->parent = node;
//	rightChild->parent = node;

	i++;
	progressBar.updateProgress(static_cast<int>((static_cast<float>(i) / boxes.size()) * 100));
}

void BVH::generateHierarchyStackLinear(std::vector<AABBox>& boxes)
{
	std::stack<constructData *> dataStack;
	dataStack.push(new constructData(&m_branchNodes[0], 0, static_cast<int>(boxes.size() - 1)));

	//	constructData *dataStackArray = new constructData[1024];
	//	dataStackArray[0] = constructData(&m_branchNodes[0], 0, static_cast<int>(boxes.size() - 1));
	//	uint32_t stackSize = 1;

	int i = 0;
	ProgressBar progressBar;

	do
	{
		constructData* data = dataStack.top();
		dataStack.pop();

		//		--stackSize;
		//		constructData data = dataStackArray[stackSize];

		// Determine where to split the range.
		int split = findSplit(boxes, data->first, data->last);

		Node* rightChild;
		if (split == data->first)
		{
			rightChild = &m_leafNodes[split];
		}
		else
		{
			rightChild = &m_branchNodes[split];

			//push next node
			dataStack.push(new constructData(rightChild, data->first, split));
			//			dataStackArray[stackSize] = constructData(rightChild, data.first, split);
			//			++stackSize;
		}

		Node* leftChild;
		if (split + 1 == data->last)
		{
			leftChild = &m_leafNodes[split + 1];
		}
		else
		{
			leftChild = &m_branchNodes[split + 1];

			//push next node
			dataStack.push(new constructData(leftChild, split + 1, data->last));
			//			dataStackArray[stackSize] = constructData(leftChild, split + 1, data.last);
			//			++stackSize;
		}

		//		m_branchNodes[i].m = max;
		//		data.node->re = data.last;
		//		data.node->rs = data.first;
		//		data.node->s = split;
		data->node->rightChild = rightChild;
		data->node->leftChild = leftChild;
		//		leftChild->parent = data.node;
		//		rightChild->parent = data.node;

		delete data;

		i++;
		progressBar.updateProgress(static_cast<int>((static_cast<float>(i) / boxes.size()) * 100));
	}
	while (!dataStack.empty());

	progressBar.end();
}

/// <summary>
/// Generates the hierarchy.  http://devblogs.nvidia.com/parallelforall/wp-content/uploads/sites/3/2012/11/karras2012hpg_paper.pdf
/// </summary>
/// <param name="boxes">The boxes.</param>
void BVH::generateHierarchyFullParallel(std::vector<AABBox>& boxes)
{
	ProgressBar progressBar;

#if !_DEBUG
#pragma omp parallel for schedule(dynamic,1)
#endif
	for (int i = 0; i < (boxes.size() - 1); i++)
	{
		//first, find the range this node corresponds to
		//d == -1 means range ends at i, d == +1 means range starts at i
		int d = sign(commonPrefix(boxes, i, i + 1) - commonPrefix(boxes, i, i - 1));

		//find a maximum and minimum value for the range this node covers.  Must be a power of two for easy binary searching
		int min = commonPrefix(boxes, i, i - d);
		int max = 128;

		while (commonPrefix(boxes, i, i + (max * d)) > min)
		{
			max *= 4;
		}

		//use binary serach to find the farthest common prefis that is greater than min
		int l = 0;
		for (int t = max >> 1; t >= 1; t = t >> 1)
		{
			if (commonPrefix(boxes, i, i + (l + t) * d) > min)
			{
				//serach to the right of this value
				l += t;
			}
		}

		//end of range
		int j = i + l * d;
		int first = i;

		if (d < 0)
		{
			first = j;
			j = i;
		}


		// Determine where to split the range.
		int split = findSplit(boxes, first, j);

		Node* rightChild;
		if (split == first)
		{
			rightChild = &m_leafNodes[split];
		}
		else
		{
			rightChild = &m_branchNodes[split];
		}

		Node* leftChild;
		if (split + 1 == j)
		{
			leftChild = &m_leafNodes[split + 1];
		}
		else
		{
			leftChild = &m_branchNodes[split + 1];
		}

//		m_branchNodes[i].m = max;
//		m_branchNodes[i].re = first;
//		m_branchNodes[i].rs = j;
//		m_branchNodes[i].s = split;
		m_branchNodes[i].rightChild = rightChild;
		m_branchNodes[i].leftChild = leftChild;
//		leftChild->parent = &m_branchNodes[i];
//		rightChild->parent = &m_branchNodes[i];

#pragma omp critical
		{
			progressBar.updateProgress(static_cast<int>((static_cast<float>(i) / boxes.size()) * 100));
		}
	}

	progressBar.end();
}

/// <summary>
/// Finds a common prefix between 2 centroids.  Note that the first index should always be guarenteed to be valid for the boxes vector
/// </summary>
/// <param name="boxes">The boxes.</param>
/// <param name="firstIndex">The first index.</param>
/// <param name="secondIndex">Index of the second.</param>
/// <returns>The size of the common prefix</returns>
int BVH::commonPrefix(std::vector<AABBox>& boxes, int firstIndex, int secondIndex)
{
	if (secondIndex < 0 || secondIndex >= static_cast<int>(boxes.size()))
	{
		return - 1;
	}

	int result = countLeadingZeros(boxes[firstIndex].mortonCentroid() ^ boxes[secondIndex].mortonCentroid());

	//duplicate codes
	if (result == 32)
	{
		return result + countLeadingZeros(firstIndex ^ secondIndex);
	}

	return result;
}

//TODO: Use parallel method
/// <summary>
/// Builds the bounding box for each node iteratively, using a stack
/// </summary>
void BVH::buildBoundingBox()
{
	bool result = false;
	std::stack<Node*> nodeStack;
	nodeStack.push(&m_branchNodes[0]);

	do
	{
		Node* node = nodeStack.top();
		nodeStack.pop();

		//if the node cannot build a bounding box, then it's children's boxes must be built
		if (!node->buildBoundingBox())
		{
			//push this node back on the stack for after it's children are calculated
			nodeStack.push(node);

			//push left child
			if (!(node->leftChild->isLeaf))
			{
				nodeStack.push(node->leftChild);
			}
			//push right child
			if (!(node->rightChild->isLeaf))
			{
				nodeStack.push(node->rightChild);
			}
		}
	}
	while (!nodeStack.empty());
}

/// <summary>
/// Finds the first morton code with a differing between two other codes in an array.
/// </summary>
/// <param name="boxes">The boxes.</param>
/// <param name="first">The first morton code.</param>
/// <param name="last">The last morton code.</param>
/// <returns>The location of the split</returns>
int BVH::findSplit(std::vector<AABBox> boxes, int first, int last)
{
	uint32_t firstMorton = boxes[first].mortonCentroid();
	uint32_t lastMorton = boxes[last].mortonCentroid();

	//for identical codes, split down the middle
	if (firstMorton == lastMorton)
	{
		return (first + last) >> 1;
	}

	//get number of leading zeros shared between first and last
	uint32_t sharedLeadingZeros = countLeadingZeros(firstMorton ^ lastMorton);

	//use binary serach to find the first differing bit
	int result = first;
	int range = last - first;
	int newSplit;

	while (range > 1)
	{
		range = (range + 1) >> 1;
		newSplit = result + range;

		if (newSplit < last)
		{
			uint32_t splitMorton = boxes[newSplit].mortonCentroid();
			uint32_t leadingZeros = countLeadingZeros(firstMorton ^ splitMorton);

			if (leadingZeros > sharedLeadingZeros)
			{
				result += range;
			}
		}
	}

	return result;
}