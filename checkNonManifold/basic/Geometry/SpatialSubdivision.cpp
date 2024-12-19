#include "StdAfx.h"
#include "spatialsubdivision.h"

CSpatialSubdivision::CSpatialSubdivision(void)
{
    m_DivisionNum=35;//这个值的确定由牙齿模型与内存分配共同决定：越大，效率越高，但内存负担越重；反之，效率低，内存负担小。
    m_ElementCount=0;
    m_NeighborBlockSize=0;
    m_NeighborBlock= nullptr;
    table = nullptr;
    m_Elements = nullptr;

}

//向空间剖分结构中加入顶点数据
void CSpatialSubdivision::Insert(int id, Point3f v)
{
    SpaceCoord spaceCoord;//插入顶点的空间剖分坐标
    CSpatialSubdivisionEntry * entry;

    //计算顶点在空间剖分结构中的存储坐标
    spaceCoord.x = (int)((v.X()-m_MinCorner.X())* m_Delta.X());
    if(spaceCoord.x == m_DivisionNum) spaceCoord.x = m_DivisionNum-1;
    spaceCoord.y = (int)((v.Y()-m_MinCorner.Y())* m_Delta.Y());
    if(spaceCoord.y == m_DivisionNum) spaceCoord.y = m_DivisionNum-1;
    spaceCoord.z = (int)((v.Z()-m_MinCorner.Z()) * m_Delta.Z());
    if(spaceCoord.z == m_DivisionNum) spaceCoord.z = m_DivisionNum-1;

    //存储管理
    entry = *(table + m_DivisionNum * m_DivisionNum * spaceCoord.x + m_DivisionNum*spaceCoord.y + spaceCoord.z);
    if (entry == nullptr)
    {
        //如果尚未分配方格空间
        *(table+m_DivisionNum*m_DivisionNum*spaceCoord.x+m_DivisionNum*spaceCoord.y+spaceCoord.z) = new CSpatialSubdivisionEntry();
        entry = *(table+m_DivisionNum*m_DivisionNum*spaceCoord.x+m_DivisionNum*spaceCoord.y+spaceCoord.z);
        entry->m_Blocks = new int[3000];//每次分配增量1000个顶点空间,这个值的确定需经过实验验证
        entry->m_CurBlockSize=3000;
    }
    int * newBlock;
    if(entry->m_Size == entry->m_CurBlockSize)//如果存储空间已满
    {
        entry->m_CurBlockSize+=3000;//增加存储容量
        newBlock=new int[entry->m_CurBlockSize];
        memcpy(newBlock,entry->m_Blocks,entry->m_Size*sizeof(int));
        delete entry->m_Blocks;
        entry->m_Blocks=newBlock;
    }

    /* 向空间剖分结构中存入数据 */
    m_Elements[m_ElementCount].x = spaceCoord.x;
    m_Elements[m_ElementCount].y = spaceCoord.y;
    m_Elements[m_ElementCount].z = spaceCoord.z;
    m_Elements[m_ElementCount].entryIndex=entry->m_Size;
    entry->m_Blocks[entry->m_Size]=id;//向方格中添加顶点索引
    entry->m_Size++;
    m_ElementCount++;
}

void CSpatialSubdivision::Initialize(Point3f maxPoint,Point3f minPoint,int vertexNum)
{
    m_ElementCount = 0;
    m_NeighborBlockSize = 0;
    m_NeighborBlock = nullptr;

    //分配空间剖分结构
    table=new  CSpatialSubdivisionEntry*[m_DivisionNum*m_DivisionNum*m_DivisionNum];
    int end = m_DivisionNum * m_DivisionNum * m_DivisionNum;
    for (int i=0;i<end;i++)
    {
        table[i] = nullptr;
    }

    //分配元素顺序存储空间
    m_Elements = new SpaceCoord[vertexNum];
    m_MaxCorner=maxPoint;
    m_MinCorner=minPoint;
    m_Delta.X()=(float) m_DivisionNum/(m_MaxCorner.X()-m_MinCorner.X());//为提高性能而进行的预计算
    m_Delta.Y()=(float) m_DivisionNum/(m_MaxCorner.Y()-m_MinCorner.Y());
    m_Delta.Z()=(float) m_DivisionNum/(m_MaxCorner.Z()-m_MinCorner.Z());
}

void CSpatialSubdivision::FindNeighborhood(int id,int * & neighbors,int & num)
{
    SpaceCoord spaceCoord;
    spaceCoord.x=m_Elements[id].x;
    spaceCoord.y=m_Elements[id].y;
    spaceCoord.z=m_Elements[id].z;
    spaceCoord.entryIndex=m_Elements[id].entryIndex;

    CSpatialSubdivisionEntry *entry;

    int * block;
    int count=0;
    num=0;//清零
    int * newNeighborBlock;

    entry =*(table+m_DivisionNum*m_DivisionNum*spaceCoord.x+m_DivisionNum*spaceCoord.y+spaceCoord.z);

    if (entry != nullptr)
    {
        block=entry->m_Blocks;
        count = 0;
        while (count < entry->m_Size)
        {
            if(block[count]!=-1 && block[count]!=id)
            {//如果元素没有被删除且不是自身

                if( num == m_NeighborBlockSize )//内存管理
                {
                    m_NeighborBlockSize+=3000;
                    newNeighborBlock= new int[m_NeighborBlockSize];
                    memcpy(newNeighborBlock,m_NeighborBlock,num*sizeof(int));
                    if(m_NeighborBlock) delete m_NeighborBlock;
                    m_NeighborBlock = newNeighborBlock;
                }

                m_NeighborBlock[num]=block[count];
                num++;//元素数增加

            }
            count++;
        }
    }

    neighbors=m_NeighborBlock;//对传入指针进行赋值
}
void CSpatialSubdivision::Delete(int id)
{
    SpaceCoord spaceCoord;
    spaceCoord=m_Elements[id];
    CSpatialSubdivisionEntry *entry;
    entry =*(table+m_DivisionNum*m_DivisionNum*spaceCoord.x+m_DivisionNum*spaceCoord.y+spaceCoord.z);
    entry->m_Blocks[spaceCoord.entryIndex]=-1;//-1表示该冗余顶点被删除
}

void CSpatialSubdivision::Clear()
{
    int end;
    CSpatialSubdivisionEntry *entry;
    end = m_DivisionNum*m_DivisionNum*m_DivisionNum;//空间剖分结构剖分大小
    if(table)
    {
        //释放单元格存储
        for (int i=0;i <end;i++)
        {
            entry =*(table+i);
            if(entry)
            {
                delete entry->m_Blocks;
                delete entry;
            }
        }
    }
    if(m_Elements){delete m_Elements;m_Elements= nullptr;}
    if(m_NeighborBlock){delete m_NeighborBlock;m_NeighborBlock= nullptr;}
    if(table) {delete table;table= nullptr;}
}
