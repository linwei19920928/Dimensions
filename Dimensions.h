#pragma once
#include <Mstn/MdlApi/MdlApi.h>
#include <Mstn/MstnTypes.h>
#include <Mstn/ElementPropertyUtils.h>
#include <DgnView/AccuDraw.h>
#include <DgnView/DgnElementSetTool.h>
#include <DgnView/DgnRegionElementTool.h>
#include <DgnPlatform/DropGraphics.h>
#include <DgnPlatform/NotificationManager.h>
#include <DgnPlatform/DimensionHandler.h>
#include <Mstn/ISessionMgr.h>
#include "Dimensionscmd.h"
USING_NAMESPACE_BENTLEY
USING_NAMESPACE_BENTLEY_DGNPLATFORM
USING_NAMESPACE_BENTLEY_MSTNPLATFORM_ELEMENT


struct SlinearDimData : public IDimCreateData
{

	SlinearDimData(DimensionStylePtr dimStyle, DgnTextStylePtr textStyle, Symbology symb,
		LevelId levelId, DirectionFormatterPtr directionFormatter) :
		m_dimStyle(dimStyle),
		m_textStyle(textStyle),
		m_symbology(symb),
		m_levelId(levelId),
		m_directionFormatter(directionFormatter),
		m_rotMat(RotMatrix::FromIdentity())
	{}
	virtual ~SlinearDimData() {}

	//!< Supplies style information for the new dimension.
	virtual DimensionStyleCR _GetDimStyle() const override
	{
		return *m_dimStyle;
	}

	//!< Supplies text style information for the new dimension.
	virtual DgnTextStyleCR _GetTextStyle() const override
	{
		return *m_textStyle;
	}

	//!< Supplies color, linestyle and weight information for the new dimension.
	virtual Symbology _GetSymbology() const override
	{
		return m_symbology;
	}

	//!< Supplies the level for the new dimension.
	virtual LevelId _GetLevelID()   const override
	{
		return m_levelId;
	}

	//!< Supplies direction formatting information for the new dimension.  Only called when creating label lines.
	virtual DirectionFormatterCR _GetDirFormat() const  override
	{
		return *m_dirFormatter;
	};

	//!< Supplies a view number which is stored on the dimension.  The value will be ignored.
	virtual int _GetViewNumber() const override
	{
		return 0;
	}

	//!< Supplies a rotation matrix that determines the orientation of the dimension.
	virtual RotMatrixCR  _GetDimRMatrix()  const override
	{
		return  m_rotMat;
	}

	//!< Supplies a rotation matrix that determines the orientation of text within the dimension.
	virtual RotMatrixCR  _GetViewRMatrix() const override
	{
		return m_rotMat;
	}

	//!< If true, the dimension will be assigned the model's annotation scale if appropriate given the style and model settings.
	virtual bool  _GetAnnScaleAllowed() const  override
	{
		return true;
	}

	DimensionStylePtr m_dimStyle;
	DgnTextStylePtr m_textStyle;
	Symbology m_symbology;
	LevelId m_levelId;
	DirectionFormatterPtr m_directionFormatter;
	RotMatrix m_rotMat;
};
class CDrawLinearDim :DgnPrimitiveTool
{
public:
	static void InstallNewInstance();

public:
	CDrawLinearDim(int toolId, int toolPrompt) : DgnPrimitiveTool(toolId, toolPrompt) {	}

protected:
	virtual void _OnPostInstall() override;
	virtual void _OnRestartTool() override;
	virtual bool _OnDataButton(DgnButtonEventCR ev) override;
	virtual bool _OnResetButton(DgnButtonEventCR ev) override;
	virtual void _OnDynamicFrame(DgnButtonEventCR ev) override;

private:
bool DrawDimImp(EditElementHandleR eeh, bvector<DPoint3d> const& points);

private:
	EditElementHandle   m_eeh;
	bvector<DPoint3d>   m_points;
};