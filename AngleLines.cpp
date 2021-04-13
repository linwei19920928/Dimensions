#include"AngleLines.h"
void  AngleLinesCreateTool::InstallNewInstance()
{
	AngleLinesCreateTool *tool = new AngleLinesCreateTool(0, 0);

	tool->InstallTool();
}
void AngleLinesCreateTool::_OnPostInstall()
{
	m_hitPath = NULL;
	AccuSnap::GetInstance().EnableSnap(true);
	m_dimPoints.clear();
	__super::_OnPostInstall();
}
bool AngleLinesCreateTool::_OnDataButton(DgnButtonEventCR ev)
{
	//View number required for creating DimCreateData object used for creating dimension element
	m_iView = ev.GetViewNum();
	if (curveVector == nullptr)
		curveVector = CurveVector::Create(CurveVector::BOUNDARY_TYPE_Inner);
	if (curveVector->size() == 2)
	{
		FinishDimension(ev);
	}
	else
	{		
		m_hitPath = _DoLocate(ev, true, ComponentMode::Innermost);
		if (!m_hitPath)
			return false;
		ICurvePrimitivePtr selLineCurve = GetSegmentLine(m_hitPath);
		curveVector->push_back(selLineCurve);
		//Adds the hit point to m_dimPoints vector
		m_dimPoints.push_back(*ev.GetPoint());
		if (curveVector->size() == 2)
			_BeginDynamics();
	}
	return false;
}
void AngleLinesCreateTool::_OnRestartTool()
{
	InstallNewInstance();
}
void AngleLinesCreateTool::_OnDynamicFrame(DgnButtonEventCR ev)
{
	EditElementHandle       elem;
	if (SUCCESS != CreateDime(elem, *ev.GetPoint()))
		return;
	RedrawElems redrawElems;
	redrawElems.SetDrawMode(DRAW_MODE_TempDraw);
	redrawElems.SetDrawPurpose(DrawPurpose::Dynamics);
	redrawElems.SetViewport(ev.GetViewport());
	redrawElems.SetRedrawOp(this);
	redrawElems.DoRedraw(elem);
}
bool AngleLinesCreateTool::_OnResetButton(DgnButtonEventCR ev)
{
	_OnRestartTool();
	return true;
}
bool AngleLinesCreateTool::ValidateSelection(HitPathCP hitPath)
{
	//Gets the ElementHandle from the hitPath
	ElementHandle eleHandle(hitPath->GetHeadElem(), nullptr);
	//Checks whether selected element is line element or not
	return (LINE_ELM == eleHandle.GetElementType()) ? true : false;
}
bool AngleLinesCreateTool::_OnPostLocate(HitPathCP path, WStringR cantAcceptReason)
{

	if (!DgnElementSetTool::_OnPostLocate(path, cantAcceptReason))
		return false;

	// Allow only line elements to be selected.
	return (ValidateSelection(path)) ? true : false;
}
/*---------------------------------------------------------------------------------**//**
* Returns the start and end points of the selected element.
*
*  @param[in]     hitPath    HitPath to get the segment points
*
* @bsimethod                                                              Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
ICurvePrimitivePtr AngleLinesCreateTool::GetSegmentLine(HitPathCP hitPath)
{
	DSegment3d              hitSeg;
	bvector<DPoint3d>       segmentPts;
	DPoint3d                orgPoint, endPoint;

	//Gets the points for dimension element from selected line element
	hitPath->GetLinearParameters(&hitSeg, nullptr, nullptr);

	ProjectPointsTo2dIfNeeded((DPoint3d*)&hitSeg, 2);

	hitSeg.GetStartPoint(orgPoint);
	hitSeg.GetEndPoint(endPoint);
	return ICurvePrimitive::CreateLine(DSegment3d::From(orgPoint, endPoint));
}
/*---------------------------------------------------------------------------------**//**
* Projects the points to 2D if creating dimension element in 2D model
*
* @param[out]     points     Points to be projected to 2D
* @param[in]      numPoints  Number of points to be projected to 2D
*
* @bsimethod                                                              Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
void AngleLinesCreateTool::ProjectPointsTo2dIfNeeded(DPoint3d   *points, int numPoints)
{

	if (!ISessionMgr::GetActiveDgnModelP()->Is3d())
		for (int iPoint = 0; iPoint < numPoints; iPoint++)
			points[iPoint].z = 0.0;
}
void AngleLinesCreateTool::FinishDimension(DgnButtonEventCR ev)
{
	EditElementHandle       elem;
	if (SUCCESS == CreateDime(elem, *ev.GetPoint()))
	{
		if (SUCCESS != elem.AddToModel())
			return;
		_OnReinitialize();
	}
}

/*---------------------------------------------------------------------------------**//**
* @description   Creates the dimension element by using the DimcreateData object, dimType = SizeArrow,
*                points to insert for dimensions and DgnModel reference.
*
* @param[out]     dimElem    Dimension element to be created
* @param[in]      point      Extension point for placing dimension element
*
* @bsimethod                                                              Bentley Systems
+---------------+---------------+---------------+---------------+---------------+------*/
BentleyStatus AngleLinesCreateTool::CreateDime(EditElementHandleR elem, DPoint3dCR point)
{
	double            witLength;
	DgnFilePtr    pActiveDgnFile = ISessionMgr::GetActiveDgnFile();
	DgnModelP pActiveModel = ISessionMgr::GetActiveDgnModelP();

	DimensionStylePtr pDimStyle = DimensionStyle::GetSettings(*pActiveDgnFile);

	DgnTextStylePtr textStyle = DgnTextStyle::GetSettings(*pActiveDgnFile);

	LevelId lvId = pActiveModel->GetLevelCache().GetLevelByName(L"Default").GetLevelId();

	Symbology symb;
	symb.style = 0; symb.weight = 0; symb.color = 27;

	DirectionFormatterPtr dirFormatter = DirectionFormatter::Create(*pActiveModel);

	RotMatrixCR rMatrix1 = RotMatrix::FromIdentity();
	RotMatrixCR rMatrix2 = RotMatrix::FromIdentity();

	CreateDimensionCallbacks* createDimdata = new CreateDimensionCallbacks(pDimStyle, textStyle, symb, lvId, dirFormatter, rMatrix1, rMatrix2);

	if (SUCCESS != DimensionHandler::CreateDimensionElement(elem, *createDimdata, DimensionType::AngleLines, ACTIVEMODEL->Is3d(), *pActiveModel))
	{
		return ERROR;
	}
	EditElementHandle lineEeh, lineEeh1;
	CurveVectorPtr intersectionA = CurveVector::Create(CurveVector::BOUNDARY_TYPE_Inner);
	CurveVectorPtr intersectionB = CurveVector::Create(CurveVector::BOUNDARY_TYPE_Inner);
	ICurvePrimitiveP curveA = curveVector->at(0).get();
	ICurvePrimitiveP curveB = curveVector->at(1).get();
	CurveCurve::IntersectionsXY(*intersectionA, *intersectionB, curveA, curveB, nullptr, true);
	DPoint3d sp, ep,startPoint,endPoint;
	auto p = intersectionB->at(0);
	p->GetStartEnd(sp, ep);	
	CurveLocationDetail location;
	curveA->ClosestPointBounded(m_dimPoints.at(0), location);
	startPoint = location.point;
	curveB->ClosestPointBounded(m_dimPoints.at(1), location);
	endPoint = location.point;
	DVec3d dvec1= DVec3d::FromStartEnd(sp, startPoint);
	DVec3d dvec2 =DVec3d::FromStartEnd(sp, endPoint);
	DVec3d dvec3 = DVec3d::FromStartEnd(sp, point);
	DVec3d dvec = DVec3d::UnitZ();
	double originAngle = mdlVec_signedAngleBetweenVectors(&dvec1, &dvec2, &dvec);
	double computeAngle;
	IDimensionEdit* iDimEdit = dynamic_cast<IDimensionEdit*>(&elem.GetHandler());
	if (SUCCESS != iDimEdit->InsertPoint(elem, &sp, nullptr, *pDimStyle, -1))
	{
		return ERROR;
	}
	if (originAngle > 0)
	{
		computeAngle= mdlVec_signedAngleBetweenVectors(&dvec1, &dvec3, &dvec);
		if (computeAngle >= 0 && computeAngle <= originAngle)
		{
			if (SUCCESS != iDimEdit->InsertPoint(elem, &startPoint, nullptr, *pDimStyle, -1))
			{
				return ERROR;
			}
			if (SUCCESS != iDimEdit->InsertPoint(elem, &endPoint, nullptr, *pDimStyle, -1))
			{
				return ERROR;
			}
		}
		else 
		{
			if (SUCCESS != iDimEdit->InsertPoint(elem, &endPoint, nullptr, *pDimStyle, -1))
			{
				return ERROR;
			}
			if (SUCCESS != iDimEdit->InsertPoint(elem, &startPoint, nullptr, *pDimStyle, -1))
			{
				return ERROR;
			}
		}
	}
	else 
	{
		double oAngle = mdlVec_signedAngleBetweenVectors(&dvec2, &dvec1, &dvec);
		computeAngle = mdlVec_signedAngleBetweenVectors(&dvec2, &dvec3, &dvec);
		if (computeAngle >= 0 && computeAngle <= oAngle)
		{
			if (SUCCESS != iDimEdit->InsertPoint(elem, &endPoint, nullptr, *pDimStyle, -1))
			{
				return ERROR;
			}
			if (SUCCESS != iDimEdit->InsertPoint(elem, &startPoint, nullptr, *pDimStyle, -1))
			{
				return ERROR;
			}
		}
		else
		{
			if (SUCCESS != iDimEdit->InsertPoint(elem, &startPoint, nullptr, *pDimStyle, -1))
			{
				return ERROR;
			}
			if (SUCCESS != iDimEdit->InsertPoint(elem, &endPoint, nullptr, *pDimStyle, -1))
			{
				return ERROR;
			}
		}
	}
	iDimEdit->SetRotationMatrix(elem, RotMatrix::FromIdentity());
	witLength = mdlVec_distance(&sp, &point);
	iDimEdit->SetHeight(elem, witLength);
	return SUCCESS;
}