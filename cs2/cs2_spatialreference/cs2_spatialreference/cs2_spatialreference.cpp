#include <iostream>
using namespace std;
//GDAL200.dll放在cpp同目录下
#include"../../../gdal/include/gdal_priv.h"
#include"../../../gdal/include/gdalwarper.h"
#include"../../../gdal/include/ogrsf_frmts.h"
//afx.h是MFC库。在属性中设置MFC为在共享DLL中使用
#include <afx.h>
#pragma comment(lib,"../../../gdal/lib/gdal.lib")
//投影，注意在属性中设置使用多字节字符集
int main()
{
	//注册
	GDALAllRegister();
	//打开
	GDALDataset* pDataset = (GDALDataset *)GDALOpen("../../../Kolovai.tif", GA_ReadOnly);
	if (!pDataset)
	{
		cout << "Can't read file!Please check your file path!" << endl;
	}
	else
	{
		cout << "Read file successfully" << endl;
	}
	//Kolovai.tif是陆丽珍空间分析的图，狠狠读取一下
	CString dataSF = pDataset->GetProjectionRef();
	cout << "数据的投影信息为：" << dataSF << endl;
	double GeoTransForm[6];
	pDataset->GetGeoTransform(GeoTransForm);    
	for (int i = 0; i < 6; i++)
	{
		cout << "仿射变换系数"<<i<<":"<<GeoTransForm[i] << endl;
	}
	//使用TIFF驱动创建新图像
	CString outpath = "../../new.tif";
	GDALDriver* pTiffDriver = GetGDALDriverManager()->GetDriverByName("GTIFF");
	//GDALDataset* pTiff = pTiffDriver->CreateCopy(outpath, pDataset, FALSE, NULL, NULL, NULL);
	//CString copySF = pTiff->GetProjectionRef();
	//cout << "copy数据的投影信息为：" << copySF << endl;
	for (int i = 0; i < 6; i++)
	{
		GeoTransForm[i] = 100 * (i + 1);
	}
	//pTiff->SetGeoTransform(GeoTransForm);
	//投影转WKT,虽然本来就是WKT
	//参考：http://www.javashuo.com/article/p-elisznpg-be.html
	char*oriWKT = (char *) malloc(strlen(dataSF) + 1);
	strcpy_s(oriWKT, strlen(dataSF) + 1, dataSF);
	cout << "WKT:"<<oriWKT << endl;
	//新建OGR的空间参考，并另存为OGR格式
	OGRSpatialReference oriSR;
	oriSR.importFromWkt(&oriWKT);
	//投影转换模板TranGauss（源->目标）,处理的时候有可能报错，返回空值
	//原因是:1、两投影坐标不支持此次转换
	//2、和PROJ4库异常有关，检查环境配置（dll文件是否放在正确位置）
	OGRSpatialReference *SR_Gauss = oriSR.CloneGeogCS();
	SR_Gauss->SetTM(0, 123, 1.0, 41500000, 0);
	OGRCoordinateTransformation* TranGauss = OGRCreateCoordinateTransformation(&oriSR, SR_Gauss);
	//计算栅格坐标为（100,100）在真实地理坐标下的位置
	pDataset->GetGeoTransform(GeoTransForm);
	int X = 100, Y = 100; //栅格坐标              
	double GaussX = GeoTransForm[0] + X * GeoTransForm[1] + Y * GeoTransForm[2];
	double GaussY = GeoTransForm[3] + X * GeoTransForm[4] + Y * GeoTransForm[5];
	cout << "原始UTM投影下坐标：(" << GaussX << "," << GaussY << ")" << endl;
	//使用模板进行坐标转换
	TranGauss->Transform(1, &GaussX, &GaussY);
	cout << "转换后高斯克吕格投影下坐标：(" << GaussX << "," << GaussY << ")" << endl;
	GDALClose(pDataset);
	return 0;
}
