/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpSegmentationRegionGrowingLocalAndGlobalThreshold.cpp,v $
Language:  C++
Date:      $Date: 2009-11-13 14:26:35 $
Version:   $Revision: 1.1.2.8 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2009
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)

MafMedical Library use license agreement

The software named MafMedical Library and any accompanying documentation, 
manuals or data (hereafter collectively "SOFTWARE") is property of the SCS s.r.l.
This is an open-source copyright as follows:
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.
* Modified source versions must be plainly marked as such, and must not be misrepresented 
as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MafMedical is partially based on OpenMAF.
=========================================================================*/

#include "medDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medOpSegmentationRegionGrowingLocalAndGlobalThreshold.h"
#include "wx/busyinfo.h"

#include "medDecl.h"
#include "mafGUI.h"
#include "mafNode.h"
#include "mafVMEVolumeGray.h"
#include "mafVMESurface.h"
#include "mafGUILutSlider.h"
#include "mafGUIHistogramWidget.h"
#include "mafGUIDialog.h"
#include "medOpVolumeResample.h"

#include "vtkMAFSmartPointer.h"
#include "vtkMEDRegionGrowingLocalGlobalThreshold.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkStructuredPoints.h"
#include "vtkImageToStructuredPoints.h"
#include "vtkPolyDataConnectivityFilter.h"
#include "vtkImageMedian3D.h"
#include "vtkMAFContourVolumeMapper.h"
#include "vtkMAFHistogram.h"

#include "itkVTKImageToImageFilter.h"
#include "itkImageToVTKImageFilter.h"
#include "itkBinaryErodeImageFilter.h"
#include "itkBinaryDilateImageFilter.h"
#include "itkBinaryBallStructuringElement.h"

const unsigned int Dimension = 3;

typedef unsigned char   InputPixelType;
typedef unsigned char   OutputPixelType;

typedef itk::Image< InputPixelType,  Dimension >   InputImageType;
typedef itk::Image< OutputPixelType, Dimension >   OutputImageType;

typedef itk::BinaryBallStructuringElement< InputPixelType,Dimension  > StructuringElementType;

typedef itk::BinaryDilateImageFilter< InputImageType,OutputImageType,StructuringElementType >  DilateFilterType;
typedef itk::BinaryErodeImageFilter< InputImageType,OutputImageType,StructuringElementType >  ErodeFilterType;

