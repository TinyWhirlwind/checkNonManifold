#include "SparseMat.h"

SparseMat::SparseMat()
{
	m_rowNum = 0;
	m_colNum = 0;
	m_nnz = 0;
}

void SparseMat::Initialize(int rowSize, int colSize, int nnz)
{
	m_rowNum = rowSize;
	m_colNum = colSize;
	m_nnz = nnz;

	m_Data.resize(m_rowNum); // 按行存储

	if (a.size() > 0) a.clear();
	if (asub.size() > 0) asub.clear();
	if (xa.size() > 0) xa.clear();

	if (nnz != 0)
	{
		a.resize(nnz);
		asub.resize(nnz);
	}
	xa.resize(rowSize + 1);
}

void SparseMat::Clear()
{
	m_nnz = 0;
	a.clear();
	asub.clear();
	xa.clear();
}

float SparseMat::GetAt(int row, int col)
{
	std::map<int, float>::iterator mapIt;
	if ((mapIt = m_Data[row].m_Row.find(col)) == m_Data[row].m_Row.end())
		return 0.0f;
	else
		return mapIt->second;
}

void SparseMat::SetAt(int row, int col, float entry)
{
	m_Data[row].m_Row[col] = entry;
}

void SparseMat::AddAt(int row, int col, float entry)
{
	std::map<int, float>::iterator mapIt;
	if ((mapIt = m_Data[row].m_Row.find(col)) == m_Data[row].m_Row.end())
		m_Data[row].m_Row[col] = entry;
	else
		mapIt->second += entry;
}

void SparseMat::GetRowVals(int row, std::vector<float>& vals, std::vector<int>& idx)
{
	std::map<int, float>::iterator mapIt;
	for (mapIt = m_Data[row].m_Row.begin(); mapIt != m_Data[row].m_Row.end(); ++mapIt)
	{
		idx.push_back(mapIt->first);
		vals.push_back(mapIt->second);
	}
}

void SparseMat::Replacement(SparseMat& result)
{
	std::vector<int> xat;//记录置换矩阵每列元素的个数
	xat.resize(m_colNum);

	result.Initialize(m_colNum, m_rowNum, m_nnz); //初始化转置矩阵

	for (int i = 0; i < m_colNum; i++) xat[i] = 0;
	for (int i = 0; i < m_rowNum; i++)   //计算原矩阵每列元素个数
	{
		for (int j = xa[i]; j < xa[i + 1]; j++)
			xat[asub[j]]++;
	}

	int sum = 0;
	for (int i = 0; i < m_colNum; i++) //计算行首元素索引
	{
		result.xa[i] = sum;
		sum += xat[i];
	}
	result.xa[m_colNum] = sum;

	int index;
	for (int i = 0; i < m_rowNum; i++)  //存储转置矩阵元素
	{
		for (int j = xa[i]; j < xa[i + 1]; j++)
		{
			index = asub[j];

			result.a[result.xa[index]] = a[j];
			result.asub[result.xa[index]] = i;

			result.xa[index]++;
		}
	}

	sum = 0;
	for (int i = 0; i < m_colNum; i++) //计算行首元素索引
	{
		result.xa[i] = sum;
		sum += xat[i];
	}
	result.xa[m_colNum] = sum;

	xat.clear(); //释放内存
}

void Multiply(SparseMat& A, SparseMat& B, SparseMat& result)
{
	result.Initialize(A.m_rowNum, B.m_colNum, 0);

	int rowIndex, colIndex;

	std::vector<double> rowValue; //存储每行的结果
	rowValue.resize(result.m_colNum);
	for (int i = 0; i < result.m_colNum; i++) rowValue[i] = 0.0;

	for (int i = 0; i < result.m_rowNum; i++)//对于每一行
	{
		result.xa[i] = result.m_nnz;

		for (int j = A.xa[i]; j < A.xa[i + 1]; j++)//每行中的非零元素
		{
			rowIndex = A.asub[j];
			for (int k = B.xa[rowIndex]; k < B.xa[rowIndex + 1]; k++)
			{
				colIndex = B.asub[k];
				rowValue[colIndex] += A.a[j] * B.a[k];
			}
		}
		for (int j = 0; j < result.m_colNum; j++)
			if (rowValue[j] != 0.0)
			{
				result.m_nnz++;//A^T*A元素数量增加
				result.a.push_back(rowValue[j]);
				result.asub.push_back(j);
				rowValue[j] = 0.0;
			}
	}
	result.xa[result.m_rowNum] = result.m_nnz;
}