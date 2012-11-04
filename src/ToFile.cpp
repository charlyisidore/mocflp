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
 
#include "ToFile.hpp"

void ToFile::removeFiles()
{
	if(modeExport)
	{
		struct dirent *lecture;	
		DIR *repertoire;
		if( opendir("res") != 0 )
		{
			repertoire = opendir("res");
			while((lecture = readdir(repertoire)))
			{
				if(lecture->d_name[0] != '.')
				{	
					string plot;
					plot += "res/";
					plot += lecture->d_name;
					if( remove(plot.c_str()) == 0 )
					{
				 	}
				}
			}
		}
	}
}


void ToFile::saveCorrelation(Data &data)
{
	if(modeExport)
	{
		//Opening output file
		string temp("res/");
		temp += data.getFileName();
		temp += "_Correlation.out";
		ofstream oFile(temp.c_str(), ios::out | ios::trunc);
		
		if(oFile)
		{
			for(unsigned int i = 0; i < data.getnbCustomer(); i++)
			{
				for(unsigned int j = 0; j < data.getnbFacility(); j++)
				{
					oFile << data.getAllocationObj1Cost(i,j) << " " << data.getAllocationObj2Cost(i,j) << endl;
				}
			}
		    
		}
		else
		{
			cerr << "Unable to open the file !" << endl;
		}
		
		//Close the file
		oFile.close();
	}
}

void ToFile::saveInitialBoxes(vector<Box*> &vectorBox, Data &data)
{
	if(modeExport)
	{
		//Opening output file
		string temp("res/");
		temp += data.getFileName();
		temp += "_InitialBoxes.out";
		ofstream oFile(temp.c_str(), ios::out | ios::trunc);
		
		if(oFile)
		{
			for(unsigned int it = 0; it < vectorBox.size(); it++)
			{
				oFile << vectorBox[it]->getMinZ1() << "\t" << vectorBox[it]->getMinZ2() << "\t" << vectorBox[it]->getMaxZ1() << "\t" << vectorBox[it]->getMaxZ2() << endl;
			}
		}
		else
		{
			cerr << "Unable to open the file !" << endl;
		}
		
		//Close the file
		oFile.close();
	}
}

void ToFile::saveFilteringBoxes(vector<Box*> &vectorBox, Data &data)
{
	if(modeExport)
	{
		//Opening output file
		string temp("res/");
		temp += data.getFileName();
		temp += "_FilteringBoxes.out";
		ofstream oFile(temp.c_str(), ios::out | ios::trunc);
		
		if(oFile)
		{
			for(unsigned int it = 0; it < vectorBox.size(); it++)
			{
				oFile << vectorBox[it]->getMinZ1() << "\t" << vectorBox[it]->getMinZ2() << "\t" << vectorBox[it]->getMaxZ1() << "\t" << vectorBox[it]->getMaxZ2() << endl;
			}
		}
		else
		{
			cerr << "Unable to open the file !" << endl;
		}
		
		//Close the file
		oFile.close();
	}
}

void ToFile::saveReconstructionBoxes(vector<Box*> &vectorBox, Data &data)
{
	if(modeExport)
	{
		//Opening output file
		string temp("res/");
		temp += data.getFileName();
		temp += "_ReconstructionBoxes.out";
		ofstream oFile(temp.c_str(), ios::out | ios::trunc);
		
		if(oFile)
		{
			for(unsigned int it = 0; it < vectorBox.size(); it++)
			{
				oFile << vectorBox[it]->getMinZ1() << "\t" << vectorBox[it]->getMinZ2() << "\t" << vectorBox[it]->getMaxZ1() << "\t" << vectorBox[it]->getMaxZ2() << endl;
			}
		}
		else
		{
			cerr << "Unable to open the file !" << endl;
		}
		
		//Close the file
		oFile.close();
	}
}

void ToFile::saveYN(list<Solution> &lsol, Data &data)
{
	if(modeExport)
	{
		//Opening output file
		string temp("res/");
		temp += data.getFileName();
		temp += "_YN.out";
		ofstream oFile(temp.c_str(), ios::out | ios::trunc);
		
		list<Solution>::iterator iter;
		
		if(oFile)
		{
			for(iter = lsol.begin(); iter != lsol.end(); iter++)
			{
				oFile << fixed << setprecision(0) << (*iter).getObj1() << "\t" << (*iter).getObj2() << endl;
			}
		}
		else
		{
			cerr << "Unable to open the file !" << endl;
		}
		
		//Close the file
		oFile.close();
	}
}
