#include "stdafx.h"
#include "UniformGrid.h"
#include <float.h>

namespace sn3DCore
{
    UniformGrid::UniformGrid(void)
    {
    }
    void UniformGrid::Initialize(Point3f * pointData,int num,int scale)
    {
        Point3f max,min;

        if(num<1)return;
        if(scale<1) return;

        m_SpaceBox.clear();
        m_DataPool.clear();//数据
        m_EntryMap.clear();//单元格映射

        min.X()=max.X()= pointData[0].X();
        min.Y()=max.Y()= pointData[0].Y();
        min.Z()=max.Z()= pointData[0].Z();

        for(int i=1;i<num;i++)
        {
            if(min.X() > pointData[i].X()) min.X() = pointData[i].X();
            if(min.Y() > pointData[i].Y()) min.Y() = pointData[i].Y();
            if(min.Z() > pointData[i].Z()) min.Z() = pointData[i].Z();
            if(max.X() < pointData[i].X()) max.X() = pointData[i].X();
            if(max.Y() < pointData[i].Y()) max.Y() = pointData[i].Y();
            if(max.Z() < pointData[i].Z()) max.Z() = pointData[i].Z();
        }

        SpaceBoxEntry entry;//单元
        BoxCoord coord;//单元坐标
        int sequence = 0;//分配过程中得单元索引
        float maxLen = -FLT_MAX;

        if(max[0]-min[0]>maxLen)maxLen = max[0] - min[0];
        if(max[1]-min[1]>maxLen)maxLen = max[1] - min[1];
        if(max[2]-min[2]>maxLen)maxLen = max[2] - min[2];

        float step = maxLen/(float)scale;

        m_DivisionNum[0] = (max[0]-min[0])/step;
        m_DivisionNum[1] = (max[1]-min[1])/step;
        m_DivisionNum[2] = (max[2]-min[2])/step;

        if(m_DivisionNum[0]==0)m_DivisionNum[0]=1;
        if(m_DivisionNum[1]==0)m_DivisionNum[1]=1;
        if(m_DivisionNum[2]==0)m_DivisionNum[2]=1;

        m_MaxCorner = max;
        m_MinCorner = min;
        m_Delta[0]=(float) m_DivisionNum[0]/(m_MaxCorner.X()-m_MinCorner.X());//为提高性能而进行的预计算
        m_Delta[1]=(float) m_DivisionNum[1]/(m_MaxCorner.Y()-m_MinCorner.Y());
        m_Delta[2]=(float) m_DivisionNum[2]/(m_MaxCorner.Z()-m_MinCorner.Z());

        //////////////////////////////////////////////////

        int *  boxNum = new int[m_DivisionNum[0]*m_DivisionNum[1]*m_DivisionNum[2]];//每个单元中有多少个元素
        int *  entryIndex = new int[num];//顶点在单元格中的存储顺序索引

        m_EntryMap.resize(m_DivisionNum[0]*m_DivisionNum[1]*m_DivisionNum[2]);//单元存储映射

        for(int i=0;i<m_DivisionNum[0];i++)
        {
            for(int j=0;j<m_DivisionNum[1];j++)
            {
                for(int k=0;k<m_DivisionNum[2];k++)
                {
                    m_EntryMap[i*m_DivisionNum[1]*m_DivisionNum[2]+j*m_DivisionNum[2]+k] = -1;//-1标识单元没有元素
                }
            }
        }

        for( int i=0;i<num;i++)
        {
            coord.x = (int)((pointData[i].X()-m_MinCorner.X())* m_Delta.X());
            if(coord.x >= m_DivisionNum[0]) coord.x = m_DivisionNum[0]-1;
            coord.y = (int)((pointData[i].Y()-m_MinCorner.Y())* m_Delta.Y());
            if(coord.y >= m_DivisionNum[1]) coord.y = m_DivisionNum[1]-1;
            coord.z = (int)((pointData[i].Z()-m_MinCorner.Z()) * m_Delta.Z());
            if(coord.z >= m_DivisionNum[2]) coord.z = m_DivisionNum[2]-1;

            if(m_EntryMap[coord.x*m_DivisionNum[1]*m_DivisionNum[2]+coord.y*m_DivisionNum[2]+coord.z]==-1)//第一次存入元素
            {
                boxNum[coord.x*m_DivisionNum[1]*m_DivisionNum[2]+coord.y*m_DivisionNum[2]+coord.z] = 1;//初始化元素数目
                m_EntryMap[coord.x*m_DivisionNum[1]*m_DivisionNum[2]+coord.y*m_DivisionNum[2]+coord.z] = sequence;//初始化单元索引
                entryIndex[i] = sequence;//单元索引
                sequence++;//单元索引增加
            }
            else
            {
                boxNum[coord.x*m_DivisionNum[1]*m_DivisionNum[2]+coord.y*m_DivisionNum[2]+coord.z]++;//元素数目增加
                entryIndex[i] = m_EntryMap[coord.x*m_DivisionNum[1]*m_DivisionNum[2]+coord.y*m_DivisionNum[2]+coord.z];
            }
        }

        m_DataPool.resize(num);//分配数据空间大小为模型顶点数目
        m_SpaceBox.resize(sequence);//分配单元空间大小为单元数目

        for(int i=0;i<m_DivisionNum[0];i++)
        {
            for(int j=0;j<m_DivisionNum[1];j++)
            {
                for(int k=0;k<m_DivisionNum[2];k++)
                {
                    int pos = m_EntryMap[i*m_DivisionNum[1]*m_DivisionNum[2]+j*m_DivisionNum[2]+k];//存储位置
                    if(pos!=-1)
                    {
                        m_SpaceBox[pos].m_Size = boxNum[i*m_DivisionNum[1]*m_DivisionNum[2]+j*m_DivisionNum[2]+k];
                    }
                }
            }
        }

        int beginIndex = 0;//单元数据起始索引

        for(int i=0;i<sequence;i++)
        {
            m_SpaceBox[i].m_percent = (double)m_SpaceBox[i].m_Size/(double)num;
            m_SpaceBox[i].m_BeginIndex = beginIndex;
            beginIndex += m_SpaceBox[i].m_Size;
            m_SpaceBox[i].m_Size = 0;//清零，存储元素时重新计数
        }

        //存储每个单元格的顶点信息
        for(int i=0;i<num;i++)
        {
            int pos = entryIndex[i];
            int begin = m_SpaceBox[pos].m_BeginIndex;
            int size = m_SpaceBox[pos].m_Size;
            m_DataPool[begin+size] = i;//存入顶点标识
            m_SpaceBox[pos].m_Size++;
        }

        delete boxNum;
        delete entryIndex;
    }

