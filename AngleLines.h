#pragma once
#pragma once
#include    <Mstn/MdlApi/MdlApi.h>
#include    <Mstn/ISessionMgr.h>
#include    <Mstn/ElementPropertyUtils.h>
#include    <DgnView/DgnElementSetTool.h>
#include    <DgnPlatform/DimensionHandler.h>
#include    <DgnPlatform/ISettings.h>
#include    <DgnPlatform/LinearHandlers.h>
#include    <DgnPlatform/AssociativePoint.h>
#include    "Dimensionscmd.h"
USING_NAMESPACE_BENTLEY_DGNPLATFORM
USING_NAMESPACE_BENTLEY_MSTNPLATFORM
USING_NAMESPACE_BENTLEY_MSTNPLATFORM_ELEMENT
struct       AngleLinesCreateTool : DgnElementSetTool
{
private:
	CurveVectorPtr                  curveVector;
	int                             m_iView;
	HitPathCP                       m_hitPath;
	bvector<DPoint3d>               m_dimPoints;
	int                             m_alignment;
	AngleLinesCreateTool(int toolName, int alignment) : DgnElementSetTool(toolName), m_alignment(alignment) { }
	~AngleLinesCreateTool()
	{
	   
	}
	virtual     void                _OnPostInstall() override;
	virtual     bool                _OnDataButton(DgnButtonEventCR ev) override;
	virtual     void                _OnRestartTool() override;
	virtual     void                _OnDynamicFrame(DgnButtonEventCR ev) override;
	virtual     bool                _OnResetButton(DgnButtonEventCR ev) override;
	virtual     bool                _OnPostLocate(HitPathCP path, WStringR cantAcceptReason) override;
	StatusInt                       _OnElementModify(EditElementHandleR eeh) override { return SUCCESS; }  // pure virtual method, sub-class must override!
	void                            ProjectPointsTo2dIfNeeded(DPoint3d   *points, int numPoints);
	ICurvePrimitivePtr              GetSegmentLine(HitPathCP hitPath);
	void                            FinishDimension(DgnButtonEventCR ev);
	BentleyStatus                   CreateDime(EditElementHandleR elem, DPoint3dCR point);
	bool                            ValidateSelection(HitPathCP hitPath);
public:

	static      void                InstallNewInstance();
};
struct CreateDimensionCallbacks :  IDimCreateData
{

      private: DimensionStylePtr m_dimStyle;
		 DgnTextStylePtr m_textStyle;
		 Symbology m_symbology;
		 LevelId m_levelId;
		 DirectionFormatterPtr m_directionFormatter;
		 RotMatrix m_rot1;
		 RotMatrix m_rot2;

     public: CreateDimensionCallbacks(DimensionStylePtr dimstyle, DgnTextStylePtr textstyle, Symbology symb, LevelId levelId, DirectionFormatterPtr formatter, RotMatrix  rot1, RotMatrix rot2)
     {
	   m_dimStyle = dimstyle;
	   m_textStyle = textstyle;
	   m_symbology = symb;
	   m_levelId = levelId;
	   m_directionFormatter = formatter;
	   m_rot1 = rot1;
	   m_rot2 = rot2;
     }
		virtual DimensionStyleCR _GetDimStyle() const
		{
			return *m_dimStyle;
		}

		virtual DgnTextStyleCR _GetTextStyle() const
		{
			return *m_textStyle;
		}

		virtual Symbology _GetSymbology() const
		{
			return m_symbology;
		}
		virtual LevelId _GetLevelID() const
		{
			return m_levelId;
		}
		virtual DirectionFormatterCR  _GetDirFormat() const
		{
			return *m_dirFormatter;
		}
		virtual int _GetViewNumber() const
		{
			return 0;
		}

		virtual RotMatrixCR _GetDimRMatrix() const
		{
			return m_rot1;
		}

		virtual RotMatrixCR _GetViewRMatrix() const
		{
			return m_rot2;
		}

		virtual bool _GetAnnScaleAllowed() const
		{
			return true;
		}
};
