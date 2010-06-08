// Copyright (C) 2003, International Business Machines
// Corporation and others.  All Rights Reserved.

#include "SmiSmpsIO.hpp"
#include <cassert>
#include <cmath>
#include <cfloat>
#include <string>
#include <cstdio>
#include <iostream>

// In CoinUtilsConfig.h the COIN_HAS_ZLIB macro is set (or not)
#include "CoinUtilsConfig.h"

#include "CoinMpsIO.hpp"
#include "CoinMessage.hpp"
#include "CoinError.hpp"


#if 1
const static char *section[] = {
  "", "NAME", "ENDATA", " ", "PERIODS", "SCENARIOS", "INDEPENDENT"," "
};

const static char *smpsType[] = {
	"SC","BL","  ","DISCRETE","ADD","REPLACE",""
};
#endif


//#############################################################################

SmiCoreData *
SmiSmpsIO::readTimeFile(SmiScnModel *smi, const char *c, const char *ext)
{

        CoinFileInput *input = 0;
        int returnCode = dealWithFileName(c,ext,input);
        if (returnCode<0) {
          return NULL;
        } else if (returnCode>0) {
          // delete cardReader_;
          smpsCardReader_ = new SmiSmpsCardReader ( input, this);
        }
	
	smpsCardReader_->readToNextSection();
	if ( smpsCardReader_->whichSection (  ) == COIN_NAME_SECTION ) {
		iftime = true;
		// check problem name and issue warning if needed
		if (strcmp(problemName_,smpsCardReader_->columnName()))
		{
			printf("Warning: Time file name %s does not match problem file name %s\n",
				smpsCardReader_->columnName(),problemName_);
		}
	} else if ( smpsCardReader_->whichSection (  ) == COIN_UNKNOWN_SECTION ) {
		handler_->message(COIN_MPS_BADFILE1,messages_)<<smpsCardReader_->card()
			<<1
			<<fileName_
			<<CoinMessageEol;
/*
#ifdef COIN_HAS_ZLIB
		if (!smpsCardReader_->filePointer()) 
			handler_->message(COIN_MPS_BADFILE2,messages_)<<CoinMessageEol;
		
#endif
*/
		return NULL;
	} else if ( smpsCardReader_->whichSection (  ) != COIN_EOF_SECTION ) {
		// save name of section
		free(problemName_);
		problemName_=strdup(smpsCardReader_->card());
	} else {
		handler_->message(COIN_MPS_EOF,messages_)<<fileName_
			<<CoinMessageEol;
		return NULL;
	}

	if (iftime)
	{
		int rowEnd, rowStart = 0;
		int colEnd, colStart = 0;
		if (	smpsCardReader_->nextSmpsField() != SMI_TIME_SECTION ) // PERIODS card
			return NULL;
	
		if (	smpsCardReader_->nextSmpsField() == SMI_TIME_SECTION ) // first period
		{
		
			cstag_ = new int[this->getNumCols()];
			rstag_ = new int[this->getNumRows()];
			std::string SunStudioNeedsThis = smpsCardReader_->periodName() ;
			periodMap_.insert(make_pair(SunStudioNeedsThis,nstag_));
		}
		else
			return NULL;

		// don't handle the unordered case yet
		if (smpsCardReader_->whichSmpsType() != SMI_TIME_ORDERED_CORE_TYPE)
			return NULL;

		while( smpsCardReader_->nextSmpsField (  ) == SMI_TIME_SECTION ) 
		{
			if (smpsCardReader_->whichSmpsType() == SMI_TIME_ORDERED_CORE_TYPE)
			{
				
				colEnd = this->columnIndex(smpsCardReader_->columnName());
				rowEnd = this->rowIndex(smpsCardReader_->rowName());
				
				std::string SunStudioNeedsThis = smpsCardReader_->periodName() ;
				periodMap_.insert(make_pair(SunStudioNeedsThis,nstag_+1));
				
				int i;
				for(i=colStart;i<colEnd;++i)
					cstag_[i]=nstag_;
				for(i=rowStart;i<rowEnd;++i)
					rstag_[i]=nstag_;
				++nstag_;
				colStart = colEnd;
				rowStart = rowEnd;
			}
			else
				return NULL;
		}
		
		if ( smpsCardReader_->whichSmpsSection() == SMI_ENDATA_SECTION
			&& smpsCardReader_->whichSmpsType() == SMI_TIME_ORDERED_CORE_TYPE)
		{
			int i;

			for(i=colStart;i<this->getNumCols();++i)
				cstag_[i]=nstag_;
			for(i=rowStart;i<this->getNumRows();++i)
				rstag_[i]=nstag_;
		}
		else
			return NULL;

	}


	SmiCoreData *smiCoreData = new SmiCoreData(this,nstag_+1,cstag_,rstag_);
	return smiCoreData;

}

