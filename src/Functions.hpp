/*
 #License and Copyright
 
 #Version : 1.1
 
 #This file is part of BiUFLv2012.
 
 #BiUFLv2012 is Copyright © 2012, University of Nantes
 
 #BiUFLv2012 is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
 
 #This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 
 #You should have received a copy of the GNU General Public License along with this program; if not, you can also find the GPL on the GNU web site.
 
 #In addition, we kindly ask you to acknowledge BiUFLv2012 and its authors in any program or publication in which you use BiUFLv2012. (You are not required to do so; it is up to your common sense to decide whether you want to comply with this request or not.) For general publications, we suggest referencing:  BiUFLv2012, MSc ORO, University of Nantes.
 
 #Non-free versions of BiUFLv2012 are available under terms different from those of the General Public License. (e.g. they do not require you to accompany any object code using BiUFLv2012 with the corresponding source code.) For these alternative terms you must purchase a license from Technology Transfer Office of the University of Nantes. Users interested in such a license should contact us (valorisation@univ-nantes.fr) for more information.
 */

/*!
* \file Functions.hpp
* \brief A set of functions usefull for our software.
* \author Salim BOUROUGAA & Alban DERRIEN & Axel GRIMAULT & Xavier GANDIBLEUX & Anthony PRZYBYLSKI
* \date 28 August 2012
* \version 1.1
* \copyright GNU General Public License 
*
* This file groups all the functions for solving our problem which are not methods of Class.
*
*/

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <vector>
#include <map>
#include <list>
#include <iostream>
#include <cmath>

#include "Data.hpp"
#include "Solution.hpp"
#include "Box.hpp"
#include "ToFile.hpp"
#include "LabelSetting.hpp"

/*!
*	\defgroup paving Methods of Paving
*/

/*!
*	\fn long int createBox(vector<Box*> &vectorBox, Data &data)
*	\ingroup paving
*	\brief This method computes all the initial \c Boxes of our algorithm.

*	This method computes all the \c Boxes in whichones the Label Setting algorithm will runs. This method uses a smart Branch&Bound (Breadth First Search, splitting on \c Facility setup variables) to compute all the feasible and important \c Boxes. Useless \c Boxes are avoided by the Branch&Bound.
*	\param[in,out] vectorBox : A vector of \c Box, empty a the beginning, and containing all the \c Boxes at the end of this method.
*	\param[in] data : A \c Data object which contains all the values of the instance.
*	\return A long int which represents the number of \c Boxes computed by the method.
*/
long int createBox(std::vector<Box*> &vectorBox, Data &data);

/*!
*	\fn void initBoxCapacitated(vector<Box*> &vectorBox, Data &data, vector<int> & sorted_fac)
*	\ingroup paving
*	\brief This method computes all the initial \c Boxes for the capacitated problem.
*/
void initBoxCapacitated(std::vector<Box*> &vectorBox, Data &data, std::vector<int> & sorted_fac);

/*!
*	\fn addChildren(Box *boxMother, vector<Box*> &vBox, vector<int> & sorted_fac)
*	\ingroup paving
*	\brief This method adds children of a \c Box into a vector of \c Boxes.

*	This method computes all the children \c Boxes of a \c Box into a vector of \c Boxes. A children is defined by a combination of \c Facility in which indices have not yet been opened.
*	\param[in] boxMother : A \c Box for which ones wants to add children \c Boxes.
*	\param[in,out] vBox : A vector of \c Box in whichone ones add all the children \c Boxes at the end (enqueue at the end of the vector).
*	\param[in] sorted_fac : A vector of indices of facilities (permutation, can be identity).
*/
void addChildren(Box *boxMother, std::vector<Box*> &vBox, const std::vector<int> & sorted_fac);

/*!
*	\fn filter(vector<Box*> &vectorBox, long int &nbToCompute, long int &nbWithNeighbor)
*	\ingroup paving
*	\brief This method filters all the \c Boxes of a vector of \c Box.

*	This method filters \c Boxes by eliminating all \c Box that are dominated by an other one.
*	\param[in,out] nbToCompute : A long int that takes the number of \c Boxes with a available decomposition.
*	\param[in,out] nbWithNeighbor : A long int that takes the number of \c Boxes overlapped by another one.
*/
void filter(std::vector<Box*> &vectorBox, long int &nbToCompute, long int &nbWithNeighbor);

/*!
*	\fn boxFiltering(vector<Box*> &vectorBox, Data &data, long int &nbToCompute, long int &nbWithNeighbor)
*	\ingroup paving
*	\brief This method splits a \c Box into two \c Boxes.

*	Using a weighted sum method to find a supported point, ones splits a \Box into two \c Boxes.
*	\param[in,out] vectorBox : A vector of \c Box in which one ones adds all the new \c Boxes computed.
*	\param[in] data : A \c Data object which contains all the values of the instance.
*	\param[in,out] nbToCompute : A long int that takes the number of \c Boxes with a available decomposition.
*	\param[in,out] nbWithNeighbor : A long int that takes the number of \c Boxes overlapped by another one.
*/
void boxFiltering(std::vector<Box*> &vectorBox, Data &data, long int &nbToCompute, long int &nbWithNeighbor);

