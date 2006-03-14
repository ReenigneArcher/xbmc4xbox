#include "../stdafx.h"


CCharsetConverter g_charsetConverter;

CCharsetConverter::CCharsetConverter()
{
  m_vecCharsetNames.push_back("ISO-8859-1");
  m_vecCharsetLabels.push_back("Western Europe (ISO)");
  m_vecCharsetNames.push_back("ISO-8859-2");
  m_vecCharsetLabels.push_back("Central Europe (ISO)");
  m_vecCharsetNames.push_back("ISO-8859-3");
  m_vecCharsetLabels.push_back("South Europe (ISO)");
  m_vecCharsetNames.push_back("ISO-8859-4");
  m_vecCharsetLabels.push_back("Baltic (ISO)");
  m_vecCharsetNames.push_back("ISO-8859-5");
  m_vecCharsetLabels.push_back("Cyrillic (ISO)");
  m_vecCharsetNames.push_back("ISO-8859-6");
  m_vecCharsetLabels.push_back("Arabic (ISO)");
  m_vecCharsetNames.push_back("ISO-8859-7");
  m_vecCharsetLabels.push_back("Greek (ISO)");
  m_vecCharsetNames.push_back("ISO-8859-8");
  m_vecCharsetLabels.push_back("Hebrew (ISO)");
  m_vecCharsetNames.push_back("ISO-8859-9");
  m_vecCharsetLabels.push_back("Turkish (ISO)");

  m_vecCharsetNames.push_back("CP1250");
  m_vecCharsetLabels.push_back("Central Europe (Windows)");
  m_vecCharsetNames.push_back("CP1251");
  m_vecCharsetLabels.push_back("Cyrillic (Windows)");
  m_vecCharsetNames.push_back("CP1252");
  m_vecCharsetLabels.push_back("Western Europe (Windows)");
  m_vecCharsetNames.push_back("CP1253");
  m_vecCharsetLabels.push_back("Greek (Windows)");
  m_vecCharsetNames.push_back("CP1254");
  m_vecCharsetLabels.push_back("Turkish (Windows)");
  m_vecCharsetNames.push_back("CP1255");
  m_vecCharsetLabels.push_back("Hebrew (Windows)");
  m_vecCharsetNames.push_back("CP1256");
  m_vecCharsetLabels.push_back("Arabic (Windows)");
  m_vecCharsetNames.push_back("CP1257");
  m_vecCharsetLabels.push_back("Baltic (Windows)");
  m_vecCharsetNames.push_back("CP1258");
  m_vecCharsetLabels.push_back("Vietnamesse (Windows)");
  m_vecCharsetNames.push_back("CP874");
  m_vecCharsetLabels.push_back("Thai (Windows)");

  m_vecCharsetNames.push_back("BIG5");
  m_vecCharsetLabels.push_back("Chinese Traditional (Big5)");
  m_vecCharsetNames.push_back("GBK");
  m_vecCharsetLabels.push_back("Chinese Simplified (GBK)");
  m_vecCharsetNames.push_back("SHIFT_JIS");
  m_vecCharsetLabels.push_back("Japanese (Shift-JIS)");
  m_vecCharsetNames.push_back("CP949");
  m_vecCharsetLabels.push_back("Korean");
  m_vecCharsetNames.push_back("BIG5-HKSCS");
  m_vecCharsetLabels.push_back("Hong Kong (Big5-HKSCS)");

  m_vecBidiCharsetNames.push_back("ISO-8859-6");
  m_vecBidiCharsets.push_back(FRIBIDI_CHARSET_ISO8859_6);
  m_vecBidiCharsetNames.push_back("ISO-8859-8");
  m_vecBidiCharsets.push_back(FRIBIDI_CHARSET_ISO8859_8);
  m_vecBidiCharsetNames.push_back("CP1255");
  m_vecBidiCharsets.push_back(FRIBIDI_CHARSET_CP1255);
  m_vecBidiCharsetNames.push_back("Windows-1255");
  m_vecBidiCharsets.push_back(FRIBIDI_CHARSET_CP1255);
  m_vecBidiCharsetNames.push_back("CP1256");
  m_vecBidiCharsets.push_back(FRIBIDI_CHARSET_CP1256);
  m_vecBidiCharsetNames.push_back("Windows-1256");
  m_vecBidiCharsets.push_back(FRIBIDI_CHARSET_CP1256);

  // reset();
}

