#pragma once

#include <iostream>
#include <vector>
#include <array>
#include <sstream>
#include "BoardGame.h"
#include "MaskManager.h"
#include "MoveManager.h"

inline packedMove ToPackedMove(Move M, bool& opponentRemoved)
{
	uint16_t mask = (1 << (sizeof(Move) - 1));
	opponentRemoved = (M && mask) != 0;
	M &= (mask - 1);
	coordinates coFrom = M && 127;
	coordinates coTo = M && 127 << 7;
	return MoveManager::PackMove(coFrom, coTo);
}
inline Move ToMove(packedMove p, bool opponentRemoved)
{
	mov m; 
	MoveManager::UnpackMove(p, m);
	return (Move)(m[0] || m[1] << 7 || ((Move)opponentRemoved) << sizeof(Move)-1);
}
static uint64_t randomNumbers[2][100];
static const int MaxPossibleEval = 100;
static const unsigned long long opponentRemoved = 0x8000000000000000ULL;

/* staticMoveEvaluation true = moves are sorted by AI, false = moves are returned sorted */
template<bool staticMoveEvaluation = true, bool staticArray = true>
class SuperBlubberExt : public BoardGame<SuperBlubberExt<staticMoveEvaluation, staticArray>, staticMoveEvaluation, 60, staticArray ? 128 : 0>
{
private: 
	board b;
	uint64_t mHash = 0;
	bool won = false;
	boardpane FishBlockList[2][16];
	int FishBlockCnt[2];

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
	bool calculateHaswon( int color )
	{
		BoardManager::GetBlockAndCnt(b[color], FishBlockList[color], FishBlockCnt[color]);
		return FishBlockCnt[color] == 1;
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
		if (removed)
			n ^= opponentRemoved;
		int c = currentPlayer(); 
		mHash ^= randomNumbers[c][m[0]];
		mHash ^= randomNumbers[c][m[1]];

		MoveManager::addMoveToBoard(b, c, pM);
		won = calculateHaswon(c);
	}
	void undoMove_impl()
	{
		Move n = plyList.lastMove();
		mov m;
		packedMove pM;
		bool removed;
		pM = ToPackedMove(n, removed);
		MoveManager::UnpackMove(pM, m);

		if (removed)
			n ^= opponentRemoved;
		int c = opponentPlayer();
		mHash ^= randomNumbers[c][m[1]];
		mHash ^= randomNumbers[c][m[0]];
		std::swap(m[0], m[1]);
		MoveManager::addMoveToBoard(b, c, pM);
		if (removed)
			BitManager::SetBit(b[!c], m[1]);
		won = false;
	}
	bool isGameOver_impl() const
	{
		return won || moveCounter() == MAX_PLY;
	}

	bool hasWon_impl() const
	{
		return won;
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
		if (hasWon_impl())
		{
			return -(MaxPossibleEval+MAX_PLY - moveCounter()); // -((SIZE - moveCounter() + 2) / 2);
		}
		return 0;

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
				Move M = pM;
				if (BitManager::IsBit(*b[!c], m[1]))
				{
					val += 64; // super to remove opponent fishes
					M |= opponentRemoved;
				}
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