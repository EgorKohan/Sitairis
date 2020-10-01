
// MiniDrawView.cpp: реализация класса CMiniDrawView
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS можно определить в обработчиках фильтров просмотра реализации проекта ATL, эскизов
// и поиска; позволяет совместно использовать код документа в данным проекте.
#ifndef SHARED_HANDLERS
#include "MiniDraw.h"
#endif

#include "MiniDrawDoc.h"
#include "MiniDrawView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMiniDrawView

IMPLEMENT_DYNCREATE(CMiniDrawView, CView)

BEGIN_MESSAGE_MAP(CMiniDrawView, CView)
	// Стандартные команды печати
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CMiniDrawView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

// Создание или уничтожение CMiniDrawView

CMiniDrawView::CMiniDrawView() noexcept
{
	m_Dragging = 0;
	m_HCross = AfxGetApp()->LoadStandardCursor(IDC_CROSS);
}

CMiniDrawView::~CMiniDrawView()
{
}

BOOL CMiniDrawView::PreCreateWindow(CREATESTRUCT& cs)
{
	m_ClassName = AfxRegisterWndClass(
		CS_HREDRAW | CS_VREDRAW, // стили окна
		0,					  // без указателя;
		(HBRUSH)::GetStockObject(WHITE_BRUSH), // задать чисто белый фон;
		0);					  // без значка
	cs.lpszClass = m_ClassName;


	return CView::PreCreateWindow(cs);
}

// Рисование CMiniDrawView

void CMiniDrawView::OnDraw(CDC* pDC)
{
	CMiniDrawDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	int index = 0;
	while (index < pDoc->getNumElements()) {
		pDoc->getElement(index++)->Draw(pDC);
	}
}


// Печать CMiniDrawView


void CMiniDrawView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CMiniDrawView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// подготовка по умолчанию
	return DoPreparePrinting(pInfo);
}

void CMiniDrawView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: добавьте дополнительную инициализацию перед печатью
}

void CMiniDrawView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: добавьте очистку после печати
}

void CMiniDrawView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CMiniDrawView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// Диагностика CMiniDrawView

#ifdef _DEBUG
void CMiniDrawView::AssertValid() const
{
	CView::AssertValid();
}

void CMiniDrawView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMiniDrawDoc* CMiniDrawView::GetDocument() const // встроена неотлаженная версия
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMiniDrawDoc)));
	return (CMiniDrawDoc*)m_pDocument;
}
#endif //_DEBUG


// Обработчики сообщений CMiniDrawView


void CMiniDrawView::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_PointBegin = point;
	m_PointEnd = point;
	SetCapture();
	m_Dragging = 1;
	RECT Rect;
	GetClientRect(&Rect);
	ClientToScreen(&Rect);
	::ClipCursor(&Rect);


	CView::OnLButtonDown(nFlags, point);
}


void CMiniDrawView::OnMouseMove(UINT nFlags, CPoint point)
{
	::SetCursor (m_HCross);
	if (m_Dragging)
	{
		COLORREF border = RGB(255, 0, 0);
		COLORREF bg = RGB(0, 0, 255);
		CClientDC ClientDC(this);
		CPen pen(PS_SOLID, 3, border);
		CBrush brush(bg);
		ClientDC.SelectObject(&pen);
		ClientDC.SelectObject(&brush);
		ClientDC.SetROP2(R2_NOTXORPEN);
		
		ClientDC.Ellipse(m_PointBegin.x, m_PointBegin.y, m_PointEnd.x, m_PointEnd.y);
		ClientDC.Ellipse(m_PointBegin.x, m_PointBegin.y, point.x, point.y);

		m_PointEnd = point;
	}


	CView::OnMouseMove(nFlags, point);
}


void CMiniDrawView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_Dragging)
	{
		COLORREF border = RGB(255, 0, 0);
		COLORREF bg = RGB(0, 0, 255);
		m_Dragging = 0;
		::ReleaseCapture();
		::ClipCursor(NULL);
		CClientDC ClientDC(this);
		CPen pen(PS_SOLID, 3, border);
		CBrush brush(bg);
		ClientDC.SelectObject(&pen);
		ClientDC.SelectObject(&brush);
		ClientDC.SetROP2(R2_COPYPEN);
		
		ClientDC.Ellipse(m_PointBegin.x, m_PointBegin.y, point.x, point.y);

		CMiniDrawDoc* pDoc = GetDocument();
		pDoc->AddEllipse(m_PointBegin, point, border, bg);
	}

	CView::OnLButtonUp(nFlags, point);
}

void CMiniDrawView::OnRButtonDown(UINT nFlags, CPoint point)
{
	CMiniDrawDoc* pDoc = GetDocument();
	int index = 0;
	while (index < pDoc->getNumElements()) {
		pDoc->getElement(index++)->setBackground(RGB(255, 0, 0));
	}
	pDoc->UpdateAllViews(0);
	CView::OnRButtonDown(nFlags, point);
}