void CCharsetConverter::clear()
{
  m_vecBidiCharsetNames.clear();
  m_vecBidiCharsets.clear();
  m_vecCharsetNames.clear();
  m_vecCharsetLabels.clear();
}

vector<CStdString> CCharsetConverter::getCharsetLabels()
{
  return m_vecCharsetLabels;
}

CStdString& CCharsetConverter::getCharsetLabelByName(const CStdString& charsetName)
{
  for (unsigned int i = 0; i < m_vecCharsetNames.size(); i++)
  {
    if (m_vecCharsetNames[i] == charsetName)
    {
      return m_vecCharsetLabels[i];
    }
  }

  return EMPTY;
}

CStdString& CCharsetConverter::getCharsetNameByLabel(const CStdString& charsetLabel)
{
  for (unsigned int i = 0; i < m_vecCharsetLabels.size(); i++)
  {
    if (m_vecCharsetLabels[i] == charsetLabel)
    {
      return m_vecCharsetNames[i];
    }
  }

  return EMPTY;
}

boolean CCharsetConverter::isBidiCharset(const CStdString& charset)
{
  for (unsigned int i = 0; i < m_vecBidiCharsetNames.size(); i++)
  {
    if (m_vecBidiCharsetNames[i].Equals(charset))
    {
      return true;
    }
  }

  return false;
}

void CCharsetConverter::reset(void)
{
  m_iconvStringCharsetToFontCharset = (iconv_t) - 1;
  m_iconvUtf8ToStringCharset = (iconv_t) - 1;
  m_iconvStringCharsetToUtf8 = (iconv_t) - 1;
  m_iconvUcs2CharsetToStringCharset = (iconv_t) - 1;
  m_iconvSubtitleCharsetToFontCharset = (iconv_t) - 1;
  m_iconvUtf16toUtf8 = (iconv_t) - 1;
  m_iconvUtf32ToStringCharset = (iconv_t) - 1;
  m_iconvUtf8toUtf16 = (iconv_t) - 1;
  m_stringFribidiCharset = FRIBIDI_CHARSET_NOT_FOUND;

  for (unsigned int i = 0; i < m_vecBidiCharsetNames.size(); i++)
  {
    if (m_vecBidiCharsetNames[i] == g_guiSettings.GetString("LookAndFeel.CharSet"))
    {
      m_stringFribidiCharset = m_vecBidiCharsets[i];
    }
  }
}

void CCharsetConverter::utf8ToUTF16(const CStdStringA& utf8String, CStdStringW &utf16String)
{
  CStdStringA strFlipped;

  const char* src;
  size_t inBytes;

  // If this is hebrew/arabic, flip the characters
  if (m_stringFribidiCharset != FRIBIDI_CHARSET_NOT_FOUND)
  {
    logicalToVisualBiDi(utf8String, strFlipped, m_stringFribidiCharset);
    src = strFlipped.c_str();
    inBytes = strFlipped.length() + 1;
  }
  else
  {
    src = utf8String.c_str();
    inBytes = utf8String.length() + 1;
  }

  if (m_iconvUtf8toUtf16 == (iconv_t) - 1)
  {
    m_iconvUtf8toUtf16 = iconv_open("UTF-16LE", "UTF-8");
  }

  if (m_iconvUtf8toUtf16 != (iconv_t) - 1)
  {
    char *dst = new char[inBytes * 2];
    size_t outBytes = inBytes * 2;
    char *outdst = dst;
    if (iconv(m_iconvUtf8toUtf16, &src, &inBytes, &outdst, &outBytes))
    {
      // For some reason it failed (maybe wrong charset?). Nothing to do but
      // return the original..
      utf16String = utf8String;
    }
    else
      utf16String = (WCHAR *)dst;
    delete[] dst;
  }
}

void CCharsetConverter::stringCharsetToFontCharset(const CStdStringA& strSource, CStdStringW& strDest)
{
  CStdStringA strFlipped;

  const char* src;
  size_t inBytes;

  // If this is hebrew/arabic, flip the characters
  if (m_stringFribidiCharset != FRIBIDI_CHARSET_NOT_FOUND)
  {
    logicalToVisualBiDi(strSource, strFlipped, m_stringFribidiCharset);
    src = strFlipped.c_str();
    inBytes = strFlipped.length() + 1;
  }
  else
  {
    src = strSource.c_str();
    inBytes = strSource.length() + 1;
  }

  if (m_iconvStringCharsetToFontCharset == (iconv_t) - 1)
  {
    m_iconvStringCharsetToFontCharset = iconv_open("UTF-16LE", g_guiSettings.GetString("LookAndFeel.CharSet").c_str());
  }

  if (m_iconvStringCharsetToFontCharset != (iconv_t) - 1)
  {
    char *dst = new char[inBytes * 2];
    size_t outBytes = inBytes * 2;
    char *outdst = dst;
    if (iconv(m_iconvStringCharsetToFontCharset, &src, &inBytes, &outdst, &outBytes))
    {
      // For some reason it failed (maybe wrong charset?). Nothing to do but
      // return the original..
      strDest = strSource;
    }
    else
      strDest = (WCHAR *)dst;
    delete[] dst;
  }
}

