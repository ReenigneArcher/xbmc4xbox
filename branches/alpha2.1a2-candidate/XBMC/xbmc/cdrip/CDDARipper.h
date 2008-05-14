#ifndef _CCDDARIPPER_H
#define _CCDDARIPPER_H

#include "CDDAReader.h"
#include "Encoder.h"

namespace MUSIC_INFO
{
  class CMusicInfoTag;
}

class CCDDARipper
{
public:
  CCDDARipper();
  virtual ~CCDDARipper();

  bool RipTrack(CFileItem* pItem);
  bool RipCD();

private:
  bool Init(const CStdString& strTrackFile, const CStdString& strFile, MUSIC_INFO::CMusicInfoTag* infoTag = NULL);
  bool DeInit();
  int RipChunk(int& nPercent);
  bool Rip(const CStdString& strTrackFile, const CStdString& strFileName, MUSIC_INFO::CMusicInfoTag& infoTag);
  const char* GetExtension(int iEncoder);
  CStdString GetTrackName(CFileItem *item, bool isFatX);

  CEncoder* m_pEncoder;
  CCDDAReader m_cdReader;
};

#endif // _CCDDARIPPERMP3_H
