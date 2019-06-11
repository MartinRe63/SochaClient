#pragma once
struct superPackedMove {
	unsigned int isSuperPackedMove : 1, packedMove : 31;
};
struct smallNode {
	union {
		superPackedMove sPM;
		unsigned int isNoNodeIdx : 1, nodeIdx : 31;
	};
};
