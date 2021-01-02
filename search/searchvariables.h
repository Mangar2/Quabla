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
 * Implements all needed variables for search in one structure
 */

#ifndef __SEARCHVARIABLES_H
#define __SEARCHVARIABLES_H

#include "../basics/types.h"
// #include "Whatif.h"
#include "../basics/move.h"
#include "../movegenerator/movegenerator.h"
#include "killermove.h"
#include "pv.h"
#include "moveprovider.h"
#include "tt.h"
#include "HistoryTable.h"
#include "SearchParameter.h"
#include "Extension.h"

using namespace ChessMoveGenerator;

namespace ChessSearch {
	struct SearchVariables {

		typedef uint32_t pvIndex_t;
		typedef uint32_t cutoff_t;

		SearchVariables() {
			cutoff = CUTOFF_NONE;
		};
		SearchVariables(const SearchVariables& searchInfo) {
			operator=(searchInfo);
		}

		SearchVariables& operator=(const SearchVariables& searchInfo) {
			alpha = searchInfo.alpha;
			beta = searchInfo.beta;
			remainingDepth = searchInfo.remainingDepth;
			bestMove = searchInfo.bestMove;
			bestValue = searchInfo.bestValue;
			moveNo = searchInfo.moveNo;
			ply = searchInfo.ply;
			moveProvider = searchInfo.moveProvider;
			searchState = searchInfo.searchState;
			noNullmove = searchInfo.noNullmove;
			return *this;
		}

		void setPVMove(Move pvMove) {
			moveProvider.setPVMove(pvMove);
		}

		void selectFirstSearchState() {
			if (doIID()) {
				searchState = IID_SEARCH;
				remainingDepth = 1;
			}
			else if (beta > alpha + 1) {
				searchState = PV_SEARCH;
			}
			else {
				searchState = NORMAL_SEARCH;
			}
		}


		void setFromPreviousPly(MoveGenerator& board, const SearchVariables& previousPlySearchInfo, Move previousPlyMove) {
			beta = -previousPlySearchInfo.alpha;
			alpha = -previousPlySearchInfo.beta;
			alphaAtPlyStart = alpha;
			betaAtPlyStart = beta;
			remainingDepth = previousPlySearchInfo.remainingDepth - 1;
			remainingDepthAtPlyStart = remainingDepth;
			pvMovesStore.setEmpty(ply);
			pvMovesStore.setEmpty(ply + 1);
			bestMove = Move::EMPTY_MOVE;
			bestValue = -MAX_VALUE;
			selectFirstSearchState();
			noNullmove = previousPlySearchInfo.previousMove.isNullMove() || previousPlyMove.isNullMove();
			moveProvider.init();
			moveNo = 0;
			doMove(board, previousPlyMove);
			cutoff = checkBasicCutoffs(board);
			keepBestMoveUnchanged = false;
			lateMoveReduction = 0;
		}

		void init(MoveGenerator& board, value_t initialAlpha, value_t initialBeta, int32_t searchDepth) {
			remainingDepth = searchDepth;
			alpha = initialAlpha;
			beta = initialBeta;
			alphaAtPlyStart = alpha;
			betaAtPlyStart = beta;
			bestMove = Move::EMPTY_MOVE;
			bestValue = -MAX_VALUE;
			searchState = PV_SEARCH;
			noNullmove = true;
			moveNo = 0;
			cutoff = CUTOFF_NONE;
			positionHashSignature = board.getBoardHash();
			moveProvider.init();
			keepBestMoveUnchanged = false;
			lateMoveReduction = 0;
		}

		void doMove(MoveGenerator& board, Move previousPlyMove) {
			previousMove = previousPlyMove;
			boardState = board.getBoardState();
			if (previousPlyMove == Move::NULL_MOVE) {
				board.doNullmove();
			}
			else {
				board.doMove(previousMove);
			}
			positionHashSignature = board.getHashKey();
		}

		void undoMove(MoveGenerator& board) {
			if (previousMove == Move::NULL_MOVE) {
				board.undoNullmove();
			}
			else {
				board.undoMove(previousMove);
			}
			board.boardState = boardState;
		}

		cutoff_t checkBasicCutoffs(MoveGenerator& board) {
			cutoff_t result = CUTOFF_NONE;
			if (alpha > MAX_VALUE - ply) {
				result = CUTOFF_FASTER_MATE_FOUND;
				bestValue = alpha;
			}
			else if (beta < -MAX_VALUE + ply) {
				result = CUTOFF_FASTER_MATE_FOUND;
				bestValue = beta;
			}
			else if (ply >= 3 && board.pieceSignature.drawDueToMissingMaterial()) {
				cutoff = CUTOFF_NOT_ENOUGH_MATERIAL;
				bestValue = 0;
			}
			return result;
		}

