#include "Dimensions.h"

void CDrawLinearDim::InstallNewInstance()
{
	CDrawLinearDim* tool = new CDrawLinearDim(0, 0);

	tool->InstallTool();

	/*auto strNum = d2c.DoConvert(1);
	auto msg = std::wstring(L"�������") + strNum.data() + L"����:";
	mdlOutput_command(msg.data());*/
}

void CDrawLinearDim::_OnPostInstall()
{
	AccuSnap::GetInstance().EnableSnap(true);

	__super::_OnPostInstall();
}

void CDrawLinearDim::_OnRestartTool()
{
	InstallNewInstance();
}

bool CDrawLinearDim::_OnDataButton(DgnButtonEventCR ev)
{
	if (0 == m_points.size())
	{
		_BeginDynamics();
		EnableUndoPreviousStep();
	}
	WString msg(L"�������");
	/*msg += d2c.DoConvert(m_points.size() + 2).data();
	msg += L"����,���Ҽ����";
	mdlOutput_command(msg.data());*/

	m_points.push_back(*ev.GetPoint());
	if (m_points.size() < 2)
		return false;
	DrawDimImp(m_eeh, m_points);
	//m_points.erase(m_points.begin());
	return true;
}

bool CDrawLinearDim::_OnResetButton(DgnButtonEventCR ev)
{
	EditElementHandle   eeh;

	if (DrawDimImp(m_eeh, m_points))
		m_eeh.AddToModel(); // Add new line string element to active model. The place linestring tool uses a reset to accept.

	_OnReinitialize(); // Exits tool if started in single-shot mode, otherwise restarts it.
	return true;
}

void CDrawLinearDim::_OnDynamicFrame(DgnButtonEventCR ev)
{
	bvector<DPoint3d>   tmpPts = m_points;
	EditElementHandle   eeh;

	tmpPts.push_back(*ev.GetPoint()); // Use current button location as end point.

	if (!DrawDimImp(eeh, tmpPts))
		return;

	RedrawElems redrawElems;

	redrawElems.SetDynamicsViews(IViewManager::GetActiveViewSet(), ev.GetViewport()); // Display in all views, draws to cursor view first...
	redrawElems.SetDrawMode(DRAW_MODE_TempDraw);
	redrawElems.SetDrawPurpose(DrawPurpose::Dynamics);

	redrawElems.DoRedraw(eeh);
}


bool CDrawLinearDim::DrawDimImp(EditElementHandleR eeh, bvector<DPoint3d> const& points)
{
	bvector<DPoint3d> pt = points;
	if (points.size() < 2)
		return false;
	IDimensionEdit* iDimEdit;
	//���ñ�ע����
		//��ȡģ�͵�ÿ������Ԫ��uors����
	static double g_1mu = mdlModelRef_getUorPerMaster(ACTIVEMODEL);
	//��ȡ��ǰ�ģ�͵�DgnModelP
	static DgnModelP pActiveModel = ISessionMgr::GetActiveDgnModelP();
	//��ȡ��ǰ���ļ���DgnFileObj��
	static DgnFileP  pActiveDgnFile = mdlDgnFileObj_getMasterFile();
	//���ļ��л�ȡ���ע���á�
	static DimensionStylePtr pActiveDimStyle = DimensionStyle::GetSettings(*pActiveDgnFile);
	//���������������Ե�ֵ��
	pActiveDimStyle->SetIntegerProp(DimStyleProp_Terminator_Mode::DIMSTYLE_VALUE_Terminator_Mode_Inside,
		DimStyleProp::DIMSTYLE_PROP_Terminator_Mode_INTEGER);
	pActiveDimStyle->SetBooleanProp(true, DIMSTYLE_PROP_ExtensionLine_OverrideColor_BOOLINT);
	pActiveDimStyle->SetColorProp(27, DIMSTYLE_PROP_ExtensionLine_Color_COLOR);
	//pActiveDimStyle->SetLineStyleProp(0, DIMSTYLE_PROP_Terminator_NoLineThruOrigin_BOOLINT);
	//��ȡ�ı���ʽ
	static DgnTextStylePtr textStyle = DgnTextStyle::GetSettings(*pActiveDgnFile);
	//��ȡͼ��id
	static LevelId levelID = pActiveModel->GetLevelCache().GetLevelByName(L"Default").GetLevelId();
	static Symbology symb;
	symb.style = 0;
	symb.weight = 0;
	static DirectionFormatterPtr dirFormatter = DirectionFormatter::Create(*pActiveModel);
	static std::unique_ptr<SlinearDimData> pDimCreateData =
		std::make_unique<SlinearDimData>(pActiveDimStyle, textStyle, symb, levelID, dirFormatter);
	DimensionHandler::CreateDimensionElement(eeh, *pDimCreateData, DimensionType::SizeArrow, ISessionMgr::GetActiveDgnModelP()->Is3d(), *pActiveModel);
	iDimEdit = dynamic_cast<IDimensionEdit*>(&eeh.GetHandler());
	double yValue = pt[0].y;
	for (bvector <DPoint3d>::const_iterator iter = pt.begin(); iter != pt.end(); ++iter)
	{
		DPoint3d Point = *iter;
		Point.y= yValue;
		iDimEdit->InsertPoint(eeh, &(*iter), nullptr, *pActiveDimStyle, 0);
	}
	return true;
}


