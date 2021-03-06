/*=========================================================================

 Program: MAF2Medical
 Module: medWizardBlockInformationTest
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

#include "medWizardBlockInformationTest.h"
#include "medWizardBlockInformation.h"
#include <math.h>
// TEST

//----------------------------------------------------------------------------
void medWizardBlockInformationTest::TestSetterGetter()
//----------------------------------------------------------------------------
{
  medWizardBlockInformation block=medWizardBlockInformation("test");

  block.SetWindowTitle("WindowTitle");

  CPPUNIT_ASSERT(block.GetWindowTitle()=="WindowTitle");

  block.SetDescription("description");

  CPPUNIT_ASSERT(block.GetDescription()=="description");

  block.SetImage("imageFilename");

  CPPUNIT_ASSERT(block.GetImage()=="imageFilename");

  block.SetBoxLabel("boxLabel");

  CPPUNIT_ASSERT(block.GetBoxLabel()=="boxLabel");
    
  block.SetImagePositionToTop();

  CPPUNIT_ASSERT(block.GetImagePositionToTop()==true);
  CPPUNIT_ASSERT(block.GetImagePositionToLeft()==false);

  block.SetImagePositionToLeft();

  CPPUNIT_ASSERT(block.GetImagePositionToLeft()==true);
  CPPUNIT_ASSERT(block.GetImagePositionToTop()==false);

}

//----------------------------------------------------------------------------
void medWizardBlockInformationTest::TestDynamicAllocation()
//----------------------------------------------------------------------------
{
  medWizardBlockInformation *block;

  block = new medWizardBlockInformation("block");

  delete  block;
}