		void getHashEntry(MoveGenerator& board) {

			if (cutoff == CUTOFF_NONE) {
				hash_t hashEntryInfo = hashPtr->getHashEntryInfo(positionHashSignature);

				if (hashEntryInfo != Hash::EMPTY_ENTRY) {
					Move move = HashEntryInfo::getMove(hashEntryInfo);
					moveProvider.setHashMove(move);
					// keeps the best move for a hash entry, if the search does stay <= alpha
					if (searchState != PV_SEARCH) {
						bestMove = move;
					}
					if (HashEntryInfo::getHashEntryValues(hashEntryInfo, bestValue, alpha, beta, remainingDepth, ply)) {
						cutoff = CUTOFF_HASH;
					}
					if (bestValue >= beta) {
						cutoff = CUTOFF_HASH;
					}
					// Narrowing the window by the hash is problematic. The search will be made with a wrong window
					betaAtPlyStart = beta;
					alphaAtPlyStart = alpha;
				}
			}
		}

		inline void cutoffSearch(uint32_t cutoffType, value_t cutoffResult) {
			// Do not "overwrite" older cutoff informations
			if (cutoff == CUTOFF_NONE && cutoffType != CUTOFF_NONE) {
				cutoff = cutoffType;
				bestValue = cutoffResult;
			}
		}

		inline void cutoffSearch(uint32_t cutoffType) {
			if (cutoff == CUTOFF_NONE && cutoffType != CUTOFF_NONE) {
				cutoff = cutoffType;
			}
		}

		void addNullmove() {
			searchState = SELECT_NULLMOVE;
		}

		void extendSearch(MoveGenerator& board) {
			assert(cutoff == CUTOFF_NONE);
			searchDepthExtension = Extension::calculateExtension(board, previousMove, remainingDepth);
			remainingDepth += searchDepthExtension;
		}

		void generateMoves(MoveGenerator& board) {
			assert(cutoff == CUTOFF_NONE);
			moveProvider.computeMoves(board, previousMove, remainingDepth);
			bestValue = moveProvider.checkForGameEnd(board, ply);
			sideToMoveIsInCheck = board.isInCheck();
		}

		bool doIID() {
			return false;
			bool result = false;
			if (remainingDepth > 4 * ONE_PLY && moveProvider.getHashMove() == 0 && searchState == PV_SEARCH) {
				result = true;
			}
		}

		void setStateToNullmove() {
			searchState = NULLMOVE_SEARCH;
			remainingDepth -= SearchParameter::getNullmoveReduction(ply, remainingDepth);
			alpha = beta - 1;
			if (remainingDepth < 1) {
				remainingDepth = 1;
			}
			keepBestMoveUnchanged = true;
		}


		Move setSearchState(MoveGenerator& board) {
			Move move = Move::EMPTY_MOVE;
			switch (searchState) {
			case START_SEARCH: selectFirstSearchState(); break;
			case IID_SEARCH:
				remainingDepth += 4;
				if (remainingDepth >= remainingDepthAtPlyStart - 3) {
					remainingDepth = remainingDepthAtPlyStart;
					searchState = NORMAL_SEARCH;
				}
				break;
			case PV_SEARCH:
				if (bestMove != Move::EMPTY_MOVE && remainingDepth >= 2) {
					beta = alpha + 1;
					searchState = NULL_WINDOW_SEARCH;
					keepBestMoveUnchanged = true;
				}
				break;
			case NULL_WINDOW_SEARCH:
				if (currentValue >= beta) {
					// Needed to ensure that the research will be > bestValue and set the PV
					bestValue = alpha;
					beta = betaAtPlyStart;
					searchState = PV_SEARCH;
					move = moveProvider.getCurrentMove();
					keepBestMoveUnchanged = false;
				}
				break;
			case SELECT_NULLMOVE:
				setStateToNullmove();
				move = Move::NULL_MOVE;
				break;
			case NULLMOVE_SEARCH:
				if (bestValue < beta) {
					bestValue = -MAX_VALUE;
					alpha = alphaAtPlyStart;
					remainingDepth = remainingDepthAtPlyStart;
					keepBestMoveUnchanged = false;
					searchState = NORMAL_SEARCH;
				}
				break;
			case NORMAL_SEARCH:

				lateMoveReduction = SearchParameter::getLateMoveReduction(false, ply, moveProvider.getTriedMovesAmount());
				if (lateMoveReduction > 0) {
					searchState = LMR_SEARCH;
					keepBestMoveUnchanged = true;
					remainingDepth -= lateMoveReduction;
				}

				break;
			case LMR_SEARCH:
				if (currentValue >= beta) {
					// Research
					searchState = NORMAL_SEARCH;
					remainingDepth = remainingDepthAtPlyStart;
					move = moveProvider.getCurrentMove();
					keepBestMoveUnchanged = false;
					lateMoveReduction = 0;
				}
				break;
			}
			return move;
		}

		Move selectNextMove(MoveGenerator& board) {
			assert(cutoff == CUTOFF_NONE);
			Move move = setSearchState(board);
			if (bestValue < betaAtPlyStart && move == Move::EMPTY_MOVE) {
				moveNo++;
				move = moveProvider.selectNextMove(board);
			}
			return move;

		}

