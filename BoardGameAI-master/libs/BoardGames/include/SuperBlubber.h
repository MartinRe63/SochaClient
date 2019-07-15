#pragma once

#include <iostream>
#include <vector>
#include <array>
#include <sstream>
#include "BoardGame.h"
#include "MaskManager.h"
#include "MoveManager.h"

inline void ToMov(Move M, mov& m, bool& opponentRemoved)
{
	uint16_t mask = (1 << (sizeof(Move)*8 - 1));
	opponentRemoved = (M & mask) != 0;
	M &= (mask - 1);
	m[0] = M % 128;
	m[1] = M / 128;
}
inline packedMove ToPackedMove(Move M, bool& opponentRemoved)
{
	mov m;
	ToMov(M, m, opponentRemoved);
	return MoveManager::PackMove(m);
}
inline Move ToMove(packedMove p, bool opponentRemoved)
{
	mov m; 
	MoveManager::UnpackMove(p, m);
	return (Move)(m[0] | m[1] << 7 | (((Move)opponentRemoved) << sizeof(Move)*8-1));
}
static uint64_t randomNumbers[2][100];
static const int MaxPossibleEval = 100;

/* staticMoveEvaluation true = moves are sorted by AI, false = moves are returned sorted */
template<bool staticMoveEvaluation = true, bool staticArray = true>
class SuperBlubberExt : public BoardGame<SuperBlubberExt<staticMoveEvaluation, staticArray>, staticMoveEvaluation, 60, staticArray ? 128 : 0>
{
private: 
	board b;
	uint64_t mHash = 0;
	bool won = false;
	bool lost = false;
	boardpane FishSwarmList[2][16];
	int FishSwarmCnt[2];
	int FishCount[2];

public:
	SuperBlubberExt()
	{
		BoardManager::FromString(std::string(
			".11111111." \
			"0........0" \
			"0........0" \
			"0.....C..0" \
			"0........0" \
			"0........0" \
			"0..C.....0" \
			"0........0" \
			"0........0" \
			".11111111."), b);
			for( int i = 0; i < 2; i++)
				for (int j = 0; j < 100; j++)
					for (int shift = 0; shift < 64; shift += 16)
					{
						randomNumbers[i][j] ^= rand() << shift;
					}
	}
	~SuperBlubberExt() {};

	uint64_t hash_impl() const
	{
		return mHash;
	}
	const board* getBoard () const
	{
		return &b;
	}
	bool calculateHaswon( int color, int moveCnt )
	{
		FishSwarmCnt[color] = BoardManager::GetBlockAndCnt(b[color], FishSwarmList[color], FishCount[color]);
		if (moveCnt % 2 == 1)
		{
			return FishSwarmCnt[color] == 1;
		}
		else
			return false;
	}
	bool isPlayable(int col) const
	{
		return true;
	}

	void makeMove_impl(Move n)
	{
		mov m;
		packedMove pM;
		bool removed;
		pM = ToPackedMove(n, removed);
		MoveManager::UnpackMove(pM, m);
		int c = currentPlayer(); 
		mHash ^= randomNumbers[c][m[0]];
		mHash ^= randomNumbers[c][m[1]];

		MoveManager::addMoveToBoard(b, c, pM);
		int cnt = moveCounter();
		won = calculateHaswon( c, cnt );
		lost = calculateHaswon( !c, cnt );
	}
	void undoMove_impl()
	{
		Move n = plyList.lastMove();
		mov m;
		bool removed;
		ToMov(n, m, removed);
		int c = opponentPlayer();
		mHash ^= randomNumbers[c][m[1]];
		mHash ^= randomNumbers[c][m[0]];
		std::swap(m[0], m[1]);
		MoveManager::addMoveToBoard(b, c, m);
		if (removed)
			BitManager::SetBit(b[!c], m[1]);
		won = false;
	}
	bool isGameOver_impl() const
	{
		return won || lost || moveCounter() == MAX_PLY;
	}

