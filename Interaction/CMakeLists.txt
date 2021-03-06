#
# Copyright and description to be placed here
#

PROJECT(medInteraction)

#DBG_MESSAGE("Configuring project ${PROJECT_NAME}")

# Set your list of sources here.
SET(PROJECT_SRCS
  medInteractorPicker.cpp
  medInteractorPicker.h
  medInteractorDICOMImporter.h
  medInteractorDICOMImporter.cpp  
  medInteractor2DAngle.cpp
  medInteractor2DAngle.h
  medInteractor2DDistance.cpp
  medInteractor2DDistance.h
  medInteractor2DIndicator.cpp
  medInteractor2DIndicator.h
	medInteractorSegmentationPicker.cpp
	medInteractorSegmentationPicker.h
	medInteractorPERScalarInformation.cpp
	medInteractorPERScalarInformation.h
	medInteractorPERBrushFeedback.cpp
	medInteractorPERBrushFeedback.h
  mmiVTKPicker.cpp
  mmiVTKPicker.h
  medGizmoPolylineGraph.cpp
  medGizmoPolylineGraph.h
  medDeviceButtonsPadMouseDialog.h
  medDeviceButtonsPadMouseDialog.cpp
  medGUITransformSliders.cpp
  medGUITransformSliders.h
  
  ##### Introduced with VolumeLarge - to be committed down ###
  # mafGizmoROI_BES.cpp
  # mafGizmoROI_BES.h  
  ############################################################

  medGizmoCrossRotateTranslate.cpp
  medGizmoCrossRotateTranslate.h
	  
  medGizmoCrossRotate.cpp
  medGizmoCrossRotate.h
  medGizmoCrossRotateAxis.cpp
  medGizmoCrossRotateAxis.h
  medGizmoCrossRotateFan.cpp
  medGizmoCrossRotateFan.h

  medGizmoCrossTranslate.cpp
  medGizmoCrossTranslate.h
  medGizmoCrossTranslateAxis.cpp
  medGizmoCrossTranslateAxis.h
  medGizmoCrossTranslatePlane.cpp
  medGizmoCrossTranslatePlane.h

)


#IF (MAF_USE_VTK)
#  SET(PROJECT_SRCS
#  ${PROJECT_SRCS}
#  )
#ENDIF (MAF_USE_VTK)

# currently special drivers only supported under WIN32
#IF (WIN32)
# SUBDIRS(Drivers)
#ENDIF (WIN32)

# List libraries that are needed by this project.
SET(PROJECT_LIBS mafBase mafIO mafGui vtkMAF)

# currently special drivers only supported under WIN32
#IF (WIN32)
#  SET(PROJECT_LIBS ${PROJECT_LIBS} mafDrivers)
#ENDIF (WIN32)

#IF (MAF_USE_VTK)
#  SET(PROJECT_LIBS ${PROJECT_LIBS} vtkHybrid)
#ENDIF (MAF_USE_VTK)

IF(MED_BUILD_MEDDLL)
  SET(BUILD_SHARED_LIBS 1)
  ADD_DEFINITIONS(-DMED_INTERACTION_EXPORTS)
  # Create the library.
  ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
  TARGET_LINK_LIBRARIES(${PROJECT_NAME} mafDLL medCommon)
ELSE(MED_BUILD_MEDDLL)
  # Create the library.
  ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
  TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})
ENDIF (MED_BUILD_MEDDLL)

ADD_DEPENDENCIES(${PROJECT_NAME} medCommon medVME vtkMED)

#IF (MAF_USE_WX)
#  ADD_DEPENDENCIES(${PROJECT_NAME} BUILD_WXWIN_LIBRARY)
#ENDIF (MAF_USE_WX)

#CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/medInteractionConfigure.h.in ${PROJECT_BINARY_DIR}/medInteractionConfigure.h IMMEDIATE)
