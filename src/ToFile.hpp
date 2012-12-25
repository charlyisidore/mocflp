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
* \file ToFile.hpp
* \brief Class of the \c ToFile.
* \author Salim BOUROUGAA & Alban DERRIEN & Axel GRIMAULT & Xavier GANDIBLEUX & Anthony PRZYBYLSKI
* \date 28 August 2012
* \version 1.1
* \copyright GNU General Public License
*
* This class exports values, solutions to file in the folder res.
*
*/

#ifndef TOFILE_H
#define TOFILE_H

#include <list>
#include <iomanip>
#include <vector>
#include <stdio.h>
#include <fstream>
#include <dirent.h>
#include <sys/types.h>
#include <sstream>
#include <string>

#include "Box.hpp"
#include "Data.hpp"
#include "Solution.hpp"

/*! \class ToFile
* \brief Class to represent a \c ToFile.
*
*  This class represents a \c ToFile with all its attributes and methods.
*/

class ToFile
{
public:

	/*!
	*	\brief \e Static method to erase file into folder /res.
	*/
	static void removeFiles();
	/*!
	*	\brief \e Static method to save correlation into a file.
	*
	*	\param[in] data : A \c Data object which contains all the values of the instance.
	*/
    static void saveCorrelation(Data &data);
    /*!
	*	\brief \e Static method to save initial \c Boxes into a file.
	*
	*	\param[in] vectorBox : A vector of initial \c Boxes.
	*	\param[in] data : A \c Data object which contains all the values of the instance.
	*/
    static void saveInitialBoxes(std::vector<Box*> &vectorBox, Data &data);
    /*!
	*	\brief \e Static method to save filtered \c Boxes into a file.
	*
	*	\param[in] vectorBox : A vector of filtered \c Boxes.
	*	\param[in] data : A \c Data object which contains all the values of the instance.
	*/
    static void saveFilteringBoxes(std::vector<Box*> &vectorBox, Data &data);
    /*!
	*	\brief \e Static method to save recomposed \c Boxes into a file.
	*
	*	\param[in] vectorBox : A vector of recomposed \c Boxes.
	*	\param[in] data : A \c Data object which contains all the values of the instance.
	*/
    static void saveReconstructionBoxes(std::vector<Box*> &vectorBox, Data &data);
    /*!
	*	\brief \e Static method to save solutions into a file.
	*
	*	\param[in] lsol : A list of \c Sotuions.
	*	\param[in] data : A \c Data object which contains all the values of the instance.
	*/
    static void saveYN(std::list<Solution> &lsol, Data &data);
};

#endif
