/*=========================================================================

 Program: MAF2Medical
 Module: medResultQueryAbstractHandlerTest
 Authors: Daniele Giunchi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __CPP_UNIT_medResultQueryAbstractHandlerTest_H__
#define __CPP_UNIT_medResultQueryAbstractHandlerTest_H__

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/TestFixture.h>

//----------------------------------------------------------------------------
// forward declarations :
//----------------------------------------------------------------------------
class vtkPolyData;

/** 
class name: medResultQueryAbstractHandlerTest
  Test class for medResultQueryAbstractHandler
*/
class medResultQueryAbstractHandlerTest : public CPPUNIT_NS::TestFixture
{
public:

	/** CPPUNIT fixture: executed before each test */
	void setUp();

	/** CPPUNIT fixture: executed after each test */
	void tearDown();

  /** Start test suite macro */
	CPPUNIT_TEST_SUITE( medResultQueryAbstractHandlerTest );
  /** macro for test TestDynamicAllocation */
	CPPUNIT_TEST( TestDynamicAllocation );
  /** macro for test TestStaticAllocation */
	CPPUNIT_TEST( TestStaticAllocation );

  /** macro for test TestStaticAllocation */
  CPPUNIT_TEST( TestGetResultAsStringMatrix );

  /** macro for test TestStaticAllocation */
  CPPUNIT_TEST( TestGetColumnsTypeInformationAsStringVector );

  /** macro for test TestStaticAllocation */
  CPPUNIT_TEST( TestGetColumnsNameInformationAsStringVector );

  /** macro for test TestStaticAllocation */
  CPPUNIT_TEST( TestGetResultAsObjectsMatrix );

  /** macro for test TestStaticAllocation */
  CPPUNIT_TEST( TestGetNumberOfRecords );

  /** macro for test TestStaticAllocation */
  CPPUNIT_TEST( TestGetNumberOfFields );

  /** End test suite macro */
	CPPUNIT_TEST_SUITE_END();

protected:

  /** Dynamic allocation test */
	void TestDynamicAllocation();
  /** Static allocation test */
	void TestStaticAllocation();

  /**Get result as string matrix */
  void TestGetResultAsStringMatrix();
  /** Get column type as list of string */
  void TestGetColumnsTypeInformationAsStringVector();
  /** Get column name as list of string */
  void TestGetColumnsNameInformationAsStringVector();
  /** Get result as string matrix */
  void TestGetResultAsObjectsMatrix();
  /** Get number of records (rows) */
  void TestGetNumberOfRecords();
  /** Get number of fields (columns) */
  void TestGetNumberOfFields();

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
	runner.addTest( medResultQueryAbstractHandlerTest::suite());
	runner.run( controller );

	// Print test in a compiler compatible format.
	CPPUNIT_NS::CompilerOutputter outputter( &result, CPPUNIT_NS::stdCOut() );
	outputter.write(); 

	return result.wasSuccessful() ? 0 : 1;
}

#endif