/*!
*	\fn recomposition(vector<Box*> &vectorBox, vector<Box*> &vectorBoxFinal, Data &data, long int &nbToCompute, long int &nbWithNeighbor)
*	\ingroup paving
*	\brief This method recomposes a group of \c Boxes into an unique one.

*	This method selects all the \c Boxes with the same combination of \c Facility. Ones creates an unique \c Box with this combination of \c Facility and update the bounds of this one w.r.t. the bounds of all \c Boxes selected.
*	\param[in,out] vectorBox : A vector of \c Box in which one ones adds all the new \c Boxes computed.
*	\param[in,out] vectorBoxFinal : A vector of \c Box containing all the recompose \c Boxes
*	\param[in] data : A \c Data object which contains all the values of the instance.
*	\param[in,out] nbToCompute : A long int that takes the number of \c Boxes with a available decomposition.
*	\param[in,out] nbWithNeighbor : A long int that takes the number of \c Boxes overlapped by another one.
*/
void recomposition(std::vector<Box*> &vectorBox, std::vector<Box*> &vectorBoxFinal, Data &data, long int &nbToCompute, long int &nbWithNeighbor);

/*!
*	\fn weightedSumOneStep(vector<Box*> &vectorBox, Data &data)
*	\ingroup paving
*	\brief Compute the weighted sum method to split a \c Box in two \c Boxes.

*	This method adds to the vectorBox the results of dividing each \c Box in two others. It uses the weighted sum method which splits \c Box by the first supported point founded.
*	\param[in] vectorBox : A vector of \c Box which contains all the \c Boxes in which one ones computes the weighted sum method.
*	\param[in] data : A \c Data object which contains all the values of the instance.
*	\return A long int which value is the number of solutions of the algorithm proposed.
*/
void weightedSumOneStep(std::vector<Box*> &vectorBox, Data &data);

/*!
*	\defgroup generating Methods of Generating
*/

/*!
*	\fn runLabelSetting(vector<Box*> &vectorBox, Data &data)
*	\ingroup generating
*	\brief This method runs the Label Setting algorithm.

*	This method executes a Label Setting algorithm in each \c Box of the input vector. 
*	\param[in] vectorBox : A vector of \c Box which contains all the \c Boxes in which one ones runs the algorithm of Label Setting.
*	\param[in] data : A \c Data object which contains all the values of the instance.
*	\return A long int which value is the number of solutions of the algorithm proposed.
*/
long int runLabelSetting(std::vector<Box*> &vectorBox, Data &data, std::list<Solution> & allSolution);

/*!
*	\fn runMOGA(vector<Box*> &vectorBox, Data &data)
*	\ingroup generating
*	\brief This method runs the multi-objective genetic algorithm.

*	This method executes a multi-objective genetic algorithm in each \c Box of the input vector. 
*	\param[in] vectorBox : A vector of \c Box which contains all the \c Boxes in which one ones runs the algorithm.
*	\param[in] data : A \c Data object which contains all the values of the instance.
*	\return A long int which value is the number of solutions of the algorithm proposed.
*/
long int runMOGA(std::vector<Box*> &vectorBox, Data &data, std::list<Solution> & allSolution);

/*!
*	\fn filterListSolution(list<Solution> &lsol)
*	\ingroup generating
*	\brief Delete the solutions dominated.

*	A method to delete all the dominated solutions into the objective space. This method computes a complete set of solution.
*	\param[in] lsol : A vector of \c Solution which contains all the \c Solutions to delete.
*/
void filterListSolution(std::list<Solution> &lsol);

/*!
*	\defgroup others Others Methods
*/

/*!
*	\fn computeCorrelation(Data &data)
*	\ingroup others
*	\brief This method computes the correlation.

*	This method computes the correlation between the two objectives w.r.t. to the \c Data.
*	\param[in] data : A \c Data object which contains all the values of the instance.
*	\return A double representing the correlation between the two objectives.
*/
double computeCorrelation(Data &data);

/*!
*	\fn time_ms_Diff(timeval tvStart, timeval tvEnd)
*	\ingroup others
*	\brief This method computes the difference in milli-seconds (ms) between two times.

*	\param[in] tvStart : A structure of time represeting the begin time.
*	\param[in] tvEnd : A structure of time represeting the end time.
*	\return A double representing the difference in ms between the two times.
*/
float time_ms_Diff(timeval tvStart, timeval tvEnd);
/*!
*	\fn time_s_Diff(timeval tvStart, timeval tvEnd)
*	\ingroup others
*	\brief This method computes the difference in seconds (ms) between two times.

*	\param[in] tvStart : A structure of time represeting the begin time.
*	\param[in] tvEnd : A structure of time represeting the end time.
*	\return A double representing the difference in s between the two times.
*/
float time_s_Diff(timeval tvStart, timeval tvEnd);

#endif
