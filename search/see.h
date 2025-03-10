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
 * @author Volker B�hm
 * @copyright Copyright (c) 2021 Volker B�hm
 * @Overview
 * Implements a move provider for search providing moves in the right order
 * Computes a "perfect SEE Value" for one position.
 * It tries all pieces from the smallest to the biggest piece and checks if any piece of this type
 * is able to take the piece of the given position. If so, it will subtract the value of the piece to the gain variable.
 *
 * gain
 * variable containing the material already gained
 *
 * alpha
 * variable containing the material value already won by white. It will be set to gain once white is at move
 * as white may stand pat, it will not capture further and loose material.
 *
 * beta
 * same as alpha, but for black
 *
 * allPiecesLeft
 * bitBoard with all pieces on the position. Every piece capturing is remove to ensure that it will not be tried again
 * and that it will not block any other piece behind (like a queen behind a rook).
 *
 * Hidden pieces
 * Sometimes pieces are hidden, lide a second rook behind the first rook. Rooks and bishops hidden by rooks and bishops
 * are solved by removing the rooks (or bishops) of own color from the all pieces mask when calculating the mask of attacking
 * fields.
 * Hiding behind the queen is solved by retrying bishops and rooks, once the queen(s) are tried.
 */

#ifndef __SEE_H
#define __SEE_H

#include "searchdef.h"
#include "../movegenerator/bitboardmasks.h"
#include "../movegenerator/movegenerator.h"

using namespace QaplaMoveGenerator;

namespace QaplaSearch {

	class SEE {
	public:
		SEE() {
			valueOfNextPieceOnTargetField = 0;
			nodeCountStatistic = 0;
		}

		/**
		 * Clears the  sse
		 */
		void clear() {
			nextPiece[WHITE] = WHITE_PAWN;
			nextPiece[BLACK] = BLACK_PAWN;

			pieceToTryBitBoard[BLACK] = 0;
			pieceToTryBitBoard[WHITE] = 0;

			alpha = -MAX_VALUE;
			beta = MAX_VALUE;
		}

			
		/**
		 * @returns true, if moving piece is move valuable than the captured piece and the captured piece is defended by a pawn
		 */
		static bool isLoosingCaptureLight(const MoveGenerator & position, Move move) {
			bool result = false;
			value_t movingPieceValue = position.getPieceValueForMoveSorting(move.getMovingPiece());
			value_t capturedPieceValue = position.getPieceValueForMoveSorting(move.getCapture());
			if (position.isWhiteToMove()) {
				result = (movingPieceValue > -capturedPieceValue) && 
					isDefendedByPawn<BLACK>(position, move.getDestination());
			}
			else {
				result = (-movingPieceValue > capturedPieceValue) && 
					isDefendedByPawn<WHITE>(position, move.getDestination());
			}
			return result;
		}

		/**
		 * Returns true, if a capture looses material
		 */
		bool isLoosingCapture(const MoveGenerator& position, Move move) {
			bool result = false;
			if (!move.isCapture()) {
				return false;
			}
			Square square = move.getDestination();
			Piece movingPiece = move.getMovingPiece();
			gain = -position.getPieceValueForMoveSorting(move.getCapture());
			value_t movingPieceValue = position.getPieceValueForMoveSorting(movingPiece);
			value_t resultValue;
			bool movingPieceMoreValuable = position.isWhiteToMove() ? movingPieceValue > gain: movingPieceValue < gain;

			if (movingPieceMoreValuable) {
				allPiecesLeft = position.getAllPiecesBB();
				allPiecesLeft &= ~(1ULL << move.getDeparture());
				whiteToMove = !position.isWhiteToMove();
				clear();
				alpha = -1;
				beta = 1;
				if (position.isWhiteToMove()) {
					if (isDefendedByPawn<BLACK>(position, move.getDestination())) {
						result = true;
					}
					else {
						nextPiece[BLACK] = BLACK_KNIGHT;
						resultValue = computeSEEValue(position, square, position.getPieceValueForMoveSorting(movingPiece));
						result = resultValue < 0;
					}
				}
				else {
					if (isDefendedByPawn<WHITE>(position, move.getDestination())) {
						result = true;
					}
					else {
						nextPiece[WHITE] = WHITE_KNIGHT;
						resultValue = computeSEEValue(position, square, position.getPieceValueForMoveSorting(movingPiece));
						result = resultValue > 0;
					}
				}
			}
			return result;
		}

