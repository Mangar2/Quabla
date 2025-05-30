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
 * State of the generation for a single piece combination
 */

#pragma once

#include <mutex>
#include <atomic>
#include "bitbase.h"
#include "bitbaseindex.h"
#include "compress.h"

namespace QaplaBitbase {

	class GenerationState
	{
	public:
		/**
		 * Creates a generation state object holding information which positions are already decided
		 * and where to look further
		 */
		GenerationState(PieceList& pieceList, uint32_t sig) 
			: _wonPositions(true, sig), _computedPositions(true, sig), _candidates(true, sig)
		{
			BitbaseIndex bitbaseIndexType(pieceList);
			_sizeInBit = bitbaseIndexType.getSizeInBit();
			_wonPositions.resize(_sizeInBit);
			_wonPositions.setLoaded();
			_computedPositions.resize(_sizeInBit);
			_computedPositions.setLoaded();
			_candidates.resize(_sizeInBit);
			_candidates.setLoaded();
			_pieceList = pieceList;
			_illegal = 0;
			_loss = 0;
			_draw = 0;
			_won = 0;
			_updateRunning = false;
		}

		/**
		 * Gets the current piece list
		 */
		const PieceList& getPieceList() const { return _pieceList; }

		/**
		 * Returns true, if the current position is a position to check
		 * @param index index of the position
		 * @param onlyCandidates if true, only candidates are provided to be checked
		 */
		bool isPositionToCheck(uint64_t index, bool onlyCandidates) {
			return !_computedPositions.getBit(index) &&
				(!onlyCandidates || _candidates.getBit(index));
		}

		/**
		 * Retrieves a list of items to check
		 */
		void getWork(vector<uint64_t>& work) const {
			_candidates.getAllIndexes(_computedPositions, work);
		}

		/**
		 * Gets the bitbase size in bits
		 */
		uint64_t getSizeInBit() const { return _sizeInBit; }

		/**
		 * Checks, if last generation loop found candidates
		 */
		bool hasCandidates() const {
			return _hasCandidates;
		}

		/**
		 * Retrieves the won positions bitbase
		 */
		const Bitbase& getWonPositions() const { return _wonPositions; }
		Bitbase& getWonPositions() { return _wonPositions; }

		/**
		 * Sets a list of candidates 
		 */
		void setCandidates(const vector<uint64_t>& candidates) {
			_hasCandidates = true;
			for (auto index : candidates) {
				setCandidate(index);
			}
		}

		/**
		 * Mutex protected version of set Candidates
		 * @param candidates candidates to add
		 */
		bool setCandidatesTreadSafe(const vector<uint64_t>& candidates, bool wait = true) {
			if (candidates.empty()) {
				return false;
			}
			if (wait || !_updateRunning) {
				const lock_guard<mutex> lock(_mtxUpdate);
				_updateRunning = true;
				setCandidates(candidates);
				_updateRunning = false;
				return true;
			} 
			else {
				return false;
			}
		}

		/**
		 * True, if the position is a candidate for further investigation
		 */
		bool isCandidate(uint64_t index) {
			return _candidates.getBit(index);
		}

		/**
		 * Clears all candidates
		 */
		void clearAllCandidates() {
			_candidates.clear();
			_hasCandidates = false;
		}

		/**
		 * Clears a position to be a candidate for lookup
		 */
		void clearCandidate(uint64_t index) {
			_candidates.clearBit(index);
		}

		/**
		 * Sets a position to win
		 */
		void setWin(uint64_t index) {
			_won++;
			_wonPositions.setBit(index);
			_computedPositions.setBit(index);
		}

		/**
		 * Sets a position to loss
		 */
		void setLoss(uint64_t index) {
			_loss++;
			_computedPositions.setBit(index);
		}

		/**
		 * Sets a position to draw
		 */
		void setDraw(uint64_t index) {
			_draw++;
			_computedPositions.setBit(index);
		}


		/**
		 * Sets the position to illegal
		 */
		void setIllegal(uint64_t index) {
			_illegal++;
			_computedPositions.setBit(index);
		}

		/**
		 * Prints a statistic 
		 */
		void printStatistic() {
			uint64_t drawOrLoss = _sizeInBit - _won - _illegal;
			cout
				<< "Won: " << _won << " (" << (_won * 100 / _sizeInBit) << "%) " 
				<< " Draw or loss: " << drawOrLoss << " (" << (drawOrLoss * 100 / _sizeInBit) << "%)"
				<< " Loss in 0: " << _loss
				<< " Illegal: " << _illegal << " (" << (_illegal * 100 / _sizeInBit) << "%)"
				<< " Uncompressed memory size " << _wonPositions.getSize()
				<< std:: endl;
			if (_won != _wonPositions.computeWonPositions()) {
				std::cout << "Error, won positions do not match!" << std::endl;
			}
		}

		/**
		 * Stores the result (bitbase with winning positions) to a file
		 */
		void storeToFile(string fileName, string signature, QaplaCompress::CompressionType compression) {
			_wonPositions.setFilename(signature, ".btb");
			_wonPositions.storeToFile(fileName, compression);
		}

		void generateCpp(string signature) {
			_wonPositions.writeAsCppFile(signature, signature + ".h");
		}

		void print() {
			std::cout << "Won positions: " << std::endl;
			_wonPositions.print();
			std::cout << "Computed positions: " << std::endl;
			_computedPositions.print();
			std::cout << "Candidates: " << std::endl;
			_candidates.print();
		}

	private:
		/**
		 * Sets a position to a candidate for lookup
		 */
		void setCandidate(uint64_t index) {
			_candidates.setBit(index);
		}

		uint64_t _sizeInBit;
		std::atomic<uint64_t> _illegal;
		std::atomic<uint64_t> _loss;
		std::atomic<uint64_t> _draw;
		std::atomic<uint64_t>  _won;
		std::atomic<uint64_t>  _hasCandidates;
		Bitbase _wonPositions;
		Bitbase _computedPositions;
		Bitbase _candidates;
		PieceList _pieceList;
		mutex _mtxUpdate;
		bool _updateRunning;
	};

}
