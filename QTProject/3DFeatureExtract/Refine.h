/************************************************************************/
/* it's used by 3D feature extract                                                                     */
/************************************************************************/

double Refine(double src_Coeff[CAMNUM][ART_ANGULAR][ART_RADIAL],
			  pVer vertex2, pTri triangle2, int NumVer2, int NumTri2, int UseCam,
			  pVer CamVertex, pTri CamTriangle, int CamNumVer, int CamNumTri);

//void RenderToMem(unsigned char *bmBits, unsigned char *bmColor, pVer CamVertex, pVer v, pTri t, int nv, int nt);