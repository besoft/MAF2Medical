/*=========================================================================

 Program: MAF2Medical
 Module: medWizardSelectionBlock
 Authors: Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "medDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medDecl.h"
#include "medWizardBlockYesNoSelection.h"




//----------------------------------------------------------------------------
medWizardBlockYesNoSelection::medWizardBlockYesNoSelection(const char *name):medWizardBlock(name)
//----------------------------------------------------------------------------
{
  m_CancelEnabled=true;
}

//----------------------------------------------------------------------------
medWizardBlockYesNoSelection::~medWizardBlockYesNoSelection()
//----------------------------------------------------------------------------
{
}

//----------------------------------------------------------------------------
void medWizardBlockYesNoSelection::SetWindowTitle( const char *Title )
//----------------------------------------------------------------------------
{
  //setting the name of the window title
  m_Title=Title;
}


//----------------------------------------------------------------------------
wxString medWizardBlockYesNoSelection::GetNextBlock()
//----------------------------------------------------------------------------
{
  wxString block;
  //Return the next block according on user choice
  if (!m_Success)
    block=m_AbortBlock;
  else if (m_UserYes==wxYES)
    block=m_YesBlock;
  else 
    block=m_NoBlock;
  return block;
}

//----------------------------------------------------------------------------
void medWizardBlockYesNoSelection::SetDescription( const char *description )
//----------------------------------------------------------------------------
{
  //set the description showed to the user
  m_Description=description;
}



//----------------------------------------------------------------------------
void medWizardBlockYesNoSelection::ExcutionBegin()
//----------------------------------------------------------------------------
{
  medWizardBlock::ExcutionBegin();

  //Show Modal window
  if (m_CancelEnabled)
    m_UserYes = wxMessageBox(m_Description,m_Title,wxYES_NO|wxCANCEL|wxYES_DEFAULT|wxICON_QUESTION|wxCENTRE|wxSTAY_ON_TOP);
  else 
    m_UserYes = wxMessageBox(m_Description,m_Title,wxYES_NO|wxYES_DEFAULT|wxICON_QUESTION|wxCENTRE|wxSTAY_ON_TOP);

  //User has pessed cancel
  if (m_UserYes==wxCANCEL)
    Abort();

}

//----------------------------------------------------------------------------
void medWizardBlockYesNoSelection::SetNextBlockOnYes( const char *block )
//----------------------------------------------------------------------------
{
  //set next block on user yes
  m_YesBlock=block;
}

//----------------------------------------------------------------------------
void medWizardBlockYesNoSelection::SetNextBlockOnNo( const char *block )
//----------------------------------------------------------------------------
{
  //set next block on user no
  m_NoBlock=block;
}

//----------------------------------------------------------------------------
void medWizardBlockYesNoSelection::EnableCancelButton( bool cancel/*=true*/ )
//----------------------------------------------------------------------------
{
  //enable/disable cancel button
  m_CancelEnabled=cancel;
}