//----------------------------------------------------------------------------
mafCxxTypeMacro(medOpSegmentationRegionGrowingLocalAndGlobalThreshold);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medOpSegmentationRegionGrowingLocalAndGlobalThreshold::medOpSegmentationRegionGrowingLocalAndGlobalThreshold(wxString label) :
mafOp(label)
//----------------------------------------------------------------------------
{
  m_OpType  = OPTYPE_OP;
  m_Canundo = true;
  m_InputPreserving = true;

  m_LowerLabel = -1;
  m_UpperLabel = 1;
  m_SphereRadius = 1;
  m_ApplyConnectivityFilter = FALSE;

  m_VolumeOutputMorpho = NULL;
  m_VolumeOutputRegionGrowing = NULL;
  m_SurfaceOutput = NULL;

  m_SegmentedImage = NULL;
  m_MorphoImage = NULL;

  m_Histogram = NULL;
  m_Dialog = NULL;

  m_ComputedMedianFilter = false;
  
}
//----------------------------------------------------------------------------
medOpSegmentationRegionGrowingLocalAndGlobalThreshold::~medOpSegmentationRegionGrowingLocalAndGlobalThreshold()
//----------------------------------------------------------------------------
{
  mafDEL(m_VolumeOutputMorpho);
  mafDEL(m_VolumeOutputRegionGrowing);
  mafDEL(m_SurfaceOutput);
  vtkDEL(m_SegmentedImage);
  vtkDEL(m_MorphoImage);

  if (m_ComputedMedianFilter)
  {
    mafDEL(m_VolumeInput);
  }

  cppDEL(m_Dialog);
}
//----------------------------------------------------------------------------
mafOp* medOpSegmentationRegionGrowingLocalAndGlobalThreshold::Copy()
//----------------------------------------------------------------------------
{
  /** return a copy of itself, needs to put it into the undo stack */
  return new medOpSegmentationRegionGrowingLocalAndGlobalThreshold(m_Label);
}
//----------------------------------------------------------------------------
bool medOpSegmentationRegionGrowingLocalAndGlobalThreshold::Accept(mafNode* vme)
//----------------------------------------------------------------------------
{
  return vme && vme->IsA("mafVMEVolumeGray");
}
//----------------------------------------------------------------------------
void medOpSegmentationRegionGrowingLocalAndGlobalThreshold::OpRun()
//----------------------------------------------------------------------------
{
  m_VolumeInput = mafVMEVolumeGray::SafeDownCast(m_Input);

  vtkStructuredPoints *sp = vtkStructuredPoints::SafeDownCast(m_VolumeInput->GetOutput()->GetVTKData());
  vtkImageData *im = vtkImageData::SafeDownCast(m_VolumeInput->GetOutput()->GetVTKData());
  if (sp == NULL || im == NULL)
  {
    int answer = wxMessageBox(_("The data will be resampled! Proceed?"),_("Confirm"), wxYES_NO|wxICON_EXCLAMATION , NULL);
    if(answer == wxNO)
    {
      OpStop(OP_RUN_CANCEL);
      return;
    }
    wxBusyInfo wait_info1("Resampling...");
    medOpVolumeResample *op = new medOpVolumeResample();
    op->SetInput(m_VolumeInput);
    op->TestModeOn();
    op->OpRun();
    op->AutoSpacing();
    op->Resample();
    mafVMEVolumeGray *volOut=mafVMEVolumeGray::SafeDownCast(op->GetOutput());
    volOut->GetOutput()->Update();
    volOut->Update();

    mafDEL(op);

    m_VolumeInput=volOut;
  }

  m_VolumeInput->Update();

  int answer = wxMessageBox(_("Would you like to apply median filter to the data?"),_("Confirm"), wxYES_NO|wxICON_EXCLAMATION , NULL);
  if(answer == wxYES)
  {
    m_ComputedMedianFilter = true;

    vtkMAFSmartPointer<vtkImageMedian3D> median;
    median->SetInput(vtkImageData::SafeDownCast(m_VolumeInput->GetOutput()->GetVTKData()));
    median->SetKernelSize(3,3,3);
    median->Update();

    mafVMEVolumeGray *volMediano;
    mafNEW(volMediano);
    vtkDataSet *d = median->GetOutput();
    d->Update();
    int k = d->GetNumberOfPoints();
    double sr[2];
    d->GetPointData()->GetScalars()->GetRange(sr);
    mafString name = m_Input->GetName();
    name<<" - Applied Median Filter";
    volMediano->SetName(name);
    vtkMAFSmartPointer<vtkImageToStructuredPoints> f;
    f->SetInput(median->GetOutput());
    f->Update();
    volMediano->SetData(f->GetOutput(),m_VolumeInput->GetTimeStamp());
    volMediano->ReparentTo(m_VolumeInput);
    volMediano->Update();

    m_VolumeInput = volMediano;
  }

  mafNEW(m_VolumeOutputMorpho);
  mafNEW(m_VolumeOutputRegionGrowing);
  mafNEW(m_SurfaceOutput);

  vtkNEW(m_SegmentedImage);
  vtkNEW(m_MorphoImage);

  CreateGui();
  CreateHistogramDialog();
}
//----------------------------------------------------------------------------
void medOpSegmentationRegionGrowingLocalAndGlobalThreshold::MorphologicalMathematics()
//----------------------------------------------------------------------------
{
  //Perform the morphological closing operation
  wxBusyInfo wait("Please wait, morphological mathematics...");

  mafEventMacro(mafEvent(this,PROGRESSBAR_SHOW));
  typedef itk::VTKImageToImageFilter< InputImageType > ConvertervtkTOitk;
  ConvertervtkTOitk::Pointer vtkTOitk = ConvertervtkTOitk::New();
  vtkTOitk->SetInput( m_SegmentedImage );
  vtkTOitk->Update();
  mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,(long)10));

  //Structuring element is a sphere
  StructuringElementType  structuringElement;
  structuringElement.SetRadius( m_SphereRadius );  // 3x3 structuring element
  structuringElement.CreateStructuringElement(); 

  DilateFilterType::Pointer binaryDilate = DilateFilterType::New();
  binaryDilate->SetKernel( structuringElement );
  binaryDilate->SetInput( vtkTOitk->GetOutput() );
  binaryDilate->SetDilateValue( m_LowerLabel );
  binaryDilate->Update();
  mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,(long)50));

  ErodeFilterType::Pointer  binaryErode  = ErodeFilterType::New();
  binaryErode->SetKernel(  structuringElement );
  binaryErode->SetInput( binaryDilate->GetOutput() );
  binaryErode->SetErodeValue( m_LowerLabel );
  binaryErode->Update();
  mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,(long)90));

  typedef itk::ImageToVTKImageFilter< OutputImageType > ConverteritkTOvtk;
  ConverteritkTOvtk::Pointer itkTOvtk = ConverteritkTOvtk::New();
  itkTOvtk->SetInput( binaryErode->GetOutput() );
  itkTOvtk->Update();
  mafEventMacro(mafEvent(this,PROGRESSBAR_SET_VALUE,(long)100));

  m_MorphoImage->DeepCopy(itkTOvtk->GetOutput());
  m_MorphoImage->Update();

  mafEventMacro(mafEvent(this,PROGRESSBAR_HIDE));

}
//----------------------------------------------------------------------------
void medOpSegmentationRegionGrowingLocalAndGlobalThreshold::RegionGrowing()
//----------------------------------------------------------------------------
{
  wxBusyInfo wait("Please wait, region growing...");
  mafEventMacro(mafEvent(this,PROGRESSBAR_SHOW));

  //Get the vtk data from the input
  vtkImageData *imageData = vtkImageData::SafeDownCast(m_VolumeInput->GetOutput()->GetVTKData());
  imageData->Update();

  //Get the thresholds values selected by the user
  double lower,upper;
  m_Histogram->GetThresholds(&lower,&upper);
  
  //Apply the region growing filter
  mafEventMacro(mafEvent(this,PROGRESSBAR_SHOW));
  vtkMAFSmartPointer<vtkMEDRegionGrowingLocalGlobalThreshold> localFilter;
  localFilter->SetInput(imageData);
  localFilter->SetLowerLabel(m_LowerLabel);
  localFilter->SetLowerThreshold(lower);
  localFilter->SetUpperLabel(m_UpperLabel);
  localFilter->SetUpperThreshold(upper);
  mafEventMacro(mafEvent(this,BIND_TO_PROGRESSBAR,localFilter));
  localFilter->Update();

  //Save the result of the region growing
  m_SegmentedImage->DeepCopy(localFilter->GetOutput());
  m_SegmentedImage->Update();

  mafEventMacro(mafEvent(this,PROGRESSBAR_HIDE));
  
}
//----------------------------------------------------------------------------
// widget ID's
//----------------------------------------------------------------------------
enum REGION_GROWING_ID
{
  ID_TO_START = MINID,
  ID_SLIDER_LABELS,
  ID_ROLLOUT_LABELS,
  ID_SPHERE_RADIUS,
  ID_APPLY_CONNECTIVITY_FILTER,
  ID_REGION_GROWING,
  ID_MORPHOLOGICAL,
  ID_APPLY_MEDIAN_FILTER,
  ID_DIALOG_HISTOGRAM,
};
//----------------------------------------------------------------------------
void medOpSegmentationRegionGrowingLocalAndGlobalThreshold::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new mafGUI(this);

