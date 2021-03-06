/*=========================================================================

 Program: MAF2Medical
 Module: itkRawMotionImporterUtilityTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h"
#include "medDefines.h"
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "itkRawMotionImporterUtilityTest.h"
#include "itkRawMotionImporterUtility.h"
#include "mafString.h"

#include "vnl/vnl_matrix.h"

#define TEST_RESULT CPPUNIT_ASSERT(m_Result)

//matrix test 
//0.0 0.1 0.2 0.3
//1.0 1.1 1.2 1.3
//2.0 2.1 2.2 2.3
//3.0 3.1 3.2 3.3

//----------------------------------------------------------------------------
void itkRawMotionImporterUtilityTest::TestFixture()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void itkRawMotionImporterUtilityTest::setUp()
//----------------------------------------------------------------------------
{
  m_Result = false;
}
//----------------------------------------------------------------------------
void itkRawMotionImporterUtilityTest::tearDown()
//----------------------------------------------------------------------------
{
}
//-----------------------------------------------------------
void itkRawMotionImporterUtilityTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  itkRawMotionImporterUtility *rMIU=new itkRawMotionImporterUtility();
  delete rMIU;
}
//-----------------------------------------------------------
void itkRawMotionImporterUtilityTest::TestReadMatrix() 
//-----------------------------------------------------------
{
  enum ID_RESULT
  {
    RES_OK = 0,
    RES_ERROR = 1,
  };

  mafString matrixFile = MED_DATA_ROOT;
  matrixFile << "/Matrix/TestMatrix001.txt";
  vnl_matrix<double> mat;
  itkRawMotionImporterUtility *rMIU=new itkRawMotionImporterUtility();
  int res = rMIU->ReadMatrix(mat,matrixFile);

  m_Result = res == RES_OK;
  TEST_RESULT;

  m_Result = mat.columns() == 4 && mat.rows() == 4;
  TEST_RESULT;

  m_Result = mat.get(0,0) == 0.0 && 
             mat.get(0,1) == 0.1 &&
             mat.get(0,2) == 0.2 &&
             mat.get(0,3) == 0.3 &&

             mat.get(1,0) == 1.0 &&
             mat.get(1,1) == 1.1 &&
             mat.get(1,2) == 1.2 &&
             mat.get(1,3) == 1.3 &&

             mat.get(2,0) == 2.0 &&
             mat.get(2,1) == 2.1 &&
             mat.get(2,2) == 2.2 &&
             mat.get(2,3) == 2.3 &&

             mat.get(3,0) == 3.0 &&
             mat.get(3,1) == 3.1 &&
             mat.get(3,2) == 3.2 &&
             mat.get(3,3) == 3.3;
  TEST_RESULT;

  delete rMIU;
}
