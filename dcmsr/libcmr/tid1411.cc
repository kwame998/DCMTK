/*
 *
 *  Copyright (C) 2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class TID1411_VolumetricROIMeasurements
 *
 *  Author: Joerg Riesmeier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/tid1411.h"
#include "dcmtk/dcmsr/cmr/cid6147.h"
#include "dcmtk/dcmsr/cmr/cid7181.h"
#include "dcmtk/dcmsr/cmr/cid7464.h"
#include "dcmtk/dcmsr/cmr/cid7469.h"
#include "dcmtk/dcmsr/cmr/logger.h"
#include "dcmtk/dcmsr/codes/dcm.h"
#include "dcmtk/dcmsr/codes/ncit.h"
#include "dcmtk/dcmsr/codes/srt.h"
#include "dcmtk/dcmsr/codes/umls.h"


// helper macros for checking the return value of API calls
#define CHECK_RESULT(call) if (result.good()) result = call
#define STORE_RESULT(call) result = call
#define GOOD_RESULT(call) if (result.good()) call
#define BAD_RESULT(call) if (result.bad()) call

// index positions in node list (makes source code more readable)
#define MEASUREMENT_GROUP               0
#define ACTIVITY_SESSION                1
#define TRACKING_IDENTIFIER             2
#define TRACKING_UNIQUE_IDENTIFIER      3
#define FINDING                         4
#define TIME_POINT                      5
#define REFERENCED_SEGMENT              6
#define SOURCE_SERIES_FOR_SEGMENTATION  7
#define REAL_WORLD_VALUE_MAP            8
#define MEASUREMENT_METHOD              9
#define FINDING_SITE                   10
#define LAST_MEASUREMENT               11

// general information on TID 1411 (Volumetric ROI Measurements)
#define TEMPLATE_NUMBER      "1411"
#define MAPPING_RESOURCE     "DCMR"
#define MAPPING_RESOURCE_UID UID_DICOMContentMappingResource
#define TEMPLATE_TYPE        OFTrue  /* extensible */

// conditions constants
makeOFConditionConst(CMR_EC_NoMeasurementGroup,                 OFM_dcmsr, 1411, OF_error, "No Measurement Group");
makeOFConditionConst(CMR_EC_InvalidSegmentationObject,          OFM_dcmsr, 1412, OF_error, "Invalid Segmentation Object");
makeOFConditionConst(CMR_EC_InvalidRealWorldValueMappingObject, OFM_dcmsr, 1413, OF_error, "Invalid Real World Value Mapping Object");


template<typename T1, typename T2, typename T3, typename T4>
TID1411_VolumetricROIMeasurements<T1, T2, T3, T4>::TID1411_VolumetricROIMeasurements(const OFBool createGroup)
  : DSRSubTemplate(TEMPLATE_NUMBER, MAPPING_RESOURCE, MAPPING_RESOURCE_UID)
{
    setExtensible(TEMPLATE_TYPE);
    /* need to store position of various content items */
    reserveEntriesInNodeList(12, OFTrue /*initialize*/);
    /* TID 1411 (Volumetric ROI Measurements) Row 1 */
    if (createGroup)
        createMeasurementGroup();
}


template<typename T1, typename T2, typename T3, typename T4>
OFBool TID1411_VolumetricROIMeasurements<T1, T2, T3, T4>::isValid() const
{
    /* check whether base class is valid and all required content items are present */
    return DSRSubTemplate::isValid() &&
        hasMeasurementGroup() && hasTrackingIdentifier() && hasTrackingUniqueIdentifier() &&
        hasReferencedSegment() && hasSourceSeriesForSegmentation() && hasROIMeasurements();
}