    Point3f UniformGrid::GetMinPoint()
    {
        return m_MinCorner;
    }

    Point3f UniformGrid::GetMaxPoint()
    {
        return  m_MaxCorner;
    }

    int UniformGrid::GetEntrySize(int x,int y,int z)
    {
        if(x>m_DivisionNum[0])return -1;
        if(y>m_DivisionNum[1])return -1;
        if(z>m_DivisionNum[2])return -1;
        if(x<0)return -1;
        if(y<0)return -1;
        if(z<0)return -1;

        int pos = m_EntryMap[x*m_DivisionNum[1]*m_DivisionNum[2]+y*m_DivisionNum[2]+z];

        if(pos==-1)return -1;

        return m_SpaceBox[pos].m_Size;//返回单元大小
    }

    int UniformGrid::GetEntryItem(int x,int y,int z,int index)
    {
        int pos = m_EntryMap[x*m_DivisionNum[1]*m_DivisionNum[2]+y*m_DivisionNum[2]+z];

        if(pos==-1)return -1;

        return m_DataPool[m_SpaceBox[pos].m_BeginIndex+index];
    }

    void UniformGrid::Clear()
    {
        m_SpaceBox.clear();
        std::vector<SpaceBoxEntry>().swap(m_SpaceBox);

        m_DataPool.clear();
        std::vector<int>().swap(m_DataPool);

        m_EntryMap.clear();
        std::vector<int>().swap(m_EntryMap);
    }
}
