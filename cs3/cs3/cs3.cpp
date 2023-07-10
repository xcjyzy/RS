//代数运算NDVI,NDWI
#include <iostream>
#include"../../gdal/include/gdal_priv.h"
#include"../../gdal/include/gdalwarper.h"
#include"../../gdal/include/ogrsf_frmts.h"
#include<afx.h>
#pragma comment(lib,"../../gdal/lib/gdal.lib")
using namespace std;
int main()
{
	GDALAllRegister();
	CString pathin = "../data/hzsq.tif";
	GDALDataset* pDataset = (GDALDataset*)GDALOpen(pathin, GA_ReadOnly);
	if (pDataset) 
	{
		cout << "READ FILE SUCCESSFULLY" << endl;
	}
	else
	{
		cout << "Can't Read Files!" << endl;
	}
	const int Ori_X = pDataset->GetRasterXSize();
	const int Ori_Y = pDataset->GetRasterYSize();
	GDALRasterBand* band2 = pDataset->GetRasterBand(2);
	GDALRasterBand* band3 = pDataset->GetRasterBand(3);
	GDALRasterBand* band4 = pDataset->GetRasterBand(4);
	//创建新数据
	GDALDriver* pDriver = GetGDALDriverManager()->GetDriverByName("GTIFF");
	GDALDataset* pNDVI = pDriver->Create("../NDVI.tif", Ori_X, Ori_Y, 1, GDT_Float64, NULL);
	GDALDataset* pNDWI = pDriver->Create("../NDWI.tif", Ori_X, Ori_Y, 1, GDT_Float64, NULL);
	if (!pNDVI || !pNDWI) cout << "Create new file failed!" << endl; 
	else cout << "Create new file successfully!" << endl;
	GDALRasterBand* pNDVIband = pNDVI->GetRasterBand(1);
	GDALRasterBand* pNDWIband = pNDWI->GetRasterBand(1);
	//读取数据
	double* pndvi = new double[Ori_X];
	double* pndwi = new double[Ori_X];
	double* oridata1 = new double[Ori_X];
	double* oridata2 = new double[Ori_X];
	double* oridata3 = new double[Ori_X];
	if (!pndvi || !pndwi || !oridata1 || !oridata2 || !oridata3)
		cout << "ERROR:Create memory space failed!" << endl;
	else cout << "Create memory space successfully!"<< endl;
	for (int i = 0; i < Ori_Y; i++)
	{
		band2->RasterIO(GF_Read, 0, i, Ori_X, 1, oridata1, Ori_X, 1, GDT_Float64, 0, 0);
		band3->RasterIO(GF_Read, 0, i, Ori_X, 1, oridata2, Ori_X, 1, GDT_Float64, 0, 0);
		band4->RasterIO(GF_Read, 0, i, Ori_X, 1, oridata3, Ori_X, 1, GDT_Float64, 0, 0);
		//计算NDVI ， NDWI
		for (int j = 0; j < Ori_X; j++)
		{
			if (abs(oridata3[j] + oridata2[j]) < 0.00000001)
				pndvi[j] = 0;
			else pndvi[j] = (oridata3[j] - oridata2[j]) / (oridata3[j] + oridata2[j]);
			if (abs(oridata3[j] + oridata1[j]) < 0.00000001)
				pndwi[j] = 0;
			else pndwi[j] = (oridata1[j] - oridata3[j]) / (oridata3[j] + oridata1[j]);
		}
		pNDVIband->RasterIO(GF_Write, 0, i, Ori_X, 1, pndvi, Ori_X, 1, GDT_Float64, 0, 0);
		pNDWIband->RasterIO(GF_Write, 0, i, Ori_X, 1, pndwi, Ori_X, 1, GDT_Float64, 0, 0);
	}
	delete pndvi, pndwi, oridata1, oridata2, oridata3;
	GDALClose(pDataset);
	GDALClose(pNDVI);
	GDALClose(pNDWI);
}