template<typename T1, typename T2, typename T3, typename T4>
OFBool TID1411_VolumetricROIMeasurements<T1, T2, T3, T4>::hasMeasurementGroup(const OFBool checkChildren) const
{
    OFBool result = OFFalse;
    /* need to check for child nodes? */
    if (checkChildren)
    {
        DSRDocumentTreeNodeCursor cursor(getRoot());
        /* go to content item at TID 1411 (Volumetric ROI Measurements) Row 1 */
        if (gotoEntryFromNodeList(cursor, MEASUREMENT_GROUP) > 0)
            result = cursor.hasChildNodes();
    } else {
        /* check for content item at TID 1411 (Volumetric ROI Measurements) Row 1 */
        result = (getEntryFromNodeList(MEASUREMENT_GROUP) > 0);
    }
    return result;
}


template<typename T1, typename T2, typename T3, typename T4>
OFBool TID1411_VolumetricROIMeasurements<T1, T2, T3, T4>::hasTrackingIdentifier() const
{
    /* check for content item at TID 1411 (Volumetric ROI Measurements) Row 2 */
    return (getEntryFromNodeList(TRACKING_IDENTIFIER) > 0);
}


template<typename T1, typename T2, typename T3, typename T4>
OFBool TID1411_VolumetricROIMeasurements<T1, T2, T3, T4>::hasTrackingUniqueIdentifier() const
{
    /* check for content item at TID 1411 (Volumetric ROI Measurements) Row 3 */
    return (getEntryFromNodeList(TRACKING_UNIQUE_IDENTIFIER) > 0);
}


template<typename T1, typename T2, typename T3, typename T4>
OFBool TID1411_VolumetricROIMeasurements<T1, T2, T3, T4>::hasReferencedSegment() const
{
    /* check for content item at TID 1411 (Volumetric ROI Measurements) Row 7 */
    return (getEntryFromNodeList(REFERENCED_SEGMENT) > 0);
}


template<typename T1, typename T2, typename T3, typename T4>
OFBool TID1411_VolumetricROIMeasurements<T1, T2, T3, T4>::hasSourceSeriesForSegmentation() const
{
    /* check for content item at TID 1411 (Volumetric ROI Measurements) Row 12 */
    return (getEntryFromNodeList(SOURCE_SERIES_FOR_SEGMENTATION) > 0);
}


template<typename T1, typename T2, typename T3, typename T4>
OFBool TID1411_VolumetricROIMeasurements<T1, T2, T3, T4>::hasROIMeasurements() const
{
    /* check for content item at TID 1419 (ROI Measurements) Row 5 */
    return (getEntryFromNodeList(LAST_MEASUREMENT) > 0);
}


template<typename T1, typename T2, typename T3, typename T4>
OFCondition TID1411_VolumetricROIMeasurements<T1, T2, T3, T4>::setActivitySession(const OFString &session,
                                                                                  const OFBool check)
{
    OFCondition result = EC_Normal;
    /* basic check of parameter */
    if (!session.empty())
    {
        /* check whether measurement group already exists */
        if (!hasMeasurementGroup())
            result = createMeasurementGroup();
        /* TID 1411 (Volumetric ROI Measurements) Row 1b */
        CHECK_RESULT(addOrReplaceContentItem(ACTIVITY_SESSION, RT_hasObsContext, VT_Text, CODE_NCIt_ActivitySession, "TID 1411 - Row 1b", check));
        CHECK_RESULT(getCurrentContentItem().setStringValue(session, check));
    } else
        result = EC_IllegalParameter;
    return result;
}


template<typename T1, typename T2, typename T3, typename T4>
OFCondition TID1411_VolumetricROIMeasurements<T1, T2, T3, T4>::setTrackingIdentifier(const OFString &trackingID,
                                                                                     const OFBool check)
{
    OFCondition result = EC_Normal;
    /* basic check of parameter */
    if (!trackingID.empty())
    {
        /* check whether measurement group already exists */
        if (!hasMeasurementGroup())
            result = createMeasurementGroup();
        /* TID 1411 (Volumetric ROI Measurements) Row 2 */
        CHECK_RESULT(addOrReplaceContentItem(TRACKING_IDENTIFIER, RT_hasObsContext, VT_Text, CODE_DCM_TrackingIdentifier, "TID 1411 - Row 2", check));
        CHECK_RESULT(getCurrentContentItem().setStringValue(trackingID, check));
    } else
        result = EC_IllegalParameter;
    return result;
}


