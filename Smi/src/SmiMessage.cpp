// Copyright (C) 2000, International Business Machines
// Corporation and others.  All Rights Reserved.

#include "CoinPragma.hpp"
#include "SmiMessage.hpp"
#include <cstring>
/// Structure for use by SmiMessage.cpp
typedef struct {
  SMI_Message internalNumber;
  int externalNumber; // or continuation
  char detail;
  const char * message;
} Smi_message;
static Smi_message smi_us_english[]=
{
  {SMI_SCENARIO_FINISHED,0,1,"Generated %d scenarios"},
  {SMI_DUMMY_END,999999,0,""}
};

/* Constructor */
SmiMessage::SmiMessage(Language language) :
  CoinMessages(sizeof(smi_us_english)/sizeof(Smi_message))
{
  language_=language;
  strcpy(source_,"Smi");
  class_ = 1; //solver
  Smi_message * message = smi_us_english;

  while (message->internalNumber!=SMI_DUMMY_END) {
     CoinOneMessage oneMessage(message->externalNumber,message->detail,
			       message->message);
     addMessage(message->internalNumber,oneMessage);
     message ++;
}

  // now override any language ones

  switch (language) {
	case 0:
    default:
    message=NULL;
    break;
  }

  // replace if any found
  if (message) {
    while (message->internalNumber!=SMI_DUMMY_END) {
      replaceMessage(message->internalNumber,message->message);
      message ++;
    }
  }
}
