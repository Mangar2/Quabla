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
 * Defines maps to create the piece signature bitmap
 */

#ifndef __PIECESIGNATUREMAPS_H
#define __PIECESIGNATUREMAPS_H

#include "move.h"

namespace ChessBasics {


	enum class SignatureShift {
		
	};



	class PieceSignatureMaps {
	private:
		PieceSignatureMaps() {}

	public:
		static const pieceSignature_t SIG_SHIFT_BLACK = 10;
		static const pieceSignature_t SIG_BLACK_PAWN = 0x00400;
		static const pieceSignature_t SIG_BLACK_KNIGHT = 0x01000;
		static const pieceSignature_t SIG_BLACK_BISHOP = 0x04000;
		static const pieceSignature_t SIG_BLACK_ROOK = 0x10000;
		static const pieceSignature_t SIG_BLACK_QUEEN = 0x40000;

		static const pieceSignature_t SIG_BLACK_PAWN_MASK = SIG_BLACK_PAWN * 3;
		static const pieceSignature_t SIG_BLACK_KNIGHT_MASK = SIG_BLACK_KNIGHT * 3;
		static const pieceSignature_t SIG_BLACK_BISHOP_MASK = SIG_BLACK_BISHOP * 3;
		static const pieceSignature_t SIG_BLACK_ROOK_MASK = SIG_BLACK_ROOK * 3;
		static const pieceSignature_t SIG_BLACK_QUEEN_MASK = SIG_BLACK_QUEEN * 3;

		static const pieceSignature_t BLACK_MATERIAL_MASK =
			SIG_BLACK_PAWN_MASK + SIG_BLACK_KNIGHT_MASK + SIG_BLACK_BISHOP_MASK + SIG_BLACK_ROOK_MASK + SIG_BLACK_QUEEN_MASK;


		static uint32_t getPieceAmount(pieceSignature_t _signature) {
			uint32_t result = 0;
			for (; _signature != 0; _signature >>= 2) {
				result += _signature & 3;
			}
			return result;
		}

		static void initDoFutility() {
			for (uint32_t index = 0; index < SIG_AMOUNT_PER_COLOR; index++) {
				doFutilityOnCaptureMap[index] = true;
				if (getPieceAmount(index) <= 2) {
					doFutilityOnCaptureMap[index] = false;
				}
			}
		}

		static void initStatics() {
			initDoFutility();
			mapPieceToSignature[NO_PIECE] = 0;
			mapPieceToSignature[WHITE_PAWN] = SIG_PAWN;
			mapPieceToSignature[WHITE_KNIGHT] = SIG_KNIGHT;
			mapPieceToSignature[WHITE_BISHOP] = SIG_BISHOP;
			mapPieceToSignature[WHITE_ROOK] = SIG_ROOK;
			mapPieceToSignature[WHITE_QUEEN] = SIG_QUEEN;
			mapPieceToSignature[WHITE_KING] = 0;
			mapPieceToSignature[BLACK_PAWN] = SIG_BLACK_PAWN;
			mapPieceToSignature[BLACK_KNIGHT] = SIG_BLACK_KNIGHT;
			mapPieceToSignature[BLACK_BISHOP] = SIG_BLACK_BISHOP;
			mapPieceToSignature[BLACK_ROOK] = SIG_BLACK_ROOK;
			mapPieceToSignature[BLACK_QUEEN] = SIG_BLACK_QUEEN;
			mapPieceToSignature[BLACK_KING] = 0;
		}

		static pieceSignature_t mapPieceToSignature[MAX_PIECE + 1];
		static bool doFutilityOnCaptureMap[PieceSignatureMaps::SIG_AMOUNT_PER_COLOR];


	};
}

#endif // __PIECESIGNATUREMAPS_H