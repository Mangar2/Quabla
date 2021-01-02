/**
 * @license
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @author Volker Böhm
 * @copyright Copyright (c) 2021 Volker Böhm
 * @Overview
 * Stores a move history for the current game
 */

#ifndef __MOVEHISTORY_H
#define __MOVEHISTORY_H

#include <vector>
#include "Move.h"
#include "GenBoard.h"
#include "Hash/Hash.h"

using namespace std;
using namespace ChessBasics;

namespace ChessSearch {

	class MoveHistory {
	public:
		MoveHistory() {};
		const MoveGenerator& getStartPosition() const { return startPosition; }

		void setStartPosition(const MoveGenerator& aBoard) {
			startPosition = aBoard;
			moveAmount = 0;
		}

		/**
		 * Adds a move to the history
		 */
		void addMove(Move move) {
			_history.push_back(move);
		}

		/**
		 * Undoes a move and returns the generated board
		 */
		MoveGenerator undoMove() {
			MoveGenerator board = startPosition;
			for (uint16_t moveNo = 0; moveNo < moveAmount - 1; moveNo++) {
				board.doMove(moveList[moveNo]);
			}
			moveAmount--;
			return board;
		}


		bool isDrawByRepetition(const MoveGenerator& board) {

			Board checkBoard = startPosition;
			uint16_t moveNo = 0;
			uint8_t samePositionCount = 1;
			while (moveNo + board.boardInfo.getHalfMovesWithoutPawnMovedOrPieceCaptured() < moveAmount) {
				checkBoard.doMove(moveList[moveNo]);
				moveNo++;
			}
			for (; moveNo < moveAmount; moveNo++) {
				if (checkBoard == board) {
					samePositionCount++;
				}
				checkBoard.doMove(moveList[moveNo]);
			}
			return samePositionCount >= 3;

		}

		void setDrawPositionsToHash(const MoveGenerator& board, Hash& hash) {
			Board checkBoard = startPosition;
			uint16_t moveNo = 0;
			value_t drawPositionValue = board.whiteToMove ? -30 : 30;
			while (moveNo + board.boardInfo.getHalfMovesWithoutPawnMovedOrPieceCaptured() < moveAmount) {
				checkBoard.doMove(moveList[moveNo]);
				moveNo++;
			}
			for (; ; moveNo++) {
				value_t positionValue = checkBoard.whiteToMove ? drawPositionValue : -drawPositionValue;
				hash.setHashEntry(checkBoard.getHashKey(), 999, 0, Move::EMPTY_MOVE, positionValue, -MAX_VALUE, MAX_VALUE, 0);
				if (moveNo == moveAmount) {
					break;
				}
				checkBoard.doMove(moveList[moveNo]);
				//checkBoard.printBoard();
			}
		}

	private:
		vector<Move> _history;
		MoveGenerator startPosition;
	};

}

#endif // __MOVEHISTORY_H