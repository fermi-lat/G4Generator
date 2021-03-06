#ifndef LocalMagneticFieldDes_H
#define LocalMagneticFieldDes_H
// $Header:$

#include <string>

// Description of a local magnetic field, real field is constructed in
// DetectorConstruction's constructor

class LocalMagneticFieldDes { 
public:

  // only used in generating a local magnetic field
  std::string m_magFieldVol;

  // field values in 3 directions, unit is tesla
  double m_magFieldX, m_magFieldY, m_magFieldZ;
};

#endif