//   wxBoxSizer *sizer3 = new wxBoxSizer(wxHORIZONTAL);
//   m_Histogram = new mafGUIHistogramWidget(m_Gui,-1,wxPoint(0,0),wxSize(20,200),wxTAB_TRAVERSAL,true);
//   m_Histogram->SetListener(m_Gui);
//   m_Histogram->SetRepresentation(vtkMAFHistogram::BAR_REPRESENTATION);
//   vtkImageData *hd = vtkImageData::SafeDownCast(m_VolumeInput->GetOutput()->GetVTKData());
//   hd->Update();
//   m_Histogram->SetData(hd->GetPointData()->GetScalars());
// 
//   sizer3->Add(m_Histogram,wxALIGN_CENTER|wxRIGHT);
//   m_Gui->Add(sizer3,1);

  m_Gui->Button(ID_DIALOG_HISTOGRAM,_("Select points"));

  m_Gui->Divider(1);

  m_GuiLabels = new mafGUI(this);
  m_Gui->RollOut(ID_ROLLOUT_LABELS, _("Labels"), m_GuiLabels, false);

  m_LowerLabel = VTK_UNSIGNED_CHAR_MIN;
  m_UpperLabel = VTK_UNSIGNED_CHAR_MAX;

  wxBoxSizer *sizer2 = new wxBoxSizer(wxHORIZONTAL);
  m_SliderLabels = new mafGUILutSlider(m_GuiLabels,ID_SLIDER_LABELS ,wxPoint(0,0),wxSize(10,24), 0, "Label");
  m_SliderLabels->SetListener(this);
  m_SliderLabels->SetSize(5,24);
  m_SliderLabels->SetMinSize(wxSize(5,24));
  m_SliderLabels->SetRange(m_LowerLabel,m_UpperLabel);
  m_SliderLabels->SetSubRange(m_LowerLabel,m_UpperLabel);

  sizer2->Add(m_SliderLabels,wxALIGN_CENTER|wxRIGHT);

  m_GuiLabels->Add(sizer2);
  m_GuiLabels->FitGui();

  m_Gui->Divider(1);
  m_Gui->Integer(ID_SPHERE_RADIUS,_("Radius"),&m_SphereRadius);
  m_Gui->Enable(ID_SPHERE_RADIUS,false);
  m_Gui->Divider(1);
  m_Gui->Bool(ID_APPLY_CONNECTIVITY_FILTER,_("Extract biggest region"),&m_ApplyConnectivityFilter,1);
  m_Gui->Enable(ID_APPLY_CONNECTIVITY_FILTER,false);
  m_Gui->Divider(1);
  m_Gui->Button(ID_REGION_GROWING,_("Region growing"));
  m_Gui->Button(ID_MORPHOLOGICAL,_("Morphological closing"));
  m_Gui->Enable(ID_MORPHOLOGICAL,false);
  m_Gui->OkCancel();
  m_Gui->Enable(wxOK,false);
  m_Gui->Divider(1);

  m_Gui->FitGui();
  ShowGui();
}
//----------------------------------------------------------------------------
void medOpSegmentationRegionGrowingLocalAndGlobalThreshold::CreateHistogramDialog()
//----------------------------------------------------------------------------
{
  m_Dialog = new mafGUIDialog("Histogram", mafCLOSEWINDOW | mafRESIZABLE);

  m_Histogram = new mafGUIHistogramWidget(m_Gui,-1,wxPoint(0,0),wxSize(400,500),wxTAB_TRAVERSAL,true);
  m_Histogram->SetListener(m_Gui);
  m_Histogram->SetRepresentation(vtkMAFHistogram::BAR_REPRESENTATION);
  vtkImageData *hd = vtkImageData::SafeDownCast(m_VolumeInput->GetOutput()->GetVTKData());
  hd->Update();
  m_Histogram->SetData(hd->GetPointData()->GetScalars());

  mafGUI *gui = new mafGUI(this);
  gui->Add(m_Histogram,1);
  gui->AddGui(m_Histogram->GetGui());
  gui->FitGui();
  gui->Update();

  m_Dialog->Add(gui,1);
  m_Dialog->SetMinSize(wxSize(600,600));
}
//----------------------------------------------------------------------------
void medOpSegmentationRegionGrowingLocalAndGlobalThreshold::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
    case ID_DIALOG_HISTOGRAM:
      {
        m_Dialog->ShowModal();
      }
      break;
    case ID_REGION_GROWING:
      {
        RegionGrowing();

        //Convert the output of the region growing into structured points
        vtkMAFSmartPointer<vtkImageToStructuredPoints> filter;
        filter->SetInput(m_SegmentedImage);
        filter->Update();

        //Generate the vme output of the region growing
        m_VolumeOutputRegionGrowing->SetData(filter->GetOutput(),m_VolumeInput->GetTimeStamp());
        m_VolumeOutputRegionGrowing->SetName(_("Segmentation Output - first step"));
        m_VolumeOutputRegionGrowing->ReparentTo(m_VolumeInput);
        m_VolumeOutputRegionGrowing->Update();

        mafEventMacro(mafEvent(this,VME_SHOW,m_VolumeOutputRegionGrowing,true));

        if (m_SegmentedImage != NULL)
        {
          m_Gui->Enable(ID_SPHERE_RADIUS,true);
          m_Gui->Enable(ID_APPLY_CONNECTIVITY_FILTER,true);
          m_Gui->Enable(ID_MORPHOLOGICAL,true);
          m_Gui->Update();
        }
      }
      break;
    case ID_MORPHOLOGICAL:
      {
        MorphologicalMathematics();

        //Convert the output of the region growing into structured points
        vtkMAFSmartPointer<vtkImageToStructuredPoints> filter;
        filter->SetInput(m_MorphoImage);
        filter->Update();

        //Generate the vme output of the morphological closing
        m_VolumeOutputMorpho->SetData(filter->GetOutput(),m_VolumeInput->GetTimeStamp());
        m_VolumeOutputMorpho->SetName(_("Segmentation Output - second step"));
        m_VolumeOutputMorpho->ReparentTo(m_VolumeInput);
        m_VolumeOutputMorpho->Update();

        vtkMAFSmartPointer<vtkMAFContourVolumeMapper> extractIsosurface;
        extractIsosurface->SetInput(filter->GetOutput());
        extractIsosurface->SetContourValue(m_UpperLabel);
        extractIsosurface->Update();

        vtkMAFSmartPointer<vtkPolyDataConnectivityFilter> connectivityFilter;
        int result = MAF_OK;
        if (m_ApplyConnectivityFilter == TRUE)
        {
          connectivityFilter->SetInput(extractIsosurface->GetOutput());
          connectivityFilter->SetExtractionModeToLargestRegion();
          connectivityFilter->Update();

          result = m_SurfaceOutput->SetData(connectivityFilter->GetOutput(),m_VolumeInput->GetTimeStamp());
        }
        else
        {
          result = m_SurfaceOutput->SetData(extractIsosurface->GetOutput(),m_VolumeInput->GetTimeStamp());
        }

        if (result == MAF_ERROR)
        {
          wxMessageBox("There was an error during extracting the surface!");
          m_Gui->Enable(wxOK,false);
        }
        else
        {
        	m_SurfaceOutput->SetName(_("Segmentation Output - extract isosurface"));
        	m_SurfaceOutput->ReparentTo(m_VolumeInput);
        	m_SurfaceOutput->Update();
  
        	mafEventMacro(mafEvent(this,VME_SHOW,m_VolumeOutputMorpho,true));
          m_Gui->Enable(wxOK,true);
        }


      }
      break;
    case ID_RANGE_MODIFIED:
      {
        if (e->GetSender() == m_SliderLabels)
        {
          //Get the label values modified by the user
          m_SliderLabels->GetSubRange(&m_LowerLabel,&m_UpperLabel);
        }
      }
      break;
    case wxOK:
      {
        this->OpStop(OP_RUN_OK);
        return;
      }
      break;
    case wxCANCEL:
      {
        this->OpStop(OP_RUN_CANCEL);
        return;
      }
      break;
    default:
      mafEventMacro(*e);
      break; 
    }
  }
}
//----------------------------------------------------------------------------
void medOpSegmentationRegionGrowingLocalAndGlobalThreshold::OpStop(int result)
//----------------------------------------------------------------------------
{
  if (result == OP_RUN_CANCEL)
  {
	  if (m_VolumeOutputRegionGrowing)
	  {
	    m_VolumeOutputRegionGrowing->ReparentTo(NULL);
	  }
	  if (m_VolumeOutputMorpho)
	  {
	    m_VolumeOutputMorpho->ReparentTo(NULL);
	  }
	  if (m_SurfaceOutput)
	  {
	    m_SurfaceOutput->ReparentTo(NULL);
	  }

    mafEventMacro(mafEvent(this,CAMERA_UPDATE));
  }

  HideGui();
  mafEventMacro(mafEvent(this,result));        
}
//----------------------------------------------------------------------------
void medOpSegmentationRegionGrowingLocalAndGlobalThreshold::OpDo()
//----------------------------------------------------------------------------
{
  if (m_VolumeOutputRegionGrowing )
  {
    m_VolumeOutputRegionGrowing->ReparentTo(m_Input);
  }
  if (m_VolumeOutputMorpho)
  {
    m_VolumeOutputMorpho->ReparentTo(m_Input);
  }
  if (m_SurfaceOutput)
  {
    m_SurfaceOutput->ReparentTo(m_Input);
  }

  if (m_VolumeOutputMorpho == NULL && m_VolumeOutputRegionGrowing == NULL && m_SurfaceOutput == NULL)
  {
    return;
  }
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void medOpSegmentationRegionGrowingLocalAndGlobalThreshold::OpUndo()
//----------------------------------------------------------------------------
{
  if (m_VolumeOutputRegionGrowing)
  {
    mafEventMacro(mafEvent(this, VME_REMOVE, m_VolumeOutputRegionGrowing));
  }
  if (m_VolumeOutputMorpho)
  {
    mafEventMacro(mafEvent(this, VME_REMOVE, m_VolumeOutputMorpho));
  }
  if (m_SurfaceOutput)
  {
    mafEventMacro(mafEvent(this, VME_REMOVE, m_SurfaceOutput));
  }

  if (m_VolumeOutputMorpho == NULL && m_VolumeOutputRegionGrowing == NULL && m_SurfaceOutput == NULL)
  {
    return;
  }
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}