		Move selectNextMoveThreadSafe(MoveGenerator& board) {
			std::lock_guard<std::mutex> lockGuard(mutex);
			Move move = Move::EMPTY_MOVE;
			if (cutoff == CUTOFF_NONE) {
				move = selectNextMove(board);
			}
			return move;
		}

		void setSearchResult(value_t searchResult, const SearchVariables& nextPlySearchInfo, Move currentMove) {
			currentValue = searchResult;
			if (searchResult > bestValue) {
				bestValue = searchResult;
				if (searchResult > alpha && currentMove != Move::NULL_MOVE) {
					if (!keepBestMoveUnchanged) {
						bestMove = currentMove;
						if (remainingDepth > 0 && searchState == PV_SEARCH) {
							pvMovesStore.copyFromPV(nextPlySearchInfo.pvMovesStore, ply + 1);
						}
						pvMovesStore.setMove(ply, bestMove);
					}
					if (searchResult < beta) {
						// Never set alpha > beta, it will harm the PVS algorithm
						alpha = searchResult;
					}

					else if (remainingDepth > 1) {
						HistoryTable::setSearchResult(remainingDepth, bestMove, moveProvider);
					}
				}
			}
			if (searchResult >= beta && currentMove == Move::NULL_MOVE) {
				//remainingDepth -= SearchParameter::getNullmoveVerificationDepthReduction(ply, remainingDepth);
			}
		}

		void setSearchResultThreadSafe(value_t searchResult, const SearchVariables& searchInfo, Move currentMove) {
			std::lock_guard<std::mutex> lockGuard(mutex);
			setSearchResult(searchResult, searchInfo, currentMove);
		}

		void setHashEntry(hash_t hashKey) {
			bool drawByRepetetivePositionValue = (bestValue == 0);
			if (!drawByRepetetivePositionValue) {
				hashPtr->setHashEntry(hashKey, remainingDepthAtPlyStart, ply, bestMove, bestValue, alphaAtPlyStart, betaAtPlyStart, false);
				WHATIF(WhatIf::whatIf.setHash(hashPtr, hashKey, remainingDepthAtPlyStart, ply, bestMove, bestValue, alphaAtPlyStart, betaAtPlyStart, false));
			}
		}

		void terminatePly(MoveGenerator& board) {

			if (cutoff == CUTOFF_NONE && bestMove != Move::NULL_MOVE) {
				moveProvider.setKillerMove(bestMove);
				setHashEntry(board.getHashKey());
			}
			undoMove(board);
		}

		bool isInPV() { return searchState == PV_SEARCH; }

		Move getMoveFromPVMovesStore(uint32_t ply) const { return pvMovesStore[ply]; }
		const KillerMove& getKillerMove() const { return moveProvider.getKillerMove(); }
		Move getHashMove() const { return moveProvider.getHashMove(); }
		void setPly(ply_t curPly) { ply = curPly; }

		const char* getSearchStateName() const {
			return searchStateNames[searchState];
		}

		void setHash(Hash* hash) {
			hashPtr = hash;
		}

		inline bool isHashValueBelowBeta(const Board& board) {
			return hashPtr->isHashValueBelowBeta(board.getHashKey(), beta);
		}

		enum searchTypes {
			START_SEARCH, PV_SEARCH, NULL_WINDOW_SEARCH, PV_LMR_SEARCH, NORMAL_SEARCH, LMR_SEARCH, SELECT_NULLMOVE, NULLMOVE_SEARCH, VERIFY_SEARCH, IID_SEARCH
		};

		static constexpr char* searchStateNames[] = { "Start", "PV", "NULL", "Normal", "SelNull", "Null", "Verify", "IID" };

		enum cutoffTypes {
			CUTOFF_NONE, CUTOFF_DRAW_BY_REPETITION, CUTOFF_HASH, CUTOFF_FASTER_MATE_FOUND, CUTOFF_RAZORING, CUTOFF_NOT_ENOUGH_MATERIAL
		};

		static const cutoff_t CUTOFF_TYPE_AMOUNT = 5;

		value_t alpha;
		value_t alphaAtPlyStart;
		value_t beta;
		value_t betaAtPlyStart;
		value_t bestValue;
		value_t currentValue;
		Move bestMove;
		Move previousMove;
		ply_t remainingDepth;
		ply_t remainingDepthAtPlyStart;
		ply_t ply;
		ply_t lateMoveReduction;
		ply_t searchDepthExtension;
		BoardState boardState;
		uint32_t moveNo;
		uint32_t searchState;
		hash_t positionHashSignature;
		bool noNullmove;
		bool sideToMoveIsInCheck;
		bool keepBestMoveUnchanged;

		cutoff_t cutoff;
		std::mutex mutex;
		MoveProvider moveProvider;
		PV pvMovesStore;

	private:
		Hash* hashPtr;
	};

}

#endif // __SEARCHVARIABLES_H