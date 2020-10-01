
// MiniDrawDoc.h: интерфейс класса CMiniDrawDoc 
//


#pragma once
#include<afxtempl.h>

class CElement : public CObject {
protected:
	CPoint m_startPoint;
	CPoint m_endPoint;
	COLORREF border;
	COLORREF bg;

public:
	CElement() {};
	CElement(CPoint begin, CPoint end, COLORREF border, COLORREF bg)
		: m_startPoint(begin), m_endPoint(end), border(border), bg(bg) {};
	virtual void Draw(CDC* PDC) = 0;
	void setBackground(COLORREF color);
};

class CEllipse : public CElement {
public:
	CEllipse() {};
	DECLARE_SERIAL(CEllipse)

	CEllipse(CPoint begin, CPoint end, COLORREF border, COLORREF bg)
		: CElement(begin, end, border, bg) {}

	void Draw(CDC* PDC) override;
	virtual void Serialize(CArchive& ar);
};

class CMiniDrawDoc : public CDocument
{
protected:
	CTypedPtrArray<CObArray, CElement*> m_ElemArr;
	CElement* lastELem;

public:
	void AddEllipse(CPoint begin, CPoint end, COLORREF border, COLORREF bg);
	CElement* getElement(int index);
	int getNumElements();

protected: // создать только из сериализации
	CMiniDrawDoc() noexcept;
	DECLARE_DYNCREATE(CMiniDrawDoc)

// Атрибуты
public:

// Операции
public:

// Переопределение
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// Реализация
public:
	virtual ~CMiniDrawDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Созданные функции схемы сообщений
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// Вспомогательная функция, задающая содержимое поиска для обработчика поиска
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	virtual void DeleteContents();
	afx_msg void OnEditClearAll();
	afx_msg void OnUpdateEditClearAll(CCmdUI* pCmdUI);
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnEditRedo();
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
};
