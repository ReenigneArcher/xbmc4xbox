
#include "stdafx.h"
#include "ArenaItem.h"
#include "Utils/KaiClient.h"


CGUIImage* CArenaItem::m_pPrivateIcon = NULL;

CArenaItem::CArenaItem(CStdString& strLabel) : CKaiItem(strLabel)
{
  SetCookie( CKaiClient::Item::Arena );
  m_strVector = "/";
  m_strDescription = "";
  m_strPassword = "";
  m_nPlayers = -1;
  m_nPlayerLimit = -1;
  m_bIsPrivate = false;
  m_bIsPersonal = false;
}

CArenaItem::~CArenaItem(void)
{}

void CArenaItem::SetIcons(INT aWidth, INT aHeight, const CStdString& aPrivateTexture)
{
  if (aPrivateTexture.length() > 0)
  {
    m_pPrivateIcon = new CGUIImage(0, 0, 0, 0, aWidth, aHeight, aPrivateTexture, 0x0);
    m_pPrivateIcon->AllocResources();
  }
}

void CArenaItem::FreeIcons()
{
  if (m_pPrivateIcon)
  {
    m_pPrivateIcon->FreeResources();
    delete m_pPrivateIcon;
    m_pPrivateIcon = NULL;
  }
}

CArenaItem::Tier CArenaItem::GetTier()
{
  int tier = 0;

  for (int characterIndex = 0; characterIndex < m_strVector.GetLength(); characterIndex++)
  {
    if (m_strVector[characterIndex] == '/')
    {
      tier++;
    }
  }

  if (tier > 4)
  {
    tier = 4;
  }

  return (CArenaItem::Tier) tier;
}

void CArenaItem::GetTier(CArenaItem::Tier aTier, CStdString& aTierName)
{
  GetTier(aTier, m_strVector, aTierName);
}

void CArenaItem::GetTier(Tier aTier, CStdString aVector, CStdString& aTierName)
{
  int tier = 0;
  int characterIndex = 0;
  CStdString name;

  for (; characterIndex < aVector.GetLength(); characterIndex++)
  {
    if (tier == (int)aTier)
    {
      int nextTier = aVector.Find('/', characterIndex + 1);
      if (nextTier >= characterIndex)
      {
        aTierName = aVector.Mid(characterIndex, nextTier - characterIndex);
      }
      else
      {
        aTierName = aVector.Mid(characterIndex);
      }
      break;
    }

    if (aVector[characterIndex] == '/')
    {
      tier++;
    }
  }
}

void CArenaItem::GetDisplayText(CStdString& aString)
{
  if (m_bIsPersonal)
  {
    char chLastLetter = m_strName[m_strName.length() - 1];
    bool bEndsInS = (chLastLetter == 's') || (chLastLetter == 'S');
    if (bEndsInS)
    {
      aString.Format("%s' arena", m_strName);
    }
    else
    {
      aString.Format("%s's arena", m_strName);
    }
  }
  else
  {
    aString = m_strName;
  }
}

void CArenaItem::OnPaint(CGUIItem::RenderContext* pContext)
{
  CKaiItem::OnPaint(pContext);

  CGUIListExItem::RenderContext* pDC = (CGUIListExItem::RenderContext*)pContext;

  if (pDC)
  {
    int iBaseLineY = pDC->m_iPositionY;
    int iBaseLineX = pDC->m_iPositionX + pDC->m_pButton->GetWidth();

    iBaseLineX -= 64;

    if (m_pPrivateIcon && m_bIsPrivate)
    {
      int iIconPosX = iBaseLineX - (m_pPrivateIcon->GetWidth() + 4);
      int iIconPosY = iBaseLineY;
      iIconPosY += (pDC->m_pButton->GetHeight() - m_pPrivateIcon->GetHeight()) / 2;

      m_pPrivateIcon->SetPosition(iIconPosX, iIconPosY);
      m_pPrivateIcon->Render();
    }

    if (pDC->m_label.font)
    {
      int iPosX = iBaseLineX;
      int iPosY = iBaseLineY;

      // render the text
      DWORD dwColor = pDC->m_bFocused ? pDC->m_label.selectedColor : pDC->m_label.textColor;

      CStdString strInfo;
      if (m_nPlayers < 0)
      {
        // orbs remeshing: don't display anything
      }
      else if (m_nPlayerLimit > 0)
      {
        strInfo.Format("%d/%d", (m_nPlayers > 0) ? m_nPlayers : 0, m_nPlayerLimit);
      }
      else
      {
        strInfo.Format("%d", (m_nPlayers > 0) ? m_nPlayers : 0);
      }

      // no need for charset conversions here - just rendering numbers
      CStdStringW strInfoUnicode = strInfo;
      float fPosX = (float)iPosX;
      float fPosY = (float)iPosY + 2;
      if (pDC->m_pButton->GetLabelInfo().align & XBFONT_CENTER_Y)
      {
        float fTextHeight, fTextWidth;
        pDC->m_label.font->GetTextExtent( strInfoUnicode.c_str(), &fTextWidth, &fTextHeight);
        fPosY = (float)iPosY + ((float)pDC->m_pButton->GetHeight() - fTextHeight) / 2;
      }
      RenderText(fPosX, fPosY, (FLOAT)pDC->m_pButton->GetWidth(), dwColor, (WCHAR*) strInfoUnicode.c_str(), pDC->m_label);
    }
  }
}
