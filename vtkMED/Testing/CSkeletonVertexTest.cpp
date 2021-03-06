/*=========================================================================

 Program: MAF2Medical
 Module: CSkeletonVertexTest
 Authors: Eleonora Mambrini
 
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

#include <cppunit/config/SourcePrefix.h>
#include "vtkMath.h"
#include "vtkMEDPolyDataDeformation.h"
#include "CSkeletonVertexTest.h"

//-------------------------------------------------------------------------
void CSkeletonVertexTest::TestDynamicAllocation()
//-------------------------------------------------------------------------
{
  vtkMEDPolyDataDeformation::CSkeletonVertex *v1, *v2;
  double coords[3];
  coords[0] = coords[1] = coords[2] = 0.0;

  v1 = new vtkMEDPolyDataDeformation::CSkeletonVertex();
  v2 = new vtkMEDPolyDataDeformation::CSkeletonVertex(coords);

  delete v1;
  delete v2;
}
//-------------------------------------------------------------------------
void CSkeletonVertexTest::TestGetDegree()
//-------------------------------------------------------------------------
{
  double coords1[3], coords2[3];
  coords1[0] = coords1[1] = coords1[2] = 0.0;
  coords2[0] = coords2[1] = coords2[2] = 1.0;

  vtkMEDPolyDataDeformation::CSkeletonVertex *vertex1 = new vtkMEDPolyDataDeformation::CSkeletonVertex(coords1);
  vtkMEDPolyDataDeformation::CSkeletonVertex *vertex2 = new vtkMEDPolyDataDeformation::CSkeletonVertex(coords2);

  CPPUNIT_ASSERT(vertex1->GetDegree() == 0);

  vtkMEDPolyDataDeformation::CSkeletonEdge *e1;
  e1 = new vtkMEDPolyDataDeformation::CSkeletonEdge(vertex1, vertex2);

  vertex1->OneRingEdges.push_back(e1);

  CPPUNIT_ASSERT(vertex1->GetDegree() == 1);
  CPPUNIT_ASSERT(vertex2->GetDegree() == 0);

  delete e1;
  delete vertex1;
  delete vertex2;
}

//-------------------------------------------------------------------------
void CSkeletonVertexTest::TestGetNumberOfJoinedVertices()
//-------------------------------------------------------------------------
{
  double coords1[3], coords2[3];
  coords1[0] = coords1[1] = coords1[2] = 0.0;
  coords2[0] = coords2[1] = coords2[2] = 1.0;

  vtkMEDPolyDataDeformation::CSkeletonVertex *vertex1 = new vtkMEDPolyDataDeformation::CSkeletonVertex(coords1);
  vtkMEDPolyDataDeformation::CSkeletonVertex *vertex2 = new vtkMEDPolyDataDeformation::CSkeletonVertex(coords2);

  CPPUNIT_ASSERT(vertex1->GetNumberOfJoinedVertices() == 0);
  CPPUNIT_ASSERT(vertex2->GetNumberOfJoinedVertices() == 0);

  vertex1->JoinedVertices.push_back(vertex2);

  CPPUNIT_ASSERT(vertex1->GetNumberOfJoinedVertices() == 1);
  CPPUNIT_ASSERT(vertex2->GetNumberOfJoinedVertices() == 0);


  delete vertex1;
  delete vertex2;
}

//-------------------------------------------------------------------------
void CSkeletonVertexTest::TestIsInPositiveHalfspace()
//-------------------------------------------------------------------------
{
  double coords1[3], coords2[3];
  coords1[0] = coords1[1] = coords1[2] = 0.0;
  coords2[0] = coords2[1] = coords2[2] = 1.0;

  vtkMEDPolyDataDeformation::CSkeletonVertex *vertex1 = new vtkMEDPolyDataDeformation::CSkeletonVertex(coords1);

  CPPUNIT_ASSERT(vertex1->IsInPositiveHalfspace(coords1));
  CPPUNIT_ASSERT(vertex1->IsInPositiveHalfspace(coords2));

  delete vertex1;
}