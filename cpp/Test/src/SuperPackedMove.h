#pragma once
// !!! stupid super pack concept !!! - we can use normal pack here and save a lot of time
struct superPackedMove {
	unsigned int isSuperPackedMove : 1, isGameEndNode : 1, totValue : 2, packedMove : 28;
};
struct smallNode {
	union {
		superPackedMove sPM;
		unsigned int isNoNodeIdx : 1, nodeIdx : 31;
	};
};
