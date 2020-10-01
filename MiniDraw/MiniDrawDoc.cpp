
// MiniDrawDoc.cpp: реализация класса CMiniDrawDoc 
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS можно определить в обработчиках фильтров просмотра реализации проекта ATL, эскизов
// и поиска; позволяет совместно использовать код документа в данным проекте.
#ifndef SHARED_HANDLERS
#include "MiniDraw.h"
#endif

#include "MiniDrawDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMiniDrawDoc

IMPLEMENT_DYNCREATE(CMiniDrawDoc, CDocument)

BEGIN_MESSAGE_MAP(CMiniDrawDoc, CDocument)
	ON_COMMAND(ID_EDIT_CLEAR_ALL, &CMiniDrawDoc::OnEditClearAll)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR_ALL, &CMiniDrawDoc::OnUpdateEditClearAll)
	ON_COMMAND(ID_EDIT_UNDO, &CMiniDrawDoc::OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, &CMiniDrawDoc::OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_REDO, &CMiniDrawDoc::OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, &CMiniDrawDoc::OnUpdateEditRedo)
END_MESSAGE_MAP()


// Создание или уничтожение CMiniDrawDoc

void CMiniDrawDoc::AddEllipse(CPoint begin, CPoint end, COLORREF border, COLORREF bg)
{
	m_ElemArr.Add(new CEllipse(begin, end, border, bg));
	SetModifiedFlag();
}

CElement* CMiniDrawDoc::getElement(int index)
{
	if (index < 0 || index > m_ElemArr.GetUpperBound()) return 0;
	return m_ElemArr.GetAt(index);
}

int CMiniDrawDoc::getNumElements()
{
	return (int)m_ElemArr.GetSize();
}

CMiniDrawDoc::CMiniDrawDoc() noexcept
{
	lastELem = nullptr;
}

CMiniDrawDoc::~CMiniDrawDoc()
{
}

BOOL CMiniDrawDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: добавьте код повторной инициализации
	// (Документы SDI будут повторно использовать этот документ)

	return TRUE;
}




// Сериализация CMiniDrawDoc

void CMiniDrawDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		m_ElemArr.Serialize(ar);
	}
	else
	{
		m_ElemArr.Serialize(ar);
	}
}

#ifdef SHARED_HANDLERS

// Поддержка для эскизов
void CMiniDrawDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// Измените этот код для отображения данных документа
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// Поддержка обработчиков поиска
void CMiniDrawDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// Задание содержимого поиска из данных документа.
	// Части содержимого должны разделяться точкой с запятой ";"

	// Например:  strSearchContent = _T("точка;прямоугольник;круг;объект ole;");
	SetSearchContent(strSearchContent);
}

void CMiniDrawDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// Диагностика CMiniDrawDoc

#ifdef _DEBUG
void CMiniDrawDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMiniDrawDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// Команды CMiniDrawDoc

IMPLEMENT_SERIAL(CEllipse, CObject, 1)
void CEllipse::Draw(CDC* PDC)
{
	CPen pen(PS_SOLID, 3, border);
	CBrush brush(bg);
	PDC->SelectObject(&pen);
	PDC->SelectObject(&brush);

	PDC->Ellipse(m_startPoint.x, m_startPoint.y, m_endPoint.x, m_endPoint.y);
}

void CEllipse::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		ar << m_startPoint << m_endPoint << border << bg;
	}
	else
	{
		ar >> m_startPoint >> m_endPoint >> border >> bg;
	}
}


void CMiniDrawDoc::DeleteContents()
{
	int index = 0;
	while (index < getNumElements() - 1) {
		delete m_ElemArr.GetAt(index++);
	}
	m_ElemArr.RemoveAll();
	CDocument::DeleteContents();
}

void CElement::setBackground(COLORREF color)
{
	bg = color;
}


void CMiniDrawDoc::OnEditClearAll()
{
	if(!lastELem)
		lastELem = m_ElemArr.GetAt(m_ElemArr.GetUpperBound());
	DeleteContents();
	UpdateAllViews(0);
	SetModifiedFlag();
}


void CMiniDrawDoc::OnUpdateEditClearAll(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(getNumElements());
}




void CMiniDrawDoc::OnEditUndo()
{
	int index = (int)m_ElemArr.GetUpperBound();
	if (index > -1) {
		if (!lastELem) {
			lastELem = m_ElemArr.GetAt(index);
		}
		else
		{
			delete m_ElemArr.GetAt(index);
		}
		m_ElemArr.RemoveAt(index);
		UpdateAllViews(0);
		SetModifiedFlag();
	}
}


void CMiniDrawDoc::OnUpdateEditUndo(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(getNumElements());
}


void CMiniDrawDoc::OnEditRedo()
{
	m_ElemArr.Add(lastELem);
	lastELem = nullptr;
	UpdateAllViews(0);
	SetModifiedFlag();
}


void CMiniDrawDoc::OnUpdateEditRedo(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(lastELem != nullptr);
}