		/**
		 * Computes a static exchange value of a move
		 */
		value_t computeSEEValueOfMove(const MoveGenerator& position, Move move) {
			Square square = move.getDestination();
			allPiecesLeft = position.getAllPiecesBB();
			allPiecesLeft &= ~(1ULL << move.getDeparture());
			value_t result = -position.getPieceValueForMoveSorting(position[square]);
			whiteToMove = !position.isWhiteToMove();
			clear();
			gain = 0;
			result += computeSEEValue(position, square, position.getPieceValueForMoveSorting(move.getMovingPiece()));
			return result;
		}

		/**
		 * Computes a static exchange value for a position poisition
		 */
		value_t computeSEEValueOfPosition(const MoveGenerator& position, Square square) {
			allPiecesLeft = position.getAllPiecesBB();
			whiteToMove = position.isWhiteToMove();
			clear();
			gain = 0;
			return computeSEEValue(position, square, position.getPieceValueForMoveSorting(position[square]));
		}

		/**
		 * Computes a statistic
		 */
		uint64_t getNodeCountStatistic() { return nodeCountStatistic; }

	private:

		/**
		 * Returns true, if a piece is defended by a pawn
		 */
		template <Piece COLOR>
		inline static bool isDefendedByPawn(const MoveGenerator& position, Square square) {
			return
				(BitBoardMasks::pawnCaptures[switchColor(COLOR)][square] &
					position.getPieceBB(PAWN + COLOR)) != 0;
		}

		/**
		 * Computes an static exchange value for a piece of the position
		 */
		value_t computeSEEValue(const MoveGenerator& position, Square square, value_t valueOfCurrentPieceOnSquare) {

			while (valueOfCurrentPieceOnSquare != 0) {
				if (whiteToMove) {
					if (gain > alpha) {
						alpha = gain;
					}
					if (gain - valueOfCurrentPieceOnSquare <= alpha) {
						gain = alpha;
						break;
					}
					valueOfNextPieceOnTargetField = tryPiece<WHITE>(position, square);
					if (valueOfNextPieceOnTargetField != 0) {
						gain -= valueOfCurrentPieceOnSquare;
					}
					else {
						gain = alpha;
					}
				}
				else {
					if (gain < beta) {
						beta = gain;
					}
					if (gain - valueOfCurrentPieceOnSquare >= beta) {
						gain = beta;
						break;
					}
					valueOfNextPieceOnTargetField = tryPiece<BLACK>(position, square);
					if (valueOfNextPieceOnTargetField != 0) {
						gain -= valueOfCurrentPieceOnSquare;
					}
					else {
						gain = beta;
					}
				}
				whiteToMove = !whiteToMove;
				valueOfCurrentPieceOnSquare = valueOfNextPieceOnTargetField;
			}
			return gain;
		}

		/**
		 * Computes the attacking of different pieces
		 */
		template <Piece COLOR>
		bitBoard_t computePawnsAttacking(Square square, bitBoard_t pawns) {
			bitBoard_t attackingPawns = BitBoardMasks::pawnCaptures[COLOR == WHITE ? BLACK : WHITE][square] & pawns;
			attackingPawns = removeAlreadyUsedPieces(attackingPawns);
			return attackingPawns;
		}

		bitBoard_t computeKnightsAttacking(Square square, bitBoard_t knights) {
			bitBoard_t attackingKnights = BitBoardMasks::knightMoves[square] & knights;
			attackingKnights = removeAlreadyUsedPieces(attackingKnights);
			return attackingKnights;
		}

		bitBoard_t computeBishopAttacking(Square square, bitBoard_t bishops) {
			bitBoard_t allPiecesButOwnBishops = allPiecesLeft & ~bishops;
			bitBoard_t attackingBishops = Magics::genBishopAttackMask(square, allPiecesButOwnBishops) & bishops;
			attackingBishops = removeAlreadyUsedPieces(attackingBishops);
			return attackingBishops;
		}

