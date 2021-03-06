/*=========================================================================

 Program: MAF2Medical
 Module: vtkMAFMeshCutter_BESTest
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_vtkMAFMeshCutter_BEStest_H__
#define __CPP_UNIT_vtkMAFMeshCuttert_BESest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/TestFixture.h>


//------------------------------------------------------------------------------
// Test class for vtkMAFMeshCutter_BES
//------------------------------------------------------------------------------
class vtkMAFMeshCutter_BESTest : public CPPUNIT_NS::TestFixture
{
  public:
  
    // CPPUNIT fixture: executed before each test
    void setUp();

    // CPPUNIT fixture: executed after each test
    void tearDown();

    CPPUNIT_TEST_SUITE( vtkMAFMeshCutter_BESTest );
    CPPUNIT_TEST( TestFixture );
    CPPUNIT_TEST( TestGetOutputHex8 );
    CPPUNIT_TEST( TestGetOutputHex8_VerticalCut1 );
    CPPUNIT_TEST( TestGetOutputHex8_VerticalCut2 );
    CPPUNIT_TEST( TestGetOutputHex8_FaceInPlane );
    CPPUNIT_TEST( TestGetOutputHex8_IncludesEdge1 );
    CPPUNIT_TEST( TestGetOutputHex8_IncludesEdge2 );
    CPPUNIT_TEST( TestGetOutputHex8_IncludesCorner );
    CPPUNIT_TEST( TestGetOutputHex8_EdgeOnly );
    CPPUNIT_TEST( TestGetOutputHex8_CornerOnly );
    CPPUNIT_TEST( TestGetOutputTet4 );
    CPPUNIT_TEST( TestUpdateChangeCutFunction );
    CPPUNIT_TEST( TestUpdateChangeInput );
    CPPUNIT_TEST( TestWait ) ;
    CPPUNIT_TEST_SUITE_END();

  protected:

    void Test();
    void TestFixture();
    void TestGetOutputHex8();
    void TestGetOutputHex8_VerticalCut1();
    void TestGetOutputHex8_VerticalCut2();
    void TestGetOutputHex8_FaceInPlane();
    void TestGetOutputHex8_IncludesEdge1();
    void TestGetOutputHex8_IncludesEdge2();
    void TestGetOutputHex8_IncludesCorner();
    void TestGetOutputHex8_EdgeOnly();
    void TestGetOutputHex8_CornerOnly();
    void TestGetOutputTet4();
    void TestUpdateChangeCutFunction();
    void TestUpdateChangeInput() ;
    void TestWait() {Sleep(5000);} ; // empty test to generate pause

    // return true if v = (1-lambda)*v0 + lambda*v1
    bool ColinearVectors(double *v, double *v0, double *v1, double lambda, double ftol) ;

    // return true if point is in the plane
    bool PointInPlane(double *v, double *origin, double *norm, double ftol) ;

    // are two points equal
    bool PointsEqual(double *v, double *v0, double ftol) ;

    // render input and output data
    void RenderPointScalars(vtkUnstructuredGrid *UG, vtkPolyData *polydata) ;
    void RenderCellScalars(vtkUnstructuredGrid *UG, vtkPolyData *polydata) ;

    // test the interpolation of scalars
    void ScalarTest(vtkMAFMeshCutter_BES *MMC, vtkUnstructuredGrid *UG, vtkPolyData *polydata) ;
};


int 
main( int argc, char* argv[] )
{

  // Create the event manager and test controller
  CPPUNIT_NS::TestResult controller;

  // Add a listener that colllects test result
  CPPUNIT_NS::TestResultCollector result;
  controller.addListener( &result );        

  // Add a listener that print dots as test run.
  CPPUNIT_NS::BriefTestProgressListener progress;
  controller.addListener( &progress );      

  // Add the top suite to the test runner
  CPPUNIT_NS::TestRunner runner;
  runner.addTest( vtkMAFMeshCutter_BESTest::suite());
  runner.run( controller );

  // Print test in a compiler compatible format.
  CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
  outputter.write(); 

  return result.wasSuccessful() ? 0 : 1;
}

#endif
