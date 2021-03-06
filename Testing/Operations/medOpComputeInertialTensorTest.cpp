/*=========================================================================

 Program: MAF2Medical
 Module: medOpComputeInertialTensorTest
 Authors: Simone Brazzale , Stefano Perticoni
 
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

#include "medOpComputeInertialTensorTest.h"
#include "medOpComputeInertialTensor.h"

#include "mafVMESurface.h"
#include "mafVMEGroup.h"
#include "vtkMAFSmartPointer.h"
#include "mafSmartPointer.h"
#include "medOpImporterVTK.h"
#include "mafTagItem.h"
#include "mafTagArray.h"

#include <string>
#include <assert.h>

//-----------------------------------------------------------
void medOpComputeInertialTensorTest::TestDynamicAllocation() 
//-----------------------------------------------------------
{
  medOpComputeInertialTensor *create=new medOpComputeInertialTensor();
  mafDEL(create);
}
//-----------------------------------------------------------
void medOpComputeInertialTensorTest::TestCopy() 
//-----------------------------------------------------------
{
  medOpComputeInertialTensor *create=new medOpComputeInertialTensor();
  medOpComputeInertialTensor *create2 = medOpComputeInertialTensor::SafeDownCast(create->Copy());

  CPPUNIT_ASSERT(create2 != NULL);

  mafDEL(create2);
  mafDEL(create);
}
//-----------------------------------------------------------
void medOpComputeInertialTensorTest::TestAccept() 
//-----------------------------------------------------------
{
  mafSmartPointer<mafVMESurface> volume;
  mafSmartPointer<mafVMEGroup> group;

  medOpComputeInertialTensor *create=new medOpComputeInertialTensor();
  CPPUNIT_ASSERT(create->Accept(volume));
  CPPUNIT_ASSERT(create->Accept(group));
  CPPUNIT_ASSERT(!create->Accept(NULL));

  mafDEL(create);
}
//-----------------------------------------------------------
void medOpComputeInertialTensorTest::TestAddAttributes() 
//-----------------------------------------------------------
{
  // import VTK  
  medOpImporterVTK *importer=new medOpImporterVTK("importerVTK");
  importer->TestModeOn();
  mafString fileName=MED_DATA_ROOT;
  fileName<<"/Surface/sphere.vtk";
  importer->SetFileName(fileName);
  importer->ImportVTK();
  mafVMESurface *surface=mafVMESurface::SafeDownCast(importer->GetOutput());
  
  CPPUNIT_ASSERT(surface!=NULL);
  CPPUNIT_ASSERT(surface->GetOutput()->GetVTKData()!=NULL);

  medOpComputeInertialTensor *op=new medOpComputeInertialTensor();
  op->TestModeOn();
  op->SetInput(surface);
  op->ComputeInertialTensor(surface);
  op->AddAttributes();

  // test attributes
  CPPUNIT_ASSERT(surface->GetTagArray()->IsTagPresent("PRINCIPAL_INERTIAL_TENSOR_COMPONENTS"));
  CPPUNIT_ASSERT(surface->GetTagArray()->IsTagPresent("SURFACE_MASS"));

  mafDEL(op);
  mafDEL(importer);

  delete wxLog::SetActiveTarget(NULL);
}
//-----------------------------------------------------------
void medOpComputeInertialTensorTest::TestComputeInertialTensorFromDefaultValue() 
//-----------------------------------------------------------
{
  // import VTK  
  medOpImporterVTK *importer=new medOpImporterVTK("importerVTK");
  importer->TestModeOn();
  mafString fileName=MED_DATA_ROOT;
  fileName<<"/Surface/sphere.vtk";
  importer->SetFileName(fileName);
  importer->ImportVTK();
  mafVMESurface *surface=mafVMESurface::SafeDownCast(importer->GetOutput());
  
  CPPUNIT_ASSERT(surface!=NULL);
  CPPUNIT_ASSERT(surface->GetOutput()->GetVTKData()!=NULL);

  // test on surface
  medOpComputeInertialTensor *op=new medOpComputeInertialTensor();
  op->TestModeOn();
  op->SetInput(surface);
  op->ComputeInertialTensor(surface);
  op->AddAttributes();

  double tagValue = -1;

  mafTagItem tag;
  surface->GetTagArray()->GetTag("PRINCIPAL_INERTIAL_TENSOR_COMPONENTS",tag);

  tagValue = tag.GetValueAsDouble(0);

  CPPUNIT_ASSERT(tag.GetNumberOfComponents()==3);
  
  tagValue = tag.GetValueAsDouble(0);
  CPPUNIT_ASSERT(46.147349667471083 == tagValue);

  tagValue = tag.GetValueAsDouble(1);
  CPPUNIT_ASSERT(46.147348793016427 == tagValue);

  tagValue = tag.GetValueAsDouble(2);
  CPPUNIT_ASSERT(44.637590473208157 == tagValue);

  surface->GetTagArray()->GetTag("SURFACE_MASS",tag);

  CPPUNIT_ASSERT(tag.GetNumberOfComponents()==1);
  
  tagValue = tag.GetValueAsDouble(0);

  CPPUNIT_ASSERT(30.403272694462050 == tagValue);

  mafDEL(op);
  mafDEL(importer);

  delete wxLog::SetActiveTarget(NULL);
}
//-----------------------------------------------------------
void medOpComputeInertialTensorTest::TestComputeInertialTensorFromGroupFromDefaultValue() 
//-----------------------------------------------------------
{
  // import VTK  
  medOpImporterVTK *importer=new medOpImporterVTK("importerVTK");
  importer->TestModeOn();
  mafString fileName=MED_DATA_ROOT;
  fileName<<"/Surface/sphere.vtk";
  importer->SetFileName(fileName);
  importer->ImportVTK();
  mafVMESurface *surface=mafVMESurface::SafeDownCast(importer->GetOutput());
  
  CPPUNIT_ASSERT(surface!=NULL);
  CPPUNIT_ASSERT(surface->GetOutput()->GetVTKData()!=NULL);

  // create surface copy
  mafVMESurface* copy;
  mafNEW(copy);
  copy->DeepCopy(surface);

  // create group
  mafVMEGroup* group;
  mafNEW(group);
  group->AddChild(surface);
  group->AddChild(copy);

  // test on group
  medOpComputeInertialTensor *op=new medOpComputeInertialTensor();
  op->TestModeOn();
  op->SetInput(group);
  op->ComputeInertialTensorFromGroup();
  op->AddAttributes();

  mafTagItem tag;
  group->GetTagArray()->GetTag("PRINCIPAL_INERTIAL_TENSOR_COMPONENTS",tag);
  CPPUNIT_ASSERT(tag.GetNumberOfComponents()==3);
  group->GetTagArray()->GetTag("SURFACE_MASS",tag);
  CPPUNIT_ASSERT(tag.GetNumberOfComponents()==1);

  double tagValue = -1;

  group->GetTagArray()->GetTag("PRINCIPAL_INERTIAL_TENSOR_COMPONENTS",tag);

  tagValue = tag.GetValueAsDouble(0);

  CPPUNIT_ASSERT(tag.GetNumberOfComponents()==3);

  tagValue = tag.GetValueAsDouble(0);
  CPPUNIT_ASSERT(92.294699334942152 == tagValue);

  tagValue = tag.GetValueAsDouble(1);
  CPPUNIT_ASSERT(92.294697586032868 == tagValue);

  tagValue = tag.GetValueAsDouble(2);
  CPPUNIT_ASSERT(89.275180946416327 == tagValue);

  group->GetTagArray()->GetTag("SURFACE_MASS",tag);

  CPPUNIT_ASSERT(tag.GetNumberOfComponents()==1);

  tagValue = tag.GetValueAsDouble(0);

  CPPUNIT_ASSERT(60.806545388924100 == tagValue);

  mafNode *child0 = NULL;
  child0 = group->GetChild(0);
  CPPUNIT_ASSERT(child0 != NULL);

  CPPUNIT_ASSERT( child0->GetTagArray()->GetTag("SURFACE_MASS") != NULL);

  child0->GetTagArray()->GetTag("SURFACE_MASS",tag);
  CPPUNIT_ASSERT(tag.GetNumberOfComponents()==1);

  CPPUNIT_ASSERT( child0->GetTagArray()->GetTag("PRINCIPAL_INERTIAL_TENSOR_COMPONENTS") == NULL);

  mafNode *child1 = NULL;
  child1 = group->GetChild(1);
  CPPUNIT_ASSERT(child1 != NULL);

  tagValue = tag.GetValueAsDouble(0);
  CPPUNIT_ASSERT(30.403272694462050 == tagValue);

  CPPUNIT_ASSERT( child1->GetTagArray()->GetTag("SURFACE_MASS") != NULL);

  child1->GetTagArray()->GetTag("SURFACE_MASS",tag);
  CPPUNIT_ASSERT(tag.GetNumberOfComponents()==1);

  tagValue = tag.GetValueAsDouble(0);
  CPPUNIT_ASSERT(30.403272694462050 == tagValue);

  CPPUNIT_ASSERT( child1->GetTagArray()->GetTag("PRINCIPAL_INERTIAL_TENSOR_COMPONENTS") == NULL);
  
  mafDEL(op);
  mafDEL(group);
  mafDEL(copy);
  mafDEL(importer);

  delete wxLog::SetActiveTarget(NULL);
}
//-----------------------------------------------------------
void medOpComputeInertialTensorTest::TestOpDoUndo() 
//-----------------------------------------------------------
{ 
  // import VTK  
  medOpImporterVTK *importer=new medOpImporterVTK("importerVTK");
  importer->TestModeOn();
  mafString fileName=MED_DATA_ROOT;
  fileName<<"/Surface/sphere.vtk";
  importer->SetFileName(fileName);
  importer->ImportVTK();
  mafVMESurface *surface=mafVMESurface::SafeDownCast(importer->GetOutput());
  
  CPPUNIT_ASSERT(surface!=NULL);
  CPPUNIT_ASSERT(surface->GetOutput()->GetVTKData()!=NULL);

  medOpComputeInertialTensor *op=new medOpComputeInertialTensor();
  op->TestModeOn();
  op->SetInput(surface);
  op->ComputeInertialTensor(surface);
  op->AddAttributes();

  op->OpUndo();

  // test undo 
  CPPUNIT_ASSERT(!surface->GetTagArray()->IsTagPresent("PRINCIPAL_INERTIAL_TENSOR_COMPONENTS"));
  CPPUNIT_ASSERT(!surface->GetTagArray()->IsTagPresent("INERTIAL_TENSOR_COMPONENTS"));
  CPPUNIT_ASSERT(!surface->GetTagArray()->IsTagPresent("SURFACE_MASS"));

  op->OpDo();

  // test do
  CPPUNIT_ASSERT(surface->GetTagArray()->IsTagPresent("PRINCIPAL_INERTIAL_TENSOR_COMPONENTS"));
  CPPUNIT_ASSERT(surface->GetTagArray()->IsTagPresent("INERTIAL_TENSOR_COMPONENTS"));
  CPPUNIT_ASSERT(surface->GetTagArray()->IsTagPresent("SURFACE_MASS"));

  mafDEL(op);
  mafDEL(importer);

  delete wxLog::SetActiveTarget(NULL);
}

//-----------------------------------------------------------
void medOpComputeInertialTensorTest::TestComputeInertialTensorFromDENSITYTag() 
	//-----------------------------------------------------------
{
	// import VTK  
	medOpImporterVTK *importer=new medOpImporterVTK("importerVTK");
	importer->TestModeOn();
	mafString fileName=MED_DATA_ROOT;
	fileName<<"/Surface/sphere.vtk";
	importer->SetFileName(fileName);
	importer->ImportVTK();
	mafVMESurface *surface=mafVMESurface::SafeDownCast(importer->GetOutput());

	surface->GetTagArray()->SetTag("DENSITY", "1.0");
	double density = surface->GetTagArray()->GetTag("DENSITY")->GetComponentAsDouble(0);

	CPPUNIT_ASSERT(surface!=NULL);
	CPPUNIT_ASSERT(surface->GetOutput()->GetVTKData()!=NULL);
	CPPUNIT_ASSERT(surface->GetTagArray()->GetTag("DENSITY") != NULL);
	CPPUNIT_ASSERT(density == 1.0);

	// test on surface
	medOpComputeInertialTensor *op=new medOpComputeInertialTensor();
	op->TestModeOn();
	op->SetDefaultDensity(5.0);
	op->SetInput(surface);
	op->ComputeInertialTensor(surface);
	op->AddAttributes();

	double tagValue = -1;

	mafTagItem tag;
	surface->GetTagArray()->GetTag("PRINCIPAL_INERTIAL_TENSOR_COMPONENTS",tag);

	tagValue = tag.GetValueAsDouble(0);

	CPPUNIT_ASSERT(tag.GetNumberOfComponents()==3);

	tagValue = tag.GetValueAsDouble(0);
	CPPUNIT_ASSERT(46.147349667471083 == tagValue);

	tagValue = tag.GetValueAsDouble(1);
	CPPUNIT_ASSERT(46.147348793016427 == tagValue);

	tagValue = tag.GetValueAsDouble(2);
	CPPUNIT_ASSERT(44.637590473208157 == tagValue);

	surface->GetTagArray()->GetTag("SURFACE_MASS",tag);

	CPPUNIT_ASSERT(tag.GetNumberOfComponents()==1);

	tagValue = tag.GetValueAsDouble(0);

	CPPUNIT_ASSERT(30.403272694462050 == tagValue);

	mafDEL(op);
	mafDEL(importer);

	delete wxLog::SetActiveTarget(NULL);
}

//-----------------------------------------------------------
void medOpComputeInertialTensorTest::TestComputeInertialTensorFromGroupFromDENSITYTag() 
//-----------------------------------------------------------
{
	// import VTK  
	medOpImporterVTK *importer=new medOpImporterVTK("importerVTK");
	importer->TestModeOn();
	mafString fileName=MED_DATA_ROOT;
	fileName<<"/Surface/sphere.vtk";
	importer->SetFileName(fileName);
	importer->ImportVTK();
	mafVMESurface *surface=mafVMESurface::SafeDownCast(importer->GetOutput());

	surface->GetTagArray()->SetTag("DENSITY", "1.0");

	CPPUNIT_ASSERT(surface!=NULL);
	CPPUNIT_ASSERT(surface->GetOutput()->GetVTKData()!=NULL);

	// create surface copy
	mafVMESurface* copy;
	mafNEW(copy);
	copy->DeepCopy(surface);

	// create group
	mafVMEGroup* group;
	mafNEW(group);
	group->AddChild(surface);
	group->AddChild(copy);

	// test on group
	medOpComputeInertialTensor *op=new medOpComputeInertialTensor();
	op->TestModeOn();
	op->SetInput(group);
	op->SetDefaultDensity(5.0);
	op->ComputeInertialTensorFromGroup();
	op->AddAttributes();

	mafTagItem tag;
	group->GetTagArray()->GetTag("PRINCIPAL_INERTIAL_TENSOR_COMPONENTS",tag);
	CPPUNIT_ASSERT(tag.GetNumberOfComponents()==3);
	group->GetTagArray()->GetTag("SURFACE_MASS",tag);
	CPPUNIT_ASSERT(tag.GetNumberOfComponents()==1);

	double tagValue = -1;

	group->GetTagArray()->GetTag("PRINCIPAL_INERTIAL_TENSOR_COMPONENTS",tag);

	tagValue = tag.GetValueAsDouble(0);

	CPPUNIT_ASSERT(tag.GetNumberOfComponents()==3);

	tagValue = tag.GetValueAsDouble(0);
	CPPUNIT_ASSERT(92.294699334942152 == tagValue);

	tagValue = tag.GetValueAsDouble(1);
	CPPUNIT_ASSERT(92.294697586032868 == tagValue);

	tagValue = tag.GetValueAsDouble(2);
	CPPUNIT_ASSERT(89.275180946416327 == tagValue);

	group->GetTagArray()->GetTag("SURFACE_MASS",tag);

	CPPUNIT_ASSERT(tag.GetNumberOfComponents()==1);

	tagValue = tag.GetValueAsDouble(0);

	CPPUNIT_ASSERT(60.806545388924100 == tagValue);

	mafDEL(op);
	mafDEL(group);
	mafDEL(copy);
	mafDEL(importer);

	delete wxLog::SetActiveTarget(NULL);
}