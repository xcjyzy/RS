#include <iostream>
#include "../../gdal/include/gdal_priv.h"
#include"../../gdal/include/gdalwarper.h"
#include"../../gdal/include/ogrsf_frmts.h"
#include <afx.h>
#pragma comment(lib,"../../gdal/lib/gdal.lib")

int main()
{
	GDALAllRegister();
	BYTE iByte = 0;
	int oneByte = sizeof(BYTE);
	GDALDataset *pRead;
	pRead = (GDALDataset *)GDALOpen("../../yzy.jpg", GA_ReadOnly);
	if (pRead == NULL)
	{
		printf("Can't Read File!!");
		return 0;
	}
	else printf("Read File Successfully\n");
	int iCols = pRead->GetRasterXSize();
	int iRows = pRead->GetRasterYSize();
	int iBands = pRead->GetRasterCount();
	const char* SR = GDALGetProjectionRef(pRead);
	printf("hang:%d lie:%d bands:%d SR:%s", iCols, iRows, iBands, SR);
	printf("Size of one Byte:%d", oneByte);
	BYTE *pR = (BYTE*)malloc(iCols*iRows * sizeof(BYTE));
	BYTE *pB = (BYTE*)malloc(iCols*iRows * sizeof(BYTE));
	BYTE *pG = (BYTE*)malloc(iCols*iRows * sizeof(BYTE)); 
	//RasterIO函数解释：
	//(方法选择，起始X位置，起始X位置，宽度，长度，储存目标，目标宽，目标长，目标储存格式，单数据储存间隔，行数据储存间隔)
	//目标长宽决定缩放，会重采样，方式是最邻近采样
	GDALRasterBand* pBand = pRead->GetRasterBand(1);
	pBand->RasterIO(GF_Read, 0, 0, iCols, iRows, pR, iCols, iRows, GDT_Byte, 0, 0);
	pBand = pRead->GetRasterBand(2);
	pBand->RasterIO(GF_Read, 0, 0, iCols, iRows, pG, iCols, iRows, GDT_Byte, 0, 0);
	pBand = pRead->GetRasterBand(3);
	pBand->RasterIO(GF_Read, 0, 0, iCols, iRows, pB, iCols, iRows, GDT_Byte, 0, 0);
	//CFile类提供了对文件进行打开，关闭，读，写，删除，重命名以及获取文件信息等文件操作的基本功能
	//此处创建新文件并写入

	/*
	CFile FileRed, FileGreen, FileBlue;
	FileRed.Open("C:\\Users\\asus\\Desktop\\Winter_3\\RemoteSence\\cs1\\redyzy.jpg", CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
	FileBlue.Open("C:\\Users\\asus\\Desktop\\Winter_3\\RemoteSence\\cs1\\buleyzy.jpg", CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
	FileGreen.Open("C:\\Users\\asus\\Desktop\\Winter_3\\RemoteSence\\cs1\\greenyzy.jpg", CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
	long int iSq = iCols * iRows;
	FileRed.Write(pR, iSq);
	for (int i = 0; i < 2 * iSq; i++)
	{
		FileRed.Write(&iByte, 1);
	}
	*/

	//打不开，应该是JPG头文件的问题。
	//创建文件可能想到使用Create函数，实际上一次性写入数据只支持TIFF。JPG等格式驱动没有相应的Create方法
	//尝试使用MEM内存储存临时缓冲区文件，用driver，copy的方式，将MEM内存转存进jpg
	//官方文档：https://www.osgeo.cn/gdal/api/gdaldriver_cpp.html
	//CSDN：https://blog.csdn.net/hong__fang/article/details/42266215?ops_request_misc=%257B%2522request%255Fid%2522%253A%2522167480414216800186523206%2522%252C%2522scm%2522%253A%252220140713.130102334.pc%255Fall.%2522%257D&request_id=167480414216800186523206&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~first_rank_ecpm_v1~rank_v31_ecpm-1-42266215-null-null.142^v71^control,201^v4^add_ask&utm_term=C%E5%88%9B%E5%BB%BAjpg%E6%97%A0%E6%B3%95%E6%89%93%E5%BC%80&spm=1018.2226.3001.4187

	GDALDriver * pMEMDriver = GetGDALDriverManager()->GetDriverByName("MEM");
	GDALDataset* pMEMdatasetred = pMEMDriver->Create("", iCols, iRows, 3, GDT_Byte, NULL);
	pMEMdatasetred->GetRasterBand(1)->RasterIO(GF_Write, 0, 0, iCols, iRows, pR, iCols, iRows, GDT_Byte, 0, 0);
	GDALDriver *pJPGDriver = GetGDALDriverManager()->GetDriverByName("JPEG");
	CString outFilename = "C:\\Users\\asus\\Desktop\\Winter_3\\RemoteSence\\cs1\\redyzy.jpg";
	pJPGDriver->CreateCopy(outFilename, pMEMdatasetred, FALSE,0,0,0);

	GDALDataset* pMEMdatasetG = pMEMDriver->Create("", iCols, iRows, 3, GDT_Byte, NULL);
	pMEMdatasetG->GetRasterBand(2)->RasterIO(GF_Write, 0, 0, iCols, iRows, pG, iCols, iRows, GDT_Byte, 0, 0);
	outFilename = "C:\\Users\\asus\\Desktop\\Winter_3\\RemoteSence\\cs1\\greenyzy.jpg";
	pJPGDriver->CreateCopy(outFilename, pMEMdatasetG, FALSE, 0, 0, 0);

	GDALDataset* pMEMdatasetB = pMEMDriver->Create("", iCols, iRows, 3, GDT_Byte, NULL);
	pMEMdatasetB->GetRasterBand(3)->RasterIO(GF_Write, 0, 0, iCols, iRows, pB, iCols, iRows, GDT_Byte, 0, 0);
	outFilename = "C:\\Users\\asus\\Desktop\\Winter_3\\RemoteSence\\cs1\\blueyzy.jpg";
	pJPGDriver->CreateCopy(outFilename, pMEMdatasetB, FALSE, 0, 0, 0);
	
	GDALDataset* pMEMdataset = pMEMDriver->Create("", iCols, iRows, 3, GDT_Byte, NULL);
	pMEMdataset->GetRasterBand(1)->RasterIO(GF_Write, 0, 0, iCols, iRows, pR, iCols, iRows, GDT_Byte, 0, 0);
	pMEMdataset->GetRasterBand(2)->RasterIO(GF_Write, 0, 0, iCols, iRows, pG, iCols, iRows, GDT_Byte, 0, 0);
	pMEMdataset->GetRasterBand(3)->RasterIO(GF_Write, 0, 0, iCols, iRows, pB, iCols, iRows, GDT_Byte, 0, 0);
	outFilename = "C:\\Users\\asus\\Desktop\\Winter_3\\RemoteSence\\cs1\\realyzy.jpg";
	pJPGDriver->CreateCopy(outFilename, pMEMdataset, FALSE, 0, 0, 0);
	//炫彩
	pMEMdataset = pMEMDriver->Create("", iCols, iRows, 3, GDT_Byte, NULL);
	pMEMdataset->GetRasterBand(1)->RasterIO(GF_Write, 0, 0, iCols-20, iRows-20, pR, iCols, iRows, GDT_Byte, 0, 0);
	pMEMdataset->GetRasterBand(2)->RasterIO(GF_Write, 0, 0, iCols-10, iRows-10, pG, iCols, iRows, GDT_Byte, 0, 0);
	pMEMdataset->GetRasterBand(3)->RasterIO(GF_Write, 0, 0, iCols, iRows, pB, iCols, iRows, GDT_Byte, 0, 0);
	outFilename = "C:\\Users\\asus\\Desktop\\Winter_3\\RemoteSence\\cs1\\shiningyzy.jpg";
	pJPGDriver->CreateCopy(outFilename, pMEMdataset, FALSE, 0, 0, 0);

	//注意JEPG格式波段排列顺序
	pMEMdataset->GetRasterBand(3)->RasterIO(GF_Write, 0, 0, iCols, iRows, pR, iCols, iRows, GDT_Byte, 0, 0);
	pMEMdataset->GetRasterBand(2)->RasterIO(GF_Write, 0, 0, iCols, iRows, pG, iCols, iRows, GDT_Byte, 0, 0);
	pMEMdataset->GetRasterBand(1)->RasterIO(GF_Write, 0, 0, iCols, iRows, pB, iCols, iRows, GDT_Byte, 0, 0);
	outFilename = "C:\\Users\\asus\\Desktop\\Winter_3\\RemoteSence\\cs1\\fakeyzy.jpg";
	pJPGDriver->CreateCopy(outFilename, pMEMdataset, FALSE, 0, 0, 0);
	//关闭所有的GDAL对象并注销驱动
	GDALClose(pMEMdataset);
	GDALClose(pMEMdatasetred);
	GDALClose(pMEMdatasetG);
	GDALClose(pMEMdatasetB);
	GDALDestroyDriverManager();
	return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