template<typename T1, typename T2, typename T3, typename T4>
OFCondition TID1411_VolumetricROIMeasurements<T1, T2, T3, T4>::setTrackingUniqueIdentifier(const OFString &trackingUID,
                                                                                           const OFBool check)
{
    OFCondition result = EC_Normal;
    /* basic check of parameter */
    if (!trackingUID.empty())
    {
        /* check whether measurement group already exists */
        if (!hasMeasurementGroup())
            result = createMeasurementGroup();
        /* TID 1411 (Volumetric ROI Measurements) Row 3 */
        CHECK_RESULT(addOrReplaceContentItem(TRACKING_UNIQUE_IDENTIFIER, RT_hasObsContext, VT_UIDRef, CODE_DCM_TrackingUniqueIdentifier, "TID 1411 - Row 3", check));
        CHECK_RESULT(getCurrentContentItem().setStringValue(trackingUID, check));
    } else
        result = EC_IllegalParameter;
    return result;
}


template<typename T1, typename T2, typename T3, typename T4>
OFCondition TID1411_VolumetricROIMeasurements<T1, T2, T3, T4>::setFinding(const DSRCodedEntryValue &finding,
                                                                          const OFBool check)
{
    OFCondition result = EC_Normal;
    /* basic check of parameter */
    if (finding.isComplete())
    {
        /* check whether measurement group already exists */
        if (!hasMeasurementGroup())
            result = createMeasurementGroup();
        /* TID 1411 (Volumetric ROI Measurements) Row 3b */
        CHECK_RESULT(addOrReplaceContentItem(FINDING, RT_contains, VT_Code, CODE_DCM_Finding, "TID 1411 - Row 3b", check));
        CHECK_RESULT(getCurrentContentItem().setCodeValue(finding, check));
    } else
        result = EC_IllegalParameter;
    return result;
}


template<typename T1, typename T2, typename T3, typename T4>
OFCondition TID1411_VolumetricROIMeasurements<T1, T2, T3, T4>::setTimePoint(const OFString &timePoint,
                                                                            const OFBool check)
{
    OFCondition result = EC_Normal;
    /* basic check of parameter */
    if (!timePoint.empty())
    {
        /* check whether measurement group already exists */
        if (!hasMeasurementGroup())
            result = createMeasurementGroup();
        /* TID 1502 (Time Point Context) Row 3 */
        CHECK_RESULT(addOrReplaceContentItem(TIME_POINT, RT_hasObsContext, VT_Text, CODE_UMLS_TimePoint, "TID 1502 - Row 3", check));
        CHECK_RESULT(getCurrentContentItem().setStringValue(timePoint, check));
    } else
        result = EC_IllegalParameter;
    return result;
}


template<typename T1, typename T2, typename T3, typename T4>
OFCondition TID1411_VolumetricROIMeasurements<T1, T2, T3, T4>::setReferencedSegment(const DSRImageReferenceValue &segment,
                                                                                    const OFBool check)
{
    OFCondition result = EC_Normal;
    /* basic check of parameter */
    if (segment.isComplete())
    {
        const char *annotationText = "TID 1411 - Row 7";
        const DSRBasicCodedEntry conceptName(CODE_DCM_ReferencedSegment);
        /* check for supported segmentation SOP classes */
        if ((segment.getSOPClassUID() != UID_SegmentationStorage) && (segment.getSOPClassUID() != UID_SurfaceSegmentationStorage))
        {
            DCMSR_CMR_WARN("Cannot set value of '" << conceptName.CodeMeaning << "' content item (" << annotationText << ") ... wrong SOP Class");
            DCMSR_CMR_DEBUG("SOP Class UID \"" << segment.getSOPClassUID() << "\" does not match one of the known Segmentation objects");
            result = CMR_EC_InvalidSegmentationObject;
        }
        /*  ... and number of referenced segments */
        else if ((segment.getSegmentList().getNumberOfItems() != 1))
        {
            DCMSR_CMR_WARN("Cannot set value of '" << conceptName.CodeMeaning << "' content item (" << annotationText << ") ... wrong number of segments");
            result = CMR_EC_InvalidSegmentationObject;
        } else {
            /* check whether measurement group already exists */
            if (!hasMeasurementGroup())
                result = createMeasurementGroup();
            /* TID 1411 (Volumetric ROI Measurements) Row 7 */
            CHECK_RESULT(addOrReplaceContentItem(REFERENCED_SEGMENT, RT_contains, VT_Image, conceptName, annotationText, check));
            CHECK_RESULT(getCurrentContentItem().setImageReference(segment, check));
        }
    } else
        result = EC_IllegalParameter;
    return result;
}