//#############################################################################

int
SmiSmpsIO::readStochFile(SmiScnModel *smi,SmiCoreData *core, const char *c, const char *ext)
{
	
        CoinFileInput *input = 0;
        int returnCode = dealWithFileName(c,ext,input);
        if (returnCode<0) {
          return -1;
        } else if (returnCode>0) {
          delete smpsCardReader_;
          smpsCardReader_ = new SmiSmpsCardReader ( input, this);
		  if(combineRuleSet)
			  smpsCardReader_->setCoreCombineRule(combineRule_);
        }
	

	smpsCardReader_->readToNextSection();
	if ( smpsCardReader_->whichSection (  ) == COIN_NAME_SECTION ) {
		ifstoch = true;
		// check problem name and issue warning if needed
		if (strcmp(problemName_,smpsCardReader_->columnName()))
		{
			printf("Warning: Time file name %s does not match problem file name %s\n",
				smpsCardReader_->columnName(),problemName_);
		}
	} else if ( smpsCardReader_->whichSection (  ) == COIN_UNKNOWN_SECTION ) {
		handler_->message(COIN_MPS_BADFILE1,messages_)<<smpsCardReader_->card()
			<<1
			<<fileName_
			<<CoinMessageEol;
#ifdef COIN_USE_ZLIB
		if (!smpsCardReader_->filePointer()) 
			handler_->message(COIN_MPS_BADFILE2,messages_)<<CoinMessageEol;
		
#endif
		return -2;
	} else if ( smpsCardReader_->whichSection (  ) != COIN_EOF_SECTION ) {
		// save name of section
		free(problemName_);
		problemName_=strdup(smpsCardReader_->card());
	} else {
		handler_->message(COIN_MPS_EOF,messages_)<<fileName_
			<<CoinMessageEol;
		return -3;
	}

	if (ifstoch)
	{
		switch( smpsCardReader_->nextSmpsField() )
		{
		case SMI_INDEPENDENT_SECTION: // INDEPENDENT card
		{
			printf("Processing INDEPENDENT section\n");

			// Create discrete distribution
			SmiDiscreteDistribution *smiDD = new SmiDiscreteDistribution(core);
			SmiDiscreteRV *smiRV = NULL;

			CoinPackedVector drlo,drup,dclo,dcup,dobj;
			CoinPackedMatrix *matrix = new CoinPackedMatrix(false,0.25,0.25);
			assert(!matrix->isColOrdered());

			int nrow = core->getNumRows();
			int ncol = core->getNumCols();
			matrix->setDimensions(nrow,ncol);

			int oldi=-1;
			int oldj=-1;
					
			while( smpsCardReader_->nextSmpsField (  ) == SMI_INDEPENDENT_SECTION ) 
			{
				
				switch (smpsCardReader_->whichSmpsType())
				{
				case SMI_BL_CARD:
				{
					//this is a block 
					// not handled yet
					return -2;
				}
				case SMI_COLUMN_CARD:
				{
					int j=this->columnIndex(smpsCardReader_->columnName());
					int i=this->rowIndex(smpsCardReader_->rowName());

					double value = smpsCardReader_->value();

					
					if (j<0) // check RHS
					{
						if (!strcmp(this->getRhsName(),"")) {
							 free(rhsName_);
		 					 rhsName_=strdup(smpsCardReader_->columnName());
						} else
							assert(!strcmp(smpsCardReader_->columnName(),this->getRhsName()));
						assert(!(i<0));

						char c=this->getRowSense()[i];

						switch(c)
						{
						case 'E':
							drlo.insert(i,value);
							drup.insert(i,value);
							break;
						case 'L':
							drlo.insert(i,value);
							break;
						case 'G':
							drup.insert(i,value);
							break;
						default:
							assert(!"bad row sense: shouldn't get here");
							break;
						}
					}
					else if(i<0 || i==getNumRows()) // check OBJ
					{
						assert(!strcmp(smpsCardReader_->rowName(),this->getObjectiveName()));
						dobj.insert(j,value);
					}
					else	// add element
					{
						matrix->modifyCoefficient(i,j,value);
					}
					if ( !( (oldi==i)&&(oldj==j) ) ) // this is a new RV
					{
						oldi=i;
						oldj=j;
						// store the old one
						if (smiRV != NULL) 
						{
							smiDD->addDiscreteRV(smiRV);
						}
						// create a new one -- row stage dominates unless is column bound
						if (i>0)
							smiRV = new SmiDiscreteRV(core->getRowStage(i));
						else
							smiRV = new SmiDiscreteRV(core->getColStage(j));
						smiRV->addEvent(*matrix,dclo,dcup,dobj,drlo,drup,smpsCardReader_->getProb());
						matrix->clear();
						dclo.clear();
						dcup.clear();
						dobj.clear();
						drlo.clear();
						drup.clear();

					}
					else
					{
						smiRV->addEvent(*matrix,dclo,dcup,dobj,drlo,drup,smpsCardReader_->getProb());
						matrix->clear();
						dclo.clear();
						dcup.clear();
						dobj.clear();
						drlo.clear();
						drup.clear();
					}




				}
				break;
				case SMI_UNKNOWN_MPS_TYPE:
				default:
					return -2;
				}
			}
			
			if (smpsCardReader_->whichSmpsSection() == SMI_ENDATA_SECTION)
			{
				//process discrete distribution
				smi->processDiscreteDistributionIntoScenarios(smiDD);
				return 0;
			}
			else
				return -2;
			

		}

		case SMI_SCENARIOS_SECTION: // SCENARIOS card
		{
			double prob=0.0;
			int scen=0,anc=0;
			int branch=0;
			CoinPackedVector drlo,drup,dclo,dcup,dobj;
			CoinPackedMatrix *matrix = new CoinPackedMatrix(false,0.25,0.25);
			assert(!matrix->isColOrdered());

			int nrow = core->getNumRows();
			int ncol = core->getNumCols();
			matrix->setDimensions(nrow,ncol);

			while( smpsCardReader_->nextSmpsField (  ) == SMI_SCENARIOS_SECTION ) 
			{
				if (smpsCardReader_->whichSmpsType() == SMI_SC_CARD)
				{
					if (scen)
					{
						
						smi->generateScenario(core,matrix,&dclo,&dcup,&dobj,&drlo,&drup,branch,anc,prob,smpsCardReader_->getCoreCombineRule() );
						matrix->clear();
						dclo.clear();
						dcup.clear();
						dobj.clear();
						drlo.clear();
						drup.clear();
						
						matrix->setDimensions(nrow,ncol);

					}

					std::string SunStudioNeedsThis = smpsCardReader_->scenarioNew() ;
					scenarioMap_.insert(make_pair(SunStudioNeedsThis,scen++));
					prob = smpsCardReader_->value();
					if (!strncmp(smpsCardReader_->scenarioAnc(),"ROOT",4))
						anc = 0;
					else
						anc = scenarioMap_[smpsCardReader_->scenarioAnc()];
					branch = periodMap_[smpsCardReader_->periodName()];
				}
				else if(smpsCardReader_->whichSmpsType() == SMI_COLUMN_CARD)
				{
					int j=this->columnIndex(smpsCardReader_->columnName());
					int i=this->rowIndex(smpsCardReader_->rowName());
					double value = smpsCardReader_->value();

					
					if (j<0) // check RHS
					{
						if (!strcmp(this->getRhsName(),"")) {
							 free(rhsName_);
		 					 rhsName_=strdup(smpsCardReader_->columnName());
						} else
							assert(!strcmp(smpsCardReader_->columnName(),this->getRhsName()));
						assert(!(i<0));

						char c=this->getRowSense()[i];

						switch(c)
						{
						case 'E':
							drlo.insert(i,value);
							drup.insert(i,value);
							break;
						case 'L':
							//drlo.insert(i,value);
							drup.insert(i,value);
							break;
						case 'G':
							//drup.insert(i,value);
							drlo.insert(i,value);
							break;
						default:
							assert(!"bad row sense: shouldn't get here");
							break;
						}
					}
					else if(i<0 || i==getNumRows()) // check OBJ
					{
						assert(!strcmp(smpsCardReader_->rowName(),this->getObjectiveName()));
						dobj.insert(j,value);
					}
					else	// add element
					{
						matrix->modifyCoefficient(i,j,value);
					}
				}
				
				else // unknown type
					return -1;

			} // while

			if (smpsCardReader_->whichSmpsSection() == SMI_ENDATA_SECTION)
				{
					if (scen)
					{
						
						smi->generateScenario(core,matrix,&dclo,&dcup,&dobj,&drlo,&drup,branch,anc,prob,smpsCardReader_->getCoreCombineRule() );
						
					}
					delete matrix;
					return 0;
				}

		} 

		default:// didn't recognize section
		return -1;
		}

		

	}



	return 0;
}