void CCharsetConverter::subtitleCharsetToFontCharset(const CStdStringA& strSource, CStdStringW& strDest)
{
  CStdStringA strFlipped;

  // No need to flip hebrew/arabic as mplayer does the flipping

  if (m_iconvSubtitleCharsetToFontCharset == (iconv_t) - 1)
  {
    m_iconvSubtitleCharsetToFontCharset = iconv_open("UTF-16LE", g_guiSettings.GetString("Subtitles.CharSet").c_str());
  }

  if (m_iconvSubtitleCharsetToFontCharset != (iconv_t) - 1)
  {
    const char* src = strSource.c_str();
    size_t inBytes = strSource.length() + 1;
    char *dst = (char*)strDest.GetBuffer(inBytes * 2);
    size_t outBytes = inBytes * 2;

    if (iconv(m_iconvSubtitleCharsetToFontCharset, &src, &inBytes, &dst, &outBytes))
    {
      strDest.ReleaseBuffer();
      // For some reason it failed (maybe wrong charset?). Nothing to do but
      // return the original..
      strDest = strSource;
    }
    strDest.ReleaseBuffer();
  }
}

void CCharsetConverter::logicalToVisualBiDi(const CStdStringA& strSource, CStdStringA& strDest, CStdStringA& charset, FriBidiCharType base)
{
  FriBidiCharSet fribidiCharset = FRIBIDI_CHARSET_UTF8;

  for (unsigned int i = 0; i < m_vecBidiCharsetNames.size(); i++)
  {
    if (m_vecBidiCharsetNames[i].Equals(charset))
    {
		fribidiCharset = m_vecBidiCharsets[i];
		break;
    }
  }

  logicalToVisualBiDi(strSource, strDest, fribidiCharset, base);
}

void CCharsetConverter::logicalToVisualBiDi(const CStdStringA& strSource, CStdStringA& strDest, FriBidiCharSet fribidiCharset, FriBidiCharType base)
{
  int sourceLen = strlen(strSource.c_str());
  FriBidiChar* logical = (FriBidiChar*) malloc((sourceLen + 1) * sizeof(FriBidiChar));
  FriBidiChar* visual = (FriBidiChar*) malloc((sourceLen + 1) * sizeof(FriBidiChar));
  // Convert from the selected charset to Unicode
  int len = fribidi_charset_to_unicode(fribidiCharset, (char*) strSource.c_str(), sourceLen, logical);

  if (fribidi_log2vis(logical, len, &base, visual, NULL, NULL, NULL))
  {
    // Removes bidirectional marks
    //len = fribidi_remove_bidi_marks(visual, len, NULL, NULL, NULL);

    char *result = strDest.GetBuffer(sourceLen);

    // Convert back from Unicode to the charset
    fribidi_unicode_to_charset(fribidiCharset, visual, len, result);
    strDest.ReleaseBuffer();
  }

  free(logical);
  free(visual);
}

void CCharsetConverter::utf8ToStringCharset(const CStdStringA& strSource, CStdStringA& strDest)
{
  if (m_iconvUtf8ToStringCharset == (iconv_t) - 1)
  {
    m_iconvUtf8ToStringCharset = iconv_open(g_guiSettings.GetString("LookAndFeel.CharSet").c_str(), "UTF-8");
  }

  if (m_iconvUtf8ToStringCharset != (iconv_t) - 1)
  {
    const char* src = strSource.c_str();
    size_t inBytes = strSource.length() + 1;

    char *dst = strDest.GetBuffer(inBytes);
    size_t outBytes = inBytes - 1;

    if (iconv(m_iconvUtf8ToStringCharset, &src, &inBytes, &dst, &outBytes) == -1)
    {
      strDest.ReleaseBuffer();
      // For some reason it failed (maybe wrong charset?). Nothing to do but
      // return the original..
      strDest = strSource;
    }
    strDest.ReleaseBuffer();
  }
}

