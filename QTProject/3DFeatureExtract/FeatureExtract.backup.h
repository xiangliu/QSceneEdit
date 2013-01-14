/************************************************************************/
/* it's used by 3D feature extract                                                                     */
/************************************************************************/

void FindCenter(unsigned char *srcBuff, int width, int height, double *CenX, double *CenY);
void display(void);

void RenderToMem(unsigned char *bmBits, unsigned char *bmColor, pVer CamVertex, pVer v, pTri t, int nv, int nt);
void keyboard (unsigned char key, int x, int y);

void init(void) ;

void reshape (int w, int h);

int FeatureExtract(char* error);
