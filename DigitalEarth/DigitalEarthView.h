// 这段 MFC 示例源代码演示如何使用 MFC Microsoft Office Fluent 用户界面
// (“Fluent UI”)。该示例仅供参考，
// 用以补充《Microsoft 基础类参考》和
// MFC C++ 库软件随附的相关电子文档。
// 复制、使用或分发 Fluent UI 的许可条款是单独提供的。
// 若要了解有关 Fluent UI 许可计划的详细信息，请访问
// https://go.microsoft.com/fwlink/?LinkId=238214.
//
// 版权所有(C) Microsoft Corporation
// 保留所有权利。

// DigitalEarthView.h: CDigitalEarthView 类的接口
//

#pragma once

#include "DigitalEarthDoc.h"
#include "OSGObject.h"

class CDigitalEarthView : public CView
{
protected: // 仅从序列化创建
	CDigitalEarthView() noexcept;
	DECLARE_DYNCREATE(CDigitalEarthView)

// 特性
public:
	CDigitalEarthDoc* GetDocument() const;

public:
	OSGObject* mOSG;
	HANDLE mThreadHandle;

	double chinaBoundariesOpt;
// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// 实现
public:
	virtual ~CDigitalEarthView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	//自定义变量区
	BOOL isShowChinaBoundary;

	//飞往
	double flylat, flylon, flyhei;

	//是否启动预设置路径
	bool isStartFly;
	bool isTrack;


protected:

// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	virtual void OnInitialUpdate();
	
	afx_msg void OnSetChinaBound();
	afx_msg void OnShowChinaBound();
	afx_msg void OnUpdateShowChinaBound(CCmdUI *pCmdUI);
	afx_msg void OnChinaBound();
	afx_msg void OnFlytoLon();
	afx_msg void OnFlytoLat();
	afx_msg void OnFlytoHei();
	afx_msg void OnButtonFlyto();
	afx_msg void OnCheck2Start();
	afx_msg void OnUpdateCheck2Start(CCmdUI *pCmdUI);
	afx_msg void OnCheck7track();
	afx_msg void OnUpdateCheck7track(CCmdUI *pCmdUI);
};

#ifndef _DEBUG  
inline CDigitalEarthDoc* CDigitalEarthView::GetDocument() const
   { return reinterpret_cast<CDigitalEarthDoc*>(m_pDocument); }
#endif

