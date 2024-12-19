#ifndef _SparseMat_
#define _SparseMat_

#include <vector>
#include <map>

/* SparseMatrix类实现了稀疏矩阵,按行顺序压缩存储,主要包括非零元素,列索引 */
class SparseMat
{
 public:

	 class MatRow
	 {
	 public:
		 std::map<int, float> m_Row; ///< Row std::map
	 };

	 SparseMat();

    void Initialize(int rowSize,int colSize,int nnz=0);//分配矩阵存储空间
    void Replacement(SparseMat & result);//计算置换
    void Clear();

	//! Gets element.
	/*!
	\param  row Row
	\param  col Column
	\return	 Matrix entry
	*/
	float GetAt(int row, int col);

	//! Sets element.
	/*!
	\param  row Row
	\param  col Column
	\param  entry Entry
	\return	 No return value (void)
	*/
	void SetAt(int row, int col, float entry);
	
	//! Adds element.
	/*!
	\param  row Row
	\param  col Column
	\param  entry Entry
	\return	 No return value (void)
	*/
	void AddAt(int row, int col, float entry);

	//! Get row values.
	/*!
	\param  row Row
	\param  vals Receives values
	\param  idx Receives indices
	\return	 No return value (void)
	*/
	void GetRowVals(int row, std::vector<float>& vals, std::vector<int>& idx);


    int m_rowNum;//行数
    int m_colNum;//列数
    int m_nnz;//非零元素计数

	std::vector<MatRow> m_Data; ///< array of elements

    std::vector<float>  a;	//按列序存储的非零元素
    std::vector<int> asub;  //非零元素所对应的行索引
    std::vector<int> xa;    //每列第一个元素在 a中的索引

};

//稀疏矩阵乘法
void Multiply(SparseMat & A, SparseMat & B, SparseMat & result);

#endif