template<typename T1, typename T2, typename T3, typename T4>
OFCondition TID1411_VolumetricROIMeasurements<T1, T2, T3, T4>::setSourceSeriesForSegmentation(const OFString &seriesUID,
                                                                                              const OFBool check)
{
    OFCondition result = EC_Normal;
    /* basic check of parameter */
    if (!seriesUID.empty())
    {
        /* check whether measurement group already exists */
        if (!hasMeasurementGroup())
            result = createMeasurementGroup();
        /* TID 1411 (Volumetric ROI Measurements) Row 12 */
        CHECK_RESULT(addOrReplaceContentItem(SOURCE_SERIES_FOR_SEGMENTATION, RT_contains, VT_UIDRef, CODE_DCM_SourceSeriesForSegmentation, "TID 1411 - Row 12", check));
        CHECK_RESULT(getCurrentContentItem().setStringValue(seriesUID, check));
    } else
        result = EC_IllegalParameter;
    return result;
}


template<typename T1, typename T2, typename T3, typename T4>
OFCondition TID1411_VolumetricROIMeasurements<T1, T2, T3, T4>::setRealWorldValueMap(const DSRCompositeReferenceValue &valueMap,
                                                                                    const OFBool check)
{
    OFCondition result = EC_Normal;
    /* basic check of parameter */
    if (valueMap.isComplete())
    {
        const char *annotationText = "TID 1411 - Row 14";
        const DSRBasicCodedEntry conceptName(CODE_DCM_RealWorldValueMapUsedForMeasurement);
        /* check for real world value mapping SOP classes */
        if (valueMap.getSOPClassUID() != UID_RealWorldValueMappingStorage)
        {
            DCMSR_CMR_WARN("Cannot set value of '" << conceptName.CodeMeaning << "' content item (" << annotationText << ") ... wrong SOP Class");
            DCMSR_CMR_DEBUG("SOP Class UID \"" << valueMap.getSOPClassUID() << "\" does not match the one of the Real World Value Mapping object");
            result = CMR_EC_InvalidRealWorldValueMappingObject;
        } else {
            /* check whether measurement group already exists */
            if (!hasMeasurementGroup())
                result = createMeasurementGroup();
            /* TID 1411 (Volumetric ROI Measurements) Row 14 */
            CHECK_RESULT(addOrReplaceContentItem(REAL_WORLD_VALUE_MAP, RT_contains, VT_Composite, conceptName, annotationText, check));
            CHECK_RESULT(getCurrentContentItem().setCompositeReference(valueMap, check));
        }
    } else
        result = EC_IllegalParameter;
    return result;
}


template<typename T1, typename T2, typename T_Method, typename T4>
OFCondition TID1411_VolumetricROIMeasurements<T1, T2, T_Method, T4>::setMeasurementMethod(const T_Method &method,
                                                                                          const OFBool check)
{
    OFCondition result = EC_Normal;
    /* basic check of parameter */
    if (method.hasSelectedValue())
    {
        /* check whether measurement group already exists */
        if (!hasMeasurementGroup())
            result = createMeasurementGroup();
        /* TID 1419 (ROI Measurements) Row 1 */
        CHECK_RESULT(addOrReplaceContentItem(MEASUREMENT_METHOD, RT_hasConceptMod, VT_Code, CODE_SRT_MeasurementMethod, "TID 1419 - Row 1", check));
        CHECK_RESULT(getCurrentContentItem().setCodeValue(method, check));
    } else
        result = EC_IllegalParameter;
    return result;
}


