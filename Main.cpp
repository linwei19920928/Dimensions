
/*---------------------------------------------------------------------------------**//**
* 
+---------------+---------------+---------------+---------------+---------------+------*/

#include "Dimensions.h"
#include "AngleLines.h"
BentleyStatus CreateDime()
{
	EditElementHandle dimEeh;
	double g_1mu = ISessionMgr::GetActiveDgnModelP()->GetModelInfoCP()->GetUorPerMeter();
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

	if (SUCCESS != DimensionHandler::CreateDimensionElement(dimEeh, *createDimdata, DimensionType::AngleLines, ACTIVEMODEL->Is3d(), *pActiveModel))
	{
		return ERROR;
	}


	EditElementHandle lineEeh, lineEeh1;
	DPoint3d pt1 = DPoint3d::From(0, 0, 0);
	DPoint3d pt2 = DPoint3d::From(10000 * g_1mu, 5000 * g_1mu, 0);
	DSegment3d seg = DSegment3d::From(pt1, pt2);

	DPoint3d pt3 = DPoint3d::From(0, 0, 0);
	DPoint3d pt4 = DPoint3d::From(0, 10000 * g_1mu, 0);
	DSegment3d seg2 = DSegment3d::From(pt3, pt4);

	DPoint3d pt5 = DPoint3d::From(5 * g_1mu, 5 * g_1mu, 0);

	LineHandler::CreateLineElement(lineEeh, nullptr, seg, ACTIVEMODEL->Is3d(), *ACTIVEMODEL);
	lineEeh.AddToModel();

	LineHandler::CreateLineElement(lineEeh1, nullptr, seg2, ACTIVEMODEL->Is3d(), *ACTIVEMODEL);
	lineEeh1.AddToModel();

	/*AssocPoint assocPt1, assocPt3;
	AssociativePoint::InitKeypoint(assocPt1, 0, 2, 0, 1);
	AssociativePoint::SetRoot(assocPt1, lineEeh.GetElementId(), 0);
	AssociativePoint::InitKeypoint(assocPt3, 1, 2, 0, 1);
	AssociativePoint::SetRoot(assocPt3, lineEeh1.GetElementId(), 1);*/

	IDimensionEdit* iDimEdit = dynamic_cast<IDimensionEdit*>(&dimEeh.GetHandler());

	if (SUCCESS != iDimEdit->InsertPoint(dimEeh, &pt1, nullptr, *pDimStyle, -1))
	{
		return ERROR;
	}
	if (SUCCESS != iDimEdit->InsertPoint(dimEeh, &pt4, nullptr, *pDimStyle, -1))
	{
		return ERROR;
	}
	if (SUCCESS != iDimEdit->InsertPoint(dimEeh, &pt2, nullptr, *pDimStyle, -1))
	{
		return ERROR;
	}

	iDimEdit->SetRotationMatrix(dimEeh, RotMatrix::FromIdentity());

	if (dimEeh.IsValid())
	{
		dimEeh.AddToModel();
	}
	return SUCCESS;
}
static MdlCommandNumber s_commandNumbers[] =
{
{ (CmdHandler)CDrawLinearDim::InstallNewInstance, CMD_DEMO_DIMENSIONS },
{ (CmdHandler)CreateDime, CMD_DEMO_CREATEDIME },
{ (CmdHandler)AngleLinesCreateTool::InstallNewInstance, CMD_DEMO_ANGLELINES },
0
};
extern "C" void MdlMain(int argc, WCharCP argv[])
{
	/*  Open the resource file  */
	RscFileHandle       rfHandle;

	mdlResource_openFile(&rfHandle, nullptr, RSC_READONLY);
	/*  Register commands */
	mdlSystem_registerCommandNumbers(s_commandNumbers);
	mdlParse_loadCommandTable(nullptr);

}