//#############################################################################
//  nextSmpsField


SmiSectionType
SmiSmpsCardReader::nextSmpsField (  )
{
  
  // find next non blank character
  char *next = position_;

  static const char *blanks = (const char *) " \t";
  char valstr[COIN_MAX_FIELD_LENGTH],*after;
  
  while ( next != eol_ ) {
    if ( *next == ' ' || *next == '\t' ) {
      next++;
    } else {
      break;
    }
  }

  bool gotCard;
  
  if ( next == eol_ ) {
	  gotCard = false;
  } else {
	  gotCard = true;
  }
  while ( !gotCard ) 
  {
	  // need new image
	  
	  if ( cleanCard() ) {
		  return SMI_EOF_SECTION;
	  }
	  if ( card_[0] == ' ' ) {
		  // not a section or comment
		  position_ = card_;
		  eol_ = card_ + strlen ( card_ );


		  if ( smiSection_ == SMI_TIME_SECTION ) 
		  {
			  smiSmpsType_ = SMI_TIME_ORDERED_CORE_TYPE;
			  if (!(next = strtok(position_,blanks)))
			  {
				  smiSmpsType_ = SMI_UNKNOWN_MPS_TYPE;
				  return smiSection_;
			  }
			  strcpy(columnName_,next);
			  
			  if (!(next = strtok(NULL,blanks)))
			  {
				  strcpy(periodName_,columnName_);
				  smiSmpsType_ = SMI_TIME_UNORDERED_CORE_TYPE;
				  return smiSection_;
			  }
			  strcpy(rowName_,next);
			  if (!(next = strtok(NULL,blanks)))
			  {
				  smiSmpsType_ = SMI_UNKNOWN_MPS_TYPE;
				  return smiSection_;
			  }
			  strcpy(periodName_,next);
			  position_ = eol_;
			  
		  }

		  if ( smiSection_ == SMI_INDEPENDENT_SECTION )
		  {
			  int i;
			  if (strlen(next)==2)
			  {
				  for (i = SMI_BL_CARD; i < SMI_UNKNOWN_MPS_TYPE; ++i) {
					  if ( !strncmp ( next, smpsType[i], 2 ) ) 
					  {
						  break;
					  }
				  }
			  }
			  else
				  i = SMI_COLUMN_CARD;

			  switch(smiSmpsType_ = (SmiSmpsType) i)
			  {
			  case SMI_BL_CARD: // not handled yet
				  break;
				 
			  case SMI_COLUMN_CARD: // card info has "col,row,value,prob"
				    
				  if (!(next = strtok(position_,blanks)))
				  {
					  smiSmpsType_ = SMI_UNKNOWN_MPS_TYPE;
					  return smiSection_;
				  }
				  //already got the name
				  strcpy(columnName_,next);
				  
				  if (!(next = strtok(NULL,blanks)))
				  {
					  smiSmpsType_ = SMI_UNKNOWN_MPS_TYPE;
					  break;
				  }
				  strcpy(rowName_,next);

				  if (!(next = strtok(NULL,blanks)))
				  {
					  smiSmpsType_ = SMI_UNKNOWN_MPS_TYPE;
					  break;
				  }
				  strcpy(valstr,next);

				  value_ = osi_strtod(valstr,&after,0);
				  // see if error
				  assert(after>valstr);
					  
				  if (!(next = strtok(NULL,blanks)))
				  {
					  smiSmpsType_ = SMI_UNKNOWN_MPS_TYPE;
					  break;
				  }
				  strcpy(valstr,next);

				  prob_ = osi_strtod(valstr,&after,0);
				  // see if error
				  assert(after>valstr);

				  position_ = eol_;  //end of card -- no more information allowed
				  break;

			  case SMI_UNKNOWN_MPS_TYPE:
				  break;
			  default:
				  smiSmpsType_=SMI_UNKNOWN_MPS_TYPE;
			  }//end switch

			  return smiSection_;

		  }

		  if ( smiSection_ == SMI_SCENARIOS_SECTION )
		  {
			

			  
			  if (!(next = strtok(position_,blanks)))
			  {
				  smiSmpsType_ = SMI_UNKNOWN_MPS_TYPE;
				  return smiSection_;
			  }
			
			  int i;
			  if (strlen(next)==2)
			  {
				  for (i = SMI_SC_CARD; i < SMI_UNKNOWN_MPS_TYPE; ++i) {
					  if ( !strncmp ( next, smpsType[i], 2 ) ) 
					  {
						  break;
					  }
				  }
				  if (i==SMI_UNKNOWN_MPS_TYPE)
					  i = SMI_COLUMN_CARD;
			  }
			  else
				  i = SMI_COLUMN_CARD;
			
			  switch(smiSmpsType_ = (SmiSmpsType) i)
			  {
			  case SMI_SC_CARD:
				  if (!(next = strtok(NULL,blanks)))
				  {
					  smiSmpsType_ = SMI_UNKNOWN_MPS_TYPE;
					  break;
				  }
				  strcpy(columnName_,next);
				  
				  if (!(next = strtok(NULL,blanks)))
				  {
					  smiSmpsType_ = SMI_UNKNOWN_MPS_TYPE;
					  break;
				  }
				  strcpy(rowName_,next);

				  if (!(next = strtok(NULL,blanks)))
				  {
					  smiSmpsType_ = SMI_UNKNOWN_MPS_TYPE;
					  break;
				  }
				  strcpy(valstr,next);

				  value_ = osi_strtod(valstr,&after,0);
				  // see if error
				  assert(after>valstr);

				  if (!(next = strtok(NULL,blanks)))
				  {
					  smiSmpsType_ = SMI_UNKNOWN_MPS_TYPE;
					  break;
				  }
				  strcpy(periodName_,next);

				  // no futher strings allowed.
				  position_=eol_;
				  break;
			  case SMI_UNKNOWN_MPS_TYPE:
				  //this in case the column name has two characters!
			  case SMI_COLUMN_CARD:

				  //already got the name
				  strcpy(columnName_,next);
				  
				  if (!(next = strtok(NULL,blanks)))
				  {
					  smiSmpsType_ = SMI_UNKNOWN_MPS_TYPE;
					  break;
				  }
				  strcpy(rowName_,next);

				  if (!(next = strtok(NULL,blanks)))
				  {
					  smiSmpsType_ = SMI_UNKNOWN_MPS_TYPE;
					  break;
				  }
				  strcpy(valstr,next);

				  value_ = osi_strtod(valstr,&after,0);
				  // see if error
				  assert(after>valstr);
				  
				  if (!(next = strtok(NULL,blanks)))
					  position_ = eol_;
				  else
					  // strcpy(next,periodName_); // store it here until next time
					  strcpy(periodName_,next); // store it here until next time
				  break;
			  default:
				  assert(smiSmpsType_ == SMI_UNKNOWN_MPS_TYPE);
				  break;
			  }
		  }
		  
		  return smiSection_;

	  } else if ( card_[0] != '*' ) {
		  // not a comment, might be a section
		  int i;
		  
		  handler_->message(COIN_MPS_LINE,messages_)<<cardNumber_
			  <<card_<<CoinMessageEol;

		  // find the section, if there is one
		  for ( i = SMI_NAME_SECTION; i < SMI_UNKNOWN_SECTION; i++ ) {
			//printf("Comparing first 3 chars of %s with %s returns %d \n",
			//	card_, section[i], strncmp(card_,section[i],3));
			  if ( !strncmp ( card_, section[i], 3) ) {
				  break;
			  }
		  }

		  // didn't find anything so quit
		  if (i==SMI_UNKNOWN_SECTION) return (SmiSectionType) i;

		  position_ = card_;
		  eol_ = card_;
		  smiSection_ = ( SmiSectionType ) i;

		  // if its a scenario card, need to process some more info
		  if ( (smiSection_ == SMI_SCENARIOS_SECTION) || 
				(smiSection_ == SMI_INDEPENDENT_SECTION) )
		  {
			  i = SMI_SMPS_COMBINE_UNKNOWN;
			  next = strtok(position_,blanks);
			  
			  if (!(next = strtok(NULL,blanks)))
			  {
				  smiSmpsType_ = SMI_UNKNOWN_MPS_TYPE;
				  break;
			  }
			  // find the section, if there is one
			  // next card should be DISCRETE
			  if (!(next = strtok(NULL,blanks)))
			  {
				  smiSmpsType_ = SMI_UNKNOWN_MPS_TYPE;
				  break;
			  }
			  // find the section, if there is one
			  
			  for ( i = SMI_SMPS_COMBINE_ADD; i < SMI_SMPS_COMBINE_UNKNOWN; i++ ) {
				  if ( !strncmp ( next, smpsType[i], strlen ( section[i] ) ) ) {
					  break;
				  }
			  }
			  
		   }
			  // set combine rule if it is not already set.
		   if (!combineRuleSet)
		   {
			  switch(i)
			  {
			  case SMI_SMPS_COMBINE_ADD:
				  this->setCoreCombineRule(SmiCoreCombineAdd::Instance());
				  break;
			  case SMI_SMPS_COMBINE_REPLACE:
				  this->setCoreCombineRule(SmiCoreCombineReplace::Instance());
				  break;
			  default:
				  this->setCoreCombineRule(SmiCoreCombineReplace::Instance());
				  // MESSAGE
				  printf(" Smps: setting default core combine rule to Replace\n");
			  }
			  
		  }
		  return smiSection_;
	  } else {
		  // comment
	  }
  }
  
  {
	  // if we get here, there are additional row fields on the line

				  strcpy(rowName_,periodName_); // stored the last field in periodName_
				  
				  if (!(next = strtok(NULL,blanks)))
				  {
					  smiSmpsType_ = SMI_UNKNOWN_MPS_TYPE;
					  return smiSection_;
				  }
				  strcpy(valstr,next);
				  
				  value_ = osi_strtod(valstr,&after,0);
				  // see if error
				  assert(after>valstr);

				  if (!(next = strtok(NULL,blanks)))
					  position_ = eol_;
				  else
					  strcpy(next,periodName_);
				  
				  
				  return smiSection_;
  }
}