template<typename T1, typename T2, typename T3, typename T4>
OFCondition TID1411_VolumetricROIMeasurements<T1, T2, T3, T4>::setFindingSite(const DSRCodedEntryValue &site,
                                                                              const OFBool check)
{
    OFCondition result = EC_Normal;
    /* basic check of parameter */
    if (site.isComplete())
    {
        /* check whether measurement group already exists */
        if (!hasMeasurementGroup())
            result = createMeasurementGroup();
        /* TID 1419 (ROI Measurements) Row 2 */
        CHECK_RESULT(addOrReplaceContentItem(FINDING_SITE, RT_hasConceptMod, VT_Code, CODE_SRT_FindingSite, "TID 1419 - Row 2", check));
        CHECK_RESULT(getCurrentContentItem().setCodeValue(site, check));
    } else
        result = EC_IllegalParameter;
    return result;
}


template<typename T_Measurement, typename T_Units, typename T_Method, typename T_Derivation>
OFCondition TID1411_VolumetricROIMeasurements<T_Measurement, T_Units, T_Method, T_Derivation>::addMeasurement(const T_Measurement &conceptName,
                                                                                                              const MeasurementValue &numericValue,
                                                                                                              const T_Method &method,
                                                                                                              const T_Derivation &derivation,
                                                                                                              const OFBool check)
{
    OFCondition result = EC_Normal;
    /* basic check of mandatory parameters */
    if (conceptName.hasSelectedValue() && numericValue.isComplete())
    {
        /* check whether measurement group already exists */
        if (!hasMeasurementGroup())
            result = createMeasurementGroup();
        if (result.good())
        {
            /* create a new subtree in order to "rollback" in case of error */
            DSRDocumentSubTree *subTree = new DSRDocumentSubTree;
            if (subTree != NULL)
            {
                /* TID 1419 (ROI Measurements) Row 5 */
                STORE_RESULT(subTree->addContentItem(RT_contains, VT_Num, conceptName, check));
                CHECK_RESULT(subTree->getCurrentContentItem().setNumericValue(numericValue, check));
                CHECK_RESULT(subTree->getCurrentContentItem().setAnnotationText("TID 1419 - Row 5"));
                const size_t lastNode = subTree->getNodeID();
                /* TID 1419 (ROI Measurements) Row 7 - optional */
                if (method.hasSelectedValue())
                {
                    CHECK_RESULT(subTree->addChildContentItem(RT_hasConceptMod, VT_Code, CODE_SRT_MeasurementMethod, check));
                    CHECK_RESULT(subTree->getCurrentContentItem().setCodeValue(method, check));
                    CHECK_RESULT(subTree->getCurrentContentItem().setAnnotationText("TID 1419 - Row 7"));
                    GOOD_RESULT(subTree->gotoParent());
                }
                /* TID 1419 (ROI Measurements) Row 8 - optional */
                if (derivation.hasSelectedValue())
                {
                    CHECK_RESULT(subTree->addChildContentItem(RT_hasConceptMod, VT_Code, CODE_DCM_Derivation, check));
                    CHECK_RESULT(subTree->getCurrentContentItem().setCodeValue(derivation, check));
                    CHECK_RESULT(subTree->getCurrentContentItem().setAnnotationText("TID 1419 - Row 8"));
                    GOOD_RESULT(subTree->gotoParent());
                }
                /* if everything was OK, insert new subtree into the template */
                if (result.good() && !subTree->isEmpty())
                {
                    /* go to last measurement (if any) */
                    if (gotoLastEntryFromNodeList(this, LAST_MEASUREMENT) > 0)
                    {
                        /* insert subtree at current position */
                        STORE_RESULT(insertSubTree(subTree, AM_afterCurrent));
                        /* store ID of recently added node for later use */
                        GOOD_RESULT(storeEntryInNodeList(LAST_MEASUREMENT, lastNode));
                    } else {
                        /* should never happen but ... */
                        result = CMR_EC_NoMeasurementGroup;
                    }
                    /* in case of error, make sure that memory is freed */
                    BAD_RESULT(delete subTree);
                } else {
                    /* delete the new subtree since it has not been inserted */
                    delete subTree;
                }
            } else
                result = EC_MemoryExhausted;
        }
    } else
        result = EC_IllegalParameter;
    return result;
}


