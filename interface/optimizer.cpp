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
 * Implements a UCI - Interface
 */

#include "optimizer.h"


void Optimizer::addPoint(double x, double pMeasured, double radius) {
    points.push_back(Point{ x, pMeasured, pMeasured, 1 });
    updateEstimates(radius);
}

std::pair<uint32_t, Optimizer::Point> Optimizer::getBest() {
    uint32_t bestIndex = 0;
	if (points.size() == 0) {
		return std::make_pair(bestIndex, Point{ 0, 0, 0, 0 });
	}
    double bestValue = points[0].pEstimated;
    for (uint32_t i = 1; i < points.size(); ++i) {
        if (points[i].pEstimated > bestValue) {
            bestValue = points[i].pEstimated;
            bestIndex = i;
        }
    }
    return std::make_pair(bestIndex, points[bestIndex]);
}


void Optimizer::updateEstimates(double radius)
{
    // Sortieren nach x, damit Nachbarschaft klar definiert ist
    std::sort(points.begin(), points.end(), [](auto& a, auto& b) {return a.x < b.x; });

    for (auto& center : points) {
        double sumWeights = 0.0;
        double sumValues = 0.0;

        // Wir bilden ein gewichtetes Mittel �ber alle Punkte
        for (const auto& nbr : points) {
            double dist = std::fabs(center.x - nbr.x);
            // Gewicht z.B. exponentiell abnehmend
            double w = std::exp(-(dist * dist) / (2.0 * radius * radius));
            sumWeights += w;
            sumValues += w * nbr.pMeasured;
        }
        center.pEstimated = sumValues / (sumWeights + 1e-12);
        center.confidence = sumWeights; // Einfache Variante: Summe der Gewichte
    }
}

// Checks if the 6 neightbors of the best point have a nearly equal value
bool Optimizer::unrelevant() {
	const int numNeighbors = 4;
    if (points.size() <= numNeighbors) { // We need at least 7 points to have 6 neighbors
        return false;
    }
    auto [index, best] = getBest();
    double bestValue = best.pMeasured;
    double tolerance = 0.003; // Tolerance for "nearly equal"

    // Check the 6 neighbors around the best point
    int start = std::max(0, static_cast<int>(index) - (numNeighbors / 2));
    int end = std::min(static_cast<int>(points.size()) - 1, static_cast<int>(index) + (numNeighbors / 2));

    // Adjust start and end to ensure we always check 6 neighbors
    if (end - start < numNeighbors) {
        if (start == 0) {
            end = std::min(static_cast<int>(points.size()) - 1, start + numNeighbors);
        }
        else if (end == static_cast<int>(points.size()) - 1) {
            start = std::max(0, end - numNeighbors);
        }
    }

    for (int i = start; i <= end; ++i) {
        if (i == index) continue; // Skip the best point itself
        double neighborValue = points[i].pMeasured;
        if (std::abs(neighborValue - bestValue) > tolerance) {
            return false; // If the neighbor value is not within the tolerance, the condition is not met
        }
    }
    return true; // All neighbors have nearly the same measured value
}

double Optimizer::nextX(double min, double max) {
    if (points.size() == 0) {
        return 1;
    } 
    else if (points.size() == 1) {
		return min;
	}
    else if (points.size() == 2) {
        return max;
    }
    uint32_t bestIndex = 0;
    double leftNeighborDiff = 0;
    double rightNeighborDiff = 0;
    double best = 0;
    double bestCandidateValue = 0;
    for (uint32_t index = 0; index < points.size(); ++index) {
		if (points[index].pEstimated > best) {
			best = points[index].pEstimated;
		}
        const double candidateValue = points[index].pEstimated + points[index].pMeasured / 10;
        if (candidateValue > bestCandidateValue) {
            bestCandidateValue = candidateValue;
            bestIndex = index;
        }
    }
	if (bestIndex == 0) {
		return (points[1].x + points[0].x) / 2;
	}
    else if (bestIndex == points.size() - 1) {
        return (points[bestIndex].x + points[bestIndex - 1].x) / 2;
    } 
	leftNeighborDiff = points[bestIndex].x - points[bestIndex - 1].x;
	rightNeighborDiff = points[bestIndex + 1].x - points[bestIndex].x;
    return rightNeighborDiff > leftNeighborDiff ? points[bestIndex].x + rightNeighborDiff / 2 : points[bestIndex].x - leftNeighborDiff / 2;
}

void Optimizer::printBest(std::ostream& os) {
	auto [index, best] = getBest();
	os << " best scale: " << best.x << " p: " << best.pMeasured << " est: " << (best.pEstimated * 100.0) << "% conf: " << best.confidence << " ";
}