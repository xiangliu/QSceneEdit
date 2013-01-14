/************************************************************************/
/* it's used by 3D feature extract                                                                     */
/************************************************************************/
void GenerateBasisLUT();
void ExtractCoefficients(unsigned char *Y, double m_Coeff[ART_ANGULAR][ART_RADIAL], unsigned char *Edge, double CenX, double CenY);
void FindRadius(unsigned char *Y[CAMNUM], double *CenX, double *CenY);
double GetDistance(double m_Coeff1[ART_ANGULAR][ART_RADIAL], double m_Coeff2[ART_ANGULAR][ART_RADIAL]);

void FindRadius1(unsigned char *Y[CAMNUM], double *CenX, double *CenY,int size);
void FindRadius2(unsigned char *Y, double *CenX, double *CenY);