	bool hasWon_impl() const
	{
		return won || lost;
	}
	int mapLastMoveToCounterMoveState_impl() const
	{
		return plyList.lastMove(); // height[plyList.lastMove()] - 1;
	}

	constexpr static int counterMoveStates_impl()
	{
		return MaxPossibleEval + MAX_PLY; // SIZE1;
	}

	int16_t evaluate_impl() const
	{
		//
		// in case the analysed move is a win it's very negative for the analysed position
		//
		if (hasWon_impl())
		{
			return (won ? -1 : 1 ) * ( MaxPossibleEval + MAX_PLY - moveCounter()); // -((SIZE - moveCounter() + 2) / 2);
		}
		int color = currentPlayer();
		int swarmValue = FishSwarmCnt[color] - FishSwarmCnt[!color]; //  (16 - FishSwarmCnt[!color]) - (16 - FishSwarmCnt[color]);
		int fishCountValue = FishCount[!color] - FishCount[color];
		return swarmValue + fishCountValue;

	}
	int16_t maxPossibleEvaluation_impl() const
	{
		return MaxPossibleEval;
	}

	template<bool> class MoveGeneratorBase {};

	// return moves with a value (AI sorts them by value)
	template<>
	class MoveGeneratorBase<true>
	{
	private:
		const SuperBlubberExt* game;
		packedMove moveList[MAX_MOVES];
		int distance_not_used;
		int moveCnt;
		int currentMove;
		
	public:
		MoveGeneratorBase() {
			moveCnt = 0;
		}
		constexpr MoveGeneratorBase(const SuperBlubberExt* game) : game(game), currentMove(0) {
			moveCnt = MoveManager::getMoveList(*game->getBoard(), game->currentPlayer(), moveList, distance_not_used);
		}
		ExtMove nextMove()
		{
			if ( currentMove < moveCnt )
			{
				const board* b = game->getBoard();
				int c = game->currentPlayer();
				packedMove pM = moveList[currentMove];
				mov m;
				MoveManager::UnpackMove(pM, m);
				Value val = MoveManager::moveValuePM(pM) + 16; // center moves might be better
				val += MoveManager::moveNeigborValue(*b[c], m) + 32; // much better to have a neighbor
				bool removeOpponent = BitManager::IsBit(*b[!c], m[1]);
				if (removeOpponent)
				{
					val += 64; // super to remove opponent fishes
				}
				Move M = ToMove(pM, removeOpponent);
				currentMove++;
				return ExtMove(M, val);
			}
			return noMoveValue();
		}

	};

	// or return ordered moves directly
	template<>
	class MoveGeneratorBase<false>
	{
	private:
		const SuperBlubberExt* game;
		int index;
		static constexpr std::array<Move, 7> orderedMoves = { { 4,3,5,2,6,1,7 } };
	public:
		MoveGeneratorBase() {}
		constexpr MoveGeneratorBase(const SuperBlubberExt* game) : game(game), index(0) {}
		Move nextMove()
		{
			int currentIndex;
			while ((currentIndex = index++) < orderedMoves.size())
			{
				if (game->isPlayable(orderedMoves[currentIndex]))
				{
					return orderedMoves[currentIndex];
				}
			}
			throw std::exception("wrong implementation.");
			return noMoveValue();
		}

	};

	using MoveGenerator = MoveGeneratorBase<STATIC_EVALUATION>;

	MoveGenerator moveGenerator_impl() const
	{
		return MoveGenerator(this);
	}


	std::string toString()
	{
		std::stringstream strm;
		//strm << "hash: " << game.getHash() << endl;
		strm << "current moves: ";
		for (auto move : *plyList.data())
		{
			strm << move << ",";
		}
		strm << std::endl;
		strm << BoardManager::ToString(b);
		if (hasWon_impl())
			strm << "won" << endl;
		return strm.str();
	}

};

using SuperBlubber = SuperBlubberExt<>;