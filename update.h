/*
 * update.h
 *
 *  Created on: Aug 14, 2018
 *      Author: nae9on
 */

#ifndef UPDATE_H_
#define UPDATE_H_

#include <Eigen/Sparse>
#include <vector>
#include <iostream>

typedef Eigen::Triplet<double> tripleData;

void insertCoefficient(int id, int i, int j, double w,
		std::vector<tripleData>& coeffs, Eigen::VectorXd& b,
		const Eigen::VectorXd& boundary) {
	int n = int(boundary.size());
	int id1 = i + j * n;

	if (i == -1 || i == n)
		b(id) -= w * boundary(j); // constrained coefficient
	else if (j == -1 || j == n)
		b(id) -= w * boundary(i); // constrained coefficient
	else
		coeffs.push_back(tripleData(id, id1, w));         // unknown coefficient
}

// Fill right hand side
void updateRHS(Eigen::VectorXd& b, Eigen::VectorXd h) {
	for (unsigned int i = 2; i < b.size() - 2; i++) {
		b(i) = h(i);
		b(i) += p1 * 0.5 * (h(i + 1) * h(i + 1) - h(i) * h(i))
				/ (h(i) * h(i + 1));
		b(i) -= p1 * 0.5 * (h(i) * h(i) - h(i - 1) * h(i - 1))
				/ (h(i) * h(i - 1));
	}

}

// Inefficient
void fillPentaDiagonal_Ineff(Eigen::SparseMatrix<double, Eigen::ColMajor> *A) {
	for (int i = 0; i < A->rows(); i++) {
		for (int j = 0; j < A->cols(); j++) {
			A->insert(i, j) = i + j;
		}
	}

}

// Efficient
void fillPentaDiagonal(std::vector<tripleData>& coefficients, int N) {

	// Fill left boundary
	coefficients.push_back(tripleData(0, 0, 1));
	coefficients.push_back(tripleData(0, N - 4, -1));
	coefficients.push_back(tripleData(1, 1, 1));
	coefficients.push_back(tripleData(1, N - 3, -1));

	// Fill the core entries
	for (int i = 2; i <= N - 3; i++) {
		coefficients.push_back(tripleData(i, i - 2, 0.7566));
		coefficients.push_back(tripleData(i, i - 1, -3.0264));
		coefficients.push_back(tripleData(i, i, 5.5396));
		coefficients.push_back(tripleData(i, i + 1, -3.0264));
		coefficients.push_back(tripleData(i, i + 2, 0.7566));
	}

	// Fill right boundary
	coefficients.push_back(tripleData(N - 2, 2, -1));
	coefficients.push_back(tripleData(N - 2, N - 2, 1));
	coefficients.push_back(tripleData(N - 1, 3, -1));
	coefficients.push_back(tripleData(N - 1, N - 1, 1));
}

// Efficient
void updateA(std::vector<tripleData>& coefficients, Eigen::VectorXd h, int N) {

	coefficients.clear();

	// Fill left boundary
	coefficients.push_back(tripleData(0, 0, 1));
	coefficients.push_back(tripleData(0, N - 4, -1));
	coefficients.push_back(tripleData(1, 1, 1));
	coefficients.push_back(tripleData(1, N - 3, -1));

	// Fill the core entries
	for (int i = 2; i <= N - 3; i++) {
		double h1 = 2 * h(i + 1) * h(i + 1) * h(i) * h(i) / (h(i + 1) + h(i));
		double h2 = 2 * h(i - 1) * h(i - 1) * h(i) * h(i) / (h(i - 1) + h(i));

		coefficients.push_back(tripleData(i, i - 2, h2 * p2));
		coefficients.push_back(tripleData(i, i - 1, -p2 * (h1 + 3 * h2)));
		coefficients.push_back(tripleData(i, i, 3 * p2 * (h1 + h2) + 1));
		coefficients.push_back(tripleData(i, i + 1, -p2 * (3 * h1 + h2)));
		coefficients.push_back(tripleData(i, i + 2, h1 * p2));
	}

	// Fill right boundary
	coefficients.push_back(tripleData(N - 2, 2, -1));
	coefficients.push_back(tripleData(N - 2, N - 2, 1));
	coefficients.push_back(tripleData(N - 1, 3, -1));
	coefficients.push_back(tripleData(N - 1, N - 1, 1));
}

#endif /* UPDATE_H_ */