		bitBoard_t computeRookAttacking(Square square, bitBoard_t rooks) {
			bitBoard_t allPiecesButOwnRooks = allPiecesLeft & ~rooks;
			bitBoard_t attackingRooks = Magics::genRookAttackMask(square, allPiecesButOwnRooks) & rooks;
			attackingRooks = removeAlreadyUsedPieces(attackingRooks);
			return attackingRooks;
		}

		bitBoard_t computeQueenAttacking(Square square, bitBoard_t queens) {
			bitBoard_t attackingQueens = Magics::genQueenAttackMask(square, allPiecesLeft) & queens;
			attackingQueens = removeAlreadyUsedPieces(attackingQueens);
			return attackingQueens;
		}

		bitBoard_t computeKingAttacking(Square square, bitBoard_t king) {
			bitBoard_t attackingKing = BitBoardMasks::kingMoves[square] & king;
			return attackingKing;
		}

		inline bitBoard_t removeAlreadyUsedPieces(bitBoard_t pieces) {

			return pieces & allPiecesLeft;
		}

		template <Piece COLOR>
		bitBoard_t getAttackingPieces(const MoveGenerator& position, Square square) {
			bitBoard_t result;
			switch (nextPiece[COLOR]) {
			case PAWN + COLOR:
				result = computePawnsAttacking<COLOR>(square, position.getPieceBB(PAWN + COLOR));
				if (result != 0) {
					nextPiece[COLOR] = KNIGHT + COLOR;
					currentValue[COLOR] = position.getPieceValueForMoveSorting(PAWN + COLOR);
					break;
				}
			case KNIGHT + COLOR:
				result = computeKnightsAttacking(square, position.getPieceBB(KNIGHT + COLOR));
				if (result != 0) {
					nextPiece[COLOR] = BISHOP + COLOR;
					currentValue[COLOR] = position.getPieceValueForMoveSorting(KNIGHT + COLOR);
					break;
				}
			case BISHOP + COLOR:
				result = computeBishopAttacking(square, position.getPieceBB(BISHOP + COLOR));
				if (result != 0) {
					nextPiece[COLOR] = ROOK + COLOR;
					currentValue[COLOR] = position.getPieceValueForMoveSorting(BISHOP + COLOR);
					break;
				}
			case ROOK + COLOR:
				result = computeRookAttacking(square, position.getPieceBB(ROOK + COLOR));
				if (result != 0) {
					nextPiece[COLOR] = QUEEN + COLOR;
					currentValue[COLOR] = position.getPieceValueForMoveSorting(ROOK + COLOR);
					break;
				}
			case QUEEN + COLOR:
				result = computeQueenAttacking(square, position.getPieceBB(QUEEN + COLOR));
				if (result != 0) {
					nextPiece[COLOR] = BISHOP + COLOR;
					currentValue[COLOR] = position.getPieceValueForMoveSorting(QUEEN + COLOR);
					break;
				}
			case KING + COLOR:
				result = computeKingAttacking(square, position.getPieceBB(KING + COLOR));
				currentValue[COLOR] = position.getPieceValueForMoveSorting(KING + COLOR);
				nextPiece[COLOR] = PIECE_AMOUNT;
				break;
			default: result = 0; break;
			}
			return result;
		}

		template <Piece COLOR>
		value_t tryPiece(const MoveGenerator& position, Square square) {
			value_t result = 0;
			nodeCountStatistic++;
			if (pieceToTryBitBoard[COLOR] == 0) {
				pieceToTryBitBoard[COLOR] = getAttackingPieces<COLOR>(position, square);
			}
			if (pieceToTryBitBoard[COLOR] != 0) {
				result = currentValue[COLOR];
				allPiecesLeft &= ~pieceToTryBitBoard[COLOR];
				pieceToTryBitBoard[COLOR] &= pieceToTryBitBoard[COLOR] - 1;
			}
			return result;
		}

		value_t currentValue[2];
		Piece nextPiece[2];
		bitBoard_t pieceToTryBitBoard[2];

		bitBoard_t allPiecesLeft;
		value_t valueOfNextPieceOnTargetField;
		bool whiteToMove;
		value_t alpha;
		value_t beta;
		value_t gain;

		uint64_t nodeCountStatistic;

	};

}

#endif // __SEE_H