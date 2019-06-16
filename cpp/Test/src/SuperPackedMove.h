#pragma once
// !!! stupid super pack concept !!! - we can use normal pack here and save a lot of time
struct superPackedMove {
	unsigned int isSuperPackedMove : 1, isGameEndNode : 1, totValue : 2, packedMove : 28;
};
struct superPackedNode {
	unsigned int isNoIdx : 1, idx : 31;
};
struct smallNode {
	union {
		superPackedMove sPM;
		superPackedNode node;
	};
};