void CCharsetConverter::stringCharsetToUtf8(const CStdStringA& strSource, CStdStringA& strDest)
{
  if (m_iconvStringCharsetToUtf8 == (iconv_t) - 1)
  {
    m_iconvStringCharsetToUtf8 = iconv_open("UTF-8", g_guiSettings.GetString("LookAndFeel.CharSet").c_str());
  }

  if (m_iconvStringCharsetToUtf8 != (iconv_t) - 1)
  {
    const char* src = strSource.c_str();
    size_t inBytes = strSource.length();

    size_t outBytes = (inBytes * 4) + 1;
    size_t originalOutBytes = outBytes;
    char *dst = strDest.GetBuffer(outBytes);

    if (iconv(m_iconvStringCharsetToUtf8, &src, &inBytes, &dst, &outBytes) == -1)
    {
      strDest.ReleaseBuffer();
      // For some reason it failed (maybe wrong charset?). Nothing to do but
      // return the original..
      strDest = strSource;
      return ;
    }

    strDest.ReleaseBuffer();
  }
}

void CCharsetConverter::utf16toUTF8(const CStdStringW& strSource, CStdStringA &strDest)
{
  if (m_iconvUtf16toUtf8 == (iconv_t) - 1)
    m_iconvUtf16toUtf8 = iconv_open("UTF-8", "UTF-16LE");
  if (m_iconvUtf16toUtf8 != (iconv_t) - 1)
  {
    const char* src = (const char*) strSource.c_str();
    size_t inBytes = (strSource.length() + 1)*2;
    char *dst = strDest.GetBuffer(inBytes);
    size_t outBytes = (inBytes * 2) + 1;  // some free for UTF8 conversion
    size_t originalOutBytes = outBytes;
    if (iconv(m_iconvUtf16toUtf8, &src, &inBytes, &dst, &outBytes))
    { // failed :(
      strDest.ReleaseBuffer();
      strDest = strSource;
    }
    strDest.ReleaseBuffer();
  }
}

void CCharsetConverter::ucs2CharsetToStringCharset(const CStdStringW& strSource, CStdStringA& strDest, bool swap)
{
  if (m_iconvUcs2CharsetToStringCharset == (iconv_t) - 1)
  {
    m_iconvUcs2CharsetToStringCharset = iconv_open(g_guiSettings.GetString("LookAndFeel.CharSet").c_str(), "UTF-16LE");
  }

  if (m_iconvUcs2CharsetToStringCharset != (iconv_t) - 1)
  {
    const char* src = (const char*) strSource.c_str();
    size_t inBytes = (strSource.length() + 1) * 2;

    if (swap)
    {
      char* s = (char*) src;

      while (*s || *(s + 1))
      {
        char c = *s;
        *s = *(s + 1);
        *(s + 1) = c;

        s++;
        s++;
      }
    }

    char *dst = strDest.GetBuffer(inBytes);
    size_t outBytes = inBytes;

    if (iconv(m_iconvUcs2CharsetToStringCharset, &src, &inBytes, &dst, &outBytes))
    {
      strDest.ReleaseBuffer();
      // For some reason it failed (maybe wrong charset?). Nothing to do but
      // return the original..
      strDest = strSource;
    }
    strDest.ReleaseBuffer();
  }
}

void CCharsetConverter::utf32ToStringCharset(const unsigned long* strSource, CStdStringA& strDest)
{
  if (m_iconvUtf32ToStringCharset == (iconv_t) - 1)
  {
    m_iconvUtf32ToStringCharset = iconv_open(g_guiSettings.GetString("LookAndFeel.CharSet").c_str(), "UTF-32LE");
  }

  if (m_iconvUtf32ToStringCharset != (iconv_t) - 1)
  {
    const unsigned long* ptr=strSource;
    while (*ptr) ptr++;
    const char* src = (const char*) strSource;
    size_t inBytes = (ptr-strSource+1)*4;

    char *dst = strDest.GetBuffer(inBytes);
    size_t outBytes = inBytes;

    if (iconv(m_iconvUtf32ToStringCharset, &src, &inBytes, &dst, &outBytes))
    {
      strDest.ReleaseBuffer();
      // For some reason it failed (maybe wrong charset?). Nothing to do but
      // return the original..
      strDest = (const char *)strSource;
    }
    strDest.ReleaseBuffer();
  }
}