// protected methods

template<typename T1, typename T2, typename T3, typename T4>
OFCondition TID1411_VolumetricROIMeasurements<T1, T2, T3, T4>::createMeasurementGroup()
{
    OFCondition result = SR_EC_InvalidTemplateStructure;
    if (isEmpty())
    {
        /* TID 1411 (Volumetric ROI Measurements) Row 1 */
        STORE_RESULT(addContentItem(RT_unknown, VT_Container, CODE_DCM_MeasurementGroup));
        CHECK_RESULT(getCurrentContentItem().setAnnotationText("TID 1411 - Row 1"));
        /* store ID of root node for later use */
        GOOD_RESULT(storeEntryInNodeList(MEASUREMENT_GROUP, getNodeID()));
    }
    return result;
}


template<typename T1, typename T2, typename T3, typename T4>
OFCondition TID1411_VolumetricROIMeasurements<T1, T2, T3, T4>::addOrReplaceContentItem(const size_t nodePos,
                                                                                       const E_RelationshipType relationshipType,
                                                                                       const E_ValueType valueType,
                                                                                       const DSRCodedEntryValue &conceptName,
                                                                                       const OFString &annotationText,
                                                                                       const OFBool check)
{
    OFCondition result = EC_Normal;
    /* check concept name and coded entry value */
    if (conceptName.isComplete())
    {
        /* check whether content item already exists */
        if (getEntryFromNodeList(nodePos) == 0)
        {
            /* if not, create the content item (at correct position) */
            if (gotoLastEntryFromNodeList(this, nodePos) == getEntryFromNodeList(MEASUREMENT_GROUP))
            {
                /* need to add the new content item as the first child */
                if (addContentItem(relationshipType, valueType, AM_belowCurrentBeforeFirstChild) > 0)
                {
                    if (getCurrentContentItem().setConceptName(conceptName, check).bad())
                        result = SR_EC_InvalidConceptName;
                } else
                    result = SR_EC_CannotAddContentItem;

            } else {
                /* add new content item as a sibling (after the current one) */
                STORE_RESULT(addContentItem(relationshipType, valueType, conceptName));
            }
            /* store ID of added node for later use */
            GOOD_RESULT(storeEntryInNodeList(nodePos, getNodeID()));
        }
        else if (gotoEntryFromNodeList(this, nodePos) > 0)
        {
            /* make sure that the value type of the existing content item is correct */
            if (getCurrentContentItem().getValueType() != valueType)
            {
                DCMSR_CMR_WARN("Cannot replace value of '" << conceptName.getCodeMeaning()
                    << "' content item (" << annotationText << ") ... wrong value type");
                result = SR_EC_InvalidContentItem;
            }
            else if (getCurrentContentItem().getConceptName() != conceptName)
            {
                DCMSR_CMR_WARN("Cannot replace value of '" << conceptName.getCodeMeaning()
                    << "' content item (" << annotationText << ") ... wrong concept name");
                result = SR_EC_InvalidConceptName;
            } else {
                DCMSR_CMR_DEBUG("Replacing value of '" << conceptName.getCodeMeaning()
                    << "' content item (" << annotationText << ")");
            }
        } else
            result = SR_EC_InvalidTemplateStructure;
        /* finally, set annotation */
        CHECK_RESULT(getCurrentContentItem().setAnnotationText(annotationText));
    } else
        result = SR_EC_InvalidConceptName;
    return result;
}


// explicit template instantiation (needed for use in TID 1500)
template class TID1411_VolumetricROIMeasurements<CID7469_GenericIntensityAndSizeMeasurements,
                                                 CID7181_AbstractMultiDimensionalImageModelComponentUnits,
                                                 CID6147_ResponseCriteria,
                                                 CID7464_GeneralRegionOfInterestMeasurementModifiers>;