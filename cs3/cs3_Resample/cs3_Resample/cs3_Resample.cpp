//重采样
#include <iostream>
#include"../../../gdal/include/gdal_priv.h"
#include"../../../gdal/include/gdalwarper.h"
#include"../../../gdal/include/ogrsf_frmts.h"
#include<afx.h>
#pragma comment(lib,"../../../gdal/lib/gdal.lib")
using namespace std;
int main()
{
	GDALAllRegister();
	CString filepathin = "../../data/hzsq.tif";
	GDALDataset* pDataset = (GDALDataset*)GDALOpen(filepathin, GA_ReadOnly);
}