#ifndef __tree
#define __tree

#include "common.h"
#include "d3d10_1.h"
#include "d3dx10.h"

namespace engine{
	class Tree{
	public:
		Tree();
		void grow();
		~Tree();
	protected:
		Tree *root;
		Tree *children;
		_ children_count;
		/*
			Two vectors describe cylinder.
		*/
		_vector3 start;
		_vector3 end;
	};
}

#endif